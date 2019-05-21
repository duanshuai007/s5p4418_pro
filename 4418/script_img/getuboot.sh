#/bin/sh
scp duanshuai@192.168.200.202:/home/duanshuai/4418/new_android/android/u-boot/u-boot-iTOP-4418.bin ./
fastboot flash uboot u-boot-iTOP-4418.bin
