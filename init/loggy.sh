#!/system/bin/sh
date=`date +%F_%H-%M-%S`
rm /cache/logcat.txt
rm /cache/kmsg.txt
logcat -b all -f  /cache/logcat.txt &
dmesg -w > /cache/kmsg.txt
