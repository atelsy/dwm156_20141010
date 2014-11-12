//
//  smsCocoa.m
//  DCT
//
//  Created by Fengping Yu on 11/21/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsCocoa.h"


@implementation smsCocoa

- (id)init
{
	self = [super init];
	if (self) 
	{
		// Initialization code here.
		msg = new SMSMessage();
	}
	
	return self;
}

- (id)initWithMsg:(SMSMessage*)pMsg
{
	self = [super init];
	
	if(self)
	{
		msg = new SMSMessage;
		msg = pMsg;
	}
	
	return self;
}

- (void)dealloc
{
	// delete msg;
	
	[super dealloc];
}

@end
