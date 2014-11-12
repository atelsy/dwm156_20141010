#!/bin/sh

# hey.sh
# 
#
# Created by MBJ on 11-2-18.
# Copyright 2011 MediaTek Inc. All rights reserved.


cd /tmp

sudo mkdir mtkdrvHi
sudo mkdir mtkdrvLo

sudo cp -R ~/documents/Focus/mediatekusbcdc/build/deployment/*.kext /tmp/mtkdrvHi
sudo cp -R ~/Documents/Focus/MediaTekUSBCDC_2.11.47/build/Deployment/*.kext /tmp/mtkdrvLo


open ~/Project/DataCard/Release/MtkDataCardTool.pmdoc