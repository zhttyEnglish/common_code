#!/bin/sh
# 

start_time=$(date +%s)

TRY_CNT=0
COUNT=10
TIME=3

check_alive()
{
  PID=`busybox ps |grep $1 |grep -v grep | wc -l`
  if [ $PID -le 0 ];then
  
     echo "QFacialGate app die , restart now"
     cd /root/meg_lg_facepass_demo
     chmod +x QFacialGate 
     ./QFacialGate &
     
     echo V > /dev/watchdog
     
     let TRY_CNT=TRY_CNT+1
     echo "TRY_CNT=TRY_CNT+1"
     if [ "$TRY_CNT" -gt "$COUNT" ]; then
       reboot
     else
       echo "TRY_CNT = $TRY_CNT"
     fi
     
  else
     echo "QFacialGate is alive"
     let TRY_CNT=0
     echo A > /dev/watchdog
     
  fi
}

while true 
do

  check_alive QFacialGate
  echo "check_alive QFacialGate"
  sleep $TIME
  
  current_time=$(date +%s)
  time_diff=$((current_time - start_time))
  if [ $time_diff -ge 3600];then
     ntpdate
     let start_time=$(date +%s)
  fi
  
done


