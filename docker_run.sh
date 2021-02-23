#!/bin/sh

if [ -z "$1" ]; then
  echo "Usage: docker_run.sh <img_name> [cmd]"
  exit 1
fi

echo mounting \""$(pwd)"\" to \"/ardupilot\"
docker run --rm -it -v "$(pwd)":/ardupilot "$1":latest "$2"