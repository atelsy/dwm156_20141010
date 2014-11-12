//
//  commonTreeControl.m
//  DCT
//
//  Created by Fengping Yu on 11/5/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsSaveTreeControl.h"
#import "imageAndTextCell.h"
#import "treeNode.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"


@implementation smsSaveTreeControl

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
	[rootNode release];
	[iconImages release];
	
	[super dealloc];
}

@synthesize pFolder, outlineView;

- (void)initWithDictionaryFile:(NSString *)dictFile andFolder:(SmsFolder*)_pFolder
{
	NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:dictFile ofType:@"dict"]];
	
	pFolder = _pFolder;
	
	rootNode = [[self treeNodeFromDictionary:dict] retain];
}


- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	NSTreeNode *node = [outlineView itemAtRow:[outlineView selectedRow]];
	treeNode *data = [node representedObject];
	
	SmsFolder *pfolder = (SmsFolder*)[data getTag];
	classToObject *folderObject = [[classToObject alloc] init];
	folderObject->pFolder = pfolder;
	
	NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:folderObject, ITEM_FOLDER, nil];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:OnSelChangedSaveSmsTree object:self userInfo:dict];
	
	[folderObject release];
}

#pragma mark -- 
#pragma mark outline view data source delegate

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

- (BOOL)treeNode:(NSTreeNode*)treenode isDescendantOfNode:(NSTreeNode*)parentnode
{
	while(treenode !=  nil)
	{
		if(treenode == parentnode)
			return YES;
		
		treenode = [treenode parentNode];
	}
	
	return NO;
}

- (NSImage*)iconImage:(NSString *)name
{
	NSArray *array = nil;
	
	if(iconImages == nil)
	{
		iconImages = [[NSMutableArray alloc] init];
		array = [NSArray arrayWithObjects:@"handset folder.png", @"inbox.png",
				 @"outbox.png", @"pc folder.png", @"sent message.png",@"draft.png",
				 @"personal folder.png", @"trash can.png", nil];
		
		for(NSString *name in array)
		{
			NSImage *image = [NSImage imageNamed:name];
			
			if(image != nil)
				[iconImages addObject:image];
		}
	}
	
	for(NSImage *image in iconImages)
	{
		if([[image name] caseInsensitiveCompare:name] == NSOrderedSame)
		{
			return image;
		}
	}
	
	return nil;
}

- (void*)setTreeFolder:(NSString*)name isHandsetfolder:(bool)isHandset
{
	int num = 3, i = 0;
	SubfolderPos pos = pFolder->GetFirstSubfolderPos();
	SmsFolder *pSubFolder, *pSubsubFolder;
	CString folderName;
	NSString *str;
	NSString *trashCan = [[NSBundle mainBundle] localizedStringForKey: @"IDS_SMS_FOLDER9" value:nil table:@"InfoPlist"];
	
	while(i < num && pos != 0)
	{
		pSubFolder = pFolder->GetNextSubfolder(pos);
		
		if(pSubFolder)
		{
			SubfolderPos subpos = pSubFolder->GetFirstSubfolderPos();
			pSubFolder->GetFolderName(folderName);
			str = [NSString getStringFromWchar:folderName];
			
			// for trash can, as it does not have children folder, so judge it in this 
			if([str caseInsensitiveCompare:trashCan] == NSOrderedSame)
			{
				pFolder->CloseNextSubfolder(pos);
				return pSubFolder;
			}
			
			while(subpos)
			{
				pSubsubFolder = pSubFolder->GetNextSubfolder(subpos);
				if(pSubsubFolder)
					pSubsubFolder->GetFolderName(folderName);
				
				str = [NSString getStringFromWchar:folderName];
				
				if(folderName.GetLength() > 0 && [name caseInsensitiveCompare: str] == NSOrderedSame)
				{
					if(i == 0 && !isHandset)
					{
						continue;
					}
					else
					{
						pSubsubFolder->CloseNextSubfolder(subpos);
						
						return pSubsubFolder;
					}
				}
			}
			pSubsubFolder->CloseNextSubfolder(subpos);
		}
		
		i++; 
	}
	
	pFolder->CloseNextSubfolder(pos);
	
	return NULL;
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

#pragma mark--
#pragma mark select operation
#pragma mark--

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

@end
