//
//  smsCocoa.h
//  DCT
//
//  Created by Fengping Yu on 11/21/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "SMSMessage.h"

@interface smsCocoa : NSObject 
{
@public
	SMSMessage *msg;
}

- (id)initWithMsg:(SMSMessage*)pMsg;

@end
