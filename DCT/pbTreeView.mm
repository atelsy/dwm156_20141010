//
//  pbTreeView.mm
//  DCT
//
//  Created by mbj on 12-1-13.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "pbTreeView.h"
#import "smsTreeCtrl.h"
#import "utility.h"
#import "DCTAppDelegate.h"
#import "treeNode.h"


enum PbMenuTag
{
	PB_CLEAN_TRANSCAN = 0,
	PB_ADD_FOLDER,
	PB_DEL_FOLDER,
	PB_RENAME_FOLDER,
	PB_SEND_MSG
};

@implementation pbTreeView

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
	[super dealloc];
}

- (void)onPbFolderDelete:(id)sender
{
	pbTreeCtrl *delegate = (pbTreeCtrl*)[self delegate];
	
	[delegate OnPbFolderDelete:sender];
}

- (void)onPbFolderNew:(id)sender
{
	pbTreeCtrl *delegate = (pbTreeCtrl*)[self delegate];
	
	[delegate OnPbFolderNew:sender];
}

- (void)onPbFolderRename:(id)sender
{
	pbTreeCtrl *delegate = (pbTreeCtrl*)[self delegate];
	
	[delegate OnPbFolderRename:sender];
}

- (void)onPbFolderSendMsg:(id)sender
{
	pbTreeCtrl *delegate = (pbTreeCtrl*)[self delegate];
	
	[delegate OnPBFolderSendMsg:sender];
}

- (NSMenu*)menuForEvent:(NSEvent *)event
{
	if([event type] == NSRightMouseDown)
	{
		NSPoint pt = [self convertPoint:[event locationInWindow] fromView:nil];
		int row = (int)[self rowAtPoint:pt];
		
		NSTreeNode *node = [self itemAtRow:row];
		if(node == nil)
			return nil;
		
		treeNode *data = (treeNode*)[node representedObject];
		
		PbFolder *pFolder = (PbFolder*)[data getTag];
		
		PbFolder *pFolderTrashCan = NULL;
		PbFolder *pFolderPC = NULL;
		NSTreeNode *hTrashCan = nil;
		NSTreeNode *hPC = nil;
		
		pbTreeCtrl *delegate = (pbTreeCtrl*)[self delegate];
		
		PbFolder* pSelectFolder = [delegate GetSelectedItemData];
		
		folder_state_struct state;
		pSelectFolder->GetFolderState(state);
		
		[delegate GetTrashCan:&hTrashCan forFolder:&pFolderTrashCan];
		[delegate GetPcRoot:&hPC forFolder:&pFolderPC];
		
		//todo: modify for compare folder
		if(pFolder == pFolderTrashCan && pSelectFolder == pFolder)
		{
			if([self isEnabled] == YES)
				return [self defaultMenuForRow:row];
			else
				return nil;
		}
		else if((pSelectFolder == pFolderPC) || (state.bModify == 1))
		{
			if([self isEnabled] == YES)
				return [self pbFolderMenu:row];
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
	NSMenu *menu = [[[NSMenu alloc] initWithTitle:@"Trash Can Context Menu"] autorelease];
		
	[menu insertItemWithTitle:LocalizeString(@"IDR_MENU_TRASHCAN") action:@selector(onPbFolderDelete:) keyEquivalent:@"" atIndex:0];
	NSMenuItem *menuitem = [menu itemAtIndex:0];
	[menuitem setTag:PB_CLEAN_TRANSCAN];
	
	return menu;
}

- (NSMenu*)pbFolderMenu:(int)row
{
	NSMenu *menu = [[[NSMenu alloc] initWithTitle:@"PB Folder Menu"] autorelease];
	
	NSMenuItem *item = nil;
	
	item = [[[NSMenuItem alloc] initWithTitle:LocalizeString(@"IDS_PB_NEWFOLDER") action:@selector(onPbFolderNew:) keyEquivalent:@""] autorelease];
	[item setTag:PB_ADD_FOLDER];
	[menu insertItem:item atIndex:0];
	
	item = [[[NSMenuItem alloc] initWithTitle:LocalizeString(@"IDS_PB_DELETE") action:@selector(onPbFolderDelete:) keyEquivalent:@""] autorelease];
	[item setTag:PB_DEL_FOLDER];
	[menu insertItem:item atIndex:1];
	
	item = [[[NSMenuItem alloc] initWithTitle:LocalizeString(@"IDS_PB_RENAMEFOLDER") action:@selector(onPbFolderRename:) keyEquivalent:@""] autorelease];
	[item setTag:PB_RENAME_FOLDER];
	[menu insertItem:item atIndex:2];
	
	item = [[[NSMenuItem alloc] initWithTitle:LocalizeString(@"IDS_PB_SENDMSG") action:@selector(onPbFolderSendMsg:) keyEquivalent:@""] autorelease];
	[item setTag:PB_SEND_MSG];
	[menu insertItem:item atIndex:3];
	
	return menu;
	
}

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	switch ([menuItem tag]) {
		case PB_CLEAN_TRANSCAN:
			{
				NSTreeNode *hTrashCan;
				PbFolder *pFolderTrashCan;
			
				pbTreeCtrl *delegate = (pbTreeCtrl*)[self delegate];
				[delegate GetTrashCan:&hTrashCan forFolder:&pFolderTrashCan];
			
				if(pFolderTrashCan && (pFolderTrashCan->GetItemSize() > 0 || pFolderTrashCan->GetSubfolderSize() > 0))
					return YES;
			}
			break;
		case PB_ADD_FOLDER:
			{
				PbFolder *pFolderPC = NULL;
				NSTreeNode *hPC = nil;
				
				pbTreeCtrl *delegate = (pbTreeCtrl*)[self delegate];
				PbFolder* pSelectFolder = [delegate GetSelectedItemData];
				[delegate GetPcRoot:&hPC forFolder:&pFolderPC];
				
				if(pSelectFolder == pFolderPC)
					return YES;
			}
			break;
		case PB_DEL_FOLDER:
		case PB_RENAME_FOLDER:
			{
				pbTreeCtrl *delegate = (pbTreeCtrl*)[self delegate];
				PbFolder* pSelectFolder = [delegate GetSelectedItemData];
				
				folder_state_struct state;
				pSelectFolder->GetFolderState(state);
				if(state.bModify)
					return YES;
			}
		case PB_SEND_MSG:
			{
				pbTreeCtrl *delegate = (pbTreeCtrl*)[self delegate];
				PbFolder* pSelectFolder = [delegate GetSelectedItemData];
				
				folder_state_struct state;
				pSelectFolder->GetFolderState(state);
				if(state.bModify && pSelectFolder->CountContacts(pSelectFolder) > 0)
					return YES;
			}
			break;
	}	
	
	return NO;
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	NSLog(@"mouse move into tree ctrl");
}

@end
