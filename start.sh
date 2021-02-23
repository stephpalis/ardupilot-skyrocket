#!/bin/bash

# Initialize git
git checkout skyviper-stable
git submodule update --init --recursive

# Compile the code
docker build -t skyviper .
./docker_run.sh skyviper

ls -l "$PWD"/arducopter.abin