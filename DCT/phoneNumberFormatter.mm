//
//  phoneNumberFormatter.m
//  DCT
//
//  Created by Fengping Yu on 11/8/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "phoneNumberFormatter.h"


@implementation phoneNumberFormatter

- (id)init
{
self = [super init];
if (self) 
{
// Initialization code here.
}

return self;
}

- (void)dealloc
{
[super dealloc];
}

+ (BOOL)checkPhonenumber:(NSString *)phoneNumber
{
NSCharacterSet *allowedCharacters;

 allowedCharacters = [NSCharacterSet characterSetWithCharactersInString:@"+*#1234567890,;"];

if([phoneNumber length] == 0)
return YES;

for(int i = 0; i < [phoneNumber length]; i++)
{
unichar character = [phoneNumber characterAtIndex:i];

if((character == 43) && i > 0)
return NO;

if([allowedCharacters characterIsMember:character] == YES)
continue;
else
return NO;
}

return YES;
}

- (BOOL)isPartialStringValid:(NSString **)partialStringPtr proposedSelectedRange:(NSRangePointer)proposedSelRangePtr originalString:(NSString *)origString originalSelectedRange:(NSRange)origSelRange errorDescription:(NSString **)error
{
int total;

NSArray *array = [*partialStringPtr componentsSeparatedByString:@","];
total = (int)[array count];

for(int i = 0; i < total; i++)
{
if([phoneNumberFormatter checkPhonenumber:[array objectAtIndex:i]] == NO)
return NO;
}

return YES;
}

@end
