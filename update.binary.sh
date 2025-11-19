#!/bin/bash
tar -czvf  uboot$1.tar.gz  ./uboot.img
[ -e /root/rk3399 ] && cp uboot$1.tar.gz /root/rk3399/ 
[ -e /root/rk3399 ] || adb push uboot$1.tar.gz /root/rk3399/

