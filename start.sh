#!/bin/bash

# Download https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q2-update/+download/gcc-arm-none-eabi-5_4-2016q2-20160622-linux.tar.bz2 
# Untar the file
# export PATH=$PATH:/path/to/gcc/arm/bin

git checkout skyviper-stable
git submodule update --init -recursive
./waf configure --board=px4-v3
./waf build --target=bin/arducopter
./Tools/scripts/make_abin.sh
ls -l $PWD/arducopter.abin
