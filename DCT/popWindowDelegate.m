//
//  popWindowDelegate.m
//  DCT
//
//  Created by mtk on 12-6-13.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import "popWindowDelegate.h"

#define WINKEY  @"windowKey"

@implementation popWindowDelegate

- (void)windowWillStartTimer:(NSNotification*)aNotification
{
	NSDictionary *dict = [aNotification userInfo];
	float interValue = [[dict objectForKey:@"TimeInterval"] floatValue];
	
	NSWindowController *win = (NSWindowController*)[aNotification object];
	
	NSDictionary *userInfo = [NSDictionary dictionaryWithObject:win forKey:WINKEY];
	
	[NSTimer scheduledTimerWithTimeInterval:interValue target:self selector:@selector(startAnimatedTimer:) userInfo:userInfo repeats:NO];
}

- (void)startAnimatedTimer:(NSNotification*)aNotification
{
	animatedTimer = [[NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(updateAlpha:) userInfo:[aNotification userInfo] repeats:YES] retain];
}

- (void)updateAlpha:(NSNotification*)aNotification
{
	NSWindowController *winControl = [[aNotification userInfo] objectForKey:WINKEY];
	
	NSWindow *win = [winControl window];
	
	[win setAlphaValue:[win alphaValue] - 0.05];
	
	if([win alphaValue] < 0.1)
	{
		[animatedTimer invalidate];
		[animatedTimer release];
		animatedTimer = nil;
		[win close];
	}
}

@end
