//
//  chListData.m
//  DCT
//
//  Created by mtk on 12-6-9.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import "chListData.h"

@implementation chListData

@synthesize name, time, number, pItem;

- (id)init
{
	self = [super init];
	
	if(self)
	{
		
	}
	
	return self;
}

- (id)initWithName:(NSString *)_name Time:(NSString *)_time Number:(NSString *)_number withItem:(PhoneBookItem*)_item
{
	self = [super init];
	
	if(self)
	{
		self.name = _name;
		self.time = _time;
		self.number = _number;
		self.pItem = _item;
	}
	
	return self;
}

- (void)dealloc
{
	[name release];
	[time release];
	[number release];
	
	[super dealloc];
}

@end
