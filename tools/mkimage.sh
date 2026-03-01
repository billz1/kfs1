#!/usr/bin/env bash
set -euo pipefail

IMG="${1:-build/kfs1.img}"
KERNEL_BIN="${2:-build/kfs1.bin}"
GRUB_CFG="${3:-grub.cfg}"
SIZE_MB="${SIZE_MB:-10}"

if [[ ! -f "$KERNEL_BIN" ]]; then
  echo "Kernel binary not found: $KERNEL_BIN" >&2
  exit 1
fi
if [[ ! -f "$GRUB_CFG" ]]; then
  echo "grub.cfg not found: $GRUB_CFG" >&2
  exit 1
fi

mkdir -p "$(dirname "$IMG")"

echo "[*] Creating ${SIZE_MB}MB raw image: $IMG"
dd if=/dev/zero of="$IMG" bs=1M count="$SIZE_MB" status=none

echo "[*] Partitioning (MBR + single ext2 partition)"
parted -s "$IMG" mklabel msdos
parted -s "$IMG" mkpart primary ext2 1MiB 100%

MNT="$(mktemp -d /tmp/kfs1-mnt.XXXXXX)"
cleanup() {
  set +e
  if mountpoint -q "$MNT"; then sudo umount "$MNT"; fi
  [[ -n "${LOOPDEV:-}" ]] && sudo losetup -d "$LOOPDEV" >/dev/null 2>&1 || true
  rm -rf "$MNT"
}
trap cleanup EXIT

echo "[*] Attaching loop device (requires sudo)"
LOOPDEV="$(sudo losetup --find --show --partscan "$IMG")"

PART="${LOOPDEV}p1"
if [[ ! -b "$PART" ]]; then
  echo "Partition device not found: $PART" >&2
  exit 1
fi

echo "[*] Formatting ext2: $PART"
sudo mkfs.ext2 -q "$PART"

echo "[*] Mounting partition"
sudo mount "$PART" "$MNT"
sudo mkdir -p "$MNT/boot/grub"

echo "[*] Copying kernel + grub.cfg"
sudo cp "$KERNEL_BIN" "$MNT/boot/kfs1.bin"
sudo cp "$GRUB_CFG" "$MNT/boot/grub/grub.cfg"

echo "[*] Installing GRUB (i386-pc BIOS) to image (requires sudo)"
sudo grub-install \
  --target=i386-pc \
  --boot-directory="$MNT/boot" \
  --modules="normal multiboot part_msdos ext2" \
  --no-floppy \
  --force \
  "$LOOPDEV" >/dev/null

echo "[*] Done: $IMG"
echo "    Run: qemu-system-i386 -drive format=raw,file=$IMG"
