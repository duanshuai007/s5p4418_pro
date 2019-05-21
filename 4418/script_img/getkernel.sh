#!/bin/sh
scp duanshuai@192.168.200.202:/home/duanshuai/kernel/linux-3.10/arch/arm/boot/uImage ./
fastboot flash boot uImage
