/*
 *  SCFLib.h
 *  DialupTool
 *
 *  Created by Juihung Weng on 1/12/11.
 *  Copyright 2011 MediaTek Inc. All rights reserved.
 *
 */

#ifndef _SCFLIB_H_
#define _SCFLIB_H_

#include <assert.h>
#include <unistd.h>

#include <CoreFoundation/CoreFoundation.h>
#include <SystemConfiguration/SystemConfiguration.h>

#include "SCFDialupServiceCreator.h"


/////////////////////////////////////////////////////////////////

enum {
	kCallbackParamsMagic = 0x66642666
};

struct CallbackParams {
	int							 magic;	  // kCallbackParamsMagic
	SCNetworkConnectionStatus	   lastMajorStatus;
	SCNetworkConnectionPPPStatus	lastMinorStatus;
};
typedef struct CallbackParams CallbackParams;

/////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////

// Search all Modem service in all Network Service from all Sets of current Preferences
extern SCNetworkConnectionPPPStatus GetMinorStatus(SCNetworkConnectionRef connection);
extern CFDictionaryRef GetPPPoMServiceIDFromSet (SCPreferencesRef PRef);
extern int DialupConnect(SCNetworkConnectionRef connection, CFRunLoopRef runloopref);
extern int DialupDisconnect(SCNetworkConnectionRef connection, CFRunLoopRef runloopref);
	
extern CFStringRef DialupServiceCreate(
										SCPreferencesRef		PRef,
										SCNetworkInterfaceRef	ModemIf,
										CFStringRef	ServiceName,
										CFStringRef Number,
										CFStringRef	APN,
										CFStringRef UserName,
										CFStringRef PassWord
									   );
extern SCNetworkInterfaceRef 
	SCNetworkInterfaceGetModemInterfaceByName(
										CFArrayRef	NIRefAll,
										CFStringRef	GivenName
											  );
/////////////////////////////////////////////////////////////////
	
#ifdef __cplusplus
}
#endif

#endif