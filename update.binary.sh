#!/bin/bash
tar -czvf  uboot$1.tar.gz  ./uboot.img
adb push uboot$1.tar.gz /root/rk3399/

