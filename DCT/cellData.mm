//
//  cellData.m
//  DCT
//  implement stored toolbar data and tooltip
//  Created by Fengping Yu on 10/31/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "cellData.h"

@implementation iconViewBox

- (id)init
{
	self = [super init];
	
	if(self)
	{
		
	}
		
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (NSView*)hitTest:(NSPoint)aPoint
{
	if(NSPointInRect(aPoint, [self convertRect:[self bounds] toView:[self superview]]))
	{
		return self;
	}
	else
	{
		return nil;
	}
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
	return YES;
}

// response mouse click toolbar event

- (void)mouseDown:(NSEvent *)theEvent
{
	[super mouseDown:theEvent];
	
	if([theEvent clickCount] >= 1)
	{
		if(delegate && [delegate respondsToSelector:@selector(click:)])
		{
			[delegate performSelector:@selector(click:) withObject:self];
		}
	}
}

@end

@implementation cellData

- (id)init
{
	self = [super init];
	if (self) 
	{
		// Initialization code here.
	}
	
	return self;
}

- (void)dealloc
{
	[image release];
	[tooltip release];
	
	[super dealloc];
}

@synthesize image, tooltip, tag, bEnable, appTag;

@end

@implementation labelCellData

- (id)init
{
	self = [super init];
	
	if(self)
	{
		
	}
	
	return self;
}

- (void)dealloc
{
	[info release];
	
	[super dealloc];
}

@synthesize info;

@end
