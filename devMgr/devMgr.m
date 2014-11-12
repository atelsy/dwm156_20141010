//
//  devMgr.m
//  DCT
//
//  Created by Fengping Yu on 11/16/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "devMgr.h"

#import <IOKit/usb/IOUSBLib.h>
#import <IOKit/IOCFPlugIn.h>

#pragma mark--
#pragma mark constant define
#pragma mark--

#define kIOProductIDKey		 "idProduct"
#define kIOVendorIDKey		  "idVendor"
#define kIOInterfaceNumberKey   "bInterfaceNumber"


io_iterator_t cdromIterator;
io_iterator_t modemIterator;
io_iterator_t rs232Iterator;
//	20120523 by foil
io_iterator_t voiceIterator;

// notification use
IONotificationPortRef notifyPort;
io_iterator_t cdDeviceAddedIter;
io_iterator_t cdDeviceRemovedIter;
io_iterator_t modemDeviceAddedIter;
io_iterator_t modemDeviceRemovedIter;
io_iterator_t rs232DeviceAddedIter;
io_iterator_t rs232DeviceRemovedIter;

// device information
searchCritical gSearchCritical;

DevChangeCallBack dev_cb;

@implementation devMgr

#pragma mark--
#pragma mark construct and destruct function
#pragma mark--

- (id)init
{
	self = [super init];
	if (self) 
	{
		// Initialization code here.
		cdromIterator = IO_OBJECT_NULL;
		modemIterator = IO_OBJECT_NULL;
		rs232Iterator = IO_OBJECT_NULL;
	}
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

#pragma mark - test

+ (NSArray*)getDeviceAttributes
{
	mach_port_t masterPort;
	CFMutableDictionaryRef matchingDict;
	
	NSMutableArray *deviceAttributes = [NSMutableArray array];
	
	kern_return_t kr;
	
	kr = IOMasterPort(MACH_PORT_NULL, &masterPort);
	
	if(kr || !masterPort)
	{
		NSLog(@"Error:Couldn't create a master I/O kit port (%08x)", kr);
		return deviceAttributes;
	}
	
	matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
	
	if (!matchingDict)
    {
        NSLog (@"Error: Couldn't create a USB matching dictionary");
		
        mach_port_deallocate(mach_task_self(), masterPort);
		
        return deviceAttributes;
    }
	
	io_iterator_t iterator;
	
	IOServiceGetMatchingServices (kIOMasterPortDefault, matchingDict, &iterator);
	
	io_service_t usbDevice;
	
	//Iterate for USB devices
	
    while ((usbDevice = IOIteratorNext (iterator)))		
    {
		if(ejectCDROMWithVendorID(gSearchCritical.productID, gSearchCritical.vendorID) == YES)
		{
			IOObjectRelease(usbDevice);
			
			break;
		}
		

		
        IOObjectRelease(usbDevice);
				
    }
	
	
    //Finished with master port
	
    mach_port_deallocate(mach_task_self(), masterPort);
	
    masterPort = 0;
	
	return deviceAttributes;	
}


#pragma mark--
#pragma mark find cdrom and eject 
#pragma mark--

kern_return_t findEjectableCDROMDevice(searchCritical critical)
{
	return scanDevice(CDROM_DEVICE_TYPE, &cdromIterator, critical.productID, critical.vendorID, critical.cdromInterfaceNumber);
}

kern_return_t getCDROMPath(char *cdromPath, CFIndex maxPathSize)
{
	return getBSDDevicePath(CDROM_DEVICE_TYPE, cdromIterator, cdromPath, maxPathSize);
}
BOOL ejectCDROMWithVendorID(int productID, int vendorID)
{
	OSStatus result = noErr;
	ItemCount volumeIndex;
		
	for(volumeIndex = 1; result == noErr || result != nsvErr; volumeIndex++)
	{
		FSVolumeRefNum actualVolume;
		HFSUniStr255 volumeName;
		FSVolumeInfo volumeInfo;
		pid_t dissenter;
		cdromIterator = IO_OBJECT_NULL;

		bzero((void*)&volumeInfo, sizeof(volumeInfo));
		
		result = FSGetVolumeInfo(kFSInvalidVolumeRefNum, volumeIndex, &actualVolume, kFSVolInfoFSInfo, &volumeInfo, &volumeName, NULL);
		
		NSLog(@"volumeindex:%lu, result:%d\n", volumeIndex, result);
		
		if(result == -35)
		{
			NSLog(@"volume not found error");
			IOIteratorReset(cdDeviceAddedIter);
			CDROMDeviceAdded(NULL, cdDeviceAddedIter);   //iterate again because some time the device is still not be ok.
		}
		
		if(result == noErr)
		{
			GetVolParmsInfoBuffer volumeParms;
			FSGetVolumeParms(actualVolume, &volumeParms, sizeof(volumeParms));
			
			NSLog(@"devid:%s, vid:%d\n", (char*)volumeParms.vMDeviceID, vendorID);
			
			if(isMTKCDROM((char*)volumeParms.vMDeviceID, productID, vendorID, &cdromIterator))
			{
				if((volumeParms.vMExtendedAttributes & (1 << bSupportsSymbolicLinks)) == 0)
				{
					FSEjectVolumeSync(actualVolume, kNilOptions, &dissenter);
					NSLog(@"Eject cdrom sucessful.\n");
					return YES;
				}
				else
				{
					NSLog(@"is U disk.\n");
				}
			}
			else
			{
				NSLog(@"Not mediatek device.\n");
			}
		}
		else
		{
			NSLog(@"FSGetVolumeInfo error = %d.\n", result);
			
			return NO;
		}
	}
}

#pragma mark--
#pragma mark find modem and rs232 device
#pragma mark--

kern_return_t findModem(searchCritical critical)
{
	return scanDevice(MODEM_DEVICE_TYPE, &modemIterator, critical.productID, critical.vendorID, critical.modemInterfaceNumber);
}

+(BOOL)ScanAllDevice:(searchCritical*)critical
{
	char modemPath[256], rs232Path[256];
	
	getModemPath(modemPath, 256);
	printf("modem path is %s.\n", modemPath);
	dev_cb(modemPath, MODEM_DEVICE_TYPE);
	
	/* kernResult = */findRS232(gSearchCritical);
	getRS232Path(rs232Path, 256);
	dev_cb(rs232Path, RS232_DEVICE_TYPE);
	
	//	20120523 by foil
	findVoicePort(gSearchCritical);
	getVoicePortPath(rs232Path, 256);
	dev_cb(rs232Path, VOICE_DEVICE_TYPE);
	
	return TRUE;
}

kern_return_t getModemPath(char *modemPath, CFIndex maxPathSize)
{
	return getBSDDevicePath(MODEM_DEVICE_TYPE, modemIterator, modemPath, maxPathSize);
}

kern_return_t findRS232(searchCritical critical)
{
	return scanDevice(RS232_DEVICE_TYPE, &rs232Iterator, critical.productID, critical.vendorID, critical.rs232InterfaceNumber);
}

kern_return_t getRS232Path(char *rs232Path, CFIndex maxPathSize)
{
	return getBSDDevicePath(RS232_DEVICE_TYPE, rs232Iterator, rs232Path, maxPathSize);
}

//	20120523 by foil
kern_return_t findVoicePort(searchCritical critical)
{
	return scanDevice(RS232_DEVICE_TYPE, &voiceIterator, critical.productID, critical.vendorID, critical.voiceInterfaceNumber);
}

kern_return_t getVoicePortPath(char *Path, CFIndex maxPathSize)
{
	return getBSDDevicePath(RS232_DEVICE_TYPE, voiceIterator, Path, maxPathSize);
}
//
#pragma mark--
#pragma mark register and unregister device plugin-out notification
#pragma mark--

+(void)unRegister
{
	IONotificationPortDestroy(notifyPort);
	
	if(cdDeviceAddedIter)
	{
		IOObjectRelease(cdDeviceAddedIter);
		cdDeviceAddedIter = 0;
	}
	
	if(cdDeviceRemovedIter)
	{
		IOObjectRelease(cdDeviceRemovedIter);
		cdDeviceRemovedIter = 0;
	}
	
	if(modemDeviceAddedIter)
	{
		IOObjectRelease(modemDeviceAddedIter);
		modemDeviceAddedIter = 0;
	}
	
	if(modemDeviceRemovedIter)
	{
		IOObjectRelease(modemDeviceRemovedIter);
		modemDeviceRemovedIter = 0;
	}
}

+(BOOL)registerCDROMChangeNotificationWithCritical:(searchCritical *)critical andCallback:(DevChangeCallBack)devChangeFunc
{
	return registerCDROMChangeNotificationWithVendorID(*critical, devChangeFunc);
}

BOOL registerCDROMChangeNotificationWithVendorID(searchCritical critical, DevChangeCallBack devChangeFunc)
{
	CFMutableDictionaryRef matchingDict, modemMatchingDict;  
	CFRunLoopSourceRef runLoopSource;
	CFNumberRef number, pidNumber;
	
	gSearchCritical = critical;
	dev_cb = devChangeFunc;
	
	// interested in instances of cd medial class
	matchingDict = IOServiceMatching(kIOCDMediaClass);
	if(!matchingDict)
	{
		return FALSE;
	}
	
	pidNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &critical.productID);
	number = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &critical.vendorID);
	CFDictionarySetValue(matchingDict, CFSTR(kIOMediaEjectableKey), kCFBooleanTrue);
	CFDictionarySetValue(matchingDict, CFSTR(kIOVendorIDKey), number);
	CFDictionarySetValue(matchingDict, CFSTR(kIOProductIDKey), pidNumber);
	
	//create modem device matching dictionary
	modemMatchingDict = IOServiceMatching(kIOSerialBSDServiceValue);
	if(!modemMatchingDict)
	{
		CFRelease(number);
		CFRelease(matchingDict);
		return FALSE;
	}
	
	CFDictionarySetValue(modemMatchingDict, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDModemType));
	CFDictionarySetValue(modemMatchingDict, CFSTR(kIOVendorIDKey), number);
	
	CFRelease(number);
	
	// Create a notification port and add its run loop event source to our run loop
	notifyPort = IONotificationPortCreate(kIOMasterPortDefault);
	runLoopSource = IONotificationPortGetRunLoopSource(notifyPort);
	
	CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
	
	// retain additional reference because we will use this same dictionary with IOServiceAddMatchingNotification, which will consume one reference
	matchingDict = (CFMutableDictionaryRef)CFRetain(matchingDict);
	modemMatchingDict = (CFMutableDictionaryRef)CFRetain(modemMatchingDict);
	
	// set up notification, one for device add and the other for device removed
	IOServiceAddMatchingNotification(notifyPort, kIOMatchedNotification, matchingDict, CDROMDeviceAdded, NULL, &cdDeviceAddedIter);
	
	CDROMDeviceAdded(NULL, cdDeviceAddedIter);  // iterator onece to arm notification
	
	IOServiceAddMatchingNotification(notifyPort, kIOTerminatedNotification, matchingDict, CDROMDeviceRemoved, NULL, &cdDeviceRemovedIter);
	
	CDROMDeviceRemoved(NULL, cdDeviceRemovedIter);
	
	IOServiceAddMatchingNotification(notifyPort, kIOFirstMatchNotification, modemMatchingDict, ModemDeviceAdded, NULL, &modemDeviceAddedIter);
	
	ModemDeviceAdded(NULL, modemDeviceAddedIter);
	
	IOServiceAddMatchingNotification(notifyPort, kIOTerminatedNotification, modemMatchingDict, ModemDeviceRemoved, NULL, &modemDeviceRemovedIter);
	
	ModemDeviceRemoved(NULL, modemDeviceRemovedIter);
		
	return TRUE;
}

#pragma mark--
#pragma mark device callback function
#pragma mark--

void CDROMDeviceAdded(void *refCon, io_iterator_t iterator)
{
	io_service_t device;
	sleep(5);
	
	while((device = IOIteratorNext(iterator)))
	{
		ejectCDROMWithVendorID(gSearchCritical.productID, gSearchCritical.vendorID);
		printf("CD device added.\n");
	    IOObjectRelease(device);
	}
}

void CDROMDeviceRemoved(void *refCon, io_iterator_t iterator)
{
	io_service_t device;
	
	while((device = IOIteratorNext(iterator)))
	{
		printf("CD device removed.\n");
	    IOObjectRelease(device);
	}
}

void ModemDeviceAdded(void *refCon, io_iterator_t iterator)
{
  //  kern_return_t kernResult;
	io_service_t device;
	char modemPath[256], rs232Path[256];
	
	NSLog(@"call modem device add.\n");
	
	while((device = IOIteratorNext(iterator)))
	{
		NSLog(@"find modem...\n");
		findModem(gSearchCritical);
		getModemPath(modemPath, 256);
		NSLog(@"modem path is %s.\n", modemPath);
		dev_cb(modemPath, MODEM_DEVICE_TYPE);
		
	   /* kernResult = */findRS232(gSearchCritical);
		getRS232Path(rs232Path, 256);
		dev_cb(rs232Path, RS232_DEVICE_TYPE);
		
		//	20120523 by foil
		findVoicePort(gSearchCritical);
		getVoicePortPath(rs232Path, 256);
		dev_cb(rs232Path, VOICE_DEVICE_TYPE);
		//
	   IOObjectRelease(device);
	}
}

void ModemDeviceRemoved(void *refCon, io_iterator_t iterator)
{
	io_service_t device;
	
	while((device = IOIteratorNext(iterator)))
	{
		printf("modem device removed.\n");
		
		dev_cb(NULL, MODEM_DEVICE_TYPE);
		
		dev_cb(NULL, RS232_DEVICE_TYPE);
		//	20120523 by foil
		dev_cb(NULL, VOICE_DEVICE_TYPE);
		
	   /* kernResult = */IOObjectRelease(device);
	}
}
/*
void Rs232DeviceAdd(void *refCon, io_iterator_t iterator)
{
	kern_return_t kernResult;
	io_service_t device;
	char rs232Path[256];
	
	while((device = IOIteratorNext(iterator)))
	{
		kernResult = findRS232(gSearchCritical);
		getRS232Path(rs232Path, 256);
		
		dev_cb(rs232Path, RS232_DEVICE_TYPE);
		
		kernResult = IOObjectRelease(device);
	}
}

void Rs232DeviceRemove(void *refCon, io_iterator_t iterator)
{
	kern_return_t kernResult;
	io_service_t device;
	
	while((device = IOIteratorNext(iterator)))
	{
		printf("RS232 com port removed.\n");
		
		dev_cb(NULL, RS232_DEVICE_TYPE);
		
		kernResult = IOObjectRelease(device);
	}
}
*/
#pragma mark--
#pragma mark match device
#pragma mark--

BOOL isIDMatch(io_iterator_t *iter, UInt32 interfaceValue, UInt32 pidValue, UInt32 vidValue, int *searchCount, devType type)
{
	io_iterator_t service;
	kern_return_t kernResult;
	
	while((service = IOIteratorNext(*iter)))
	{
		(*searchCount)++;
		io_registry_entry_t parent;
		
		kernResult = IORegistryEntryGetParentEntry(service, kIOServicePlane, &parent);
		if(kernResult != KERN_SUCCESS)
		{
			return FALSE;
		}
		else
		{
			CFNumberRef vid, interfaceNumber, pid;
			UInt32 vValue, vInterfaceNumber, pValue;
			
			vid = IORegistryEntryCreateCFProperty(parent, CFSTR(kIOVendorIDKey), kCFAllocatorDefault, 0);
			
			while(vid == 0)
			{
				service = parent;
				kernResult = IORegistryEntryGetParentEntry(service, kIOServicePlane, &parent);
				
				if(kernResult != KERN_SUCCESS)
				{
					break;
				}
				
				vid = IORegistryEntryCreateCFProperty(parent, CFSTR(kIOVendorIDKey), kCFAllocatorDefault, 0);
			}
			
			if(vid != 0)
			{
				CFNumberGetValue(vid, kCFNumberSInt32Type, &vValue);
				CFRelease(vid);
				
				service = parent;
			    IORegistryEntryGetParentEntry(service, kIOServicePlane, &parent);
			   
				interfaceNumber = IORegistryEntryCreateCFProperty(parent, CFSTR(kIOInterfaceNumberKey), kCFAllocatorDefault, 0);
				
				if(interfaceNumber != 0)
				{
					CFNumberGetValue(interfaceNumber, kCFNumberSInt32Type, &vInterfaceNumber);
					CFRelease(interfaceNumber);
				}
				else
				{
					// Note: for cdrom, there is no interface exit, so we set it to -1
					vInterfaceNumber = -1;
				}
				
				if(type == CDROM_DEVICE_TYPE)
				{
					pid = IORegistryEntryCreateCFProperty(parent, CFSTR(kIOProductIDKey), kCFAllocatorDefault, 0);
					
					if(pid != 0)
					{
						CFNumberGetValue(pid, kCFNumberSInt32Type, &pValue);
						CFRelease(pid);
					}
				}
				
				
				if(vValue == vidValue && vInterfaceNumber == interfaceValue)
				{
					if(type == CDROM_DEVICE_TYPE)
					{
						if(pValue == pidValue)
							return TRUE;
						else
							return FALSE;
						
						NSLog(@"device product id = %d, vendor id = %d.\n", pValue, vValue);
					}
					
					return TRUE;
				}
				else
				{
					continue;
				}
			}
		}
	}
	
   return FALSE;
}

BOOL isMTKCDROM(char *bsdName, int productID, int vendorID, io_iterator_t *iterator)
{
	CFMutableDictionaryRef matchingDict;
	kern_return_t kernResult;
	
	matchingDict = IOBSDNameMatching(kIOMasterPortDefault, 0, bsdName);
	
	NSLog(@"matching name error, name = %s, not mediatek device.\n", bsdName);
	
	if(NULL == matchingDict)
	{
		NSLog(@"matching name error, name = %s, not mediatek device.\n", bsdName);
		return FALSE;
	}
	else
	{
		CFDictionarySetValue(matchingDict, CFSTR(kIOMediaEjectableKey), kCFBooleanTrue);
	}
	
	kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, iterator);
	
	if(kernResult != KERN_SUCCESS)
	{
		NSLog(@"matching service error, not mediatek device.\n");
		return FALSE;
	}
	
	int count = 0;
	
	if(isIDMatch(iterator, -1, productID, vendorID, &count, CDROM_DEVICE_TYPE))
	{
		IOIteratorReset(*iterator);
		
		while(count - 1 > 0)
		{
			IOIteratorNext(*iterator);
			count--;
		}

		return TRUE;
	}
	
	return FALSE;
}

kern_return_t scanDevice(devType type, io_iterator_t *iterator, int productID, int vendorID, int intervaceNumber)
{
	kern_return_t kernResult;
	CFMutableDictionaryRef classesToMatch;
	
	switch (type)
	{
		case CDROM_DEVICE_TYPE:
			classesToMatch = IOServiceMatching(kIOCDMediaClass);
			break;
			
		case MODEM_DEVICE_TYPE:
			classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
			break;
			
		case RS232_DEVICE_TYPE:
			classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
			break;
			
		default:
			break;
	}
	
	if(classesToMatch == NULL)
	{
		return 1;
	}
	else
	{
		switch (type)
		{
			case CDROM_DEVICE_TYPE:
				CFDictionarySetValue(classesToMatch, CFSTR(kIOMediaEjectableKey), kCFBooleanTrue);
				break;
				
			case MODEM_DEVICE_TYPE:
				CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDModemType));
				break;
				
			case RS232_DEVICE_TYPE:
				CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDModemType));
				break;
				
			default:
				break;
		}
	}
	
	kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, iterator);
	
	if(kernResult != KERN_SUCCESS)
	{
		return kernResult;
	}
	
	int count = 0;
	
	if(isIDMatch(iterator, intervaceNumber, productID, vendorID, &count, type))
	{
		IOIteratorReset(*iterator);
		
		// this is due to in the isIDMatch function, the iterator will move to the found item next position, so we need reset it and move it to the right position
		while(count - 1 > 0)
		{
			IOIteratorNext(*iterator);
			count--;
		}
		
		return KERN_SUCCESS;
	}
	
	return 1;
}

kern_return_t getBSDDevicePath(devType type, io_iterator_t iterator, char *bsdPath, CFIndex maxPathSize)
{
	io_object_t next;
	kern_return_t kernResult = KERN_SUCCESS;
	
	*bsdPath = '\0';
	
	next = IOIteratorNext(iterator);
   
	if(next)
	{
		CFTypeRef bsdPathAsCFString;
		
		switch (type)
		{
			case CDROM_DEVICE_TYPE:
				bsdPathAsCFString = IORegistryEntryCreateCFProperty(next, CFSTR(kIOBSDNameKey), kCFAllocatorDefault, 0);
				break;
				
			case MODEM_DEVICE_TYPE:
				bsdPathAsCFString = IORegistryEntryCreateCFProperty(next, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0);
				break;
				
			case RS232_DEVICE_TYPE:
				bsdPathAsCFString = IORegistryEntryCreateCFProperty(next, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0);
				break;
				
			default:
				break;
		}
		
		if(bsdPathAsCFString)
		{
			if(type == CDROM_DEVICE_TYPE)
			{
				strlcpy(bsdPath, _PATH_DEV, maxPathSize);
				strlcat(bsdPath, "r", maxPathSize);
			}
			
			if(CFStringGetCString(bsdPathAsCFString, bsdPath + strlen(bsdPath), maxPathSize - strlen(bsdPath), kCFStringEncodingASCII))
			{
				kernResult = KERN_SUCCESS;
				NSLog(@"Device file path is %s", bsdPath);
			}
			
			CFRelease(bsdPathAsCFString);
		}
		
		IOObjectRelease(next);
	}
	
	return kernResult;
}

@end
