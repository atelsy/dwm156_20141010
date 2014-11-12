//
//  pbListData.mm
//  DCT
//
//  Created by mbj on 12-1-31.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "pbListData.h"


@implementation pbListData

@synthesize memory, name, mobile, image, pContact;

- (id)init
{
	self = [super init];
	if (self)
	{
		// Initialization code here.
	}
	
	return self;
}

- (id)initWithMemory:(NSString *)_memory Name:(NSString *)_name Mobile:(NSString *)_mobile  withContact:(PhoneBookItem *)_pContact
{
	self = [super init];
	if(self)
	{
		self.memory = _memory;
		self.mobile = _mobile;
		self.name = _name;
		self.pContact = _pContact;
	}
	
	return self;
}

- (void)dealloc
{
	[memory release];
	[name release];
	[mobile release];
	[image release];
	
	[super dealloc];
}

- (NSComparisonResult)MemoryCellComparison:(pbListData *)otherListData
{
	return [memory compare:otherListData.memory];
}

- (NSComparisonResult)NameCellComparison:(pbListData *)otherListData
{
	return [name compare:otherListData.name];
}

- (NSComparisonResult)MobileCellComparison:(pbListData *)otherListData
{
	return [mobile compare:otherListData.mobile];
}

@end

