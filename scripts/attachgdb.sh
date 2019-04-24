#!/bin/bash

if [ $(uname -s) = "Darwin" ]; then
    vte=kitty
    gdb=i386-elf-gdb
else
    vte='urxvt -e'
    gdb=gdb
fi

$vte qemu-system-i386 -kernel ./img/rose \
    -m 256 -no-shutdown -no-reboot -s -S -curses &
sleep 2; $gdb ./img/rose -ex 'target remote localhost:1234'
