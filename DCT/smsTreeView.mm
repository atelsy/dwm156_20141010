//
//  smsTreeView.m
//  DCT
//
//  Created by Fengping Yu on 12/6/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsTreeView.h"
#import "smsTreeCtrl.h"
#import "utility.h"
#import "DCTAppDelegate.h"
#import "treeNode.h"
#import "SMSViewCtrl.h"

@implementation smsTreeView

@synthesize menu;

- (id)init
{
	self = [super init];
	if (self) {
		// Initialization code here.
	}
	
	return self;
}

- (void)dealloc
{
	[menu release];
	[super dealloc];
}

- (void)onSmsFolderDelete:(id)sender
{
	smsTreeCtrl *delegate = (smsTreeCtrl*)[self delegate];
	
	[delegate onSmsFolderDelete:sender];
}

- (NSMenu*)menuForEvent:(NSEvent *)event
{
	if([event type] == NSRightMouseDown)
	{
		NSPoint pt = [self convertPoint:[event locationInWindow] fromView:nil];
		int row = (int)[self rowAtPoint:pt];
		
		NSTreeNode *node = [self itemAtRow:row];
		treeNode *data = (treeNode*)[node representedObject];
		
		SmsFolder *pFolder = (SmsFolder*)[data getTag];
		
		SmsFolder *pFolderTrashCan = NULL;
		NSTreeNode *hTrashCan = nil;
		
		smsTreeCtrl *delegate = (smsTreeCtrl*)[self delegate];
		
		[delegate getTrashCan:&hTrashCan forFolder:&pFolderTrashCan];
		
		SmsFolder* pSelectFolder = [delegate getSelectedItemData];
		
		//todo: modify for compare folder
		if(pFolder == pFolderTrashCan && pFolder == pSelectFolder)
		{
			if([self isEnabled] == YES)
				return [self defaultMenuForRow:row];
			else
				return nil;
		}
		else
			return nil;
	}
	
	return [super menuForEvent:event];
}

- (NSMenu*)defaultMenuForRow:(int)row
{
	if(menu == nil)
	{
		menu = [[NSMenu alloc] initWithTitle:@"Trash Can Context Menu"];
		[menu insertItemWithTitle:LocalizeString(@"IDR_MENU_TRASHCAN") action:@selector(onSmsFolderDelete:) keyEquivalent:@"" atIndex:0];
	}
	
	return menu;
}

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	NSTreeNode *hTrashCan;
	SmsFolder *pFolderTrashCan;
	
	smsTreeCtrl *delegate = (smsTreeCtrl*)[self delegate];
	[delegate getTrashCan:&hTrashCan forFolder:&pFolderTrashCan];
	
	SMSViewCtrl* view = [delegate getParent];
	
	if(pFolderTrashCan && pFolderTrashCan->GetItemSize() > 0 && view.m_nEnterDlg <= 0)
		return YES;
	
	return NO;
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	NSLog(@"mouse move into tree ctrl");
}

@end
