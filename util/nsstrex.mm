//
//  string.mm
//  DCT
//
//  Created by MBJ on 12-7-7.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "Win2Mac.h"
#import "nsstrex.h"

@implementation NSString(WCharToString)

+ (NSString*)getStringFromWchar:(const wchar_t *)instr
{
	char *str = new char[wcslen(instr) + 1];
	
	wcstombs(str, instr, wcslen(instr) + 1);
	
	NSString *wstr = [NSString stringWithCString:str
										encoding:NSUTF8StringEncoding];
	
	delete []str;
	
	return wstr;
}

+ (CString)getCStringFromNSString:(NSString*)str
{
	return CString([str UTF8String]);
}

+ (NSString*)CStringToNSString:(const wchar_t*)instr
{		
	/*NSString *str = */
	return [[[NSString alloc] initWithBytes:instr 
											 length:wcslen(instr)*4 
										   encoding:NSUTF32LittleEndianStringEncoding] autorelease];
/*	[NSString stringWithString:str]; // ???
	
	[str release];
	
	return str;*/
}

+ (CString)NSStringToCString:(NSString*)instr
{
	wchar_t *str = (wchar_t *)[instr cStringUsingEncoding:NSUTF32LittleEndianStringEncoding];
	
	return (CString)str;
}

@end
