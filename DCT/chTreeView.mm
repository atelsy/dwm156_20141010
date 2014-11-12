//
//  chTreeView.m
//  DCT
//
//  Created by mtk on 12-6-6.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import "chTreeView.h"
#import "chTreeControl.h"
#import "treeNode.h"

@implementation chTreeView

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

- (void)onChFolderDelete:(id)sender
{
	chTreeControl *control = [self delegate];
	
	[control onChCleanCall];
}

- (NSMenu*)defaultMenuForRow:(int)row
{
	NSMenu *menu = [[[NSMenu alloc] initWithTitle:@"callhistory context menu"] autorelease];
	
	[menu insertItemWithTitle:LocalizeString(@"IDR_MENU_CH") action:@selector(onChFolderDelete:) keyEquivalent:@"" atIndex:0];
	
	return menu;
}

- (NSMenu*)menuForEvent:(NSEvent *)event
{
	if([event type] == NSRightMouseDown)
	{
		NSPoint pt = [self convertPoint:[event locationInWindow] fromView:nil];
		NSInteger row = [self rowAtPoint:pt];
		
		NSTreeNode *node = [self itemAtRow:row];
		
		if(node != nil)
			return [self defaultMenuForRow:(int)row];
		else 
			return nil;
	}
	
	return [super menuForEvent:event];
}

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	chTreeControl *delegate = [self delegate];
	
	ChFolder *folder = [delegate getSelectedItemData];
	
	if(folder->GetItemSize() > 0)
		return YES;
	
	return NO;
}

@end
