# KFS1 - multiboot kernel (mandatory + bonus)
# Build requires: nasm, ld (binutils), gcc capable of -m32 (or i686-elf-gcc), grub tools (grub-install, grub-mkrescue), qemu-system-i386

NAME        := kfs1
BUILD_DIR   := build
KERNEL_BIN  := $(BUILD_DIR)/$(NAME).bin
ISO_OUT     := $(BUILD_DIR)/$(NAME).iso
IMG_OUT     := $(BUILD_DIR)/$(NAME).img

NASM        := nasm
LD          := ld

# Prefer a cross-compiler if available (recommended by OSDev / 42).
CC          := $(shell command -v i686-elf-gcc >/dev/null 2>&1 && echo i686-elf-gcc || echo gcc)

CFLAGS      := -ffreestanding -O2 -Wall -Wextra -Werror -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs
# If using host gcc, force 32-bit codegen (needs multilib).
CFLAGS      += $(shell [ "$(CC)" = "gcc" ] && echo -m32 || true)
CFLAGS      += -Iinclude

LDFLAGS     := -m elf_i386 -T src/linker.ld

ASM_SRC     := src/boot.s
C_SRC       := src/kernel.c src/terminal.c src/kprintf.c src/keyboard.c

OBJ         := $(BUILD_DIR)/boot.o \
               $(BUILD_DIR)/kernel.o \
               $(BUILD_DIR)/terminal.o \
               $(BUILD_DIR)/kprintf.o \
               $(BUILD_DIR)/keyboard.o

all: $(KERNEL_BIN)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/boot.o: $(ASM_SRC) | $(BUILD_DIR)
	$(NASM) -f elf32 $< -o $@

$(BUILD_DIR)/%.o: src/%.c include/*.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)
	@echo "Built: $(KERNEL_BIN)"

# Bootable GRUB ISO (no sudo)
iso: $(KERNEL_BIN)
	@mkdir -p $(BUILD_DIR)/iso/boot/grub
	@cp grub.cfg $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@cp $(KERNEL_BIN) $(BUILD_DIR)/iso/boot/$(NAME).bin
	@grub-mkrescue -o $(ISO_OUT) $(BUILD_DIR)/iso >/dev/null
	@echo "Built: $(ISO_OUT)"

# Bootable raw disk image with GRUB installed (requires sudo for loop/mount)
image: $(KERNEL_BIN)
	@./tools/mkimage.sh $(IMG_OUT) $(KERNEL_BIN) grub.cfg

run: $(KERNEL_BIN)
	qemu-system-i386 -kernel $(KERNEL_BIN)

run-iso: iso
	qemu-system-i386 -cdrom $(ISO_OUT)

run-image: image
	qemu-system-i386 -drive format=raw,file=$(IMG_OUT)

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all iso image run run-iso run-image clean
