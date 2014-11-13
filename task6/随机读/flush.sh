#!/bin/bash
#time dd if=/dev/zero of=/home/o_file bs=8k count=300000
echo 3 > /proc/sys/vm/drop_caches
sync
free
