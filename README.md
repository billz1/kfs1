# KFS1 (mandatory + bonus)

This is a small 32-bit Multiboot kernel (GRUB) for the KFS1 project.

## Features

### Mandatory (subject)
- Multiboot header (GRUB loads the kernel)
- ASM entry sets up stack and calls `kmain`
- Writes to VGA text buffer
- Displays **"42"**

### Bonus (implemented)
- Scrolling (when you print past 25 lines)
- Hardware cursor updates
- Colors (per-terminal)
- `kprintf()` formatting: `%s %c %d %u %x %X %p %%`
- Keyboard input (polling the controller)
- 3 virtual terminals (switch with **F1 / F2 / F3**)

## Build

Install deps (Ubuntu/Debian):
```bash
sudo apt update
sudo apt install nasm build-essential grub-pc-bin xorriso qemu-system-x86
```

Build + run directly:
```bash
make
make run
```

## Boot via GRUB ISO (no sudo)
```bash
make run-iso
```

## Boot via raw disk image (GRUB installed) (requires sudo)
This creates `build/kfs1.img` (10MB) with an MBR partition and GRUB installed into the image.

```bash
make run-image
```

## Notes
- If your host `gcc` cannot build 32-bit (`-m32`), install multilib:
  ```bash
  sudo apt install gcc-multilib
  ```
  or use a cross-compiler `i686-elf-gcc`.
