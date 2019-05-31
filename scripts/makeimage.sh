#!/usr/bin/env bash

mkdir -p ./iso/boot/grub

cat <<EOF > ./iso/boot/grub/grub.cfg
set timeout=10
set default=0

menuentry "rose" {
    multiboot /boot/rose
    boot
}
EOF

cp ./img/rose ./iso/boot/
i386-elf-grub-mkrescue -o ./img/rose.iso ./iso

rm -r ./iso
