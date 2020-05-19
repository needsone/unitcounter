#!/bin/ash

MYIP=`ifconfig  | grep inet | grep -v "127.0.0.1" | cut -d : -f 2 | cut -d ' ' -f 1`
curl http://${MYIP}/arduino/rm/$1
