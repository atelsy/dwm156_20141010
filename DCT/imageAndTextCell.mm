//
//  imageAndTextCell.m
//  DCT
//
//  Created by Fengping Yu on 11-10-14.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "imageAndTextCell.h"
#import <AppKit/NSCell.h>


@implementation imageAndTextCell

@synthesize image;

- (id)init
{
	self = [super init];
	if (self)
	{
		// Initialization code here.
		[self setLineBreakMode:NSLineBreakByTruncatingTail];
		[self setSelectable:YES];
	}
	
	return self;
}

- (void)dealloc
{
	[image release];
	
	[super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
	imageAndTextCell *cell = (imageAndTextCell*)[super copyWithZone:zone];
	
	cell->image = [image retain];
	
	return cell;
}

- (NSSize)imageSize
{
	return NSMakeSize(16, 16);
}

- (NSRect)imageRectForBounds:(NSRect)theRect
{
	NSRect result;
	
	if(image != nil)
	{
		result.size = [self imageSize];
		result.origin = theRect.origin;
		result.origin.x += 3;
		result.origin.y += ceil((theRect.size.height - result.size.height)/2);
	}
	else
	{
		result = NSZeroRect;
	}
	
	return result;
}

- (NSRect)titleRectForBounds:(NSRect)theRect
{
	NSRect result;
	
	if(image != nil)
	{
		CGFloat imageWidth = [self imageSize].width;
		result = theRect;
		result.origin.x += (3 + imageWidth);
		result.size.width -= (3 + imageWidth);
	}
	else
	{
		result = [super titleRectForBounds: theRect];
	}
	
	return result;
}

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	if(image != nil)
	{
		NSRect imageFrame = [self imageRectForBounds:cellFrame];
		[image setFlipped:YES];
		[image drawInRect:imageFrame fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
   //	 [image drawInRect:imageFrame fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
		NSInteger newX = NSMaxX(imageFrame) + 3;
		cellFrame.size.width = NSMaxX(cellFrame) - newX;
		cellFrame.origin.x = newX;
	}
	
	[super drawWithFrame:cellFrame inView:controlView];
}

-(NSSize)cellSize
{
	NSSize size = [super cellSize];
	
	if(image != nil)
	{
		size.width += [self imageSize].width;
	}
	
	size.width += 3;
	
	return size;
}

- (NSUInteger)hitTestForEvent:(NSEvent *)event inRect:(NSRect)cellFrame ofView:(NSView *)controlView
{
	NSPoint point = [controlView convertPoint:[event locationInWindow] fromView:nil];
	
	if(image != nil)
	{
		NSSize imageSize = [self imageSize];
		NSRect imageFrame;
		NSDivideRect(cellFrame, &imageFrame, &cellFrame, 3 + imageSize.width, NSMinXEdge);
		imageFrame.origin.x += 3;
		imageFrame.size = imageSize;
		
		if(NSMouseInRect(point, imageFrame, [controlView isFlipped]))
		{
			return NSCellHitContentArea;
		}
	}
	
	return [super hitTestForEvent:event inRect:cellFrame ofView:controlView];
}

@end
