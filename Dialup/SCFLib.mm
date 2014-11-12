/*
 *  SCFLib.c
 *  DialupTool
 *
 *  Created by Juihung Weng on 1/12/11.
 *  Copyright 2011 MediaTek Inc. All rights reserved.
 *
 */
#include "Win2Mac.h"
#include "SCFLib.h"
#include <Security/Authorization.h>
//#include "/System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Headers/MacErrors.h"

CFDictionaryRef GetPPPoMServiceIDArray (CFArrayRef NSRefAll)
{
	SCNetworkServiceRef		NSRef;
	SCNetworkInterfaceRef	NIRef, NIRefNext;
	
	NSRef		= NULL;
	NIRef		= NULL;
	NIRefNext	= NULL;
		
	if(NSRefAll == NULL) return NULL;

	CFMutableDictionaryRef thisDictMutable = NULL;
	thisDictMutable = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	if (thisDictMutable == NULL) {
		return NULL;
	}
	
	CFIndex i;
	CFIndex countNS = CFArrayGetCount(NSRefAll);
	
	for(i=0; i<countNS; i++)
	{
		// get each Network Service
		NSRef = (SCNetworkServiceRef) CFArrayGetValueAtIndex(NSRefAll, i);		
		CFStringRef MyNSname = SCNetworkServiceGetName(NSRef);		//debug
		
		// get corresponding Network Interface
		NIRef = SCNetworkServiceGetInterface(NSRef);
		
//		CFStringRef MyNIbname = SCNetworkInterfaceGetBSDName(NIRef);				//debug
//		CFStringRef MyNIlname = SCNetworkInterfaceGetLocalizedDisplayName(NIRef);	//debug
		
		// if interface type = PPP
		if(NIRef && SCNetworkInterfaceGetInterfaceType(NIRef) == kSCNetworkInterfaceTypePPP) {
			// get next layer interface of this interface
			NIRefNext = SCNetworkInterfaceGetInterface(NIRef);
			
			if(NIRefNext && SCNetworkInterfaceGetInterfaceType(NIRefNext) == kSCNetworkInterfaceTypeModem) {
				// PPP over Modem (Dailup) found!!
				CFStringRef ServiceId = SCNetworkServiceGetServiceID(NSRef);					
				CFDictionaryAddValue(thisDictMutable, MyNSname, ServiceId);	// key (MyNSname), value (ServiceId);
			}
		}
	}
	
	return thisDictMutable;
}

// Search all Modem service in all Network Service from all Sets of current Preferences
CFDictionaryRef GetPPPoMServiceIDFromSet (SCPreferencesRef	PRef)
{
	CFDictionaryRef thisDict = NULL;
	
	if(PRef == NULL) return NULL;
/***************** 20131115 by zhuwei *****************************************/
    SCNetworkSetRef SetRef = SCNetworkSetCopyCurrent(PRef);
    CFArrayRef NSRefAll = SCNetworkSetCopyServices(SetRef);
    thisDict = GetPPPoMServiceIDArray(NSRefAll);
    CFRelease(NSRefAll);
    
    /*
	// get all Sets of current preference
	CFArrayRef	SetRefAll = SCNetworkSetCopyAll(PRef);
	if(SetRefAll == NULL) return NULL;
	
	CFIndex i;
	CFIndex countSet = CFArrayGetCount(SetRefAll);
   
	for(i=0; i<countSet; i++)
	{
		SCNetworkSetRef SetRef = (SCNetworkSetRef) CFArrayGetValueAtIndex(SetRefAll, i);
//		CFStringRef MySetname = SCNetworkSetGetName(SetRef);	//debug
        
		// get all Network Services of current Set
		CFArrayRef NSRefAll = SCNetworkSetCopyServices(SetRef);
		if(NSRefAll == NULL) continue;
		
		thisDict = GetPPPoMServiceIDArray(NSRefAll);
		CFRelease(NSRefAll);
		
		if(thisDict != NULL) break;	//found!!
	}
	
	CFRelease(SetRefAll);
    */
/******************************************************************************/
	return thisDict;
}

SCNetworkConnectionPPPStatus GetMinorStatus(SCNetworkConnectionRef connection)
{
	SCNetworkConnectionPPPStatus	result;
	CFDictionaryRef				 statusDict;
	CFDictionaryRef				 pppDict;
	CFNumberRef					 minorStatusNum;
	
	result = -1;
	
	// Get the extended status dictionary.
	
	statusDict = SCNetworkConnectionCopyExtendedStatus(connection);
	if (statusDict != NULL) 
	{
		// Extract the PPP sub-dictionary.
		pppDict = (CFDictionaryRef)CFDictionaryGetValue(statusDict, kSCEntNetPPP);
		
		if (pppDict != NULL && CFGetTypeID(pppDict) == CFDictionaryGetTypeID()) 
		{
			// Extract the minor status value.
			minorStatusNum = (CFNumberRef)CFDictionaryGetValue(pppDict, kSCPropNetPPPStatus);
			
			if (minorStatusNum != NULL && CFGetTypeID(minorStatusNum) == CFNumberGetTypeID()) 
			{
				SInt32 tmp;
				if (CFNumberGetValue(minorStatusNum, kCFNumberSInt32Type, &tmp))
					result = tmp;
			}
		}
		CFRelease(statusDict);
	}
	return result;
}

int DialupConnect(SCNetworkConnectionRef connection, CFRunLoopRef runloopref)
{
	int err = kSCStatusOK;
	if (connection == NULL || runloopref == NULL) {
		fprintf(stderr, "DialupConnect: invalid input arguments.\n");
		return kSCStatusInvalidArgument;
	}
	
	// Check the status.  If we're already connected tell the user. 
	// If we're not connected, initiate the connection. 
	bool bRet = false;
	switch (SCNetworkConnectionGetStatus(connection)) {
		case kSCNetworkConnectionDisconnected:
			fprintf(stderr, "DialupConnect: Connecting...\n");
			bRet = SCNetworkConnectionStart(connection, NULL, false);
			if (!bRet) {
				err = SCError();
				fprintf(stderr, "DialupConnect: SCNetworkConnectionStart error: %d\n", err);
				if (connection != NULL)
					(void) SCNetworkConnectionUnscheduleFromRunLoop(connection,
																	runloopref,
																	kCFRunLoopDefaultMode);
			}
			break;
		case kSCNetworkConnectionConnecting:
			fprintf(stderr, "DialupConnect: Service is already connecting.\n");
			err = kSCStatusFailed;
			break;
		case kSCNetworkConnectionDisconnecting:
			fprintf(stderr, "DialupConnect: Service is disconnecting.\n");
			err = kSCStatusFailed;
			break;
		case kSCNetworkConnectionConnected:
			fprintf(stderr, "DialupConnect: Service is already connected.\n");
			err = kSCStatusOK;
			break;
		case kSCNetworkConnectionInvalid:
			fprintf(stderr, "DialupConnect: Service is invalid. Weird.\n");
			err = kSCStatusFailed;
			break;
		default:
			fprintf(stderr, "DialupConnect: Unexpected service status.\n");
			err = kSCStatusFailed;
			break;
	}
	
	return err;
}

int DialupDisconnect(SCNetworkConnectionRef connection, CFRunLoopRef runloopref)
{
	int err = kSCStatusOK;
	if (connection == NULL || runloopref == NULL) {
		fprintf(stderr, "DialupDisconnect: invalid input arguments.\n");
		return kSCStatusInvalidArgument;
	}
	
	bool bRet = false;
	switch (SCNetworkConnectionGetStatus(connection)) {
		case kSCNetworkConnectionDisconnected:
			fprintf(stderr, "DialupDisconnect: Service is already disconnected.\n");
			err = kSCStatusOK;
			break;
		case kSCNetworkConnectionDisconnecting:
			fprintf(stderr, "DialupDisconnect: Service is disconnecting.\n");
			err = kSCStatusFailed;
			break;
		case kSCNetworkConnectionConnected:
		case kSCNetworkConnectionConnecting:
			fprintf(stderr, "DialupDisconnect: Disconnecting...\n");
			bRet = SCNetworkConnectionStop(connection, true);
			if (!bRet) {
				err = SCError();
				fprintf(stderr, "DialupDisconnect: SCNetworkConnectionStop error: %d\n", err);
				if (connection != NULL)
					(void) SCNetworkConnectionUnscheduleFromRunLoop(connection,
																	runloopref,
																	kCFRunLoopDefaultMode);
			}
			break;
		case kSCNetworkConnectionInvalid:
			fprintf(stderr, "DialupDisconnect: Service is invalid. Weird.\n");
			err = kSCStatusFailed;
			break;
		default:
			fprintf(stderr, "DialupDisconnect: Unexpected service status.\n");
			err = kSCStatusFailed;
			break;
	}
	
	return err;
}
/*
static OSStatus GetStringC(CFStringRef CFStr, char* pCStr, size_t CStrSize)
{
	OSStatus	err;
	Boolean	 success;
	
	assert(CFStr != NULL);
	assert(pCStr != NULL);
	assert(CStrSize > 0);
	
	err = noErr;
	success = CFStringGetFileSystemRepresentation(CFStr, pCStr, CStrSize);
	if (!success)
		err = coreFoundationUnknownErr;
	
	return err;
}

static OSStatus GetToolPath(CFStringRef bundleID, CFStringRef toolName, char *toolPath, size_t toolPathSize)
// Given a bundle identifier and the name of a tool embedded within that bundle, 
// get a file system path to the tool.
{
	OSStatus	err;
	CFBundleRef bundle;
	Boolean	 success;
	CFURLRef	toolURL;
	
	assert(bundleID != NULL);
	assert(toolName != NULL);
	assert(toolPath != NULL);
	assert(toolPathSize > 0);
	
	toolURL = NULL;
	
	err = noErr;
	bundle = CFBundleGetBundleWithIdentifier(bundleID);
	if (bundle == NULL)
		err = coreFoundationUnknownErr;

	if (err == noErr) {
		toolURL = CFBundleCopyAuxiliaryExecutableURL(bundle, toolName);
		if (toolURL == NULL)
			err = coreFoundationUnknownErr;
	}
	if (err == noErr) {
		success = CFURLGetFileSystemRepresentation(toolURL, true, (UInt8 *) toolPath, toolPathSize);
		if ( ! success )
			err = coreFoundationUnknownErr;
	}
	
	if (toolURL != NULL)
		CFRelease(toolURL);
	
	return err;
}
*/
// Create a new NetworkServiceID
CFStringRef DialupServiceCreate(
								   SCPreferencesRef		PRef,
								   SCNetworkInterfaceRef	ModemIf,
								   CFStringRef	ServiceName,
								   CFStringRef Number,
								   CFStringRef	APN,
								   CFStringRef UserName,
								   CFStringRef PassWord
								   )
{
	Boolean result = TRUE;
		
	SCNetworkInterfaceRef PPPoMIf	= NULL;
	SCNetworkServiceRef	  PPPoMSer	= NULL;
	SCNetworkSetRef		  SetRef	= NULL;
	
	
//	CFStringRef MyNIbname = NULL;
//	CFStringRef MyNIlname = NULL;
	
	// 1. Create a new PPP interface based on Modem interface
	PPPoMIf = SCNetworkInterfaceCreateWithInterface(ModemIf, kSCNetworkInterfaceTypePPP);
	

	// 2. Create a new Service based on PPPoM interface in current Preference
	PPPoMSer = SCNetworkServiceCreate(PRef, PPPoMIf);
	if(PPPoMSer == NULL) return NULL;
	
	//	CFStringRef	ServiceName = SCNetworkServiceGetName(PPPoMSer);
	//	ServiceName = CFSTR("MTK-Modem");
	result = SCNetworkServiceSetName(PPPoMSer, ServiceName);	// give a Service Name!!
	
	
	// 3. Set options to new PPP interface
	
	// re-cap the PPPoM Interface
	PPPoMIf = SCNetworkServiceGetInterface(PPPoMSer);	
	
	// get default Options of PPP interface
	CFDictionaryRef oldpppOptions = SCNetworkInterfaceGetConfiguration(PPPoMIf);
	CFIndex iPPP = CFDictionaryGetCount(oldpppOptions);	

	// add new Options to PPP interface
	CFMutableDictionaryRef pppOptions = CFDictionaryCreateMutableCopy(NULL, iPPP+2, oldpppOptions);
	CFDictionaryAddValue(pppOptions, kSCPropNetPPPCommRemoteAddress, Number);
	//	20111211 by foil - add username & password
	CFDictionaryAddValue(pppOptions, kSCPropNetPPPAuthName, UserName);
	CFDictionaryAddValue(pppOptions, kSCPropNetPPPAuthPassword, PassWord);
	//	
	// set new Optiont to PPP interface
	result = SCNetworkInterfaceSetConfiguration(PPPoMIf, pppOptions);
	if(!result) fprintf(stderr, " [PPP Dialup] -- SCNetworkInterfaceSetConfiguration() failed!\nerror string:%s\n", SCErrorString(SCError()));


	// 4. Set options to Modem interface
			
	// re-cap Modem interface
	SCNetworkInterfaceRef newModemIf = NULL;
	newModemIf = SCNetworkInterfaceGetInterface(PPPoMIf);
	
	// get default Options of Modem interface
	CFDictionaryRef oldModemOptions = SCNetworkInterfaceGetConfiguration(newModemIf);
	CFIndex iModem = CFDictionaryGetCount(oldModemOptions);	
	
	// reset new Options to Modem interface
	CFMutableDictionaryRef ModemOptions = CFDictionaryCreateMutableCopy(NULL, iModem+2, oldModemOptions);
	CFDictionarySetValue(ModemOptions, kSCPropNetModemConnectionScript, CFSTR("/Library/Modem Scripts/Generic GPRS.ccl"));			// supported from 10.1
	CFDictionarySetValue(ModemOptions, kSCPropNetModemDeviceVendor, CFSTR("Generic"));		// supported from 10.5
	CFDictionarySetValue(ModemOptions, kSCPropNetModemDeviceModel, CFSTR("GPRS (GSM/3G)"));	// supported from 10.5
	// add new Options to Modem interface
	CFDictionaryAddValue(ModemOptions, kSCPropNetModemAccessPointName, APN);				// supported from 10.5
	CFDictionaryAddValue(ModemOptions, kSCPropNetModemDeviceContextID, CFSTR("1"));			// supported from 10.5
	// set new Optiont to Modem interface
	result = SCNetworkInterfaceSetConfiguration(newModemIf, ModemOptions);
	if(!result) 
		fprintf(stderr, " [PPP Dialup] -- SCNetworkInterfaceSetConfiguration() failed!\nerror string:%s\n", SCErrorString(SCError()));
		
	
	// 5. Add this Service in current set of current Preference
	result = SCNetworkServiceEstablishDefaultConfiguration(PPPoMSer);
	if(!result) 
		fprintf(stderr, " [PPP Dialup] -- SCNetworkServiceEstablishDefaultConfiguration() failed!\nerror string:%s\n", SCErrorString(SCError()));
		
	if( result  )
	{
		SetRef = SCNetworkSetCopyCurrent(PRef);
			
		if( SetRef )
		{
			result = SCNetworkSetAddService(SetRef, PPPoMSer);
			if(!result) 
				fprintf(stderr, " [PPP Dialup] -- SCNetworkSetAddService() failed!\nerror string:%s\n", SCErrorString(SCError()));
			
			if( result )
			{
				result = SCPreferencesCommitChanges(PRef);
				if(!result) 
					fprintf(stderr, "[PPP Dialup] -- SCPreferencesCommitChanges() failed!\nerror string:%s\n", SCErrorString(SCError()));
				
				result = SCPreferencesApplyChanges(PRef);
				if(!result) 
					fprintf(stderr, "[PPP Dialup] -- SCPreferencesApplyChanges() failed!\nerror string:%s\n", SCErrorString(SCError()));
			
				if(!result)
					return NULL;	// Failed to SCPreferencesCommitChanges or SCPreferencesApplyChanges

				CFStringRef ServiceID = SCNetworkServiceGetServiceID(PPPoMSer);
				return ServiceID;
			}	
		}
		else 
		{
			fprintf(stderr, "[PPP Dialup] -- SCNetworkSetCopyCurrent() failed!\nerror string:%s\n", SCErrorString(SCError()));
		}

	}
			
	return NULL;
}
	
SCNetworkInterfaceRef SCNetworkInterfaceGetModemInterfaceByName(
																CFArrayRef			NIRefAll,
																CFStringRef			GivenName
																)
{
	if(NIRefAll == NULL) return NULL;
	
	CFIndex i;
	CFIndex countNI = CFArrayGetCount(NIRefAll);
		
	for(i=0; i<countNI; i++){
		// get each Network Interface
		SCNetworkInterfaceRef NIRef = (SCNetworkInterfaceRef) CFArrayGetValueAtIndex(NIRefAll, i);
	
		CFStringRef MyNIbname = SCNetworkInterfaceGetBSDName(NIRef);				//debug
		// if interface type = PPP
		if( SCNetworkInterfaceGetInterfaceType(NIRef) == kSCNetworkInterfaceTypeModem){
			// PPP over Modem (Dailup) found!!
			if(CFEqual(MyNIbname, GivenName)){						
				return NIRef;
			}
		}
	}
	
	return NULL;
}
