#!/bin/sh

killall -9 bbsd
killall -9 chatd
killall -9 bbsnet
ipcrm shm `ipcs | cut -f 2 -d " "` > /dev/null 2>&1
