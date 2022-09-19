#!/bin/bash

"${HOME}"/.arduino15/packages/esp8266/tools/mklittlefs/3.0.4-gcc10.3-1757bed/mklittlefs \
    --create 'wwswitch/data' \
    --debug 5 \
    --block 4096 \
    --page 256 \
    --size 1024000 \
    'littlefs.bin'
