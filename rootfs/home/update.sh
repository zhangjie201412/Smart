#! /bin/sh

version_file="/home/ota.version"
cd /home

if [ ! -f "$version_file" ];then
	ftpget -u jayzhang -p zhangjie -P 801 139.224.227.174 ota.version ota.version
	ftpget -u jayzhang -p zhangjie -P 801 139.224.227.174 ota.tar.gz ota.tar.gz
	tar xzvf ota.tar.gz
	cp -ra ./ota/* /
	rm -rf ota ota.tar.gz
	reboot
else
	echo "ota version file exist!"
	ftpget -u jayzhang -p zhangjie -P 801 139.224.227.174 ota.version.new ota.version
	diff ota.version ota.version.new
	if [ $? -eq 1 ]; then
		ftpget -u jayzhang -p zhangjie -P 801 139.224.227.174 ota.tar.gz ota.tar.gz
        	tar xzvf ota.tar.gz
        	cp -ra ./ota/* /
        	rm -rf ota ota.tar.gz
		mv ota.version.new ota.version
		reboot
	else
		echo "already update to latest!"
	fi
fi


#ftpget -u jayzhang -p zhangjie -P 801 139.224.227.174 ota.tar.gz ota.tar.gz
