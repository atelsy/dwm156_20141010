//
//  pbSelectTreeCtrl.m
//  DCT
//
//  Created by Fengping Yu on 12/14/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "pbSelectTreeCtrl.h"
#import "imageAndTextCell.h"
#import "treeNode.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"

@implementation pbSelectTreeCtrl


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
	[iconImage release];
	[rootNode release];
	[outlineView release];
	
	[super dealloc];
}

@synthesize m_pFolder, outlineView, rootNode, iconImage;

- (void)initWithDictionaryFile:(NSString *)fileName andFolder:(PbFolder *)pFolder
{
	NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:fileName ofType:@"dict"]];
	
	m_pFolder = pFolder;
	
	rootNode = [[self treeNodeFromDictionary:dict] retain];
}

- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary *)dict
{
	NSString *nodeName = [dict objectForKey:NAME_KEY];
	treeNode *nodeData = [treeNode nodeDataWithName:nodeName];
	
	NSTreeNode *result = [NSTreeNode treeNodeWithRepresentedObject:nodeData];
	
	NSArray *children = [dict objectForKey:CHILDREN_KEY];
//	bool isHandsetFolder = [children count] == 2 ? YES: NO;
	
	for(id item in children)
	{
		NSTreeNode *childTreeNode;
		
		if([item isKindOfClass:[NSDictionary class]])
		{
			NSString *nodeName = [item objectForKey:NAME_KEY];
			childTreeNode = [self TreeInsert:item Folder:(PbFolder*)[self setFolder:nodeName]];
			if(childTreeNode != nil)
			{
				treeNode *data = [childTreeNode representedObject];
				PbFolder *pFolder = (PbFolder*)[self setFolder:nodeName];
				[data setTag:pFolder];
			}
			//childTreeNode = [self treeNodeFromDictionary:item];
		}
		else
		{
			treeNode *childNodeData = [[treeNode alloc] initWithName:item];
			childNodeData.container = NO;
		
			//[childNodeData setTag:[self SetTreeFolder:item isHandsetfolder:isHandsetFolder]];
			[childNodeData setTag:[self setFolder:item]];
			
			childTreeNode = [NSTreeNode treeNodeWithRepresentedObject:childNodeData];
			[childNodeData release];
		}
		
		[[result mutableChildNodes] addObject:childTreeNode];
	}
	
	return result;
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

#pragma mark --
#pragma mark nsoutlineview datasource and delegate method
#pragma mark--

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	PbFolder *folder = [self getSelectItemData];
	
	classToObject *object = [[classToObject alloc] init];
	
	object->pbFolder = folder;
	
	NSDictionary *dict = [[NSDictionary alloc] initWithObjectsAndKeys:object, ITEM_FOLDER, nil];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PBSELECT_CHANGE object:self userInfo:dict];
	
	[object release];
	
	[dict release];
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
	treeNode *node = [item representedObject];
	
	objectValue = node.name;
	
	return objectValue;
}

- (NSInteger)outlineView:(NSOutlineView*)outlineView numberOfChildrenOfItem:(id)item
{
	NSArray *children = [self childrenForItem:item];
	
	return [children count];
}

- (BOOL)outlineView:(NSOutlineView*)outlineView shouldExpandItem:(id)item
{
	treeNode *node = [item representedObject];
	
	return node.expandable;
}

- (void)outlineView:(NSOutlineView*)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	treeNode *node = [item representedObject];
	
	if(node.image == nil)
		node.image = [self iconImage:node.name];
	
	imageAndTextCell *imageCell = (imageAndTextCell*)cell;
	[imageCell setImage:node.image];
}

- (BOOL)outlineView:(NSOutlineView*)outlineView shouldSelectItem:(id)item
{
	treeNode *node = [item representedObject];
	
	return node.selectable;
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

- (NSImage*)iconImage:(NSString *)name
{
	NSArray *array = nil;
	
	if(iconImage == nil)
	{
		iconImage = [[NSMutableArray alloc] init];
		
		array = [NSArray arrayWithObjects:@"handset folder.png", @"pc folder.png", @"trash can.png", nil];
		
		for(NSString *name in array)
		{
			NSImage *image = [NSImage imageNamed:name];
			
			if(image != nil)
				[iconImage addObject:image];
		}
	}
	
	for(NSImage *image in iconImage)
	{
		if([[image name] caseInsensitiveCompare:name] == NSOrderedSame)
			return image;
	}
	
	NSImage *imgFolder = [NSImage imageNamed:@"personal folder.png"];

	return imgFolder;
}

- (void*)setFolder:(NSString *)name
{
	if(m_pFolder)
	{
		SubfolderPos pos = m_pFolder->GetFirstSubfolderPos();
		PbFolder *pSubFolder;
		CString folderName;
		NSString *str;
		
		while(pos != 0)
		{
			pSubFolder = m_pFolder->GetNextSubfolder(pos);
			
			if(pSubFolder)
				pSubFolder->GetFolderName(folderName);
			
			str = [NSString getStringFromWchar:folderName];
			
			if([str caseInsensitiveCompare:name] == NSOrderedSame)
			{
				m_pFolder->CloseNextSubfolder(pos);
				return pSubFolder;
			}
		}
		
		m_pFolder->CloseNextSubfolder(pos);
	}
	
	return NULL;
}

#pragma mark--
#pragma mark get operation
#pragma mark--

- (PbFolder*)getItemData:(NSTreeNode *)hItem
{
	if(hItem != nil)
	{
		treeNode *node = [hItem representedObject];
		if(node != nil)
			return (PbFolder*)[node getTag];
	}
	
	return NULL;
}

- (PbFolder*)getSelectItemData
{
	if([[outlineView selectedRowIndexes] count] == 1)
	{
		NSTreeNode *node = [outlineView itemAtRow:[outlineView selectedRow]];
		treeNode *data = [node representedObject];
		
		PbFolder *pFolder = (PbFolder*)[data getTag];
		
		return pFolder;
	}
	
	return NULL;
}

@end
