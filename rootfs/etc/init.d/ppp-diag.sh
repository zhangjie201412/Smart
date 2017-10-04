#! /bin/sh

WR="/etc/ppp/script"
LOG="/home/pppd.log"

${WR}/ppp-on > ${LOG} &
sleep 12

INTERFACENAME="Using interface"
ADDRESSIPNAME="local  IP address"
GATEWAYNAME="remote IP address "

INTERFACE=`cat /home/pppd.log | grep "$INTERFACENAME" | cut -d " " -f3`
ADDRESSIP=`cat /home/pppd.log | grep "$ADDRESSIPNAME" | cut -d " " -f5`
GATEWAY=`cat /home/pppd.log | grep "$GATEWAYNAME" | cut -d " " -f4`

/usr/sbin/iptables -t nat -A POSTROUTING -o ppp0 -j MASQUERADE

first=1

while true
do
	ping -c 1 www.baidu.com -I ppp0 > /dev/null 2>&1
	if [ $? -eq 0 ]
	then
		echo "${INTERFACE}, ${ADDRESSIP}, ${GATEWAY}" > /home/netinfo.txt
		echo 1 > /sys/devices/platform/gprs_power.0/led_ppp
		echo 1 > /home/pppd_status.txt
		if [ $first -eq 1 ];then
			echo "1" > /proc/sys/net/ipv4/ip_forward
			source /etc/ifconfig-eth0
			echo "update ota"
			/home/update.sh
		fi
		first=0
		sleep 60
	else
		echo 0 > /sys/devices/platform/gprs_power.0/led_ppp
		echo 0 > /home/pppd_status.txt
		sleep 10
		killall pppd
		echo 0 > /sys/devices/platform/gprs_power.0/gprs_power
		sleep 8
		echo 1 > /sys/devices/platform/gprs_power.0/gprs_power
		sleep 10
		${WR}/ppp-on > ${LOG} &
		INTERFACE=`cat /home/pppd.log | grep "$INTERFACENAME" | cut -d " " -f3`
		ADDRESSIP=`cat /home/pppd.log | grep "$ADDRESSIPNAME" | cut -d " " -f5`
		GATEWAY=`cat /home/pppd.log | grep "$GATEWAYNAME" | cut -d " " -f4`
		sleep 12
	fi
done
