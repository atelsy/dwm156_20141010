#!/bin/sh

# post.sh
# 
#
# Created by MBJ on 11-1-11.
# Copyright 2011 MediaTek Inc. All rights reserved.

sudo touch /system/library/extensions/
sudo killall -HUP kextd
