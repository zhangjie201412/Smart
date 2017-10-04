#!/bin/sh
echo "if1.sh"
echo $#
echo $0
echo $1
echo $2
if [ $# -eq 2 ];then
   if [ $1 = "eth0" ];then
   echo -n "ech0  "
   fi
   if [ $2 = "up" ];then
   echo "up"
   exec /etc/network/net_up
   elif [ $2 = "down" ];then
   echo "down"
   exec /etc/network/net_down
   elif [ $2 = "disable" ];then
   echo "disable"
   elif [ $2 = "error" ];then
   echo "error"
   fi
fi
