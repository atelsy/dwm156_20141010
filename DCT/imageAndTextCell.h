//
//  imageAndTextCell.h
//  DCT
//
//  Created by Fengping Yu on 11-10-14.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface imageAndTextCell : NSTextFieldCell 
{
@private
	NSImage *image;	
}

@property(readwrite, retain)NSImage *image;

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView*)controlView;
- (NSSize)cellSize;

@end
