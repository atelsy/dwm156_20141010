//
//  pbFormView.mm
//  DCT
//
//  Created by mbj on 12-1-14.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "pbFormView.h"


@implementation pbFormView

- (id)initWithFrame:(NSRect)frame 
{
	self = [super initWithFrame:frame];
	if (self) {
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
