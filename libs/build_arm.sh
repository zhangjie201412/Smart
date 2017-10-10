#!/bin/bash

export CC=/usr/local/arm_linux_4.8/bin/arm-linux-gcc
export CXX=/usr/local/arm_linux_4.8/bin/arm-linux-g++

echo "build openssl..."
cd /home/jay/Projects/Smart/libs/openssl-1.0.2l-arm
#./config shared --prefix=/home/jay/Projects/Smart/libs/target/arm/
make
make install
cd /home/jay/Projects/Smart/libs/

echo "build c-ares..."
cd /home/jay/Projects/Smart/libs/c-ares-1.12.0
make clean
./configure --host=arm-linux --prefix=/home/jay/Projects/Smart/libs/target/arm/
make
make install
cd /home/jay/Projects/Smart/libs/

echo "build uuid..."
cd /home/jay/Projects/Smart/libs/libuuid-1.0.3
make clean
./configure --host=arm-linux --prefix=/home/jay/Projects/Smart/libs/target/arm/
make
make install
cd /home/jay/Projects/Smart/libs/

echo "build mqtt..."
cd /home/jay/Projects/Smart/libs/mosquitto-1.4.14-arm
make clean
make CFLAGS="-I/home/jay/Projects/Smart/libs/target/arm/include" LDFLAGS="-L/home/jay/Projects/Smart/libs/target/arm/lib -lssl -lcrypto -lcares -luuid" prefix=/home/jay/Projects/Smart/libs/target/arm
make install
cd /home/jay/Projects/Smart/libs/
mkdir target/arm/sbin
cp mosquitto-1.4.14-arm/src/mosquitto target/arm/sbin/
cp mosquitto-1.4.14-arm/client/mosquitto_pub target/arm/bin/
cp mosquitto-1.4.14-arm/client/mosquitto_sub target/arm/bin/
cp mosquitto-1.4.14-arm/lib/libmosquitto.so.1 target/arm/lib/
cp mosquitto-1.4.14-arm/lib/cpp/libmosquittopp.so.1 target/arm/lib/

