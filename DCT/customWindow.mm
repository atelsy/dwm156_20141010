//
//  customWindow.m
//  DCT
//
//  Created by Fengping Yu on 11/4/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "customWindow.h"


@implementation customWindow

- (id)initWithContentRect:(NSRect)contentRect 
				styleMask:(NSUInteger)aStyle 
				  backing:(NSBackingStoreType)bufferingType 
					defer:(BOOL)flag
{
	self = [super initWithContentRect:contentRect 
							styleMask:NSBorderlessWindowMask 
							  backing:NSBackingStoreBuffered 
								defer:NO];
	
	if(self)
	{
		[self setAlphaValue:1.0];
		[self setOpaque:NO];
	}
	
	return self;
}

- (void)awakeFromNib
{
	[self setAlphaValue:0.0];
	[self.animator setAlphaValue:1.0];
}

- (void)dealloc
{
	[super dealloc];
}

@end
