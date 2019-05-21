#!/bin/sh

mkimage -n "linux-waming" -A arm -O linux -T kernel -C none -a 0x48000000 -e 0x48000000 -d arch/arm/boot/zImage arch/arm/boot/uImage
