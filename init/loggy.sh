#!/system/bin/sh
date=`date +%F_%H-%M-%S`
logcat -b all -f  /cache/logcat_${_date}.txt &
dmesg -w > /cache/kmsg_${_date}.txt
