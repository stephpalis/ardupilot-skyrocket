#!/bin/sh

mkdir /opt
cd /opt || exit 1
wget https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q2-update/+download/gcc-arm-none-eabi-5_4-2016q2-20160622-linux.tar.bz2
tar xvjf gcc-arm-none-eabi-5_4-2016q2-20160622-linux.tar.bz2
rm gcc-arm-none-eabi-5_4-2016q2-20160622-linux.tar.bz2