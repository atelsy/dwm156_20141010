/*
 *  SCFDialupServiceCreator.h
 *  DialupTool
 *
 *  Created by Juihung Weng on 1/20/11.
 *  Copyright 2011 MediaTek Inc. All rights reserved.
 *
 */

#include <assert.h>
#include <unistd.h>

#include <CoreFoundation/CoreFoundation.h>
#include <SystemConfiguration/SystemConfiguration.h>


// Magic values used to bracket the process ID returned by the install tool.

#define kBASAntiZombiePIDToken1 "cricket<"
#define kBASAntiZombiePIDToken2 ">bat"

// Magic value used to indicate success or failure from the install tool.

#define kBASInstallToolSuccess "oK"
#define kBASInstallToolFailure "FailUrE %d"

// Executable File Name

#define DIALUP_SERVICE_CREATOR_NAME	"DialupServiceCreator"