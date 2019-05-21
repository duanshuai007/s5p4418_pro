#/bin/sh
#ip=$(curl http://shenyangip2.frogshealth.com)
#scp -P 9876 duanshuai@${ip}:/home/duanshuai/4418/new_android/android/result/boot.img ./
USR="duanshuai"
ip="192.168.200.202"
scp duanshuai@${ip}:/home/duanshuai/4418/new_android/android/result/boot.img ./
fastboot flash boot boot.img
