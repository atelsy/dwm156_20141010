//
//  pbTreeCtrl.mm
//  DCT
//
//  Created by mbj on 12-1-13.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//
#include "Win2Mac.h"
#import "pbTreeCtrl.h"
#import "imageAndTextCell.h"
#import "treeNode.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"

@implementation pbTreeCtrl

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
		NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"contactTree" ofType:@"dict"]];
		
		[[TheDelegate getPbViewController] InitFolder];
		
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
	[[NSNotificationCenter defaultCenter] postNotificationName:PBTREE_TVN_SELCHANGED object:self];
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

- (void)outlineView:(NSOutlineView *)outlineView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn byItem:(id)item;
{
	treeNode *node = [item representedObject];
	node.name = object;
	
	PbFolder *pFolder = [self GetItemData:item];
	if(pFolder)
		pFolder->SetFolderName([NSString NSStringToCString:object]);
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
	treeNode *nodeData = [item representedObject];
	return nodeData.bEdit;
}

- (BOOL)treeNode:(NSTreeNode*)Node isDescendantOfNode:(NSTreeNode*)parentNode
{
	while(Node != nil)
	{
		if(Node == parentNode)
			return YES;
		
		Node = [Node parentNode];
	}
	
	return NO;
}

- (void*)SetTreeFolder:(NSString*)name isHandsetfolder:(bool)isHandset
{
	PbFolder *folder = [TheDelegate getPbFolder];
	int num = 3, i = 0;
	PbFolder *pSubFolder;
	CString foldername;
	NSString *str;
	
	SubfolderPos pos = folder->GetFirstSubfolderPos();
	
	while(i < num && pos != 0)
	{
		pSubFolder = folder->GetNextSubfolder(pos);
		
		if(pSubFolder)
		{
			pSubFolder->GetFolderName(foldername);
			str = [NSString getStringFromWchar:foldername];
			
			if([str caseInsensitiveCompare:name] == NSOrderedSame)
			{
				folder->CloseNextSubfolder(pos);				
				return pSubFolder;
			}
		}
		
		i++;
	}
	
	folder->CloseNextSubfolder(pos);
	
	return NULL;
}

- (NSImage*)iconImage:(NSString *)aName
{
	NSArray *array = nil;
	
	if(iconImage == nil)
	{
		iconImage = [[NSMutableArray alloc] init];
		
		
		array = [NSArray arrayWithObjects:@"handset folder.png",
				 @"pc folder.png", @"trash can.png", nil];
		
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
	
	NSImage *imgFolder = [NSImage imageNamed:@"personal folder.png"];
	return imgFolder;
}

- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary *)dict
{
	NSString *nodeName = [dict objectForKey:NAME_KEY];
	treeNode *nodeData = [treeNode nodeDataWithName:nodeName];
	
	NSTreeNode *result = [NSTreeNode treeNodeWithRepresentedObject:nodeData];
	
	// walk through dictionary and create treenode for each children
	NSMutableArray *children = [dict objectForKey:CHILDREN_KEY];
	bool isHandsetFolder = [children count] == 2 ? YES: NO;
	
	for(id item in children)
	{
		NSTreeNode *childTreeNode = nil;
		
		if([item isKindOfClass:[NSDictionary class]])
		{
			NSString *nodeName = [item objectForKey:NAME_KEY];
			//childTreeNode = [self treeNodeFromDictionary:item];
			childTreeNode = [self TreeInsert:item Folder:(PbFolder*)[self SetTreeFolder:nodeName isHandsetfolder:isHandsetFolder]];
			if(childTreeNode != nil)
			{
				treeNode *data = [childTreeNode representedObject];
				PbFolder *pFolder = (PbFolder*)[self SetTreeFolder:nodeName isHandsetfolder:isHandsetFolder];
				[data setTag:pFolder];
			}
		}
		else
		{
			treeNode *childNodeData = [[treeNode alloc] initWithName:item];
			childNodeData.container = NO;
			[childNodeData setTag:[self SetTreeFolder:item isHandsetfolder:isHandsetFolder]];
			
			childTreeNode = [NSTreeNode treeNodeWithRepresentedObject:childNodeData];
				
			[childNodeData release];
		}
		
		[[result mutableChildNodes] addObject:childTreeNode];
		
	}
	
	return result;
}

-(void)TestFolder:(PbFolder*)pFolder
{
	if(pFolder)
	{
		PbFolder *pSubfolder;
		SubfolderPos pos = pFolder->GetFirstSubfolderPos();
		while(pos != 0)
		{
			pSubfolder = pFolder->GetNextSubfolder(pos);
			if(pSubfolder)
			{
				[self TestFolder:pSubfolder];
			}
		}
		pFolder->CloseNextSubfolder(pos);
	}
}

- (NSTreeNode*)TreeInsert:(NSDictionary *)dict Folder:(PbFolder*)pFolder
{
	CString strName;
	NSString *nodeName = [dict objectForKey:NAME_KEY];
	treeNode *nodeData = [treeNode nodeDataWithName:nodeName];
	
	NSTreeNode *result = [NSTreeNode treeNodeWithRepresentedObject:nodeData];
	
	if(pFolder)
	{
		PbFolder *pSubfolder;
		SubfolderPos pos = pFolder->GetFirstSubfolderPos();
		
		while(pos != 0)
		{
			NSTreeNode *childTreeNode = nil;
			pSubfolder = pFolder->GetNextSubfolder(pos);
			
			if(pSubfolder)
			{				
				pSubfolder->GetFolderName(strName);
				treeNode *childNodeData = [[treeNode alloc] initWithName:[NSString CStringToNSString:strName]];
				childNodeData.container = NO;
				childNodeData.bEdit = YES;
				[childNodeData setTag:pSubfolder];
			
				childTreeNode = [NSTreeNode treeNodeWithRepresentedObject:childNodeData];
			
				[childNodeData release];
				
				[[result mutableChildNodes] addObject:childTreeNode];
			}
		}
	}
	
	return result;
}

- (BOOL)IsPCTreeNode:(NSString*)name
{
	NSString *str = LoadString(@"IDS_PB_FOLDER1");
	if([str caseInsensitiveCompare:name] == NSOrderedSame)
		return YES;
	
	return NO;
}

#pragma mark --
#pragma mark select operation
#pragma mark --

- (PbFolder*)GetItemData:(NSTreeNode *)hItem
{
	if(hItem != nil)
	{
		treeNode *node = [hItem representedObject];
		if(node != NULL)
			return (PbFolder*)[node getTag];
	}
	
	return NULL;
}

- (NSTreeNode*)GetSelectedItem
{
	int item = (int)[outlineView selectedRow];
	
	return [outlineView itemAtRow:item];
}

- (PbFolder*)GetSelectedItemData
{
	NSTreeNode *node = [self GetSelectedItem];
	
	return (PbFolder*)[[node representedObject] getTag];
}

- (void)GetSelectedItem:(id)item
{
	NSInteger itemIndex = [outlineView rowForItem:item];
	
	if(itemIndex < 0)
		return;
	
	[outlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:itemIndex] byExtendingSelection:NO];
}

- (void)SetSelectedItemWithIndex:(NSInteger)index
{
	[outlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:index] byExtendingSelection:NO];
}

#pragma mark--
#pragma mark folder judgement
#pragma mark--

- (BOOL)InTrashCan:(NSTreeNode *)node
{
	NSTreeNode *hTrash;
	PbFolder *pFolderTrash;
	
	if([self GetTrashCan:&hTrash forFolder:&pFolderTrash])
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

- (BOOL)GetHandsetRoot:(NSTreeNode**)node forFolder:(PbFolder **)pfolder
{
	return [self GetItem:0 withIndex:0 forTreeNode:node andFolder:pfolder];
}

- (BOOL)GetPcRoot:(NSTreeNode**)node forFolder:(PbFolder **)pfolder
{
	return [self GetItem:1 withIndex:0 forTreeNode:node andFolder:pfolder];
}

- (BOOL)GetTrashCan:(NSTreeNode**)node forFolder:(PbFolder **)pfolder
{
	return [self GetItem:2 withIndex:0 forTreeNode:node andFolder:pfolder];
}



- (NSTreeNode*)GetRootNode:(int)index withIndex:(int)index2
{
	NSIndexPath *path = [[[NSIndexPath alloc] initWithIndex:index] autorelease];
	
	[path indexPathByAddingIndex:index2];
	
	NSTreeNode *node = [[rootNode descendantNodeAtIndexPath:path] retain];
	
	return node;
}

- (BOOL)GetItem:(int)index1 withIndex:(int)index2 forTreeNode:(NSTreeNode**)node andFolder:(PbFolder **)pfolder
{
	NSIndexPath *path = [[[NSIndexPath alloc] initWithIndex:index1] autorelease];
	
	[path indexPathByAddingIndex:index2];
	
	*node = [[rootNode descendantNodeAtIndexPath:path] retain];
	
	if(*node != nil)
	{
		*pfolder = (PbFolder*)[((treeNode*)[(*node) representedObject]) getTag];
		if(*pfolder)
			return YES;
	}
	
	return NO;
}

- (void*)GetFolder:(int)row
{
	//todo:
	id item = [outlineView itemAtRow:row];
	treeNode *node = [item representedObject];
	
	if([node getTag] != NULL)
		return [node getTag];
	
	return NULL;
}

- (BOOL)IfSelectedFolder:(PbFolder *)pFolder
{
	PbFolder *pFolderSel = [self GetSelectedItemData];
	
	if((pFolderSel != NULL) && (pFolderSel == pFolder))
		return YES;
	
	return NO;
}

- (BOOL)IfHandsetFolder:(PbFolder*)pFolder
{
	NSTreeNode *hHandset;
	PbFolder *pFolderHandset;	
	
	if([self GetHandsetRoot:&hHandset forFolder:&pFolderHandset])
	{
		if(pFolder == pFolderHandset)
			return YES;
	}
	
	return NO;
}

#pragma mark--
#pragma mark common function
#pragma mark--

- (BOOL)GetModify:(NSTreeNode **)hItem withFolder:(PbFolder **)pFolder
{
	*hItem = [self GetSelectedItem];
	
	if(*hItem && pFolder)
	{
		*pFolder = [self GetItemData:*hItem];
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

- (void)OnPbFolderDelete:(id)sender
{
	NSTreeNode *hItem, *hTrash;
	PbFolder *pFolder, *pFolderTrash;
	
	// Can get the Trash Can folder	
	if(![self GetTrashCan:&hTrash forFolder:&pFolderTrash])
		return;
	
	pbViewControl *pView = (pbViewControl*)[self GetParent];
	
	if(![self GetModify:&hItem withFolder:&pFolder])
	{
		// Check if Trash Can
		if(hItem == hTrash)
		{
			if([utility showMessageBox: LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_FOLDER_TRASHDELETE") forType:MB_YESNO] == NSAlertDefaultReturn)
			{
				[pView CheckClipboardFolder:pFolder];
				pFolder->DeleteAll(INCLUDE_SUBFOLDER);
				[pView.pbList deleteAllItems];				
				[pView Update];
				
				[[hItem mutableChildNodes] removeAllObjects];
				[outlineView reloadData];
				
				PostMessage([TheDelegate getCallhistoryViewController], WM_REFRESH_LIST, 0, 0);
			}
		}
	}
	else 
	{
		if([self InTrashCan:hItem])
		{
			if([utility showMessageBox: LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_FOLDER_TRASHDELETE") forType:MB_YESNO] == NSAlertDefaultReturn)
			{
				[pView CheckClipboardFolder:pFolder];
				[self Delete:hItem Folder:pFolder];
			}
			return;
		}

		NSInteger result = [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_PB_MOVEFOLDER") forType:MB_YESNOCANCEL];
		switch(result)
		{
			case NSAlertDefaultReturn:
				[self MoveFolder:hItem To:hTrash FromFolder:pFolder ToFolder:pFolderTrash];
				break;
	
			case NSAlertAlternateReturn:
				[pView CheckClipboardFolder:pFolder];
				[self Delete:hItem Folder:pFolder];
				PostMessage([TheDelegate getCallhistoryViewController], WM_REFRESH_LIST, 0, 0);
				break;
		}
	}
	
}

- (void)OnPbFolderNew:(id)sender
{
	NSTreeNode *hItem = [self GetSelectedItem];
	
	if([self NewFolder:hItem])
	{
		//[self OnPbFolderRename]
	}
}

- (PbFolder*)NewFolder:(NSTreeNode*)hItem
{
	PbFolder *pFolder = [self GetItemData:hItem];
	if(hItem && pFolder)
	{
		PbFolder *pSubFolder = [utility newPbFolder];
		if(pSubFolder)
		{			
			//NSString *strName = LoadString(@"IDS_PB_NEWFOLDER");
			CString strName = [NSString NSStringToCString:LoadString(@"IDS_PB_NEWFOLDER")];
			pSubFolder->SetFolderName(strName);
			
			//set folder state
			folder_state_struct state;
			pFolder->GetFolderState(state);
			state.bModify = true;
			//state.nImage = USER_IMAGE
			//state.nSelImage = USER_IMAGE
			pSubFolder->SetFolderState(state);
			
			//insert subFolder
			pFolder->InsertSubfolder(pSubFolder);
			
			//Set tag			
			treeNode *childNodeData = [[treeNode alloc] initWithName:LoadString(@"IDS_PB_NEWFOLDER")];
			childNodeData.container = NO;
			childNodeData.bEdit = YES;
			[childNodeData setTag:pSubFolder];
			
			NSTreeNode *newNode = [NSTreeNode treeNodeWithRepresentedObject:childNodeData];
			
			NSTreeNode *pcNode;
			PbFolder *pFolder;
			
			[self GetPcRoot:&pcNode forFolder:&pFolder];
			[[pcNode mutableChildNodes] addObject:newNode];
			
			[childNodeData release];			
			[outlineView reloadData];
			
		}
	}
	
	return NULL;
}

- (void)OnPbFolderRename:(id)sender
{	
	int nRow = (int)[outlineView selectedRow];
	[outlineView editColumn:0 row:nRow withEvent:nil select:YES];
}

- (void)OnPBFolderSendMsg:(id)sender
{
	CString strNum;
	if([self GetPhoneNumber:(CString&)strNum])
	{
		[[TheDelegate getSmsViewController] sendNewMessage:[NSString CStringToNSString:strNum]];
	}
}

- (bool)GetPhoneNumber:(CString &)strNum
{
	CString sName, sNumber;
	PbFolder *pFolder = [self GetSelectedItemData];
	if(pFolder->CountContacts(pFolder) < 20)
	{
		PhoneBookItem *pItem = NULL;
		ItemPos pos = pFolder->GetFirstItemPos();
		
		while(pos != 0)
		{
			if((pItem = pFolder->GetNextItem(pos)))
			{
				pItem->GetPhone(sName, sNumber);
				strNum += sNumber;
				strNum += _T(";");
			}
		}
		pFolder->CloseNextItem(pos);
		return true;
	}
	
	return false;
}

- (void)MoveFolder:(NSTreeNode*)hItem To:(NSTreeNode*)hItemTarget FromFolder: (PbFolder*)pFolder ToFolder:(PbFolder*)pFolderTarget
{
	if(!hItem || !hItemTarget) return;
	if(!pFolder || !pFolderTarget) return;
	
	// Check if move to self
	if(hItem == hItemTarget) return;
	
	// Check if move to parent
	PbFolder *pFolderParent = pFolder->GetParentFolder();	
	if(pFolderParent != pFolderTarget)
	{
		// Move Tree Item

		NSTreeNode *pcNode;
		PbFolder *folder;
		
		[self GetTrashCan:&pcNode forFolder:&folder];
		treeNode *data = [hItem representedObject];
		data.bEdit = false;
		[[pcNode mutableChildNodes] addObject:hItem];
		
		[self GetPcRoot:&pcNode forFolder:&folder];
		[[pcNode mutableChildNodes] removeObject:hItem];
		
		[outlineView reloadData];
		
		// Move Subfolder
		pFolderParent->MoveSubfoldertoFolder(pFolder, pFolderTarget);
		
		pbViewControl *pView = (pbViewControl*)[self GetParent];
		[pView.pbList deleteAllItems];				
		[pView Update];
		
		folder_state_struct state;
		pFolder->GetFolderState(state);
		state.bModify = false;
		pFolder->SetFolderState(state);		
	}
}

- (void)Delete:(NSTreeNode*)hItem Folder:(PbFolder*)pFolder
{
	if(!hItem || !pFolder) return;
	
	// Delete Tree Item
	NSTreeNode *pcNode;
	PbFolder *folder;
	[self GetPcRoot:&pcNode forFolder:&folder];
	[[pcNode mutableChildNodes] removeObject:hItem];	
	[outlineView reloadData];
		
	// Delete Folder
	PbFolder *pParentFolder = pFolder->GetParentFolder();
	if(pParentFolder)
		pParentFolder->DeleteSubfolder(pFolder);
}

#pragma mark--
#pragma mark help methods
#pragma mark--

- (id)GetParent
{
	return [TheDelegate getPbViewController];
}

@end
