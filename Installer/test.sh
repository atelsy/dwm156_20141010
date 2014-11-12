#!/bin/sh

# hey.sh
# 
#
# Created by MBJ on 11-2-18.
# Copyright 2011 MediaTek Inc. All rights reserved.
  
#get script's path, named p
x='echo $0 | grep "^/"' 
if test "${x}"
then 
   p=`dirname $0`
else
   p=`dirname 'pwd'/$0`
fi
echo $p

#copy driver files to tmp folder
#cd /tmp
sudo mkdir /tmp/mtkdrvHi
sudo mkdir /tmp/mtkdrvLo
sudo cp -R $p/driver/10.4/*.kext /tmp/mtkdrvLo
sudo cp -R $p/driver/10.6/*.kext /tmp/mtkdrvHi
  
open $p/MtkDataCardTool.pmdoc
















