//
//  smsPopUp.m
//  DCT
//
//  Created by mtk on 12-6-13.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import "smsPopUp.h"

@interface NSObject (popupWindowDelegateSupport)

- (void)windowWillStartTimer:(NSNotification*)aNotification;

@end

@implementation smsPopUp

@synthesize delegate;

@synthesize address, content;

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	
	if (self) 
	{
		// Initialization code here.
	}
	
	return self;
}

- (void)dealloc
{
	[address release];
	[content release];
    [super dealloc];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (void)awakeFromNib
{
	[address setStringValue:strAddress];
	[content setString:strContent];
}

- (void)initwithAddress:(NSString *)_address andContent:(NSString *)_content
{
	strAddress = _address;
	strContent = _content;
}

- (void)_myWindowWillStartTimer
{
	NSNotification *notification;
	
	NSDictionary *dict = [NSDictionary dictionaryWithObject:[NSNumber numberWithFloat:2.0f] forKey:@"TimeInterval"];
	
	notification = [NSNotification notificationWithName:windowWillStartTimerNotification object:self userInfo:dict];
	
	if([[self delegate] respondsToSelector:@selector(windowWillStartTimer:)])
	{
		[[self delegate] windowWillStartTimer:notification];
	}
	
	[[NSNotificationCenter defaultCenter] postNotification:notification];
}

- (id)delegate
{
	return delegate;
}

- (void)setDelegate:(id)anObject
{
	delegate = anObject;
}

- (void)startTimer
{
	[self _myWindowWillStartTimer];
}

NSString *windowWillStartTimerNotification = @"windowWillStartTimerNotification";

@end
