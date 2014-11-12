//
//  TreeCtrl.m
//  DCT
//
//  Created by ____ on 11-10-14.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsTreeCtrl.h"
#import "imageAndTextCell.h"
#import "treeNode.h"
#import "DCTAppDelegate.h"
#import "utility.h"
#import "Win2Mac.h"
#import "nsstrex.h"

@implementation smsTreeCtrl

@synthesize outlineView;

#pragma mark--
#pragma mark construct and destruct
#pragma mark--

- (id)init
{
	self = [super init];
	if (self) 
	{
		// Initialization code here.
		NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"smsTree" ofType:@"dict"]];
		
		[[TheDelegate getSmsViewController] initFolder];
		
		rootNode = [[self treeNodeFromDictionary:dict] retain];
	}
	
	return self;
}

- (void)dealloc
{
	[rootNode release];
	[iconImage release];
	
	[super dealloc];
}

#pragma mark --
#pragma mark NSOutlineView data source methods
#pragma mark --

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	[[NSNotificationCenter defaultCenter] postNotificationName:SMSTREE_TVN_SELCHANGED object:self];
}

- (NSArray*)childrenForItem:(id)item
{
	if(item == nil)
	{
		return [rootNode childNodes];
	}
	else
	{
		return [item childNodes];
	}
}

- (id)outlineView:(NSOutlineView*)outlineView child:(NSInteger)index ofItem:(id)item
{
	NSArray *children = [self childrenForItem:item];
	
	return [children objectAtIndex:index];
}

- (BOOL)outlineView:(NSOutlineView*)outlineView isItemExpandable:(id)item
{
	treeNode *node = [item representedObject];
	
	return node.container;
}

- (id)outlineView:(NSOutlineView*)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
	id objectValue = nil;
	treeNode *nodeData = [item representedObject];
	
	objectValue = nodeData.name;
	
	return objectValue;
}

- (NSInteger)outlineView:(NSOutlineView*)outlineView numberOfChildrenOfItem:(id)item
{
	NSArray *children = [self childrenForItem:item];
	
	return [children count];
}

- (BOOL)outlineView:(NSOutlineView*)outlineView shouldExpandItem:(id)item
{
	treeNode *nodeData = [item representedObject];
	
	return nodeData.expandable;
}

- (void)outlineView:(NSOutlineView*)outlineView willDisplayCell:(NSCell*)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	treeNode *nodeData = [item representedObject];
	
	if(nodeData.image == nil)
		nodeData.image = [self iconImage: nodeData.name];
	
	imageAndTextCell *imageCell = (imageAndTextCell*)cell;
	[imageCell setImage:nodeData.image];
}

- (BOOL)outlineView:(NSOutlineView*)outlineView shouldSelectItem:(id)item
{
	treeNode *nodeData = [item representedObject];
	
	return nodeData.selectable;
}

- (BOOL)outlineView:(NSOutlineView*)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	return NO;
}

- (BOOL)treeNode:(NSTreeNode*)treenode isDescendantOfNode:(NSTreeNode*)parentnode
{
	while(treenode != nil)
	{
		if(treenode == parentnode)
			return YES;
		
		treenode = [treenode parentNode];
	}
	
	return NO;
}

- (void*)setTreeFolder:(NSString*)name isHandsetfolder:(bool)isHandset
{
	SmsFolder *pFolder = [TheDelegate getSmsFolder];
	int num = 3, i = 0;
	SubfolderPos pos = pFolder->GetFirstSubfolderPos();
	SmsFolder *pSubFolder, *pSubsubFolder;
	CString folderName;
	NSString *str;
	NSString *trashCan = [[NSBundle mainBundle] localizedStringForKey: @"IDS_SMS_FOLDER9" value:nil table:@"InfoPlist"];
	NSString* handset = [[NSBundle mainBundle] localizedStringForKey:@"IDS_SMS_FOLDER0" value:nil table:@"InfoPlist"];
	
	NSString* pcfolder = [[NSBundle mainBundle] localizedStringForKey:@"IDS_SMS_FOLDER3" value:nil table:@"InfoPlist"];
	
	while(i < num && pos != 0)
	{
		pSubFolder = pFolder->GetNextSubfolder(pos);
		
		if(pSubFolder)
		{
			SubfolderPos subpos = pSubFolder->GetFirstSubfolderPos();
			pSubFolder->GetFolderName(folderName);
			str = [NSString getStringFromWchar:(LPCTSTR)folderName];
			
			// for trash can, as it does not have children folder, so judge it in this 
			if([str caseInsensitiveCompare:trashCan] == NSOrderedSame || 
			   [name caseInsensitiveCompare:handset] == NSOrderedSame ||
			   [name caseInsensitiveCompare:pcfolder] == NSOrderedSame)
			{
				//	ASSERT(0);// memory leak
				pSubFolder->CloseNextSubfolder(subpos);
				pFolder->CloseNextSubfolder(pos);
				return pSubFolder;
			}
			
			while(subpos)
			{
				pSubsubFolder = pSubFolder->GetNextSubfolder(subpos);
				if(pSubsubFolder)
					pSubsubFolder->GetFolderName(folderName);
				
				str = [NSString getStringFromWchar:(LPCTSTR)folderName];
				
				if(folderName.GetLength() > 0 && [name caseInsensitiveCompare: str] == NSOrderedSame)
				{
					if(i == 0 && !isHandset)
					{
						continue;
					}
					else
					{
						pSubFolder->CloseNextSubfolder(subpos);
						pFolder->CloseNextSubfolder(pos);
						return pSubsubFolder;
					}
				}
			}
			pSubFolder->CloseNextSubfolder(subpos);
		}
		
		i++; 
	}
	
	pFolder->CloseNextSubfolder(pos);
	
	return NULL;
}

- (NSImage*)iconImage:(NSString *)aName
{
	NSArray *array = nil;
	
	if(iconImage == nil)
	{
		iconImage = [[NSMutableArray alloc] init];
		
		
		array = [NSArray arrayWithObjects:@"handset folder.png", @"inbox.png",
				 @"outbox.png", @"pc folder.png", @"sent message.png",@"draft.png",
				 @"personal folder.png", @"trash can.png", nil];
		
		for(NSString *name in array)
		{
			NSImage *image = [NSImage imageNamed:name];
			
			if(image != nil)
				[iconImage addObject:image];
		}
	}
	
	for(NSImage *image in iconImage)
	{
		if([[image name] caseInsensitiveCompare: aName] == NSOrderedSame)
		{
			return image;
		}
	}
	
	return nil;
}

- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary *)dict
{
	NSString *nodeName = [dict objectForKey:NAME_KEY];
	treeNode *nodeData = [treeNode nodeDataWithName:nodeName];
	
	NSTreeNode *result = [NSTreeNode treeNodeWithRepresentedObject:nodeData];
	
	// walk through dictionary and create treenode for each children
	NSArray *children = [dict objectForKey:CHILDREN_KEY];
	bool isHandsetFolder = [children count] == 2 ? YES: NO;
	
	for(id item in children)
	{
		NSTreeNode *childTreeNode;
		
		if([item isKindOfClass:[NSDictionary class]])
		{
			// recursively create the child tree node
			childTreeNode = [self treeNodeFromDictionary:item]; 
		}
		else
		{
			treeNode *childNodeData = [[treeNode alloc] initWithName:item];
			childNodeData.container = NO;
			[childNodeData setTag:[self setTreeFolder:item isHandsetfolder:isHandsetFolder]];
			
			childTreeNode = [NSTreeNode treeNodeWithRepresentedObject:childNodeData];
			[childNodeData release];
		}
		
		[[result mutableChildNodes] addObject:childTreeNode];
	}
	
	return result;
}

#pragma mark --
#pragma mark select operation
#pragma mark --

- (SmsFolder*)getItemData:(NSTreeNode *)hItem
{
	if(hItem != nil)
	{
		treeNode *node = [hItem representedObject];
		if(node != NULL)
			return (SmsFolder*)[node getTag];
	}
	
	return NULL;
}

- (NSTreeNode*)getSelectedItem
{
	int item = (int)[outlineView selectedRow];
	
	return [outlineView itemAtRow:item];
}

- (SmsFolder*)getSelectedItemData
{
	NSTreeNode *node = [self getSelectedItem];
	
	return (SmsFolder*)[[node representedObject] getTag];
}

- (void)setSelectedItem:(id)item
{
	NSInteger itemIndex = [outlineView rowForItem:item];
	
	if(itemIndex < 0)
		return;
	
	[outlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:itemIndex] byExtendingSelection:NO];
}

- (void)setSelectedItemWithIndex:(NSInteger)index
{
	[outlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:index] byExtendingSelection:NO];
}

#pragma mark--
#pragma mark folder judgement
#pragma mark--

- (id)getParent
{
	DCTAppDelegate *delegate = TheDelegate;
	
	return [delegate getSmsViewController];
}

- (SmsFolder*)getFolderFromName:(NSString *)name
{
	SmsFolder* pFolder = NULL;
	
	pFolder = (SmsFolder*)[self setTreeFolder:name isHandsetfolder:NO];
	
	return pFolder;
}

- (BOOL)ifHandsetInbox:(SmsFolder *)pFolder
{
	NSTreeNode *hInbox;
	SmsFolder *pFolderInbox;
	
	if([self getHandsetInbox:&hInbox forFolder:&pFolderInbox])
	{
		if(pFolder == pFolderInbox)
			return YES;
	}
	
	return NO;
}

- (BOOL)ifHandsetOutbox:(SmsFolder *)pFolder
{
	NSTreeNode *hOutbox;
	SmsFolder *pFolderOutbox;
	
	if([self getHandsetOutbox:&hOutbox forFolder:&pFolderOutbox])
	{
		if(pFolder == pFolderOutbox)
			return YES;
	}
	
	return NO;
}

- (BOOL)ifHandsetOutbox
{
	SmsFolder *pFolder = [self getSelectedItemData];
	
	if(pFolder && [self ifHandsetOutbox:pFolder])
		return YES;
	
	return NO;
}

- (BOOL)ifSentbox
{
	SmsFolder *pFolderSent;
	NSTreeNode *node;
	
	if([self getSentbox:&node forFolder:&pFolderSent])
	{
		NSTreeNode *parent = [self getSelectedItem];
		do 
		{
			if(parent != nil && parent == node)
				return YES;
		} 
		while ((parent = [parent parentNode]));
	}
	
	return NO;
}

- (BOOL)ifPcInbox:(SmsFolder *)pFolder
{
	NSTreeNode *hInbox;
	SmsFolder *pfolderInbox;
	
	if([self getPcInbox:&hInbox forFolder:&pfolderInbox])
	{
		if(pFolder == pfolderInbox)
			return YES;
	}
	
	return NO;
}

- (BOOL)ifPcOutbox:(SmsFolder *)pFolder
{
	NSTreeNode *hInbox;
	SmsFolder *pfolderInbox;
	
	if([self getPcOutbox:&hInbox forFolder:&pfolderInbox])
	{
		if(pFolder == pfolderInbox)
			return YES;
	}
	
	return NO;
}

- (BOOL)ifPcSentbox:(SmsFolder *)pfolder
{
	NSTreeNode *hInbox;
	SmsFolder *pfolderInbox;
	
	if([self getSentbox:&hInbox forFolder:&pfolderInbox])
	{
		if(pfolder == pfolderInbox)
			return YES;
	}
	
	return NO;
}

- (BOOL)ifPcDraft:(SmsFolder *)pFolder
{
	NSTreeNode *hInbox;
	SmsFolder *pfolderInbox;
	
	if([self getPcDraft:&hInbox forFolder:&pfolderInbox])
	{
		if(pFolder == pfolderInbox)
			return YES;
	}
	
	return NO;
}

- (BOOL)ifPcPersonalFolder:(SmsFolder *)pFolder
{
	NSTreeNode *hInbox;
	SmsFolder *pfolderInbox;
	
	if([self getPersonal:&hInbox forFolder:&pfolderInbox])
	{
		if(pFolder == pfolderInbox)
			return YES;
	}
	
	return NO;
}

- (BOOL)ifTrashCan:(SmsFolder *)pFolder
{
	NSTreeNode *hInbox;
	SmsFolder *pfolderInbox;
	
	if([self getTrashCan:&hInbox forFolder:&pfolderInbox])
	{
		if(pFolder == pfolderInbox)
			return YES;
	}
	
	return NO;
}

- (BOOL)ifPcOutbox
{
	SmsFolder *pfolder = [self getSelectedItemData];
	if(pfolder)
	{
		SmsFolder *pFolderoutbox;
		NSTreeNode *node;
		if([self getPcOutbox:&node forFolder:&pFolderoutbox]  == YES && (pfolder == pFolderoutbox))
			return YES;
	}
	
	return NO;
}

- (BOOL)ifTrashCan
{
	SmsFolder *pFolder = [self getSelectedItemData];
	if(pFolder)
	{
		SmsFolder *pFolderTrash;
		NSTreeNode *hTrash;
		
		if([self getTrashCan:&hTrash forFolder:&pFolderTrash] == YES && (pFolder == pFolderTrash))
			return YES;
	}
	
	return NO;
}

- (BOOL)inTrashCan:(NSTreeNode *)node
{
	NSTreeNode *hTrash;
	SmsFolder *pFolderTrash;
	
	if([self getTrashCan:&hTrash forFolder:&pFolderTrash])
	{
		while (node)
		{
			if(node == hTrash)
				return YES;
			
			node = [node parentNode];
		}
	}
	
	return NO;
}

- (BOOL)getHandsetRoot:(NSTreeNode**)node forFolder:(SmsFolder **)pfolder
{
	return [self getItem:0 withIndex:1 forTreeNode:node andFolder:pfolder];
}

- (BOOL)getPcRoot:(NSTreeNode**)node forFolder:(SmsFolder **)pfolder
{
	return [self getItem:1 withIndex:1 forTreeNode:node andFolder:pfolder];
}

- (BOOL)getHandsetInbox:(NSTreeNode**)node forFolder:(SmsFolder **)pfolder
{
	return [self getfolder:0 withIndex:1 isHandset:YES forTreeNode:node andFolder:pfolder];
}

- (BOOL)getHandsetOutbox:(NSTreeNode**)node forFolder:(SmsFolder **)pfolder
{
	return [self getfolder:1 withIndex:1 isHandset:YES forTreeNode:node andFolder:pfolder];
}

- (BOOL)getPcInbox:(NSTreeNode **)node forFolder:(SmsFolder **)pFolder
{
	return [self getfolder:0 withIndex:1 isHandset:NO forTreeNode:node andFolder:pFolder];
}

- (BOOL)getPcOutbox:(NSTreeNode**)node forFolder:(SmsFolder **)pfolder
{
	return [self getfolder:1 withIndex:1 isHandset:NO forTreeNode:node andFolder:pfolder];
}

- (BOOL)getSentbox:(NSTreeNode**)node forFolder:(SmsFolder **)pfolder
{
	return [self getfolder:2 withIndex:1 isHandset:NO forTreeNode:node andFolder:pfolder];
}

- (BOOL)getPcDraft:(NSTreeNode **)node forFolder:(SmsFolder **)pfolder
{
	return [self getfolder:3 withIndex:1 isHandset:NO forTreeNode:node andFolder:pfolder];
}

- (BOOL)getPersonal:(NSTreeNode**)node forFolder:(SmsFolder **)pfolder
{
	return [self getfolder:4 withIndex:1 isHandset:NO forTreeNode:node andFolder:pfolder];
}

- (BOOL)getTrashCan:(NSTreeNode**)node forFolder:(SmsFolder **)pfolder
{
	return [self getItem:2 withIndex:1 forTreeNode:node andFolder:pfolder];
}

- (BOOL)getfolder:(int)index withIndex:(int)index2 isHandset:(signed char)bHandset forTreeNode:(NSTreeNode**)node andFolder:(SmsFolder **)pFolder
{
	NSTreeNode *root;
	
	if(bHandset)
		root = [self getRootNode:0 withIndex:1];
	else
		root = [self getRootNode:1 withIndex:1];
	
	NSIndexPath *path = [[NSIndexPath alloc] initWithIndex:index];
	
	[path indexPathByAddingIndex:index2];
	
	if(root == nil)
	{
		[path release];
		return NO;
	}
	
	*node = [root descendantNodeAtIndexPath:path];
	
	if(*node)
	{
		*pFolder = (SmsFolder*)[((treeNode*)[(*node) representedObject]) getTag];
		if(*pFolder)
		{
			[path release];
			return YES;
		}
	}
	
	[path release];
	
	return NO;
}

- (NSTreeNode*)getRootNode:(int)index withIndex:(int)index2
{
	NSIndexPath *path = [[[NSIndexPath alloc] initWithIndex:index] autorelease];
	
	[path indexPathByAddingIndex:index2];
	
	NSTreeNode *node = [rootNode descendantNodeAtIndexPath:path];
	
	return node;
}

- (BOOL)getItem:(int)index1 withIndex:(int)index2 forTreeNode:(NSTreeNode**)node andFolder:(SmsFolder **)pfolder
{
	NSIndexPath *path = [[NSIndexPath alloc] initWithIndex:index1];
	
	[path indexPathByAddingIndex:index2];
	
	*node = [[rootNode descendantNodeAtIndexPath:path] retain];
	
	if(*node != nil)
	{
		*pfolder = (SmsFolder*)[((treeNode*)[(*node) representedObject]) getTag];
		if(*pfolder)
		{
			[path release];
			return YES;
		}
	}
	
	[path release];
	
	return NO;
}

- (void*)getFolder:(int)row
{
	//todo:
	id item = [outlineView itemAtRow:row];
	treeNode *node = [item representedObject];
	
	if([node getTag] != NULL)
		return [node getTag];
	
	return NULL;
}

- (BOOL)ifSelectedFolder:(SmsFolder *)pFolder
{
	SmsFolder *pFolderSel = [self getSelectedItemData];
	
	if((pFolderSel != NULL) && (pFolderSel == pFolder))
		return YES;
	
	return NO;
}

#pragma mark--
#pragma mark common function
#pragma mark--

- (BOOL)getModify:(NSTreeNode **)hItem withFolder:(SmsFolder **)pFolder
{
	*hItem = [self getSelectedItem];
	
	if(*hItem && pFolder)
	{
		*pFolder = [self getItemData:*hItem];
		if(*pFolder)
		{
			folder_state_struct state;
			(*pFolder)->GetFolderState(state);
			if(state.bModify)
				return YES;
		}
	}
	
	return NO;
}

- (void)onSmsFolderDelete:(id)sender
{
	NSTreeNode *hItem, *hTrash;
	SmsFolder *pFolder, *pFolderTrash;
	
	if([self getTrashCan:&hTrash forFolder:&pFolderTrash] == NO)
		return;
	
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	
	if([self getModify:&hItem withFolder:&pFolder] == NO)
	{
		if(hItem == hTrash)
		{
			if([utility showMessageBox: LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_FOLDER_TRASHDELETE") forType:MB_YESNO] == NSAlertDefaultReturn)
			{
				[pView checkClipboardforFolder:pFolder];
				pFolder->DeleteAll(INCLUDE_SUBFOLDER);
				[pView.smsList deleteAllItems];
				[pView update];
			}
		}
	}
}


@end
