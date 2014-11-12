//
//  customView.h
//  DCT
//  implement custom view for the sms popup view
//  Created by Fengping Yu on 11/4/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface customView : NSView 
{
@private
	NSImage* appleImage;
}

@property(retain)NSImage *appleImage;

@end
