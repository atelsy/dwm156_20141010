//
//  string.h
//  DCT
//
//  Created by MBJ on 12-7-7.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface NSString (WCharToString) 

+ (NSString*)getStringFromWchar:(const wchar_t*)instr;
+ (CString)getCStringFromNSString:(NSString*)str;
+ (NSString*)CStringToNSString:(const wchar_t*)instr;
+ (CString)NSStringToCString:(NSString*)instr;

@end

