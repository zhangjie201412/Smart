#!/bin/bash

echo "build openssl..."
cd /home/jay/Projects/Smart/libs/openssl-1.0.2l
make clean
./config shared --prefix=/home/jay/Projects/Smart/libs/target/x86/
make
make install
cd /home/jay/Projects/Smart/libs/

echo "build c-ares..."
cd /home/jay/Projects/Smart/libs/c-ares-1.12.0
make clean
./configure --prefix=/home/jay/Projects/Smart/libs/target/x86/
make
make install
cd /home/jay/Projects/Smart/libs/

echo "build uuid..."
cd /home/jay/Projects/Smart/libs/libuuid-1.0.3
make clean
./configure --prefix=/home/jay/Projects/Smart/libs/target/x86/
make
make install
cd /home/jay/Projects/Smart/libs/

echo "build mqtt..."
cd /home/jay/Projects/Smart/libs/mosquitto-1.4.14
make clean
make CFLAGS="-I/home/jay/Projects/Smart/libs/target/x86/include" LDFLAGS="-L/home/jay/Projects/Smart/libs/target/x86/lib -lssl -lcrypto -lcares -luuid" prefix=/home/jay/Projects/Smart/libs/target/x86
make install
cd /home/jay/Projects/Smart/libs/
