//
//  pbCocoa.mm
//  DCT
//
//  Created by mbj on 12-1-31.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "pbCocoa.h"


@implementation pbCocoa

- (id)init
{
	self = [super init];
	if (self) 
	{
		// Initialization code here.
		contact = new PhoneBookItem;
	}
	
	return self;
}

- (id)initWithPbk:(PhoneBookItem*)pContact
{
	self = [super init];
	
	if(self)
	{
		contact = new PhoneBookItem;
		contact = pContact;
	}
	
	return self;
}

- (void)dealloc
{
	// delete contact;
	
	[super dealloc];
}

@end
