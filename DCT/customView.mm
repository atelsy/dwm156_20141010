//
//  customView.m
//  DCT
//
//  Created by Fengping Yu on 11/4/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "customView.h"


@implementation customView

@synthesize appleImage;

- (void)awakeFromNib
{
	self.appleImage = [NSImage imageNamed:@"grayapple"];
}

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
	[appleImage release];
	
	[super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect
{
	// Drawing code here.clear the drawing rect
	//[[NSColor clearColor] set];
	
        //NSRectFill([self frame]);
	[[NSColor whiteColor] set];
	
    NSRectFill(dirtyRect);
/*
	[appleImage drawAtPoint:NSZeroPoint 
				   fromRect:[self frame] 
				  operation:NSCompositeCopy 
				   fraction:0.8];
*/
    /*[appleImage drawInRect:[self frame]
				  fromRect:NSZeroRect
				 operation:NSCompositeCopy
				  fraction:0.8];*/
	
  //  [appleImage compositeToPoint:NSZeroPoint operation:NSCompositeSourceOver];
}

@end
