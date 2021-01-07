#!/bin/bash

kitty --single-instance \
    qemu-system-i386 -drive format=raw,file=./img/rose.iso \
        -m 256 -no-shutdown -no-reboot -s -S -curses &
sleep 2; gdb ./img/rose -ex 'target remote localhost:1234'
