//
//  phoneNumberFormatter.h
//  DCT
//
//  Created by Fengping Yu on 11/8/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface phoneNumberFormatter : NSNumberFormatter
{
@private

}

+ (BOOL)checkPhonenumber:(NSString*)phoneNumber;

@end
