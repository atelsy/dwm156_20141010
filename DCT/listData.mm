//
//  listData.m
//  DCT
//
//  Created by Fengping Yu on 10/19/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "listData.h"


@implementation listData

@synthesize memory, fromTo, time, content, image, pMsg;

- (id)init
{
	self = [super init];
	if (self)
	{
		// Initialization code here.
	}
	
	return self;
}

- (id)initWithMemory:(NSString *)_memory FromTo:(NSString *)_frmoTo Time:(NSString *)_time Content:(NSString *)_content withMsg:(SMSMessage *)_pMsg
{
	self = [super init];
	if(self)
	{
		self.memory = _memory;
		self.fromTo = _frmoTo;
		self.time = _time;
		self.content = _content;
		self.pMsg = _pMsg;
	}
	
	return self;
}

- (void)dealloc
{
	[memory release];
	[fromTo release];
	[time release];
	[content release];
	[image release];
	
	[super dealloc];
}

//@end

//@implementation listData(sorting)

- (NSComparisonResult)MemoryCellComparison:(listData *)otherListData
{
	return [memory compare:otherListData.memory];
}

- (NSComparisonResult)FromToCellComparison:(listData *)otherListData
{
	return [fromTo compare:otherListData.fromTo];
}

- (NSComparisonResult)TimeCellComparison:(listData *)otherListData
{
	return [time compare:otherListData.time];
}

- (NSComparisonResult)ContentCellComparison:(listData *)otherListData
{
	return [content compare:otherListData.content];
}

@end
