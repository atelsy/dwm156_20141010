//
//  chTreeControl.m
//  DCT
//
//  Created by mtk on 12-6-6.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import "chTreeControl.h"
#import "imageAndTextCell.h"
#import "DCTAppDelegate.h"
#import "treeNode.h"
#import "nsstrex.h"

@implementation chTreeControl

@synthesize outlineView;

#pragma mark - init function

- (id)init
{
	self = [super init];
	
	if(self)
	{
		NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"chTree" ofType:@"plist"]];
		
		[[TheDelegate getCallhistoryViewController] initFolder];
		
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

#pragma mark - nsoutlineview datasource and delegate

- (void)outlineViewSelectionDidChange:(NSNotification*)notification
{
	[[NSNotificationCenter defaultCenter] postNotificationName:CHTREE_TVN_SELCHANGED object:self];
}

- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary *)dict
{
	NSString *nodeName = [dict objectForKey:NAME_KEY];
	treeNode *node = [treeNode nodeDataWithName:nodeName];
	
	NSTreeNode *result = [NSTreeNode treeNodeWithRepresentedObject:node];
	
	NSArray *children = [dict objectForKey:CHILDREN_KEY];
	
	for(id item in children)
	{
		NSTreeNode *childNode;
		
		if([item isKindOfClass:[NSDictionary class]])
		{
			childNode = [self treeNodeFromDictionary:item];
		}
		else
		{
			treeNode *childeNodeData = [[treeNode alloc] initWithName:item];
			childeNodeData.container = NO;
			[childeNodeData setTag:[self setTreeFolder:item]];
			
			childNode = [NSTreeNode treeNodeWithRepresentedObject:childeNodeData];
			[childeNodeData release];
		}
		
		[[result mutableChildNodes] addObject:childNode];
	}
	
	return  result;
}

- (void*)setTreeFolder:(NSString*)name
{
	ChFolder *folder = [TheDelegate getChFolder];
	int num = 3, i = 0;
	
	ChFolder *pSubFolder;
	CString foldername;
	NSString *str;
	
	SubfolderPos pos = folder->GetFirstSubfolderPos();
	
	while (i < num && pos != 0)
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
		
		array = [NSArray arrayWithObjects:@"missed calls.png",
				 @"received calls.png", @"dialled calls.png", nil];
		
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

#pragma mark - get folder

- (ChFolder*)getItemData:(NSTreeNode *)item
{
	if(item != nil)
	{
		treeNode *node = [item representedObject];
		
		if(node)
			return (ChFolder*)[node getTag];
	}
	
	return NULL;
}

- (NSTreeNode*)getSelectedItem
{
	NSInteger item = [outlineView selectedRow];
	
	return [outlineView itemAtRow:item];
}

- (void*)getFolder:(int)row
{
	id item = [outlineView itemAtRow:row];
	treeNode *node = [item representedObject];
	
	if([node getTag] != NULL)
		return [node getTag];
	
	return NULL;
}

- (ChFolder*)getSelectedItemData
{
	NSTreeNode *item = [self getSelectedItem];
	
	return [self getItemData:item];
}

- (BOOL)getIndex:(int)index withItem:(NSTreeNode **)item andFolder:(ChFolder **)folder
{
	NSIndexPath *path = [[[NSIndexPath alloc] initWithIndex:index] autorelease];
	
	*item = [[rootNode descendantNodeAtIndexPath:path] retain];
	
	if(*item != nil)
	{
		*folder = (ChFolder*)[((treeNode*)[(*item) representedObject]) getTag];
		if(*folder)
			return YES;
	}
	
	return NO;
}

- (BOOL)getMissCallItem:(NSTreeNode **)item andFolder:(ChFolder **)folder
{
	return [self getIndex:0 withItem:item andFolder:folder];
}

- (BOOL)getResvCallItem:(NSTreeNode **)item andFolder:(ChFolder **)folder
{
	return [self getIndex:1 withItem:item andFolder:folder];
}

- (BOOL)getDialCallItem:(NSTreeNode **)item andFolder:(ChFolder **)folder
{
	return [self getIndex:2 withItem:item andFolder:folder];
}

#pragma mark - folder detect

- (BOOL)ifMissCallFolder:(ChFolder *)pFolder
{
	NSTreeNode *node;
	ChFolder *folder;
	
	if([self getMissCallItem:&node andFolder:&folder])
	{
		if(folder == pFolder)
			return YES;
	}
	
	return NO;
}

- (BOOL)ifResvCallFolder:(ChFolder *)pFolder
{
	NSTreeNode *node;
	ChFolder *folder;
	
	if([self getResvCallItem:&node andFolder:&folder])
	{
		if(folder == pFolder)
			return YES;
	}
	
	return NO;
}

- (BOOL)ifDialCallFolder:(ChFolder *)pFolder
{
	NSTreeNode *node;
	ChFolder *folder;
	
	if([self getDialCallItem:&node andFolder:&folder])
	{
		if(folder == pFolder)
			return YES;
	}
	
	return NO;
}

- (BOOL)inMissCallFolder:(NSTreeNode *)item
{
	NSTreeNode *node;
	ChFolder *folder;
	
	if([self getMissCallItem:&node andFolder:&folder])
	{
		if(item == node)
			return YES;
	}
	
	return NO;
}

- (BOOL)inResvCallFolder:(NSTreeNode *)item
{
	NSTreeNode *node;
	ChFolder *folder;
	
	if([self getResvCallItem:&node andFolder:&folder])
	{
		if(item == node)
			return YES;
	}
	
	return NO;
}

- (BOOL)inDialCallFolder:(NSTreeNode*)item
{
	NSTreeNode *node;
	ChFolder *folder;
	
	if([self getDialCallItem:&node andFolder:&folder])
	{
		if(item == node)
			return YES;
	}
	
	return NO;
}

- (BOOL)ifSelectedFolder:(ChFolder *)pFolder
{
	ChFolder *folder = [self getSelectedItemData];
	
	if(folder != NULL && folder == pFolder)
		return YES;
	
	return NO;
}

#pragma mark - context menu action

- (void)onChCleanCall
{
	ChFolder *folder = [self getSelectedItemData];
	
	NSString *caption = nil;
	
	if([self ifMissCallFolder:folder])
		caption = LocalizeString(@"IDS_CH_MISSCALLFOLDER_CLEAN");
	else if([self ifResvCallFolder:folder])
		caption = LocalizeString(@"IDS_CH_RESVCALLFOLDER_CLEAN");
	else if([self ifDialCallFolder: folder])
		caption = LocalizeString(@"IDS_CH_DIALCALLFOLDER_CLEAN");
	
	if([utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:caption forType:MB_YESNO] == NSAlertDefaultReturn)
		[self deleteChildFolder];
}

- (void)deleteChildFolder
{
	DCTAppDelegate *delegate = TheDelegate;
	callHistoryViewController *pView = delegate.chController;
	
	pView.m_bClean = YES;
	
	[pView.chList onChDelete];
	
	pView.m_bClean = NO;
}

@end
