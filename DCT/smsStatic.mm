//
//  smsStatic.m
//  DCT
//
//  Created by Fengping Yu on 11/4/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsStatic.h"


@implementation smsStatic

- (id)init
{
	self = [super init];
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
	NSBezierPath *line = [NSBezierPath bezierPath];
	
	NSPoint start = [self bounds].origin;
	
	NSPoint end = NSMakePoint([self bounds].origin.y + [self bounds].size.width, [self bounds].origin.y);
	
	[line moveToPoint:start];
	[line lineToPoint:end];
}

@end
