//
//  devMgr.h
//  DCT
//  implement device search
//  Created by Fengping Yu on 11/16/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//
// For search device, you need first alloc a devMgr, and then call it's method
// For example:
/*
	#include <devMgr/devMgr.h>
	devMgr *dev = [[[devMgr alloc] init] autorelease];
	[dev findModemFromVendorID: vendorID AndInterfaceNumber: interfaceNUmber];
	[dev getModemPath:bsdPath maxSize:sizeof(bsdPath)]; 
 */


#import <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <paths.h>
#include <errno.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOBSD.h>
#include <IOKit/IOMessage.h>
#include <IOKit/storage/IOCDTypes.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOCDMediaBSDClient.h>
#include <IOKit/serial/IOSerialKeys.h>

#define DEVICE_PLUGIN   0
#define DEVICE_REMOVED  1

typedef enum _devType
{
	CDROM_DEVICE_TYPE   = 0,
	MODEM_DEVICE_TYPE   = 1,
	RS232_DEVICE_TYPE,
	//	20120523 by foil
	VOICE_DEVICE_TYPE
}devType;

typedef struct _searchCritical
{
	int productID;
	int vendorID;
	int modemInterfaceNumber;
	int rs232InterfaceNumber;
	int voiceInterfaceNumber;
	int cdromInterfaceNumber;   //fake, for implement convence, set it to -1
}searchCritical;

#ifndef IO_OBJECT_NULL
#define IO_OBJECT_NULL ((io_object_t)0)
#endif

typedef void (*DevChangeCallBack)(char *path, devType dev);


@interface devMgr : NSObject 
{
@private
	
}

// cd-rom relate
kern_return_t findEjectableCDROMDevice(searchCritical critical);
kern_return_t getCDROMPath(char *cdromPath, CFIndex maxPathSize);
BOOL ejectCDROMWithVendorID(int productID, int vendorID);

// modem relate
kern_return_t findModem(searchCritical critical);
kern_return_t getModemPath(char *modemPath, CFIndex maxPathSize);

// RS232 relate
kern_return_t findRS232(searchCritical critical);
kern_return_t getRS232Path(char *rs232Path, CFIndex maxPathSize);
kern_return_t findVoicePort(searchCritical critical);
kern_return_t getVoicePortPath(char *Path, CFIndex maxPathSize);

+(BOOL)ScanAllDevice:(searchCritical*)critical ;

+ (NSArray*)getDeviceAttributes;

//  register device arrive and out notification
BOOL registerCDROMChangeNotificationWithVendorID(searchCritical critical, DevChangeCallBack devChangeFunc);
+(BOOL)registerCDROMChangeNotificationWithCritical:(searchCritical*)critical andCallback: (DevChangeCallBack)devChangeFunc;
+(void)unRegister;
void CDROMDeviceAdded(void *refCon, io_iterator_t iterator);
void CDROMDeviceRemoved(void *refCon, io_iterator_t iterator);
void ModemDeviceAdded(void *refCon, io_iterator_t iterator);
void ModemDeviceRemoved(void *refCon, io_iterator_t iterator);
//void Rs232DeviceAdd(void *refCon, io_iterator_t iterator);
//void Rs232DeviceRemove(void *refCon, io_iterator_t iterator);

// common function
kern_return_t scanDevice(devType type, io_iterator_t *iterator, int productID, int vendorID, int interfaceNumber);
kern_return_t getBSDDevicePath(devType type, io_iterator_t iterator, char*bsdPath, CFIndex maxPathSize);
BOOL isIDMatch(io_iterator_t *iter, UInt32 interfaceValue, UInt32 pidValue, UInt32 vidValue, int *searchCount, devType type);
BOOL isMTKCDROM(char *bsdName, int productID, int vendorID, io_iterator_t *iterator);

@end
