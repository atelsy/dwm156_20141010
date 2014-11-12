//
//  ImageTextCell.m
//  SofaControl
//
//  Created by Martin Kahr on 10.10.06.
//  Copyright 2006 CASE Apps. All rights reserved.
//

#import "ImageTextCell.h"

@implementation ImageTextCell

- (void)dealloc 
{
	[super dealloc];
}

- copyWithZone:(NSZone *)zone
{
	ImageTextCell *cell = (ImageTextCell *)[super copyWithZone:zone];

	return cell;
}

- (void) setIcon: (NSImage*) image
{
	icon = [image retain];
}
- (void) setPrimaryTextKey: (NSString*) text
{
	primaryTextKey = [text retain];	
}

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView 
{
	[self setTextColor:[NSColor blackColor]];

	//TODO: Selection with gradient and selection color in white with shadow
	// check out http://www.cocoadev.com/index.pl?NSTableView
	
	BOOL elementDisabled	= NO;	

	NSColor* primaryColor   = [self isHighlighted] ? [NSColor alternateSelectedControlTextColor] : (elementDisabled? [NSColor disabledControlTextColor] : [NSColor textColor]);
	NSString* primaryText   = primaryTextKey; 
	NSDictionary* primaryTextAttributes = [NSDictionary dictionaryWithObjectsAndKeys: primaryColor, NSForegroundColorAttributeName,
		[NSFont systemFontOfSize:12], NSFontAttributeName, nil];	
	
	//[primaryText drawAtPoint:NSMakePoint(cellFrame.origin.x+cellFrame.size.height + 10, cellFrame.origin.y) withAttributes:primaryTextAttributes];
	
	[primaryText drawInRect:NSMakeRect(cellFrame.origin.x+cellFrame.size.height + 10, cellFrame.origin.y, cellFrame.size.width - cellFrame.size.height - 1, cellFrame.size.height-2) withAttributes:primaryTextAttributes];
	
	[[NSGraphicsContext currentContext] saveGraphicsState];
	float yOffset = cellFrame.origin.y;
	if ([controlView isFlipped]) 
	{
		NSAffineTransform* xform = [NSAffineTransform transform];
		[xform translateXBy:0.0 yBy: cellFrame.size.height];
		[xform scaleXBy:1.0 yBy:-1.0];
		[xform concat];		
		yOffset = 0-cellFrame.origin.y;
	}		
	
	NSImageInterpolation interpolation = [[NSGraphicsContext currentContext] imageInterpolation];
	[[NSGraphicsContext currentContext] setImageInterpolation: NSImageInterpolationHigh];	
	
	[icon drawInRect:NSMakeRect(cellFrame.origin.x+5,yOffset+3,cellFrame.size.height-6, cellFrame.size.height-6)
			fromRect:NSMakeRect(0,0,[icon size].width, [icon size].height)
		   operation:NSCompositeSourceOver
			fraction:1.0];
	
	[[NSGraphicsContext currentContext] setImageInterpolation: interpolation];
	
	[[NSGraphicsContext currentContext] restoreGraphicsState];	
}

@end
