//
//  smsFormView.m
//  DCT
//
//  Created by Fengping Yu on 10/31/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsFormView.h"


@implementation smsFormView

- (id)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame];
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

- (void)drawRect:(NSRect)dirtyRect
{
	// Drawing code here.
	[super drawRect:dirtyRect];
	
	[[NSColor colorWithDeviceRed:98 green:108 blue:153 alpha:0.9] set];
	NSRectFill(dirtyRect);
}

@end
