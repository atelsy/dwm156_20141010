//
//  pbListView.mm
//  DCT
//
//  Created by mbj on 12-1-13.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "pbListView.h"
#import "pbListCtrl.h"
#import "DCTAppDelegate.h"
#import "pbViewControl.h"


@implementation pbListView

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
	[super dealloc];
}

- (void)keyDown:(NSEvent *)theEvent
{
	unichar key = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
	pbListCtrl *delegate = (pbListCtrl*)[self delegate];
	pbViewControl *pView = (pbViewControl*)[delegate getParent];
	
	if(key == NSDeleteCharacter)
	{
		[pView OnEditClear:nil];
	}
	else if(([theEvent modifierFlags] & NSCommandKeyMask) && ([[theEvent characters] isEqualToString:@"a"] || [[theEvent characters] isEqualToString:@"A"]))
	{
		[self selectAll:nil];
	}
	else if(([theEvent modifierFlags] & NSCommandKeyMask) && ([[theEvent characters] isEqualToString:@"x"] || [[theEvent characters] isEqualToString:@"X"]))
	{
		[pView OnEditCut:nil];
	}
	else if(([theEvent modifierFlags] & NSCommandKeyMask) && ([[theEvent characters] isEqualToString:@"c"] || [[theEvent characters] isEqualToString:@"C"]))
	{
		[pView OnEditCopy:nil];
	}
	else if(([theEvent modifierFlags] & NSCommandKeyMask) && ([[theEvent characters] isEqualToString:@"v"] || [[theEvent characters] isEqualToString:@"V"]))
	{
		[pView OnEditPaste:nil];
	}
	else
	{
		[super keyDown:theEvent];
	}
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	NSLog(@"mouse move into list ctrl.");
}

- (NSMenu*)menuForEvent:(NSEvent *)event
{
	if([event type] == NSRightMouseDown)
	{
		if([self isEnabled] == NO)
			return nil;
	}
	
	return [super menuForEvent:event];
}

@end
