//
//  chTreeControl.h
//  DCT
//
//  Created by mtk on 12-6-6.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "chTreeView.h"
#import "utility.h"

#define CHTREE_TVN_SELCHANGED  @"onSelChangedTreeCH"

@interface chTreeControl : NSObject
{
@private
	NSTreeNode *rootNode;
	NSMutableArray *iconImage;
	IBOutlet chTreeView *outlineView;
}

@property (nonatomic, retain) IBOutlet chTreeView *outlineView;

- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary *)dict;
- (void*)setTreeFolder:(NSString*)name;
- (NSImage*)iconImage:(NSString *)aName;
- (NSArray*)childrenForItem:(id)item;
- (BOOL)treeNode:(NSTreeNode*)Node isDescendantOfNode:(NSTreeNode*)parentNode;

- (ChFolder*)getSelectedItemData;
- (ChFolder*)getItemData:(NSTreeNode*)item;
- (NSTreeNode*)getSelectedItem;
- (BOOL)getMissCallItem:(NSTreeNode**)item andFolder:(ChFolder**)folder;
- (BOOL)getResvCallItem:(NSTreeNode**)item andFolder:(ChFolder**)folder;
- (BOOL)getDialCallItem:(NSTreeNode**)item andFolder:(ChFolder**)folder;
- (BOOL)getIndex:(int)index withItem:(NSTreeNode**)item andFolder:(ChFolder**)folder;
- (void*)getFolder:(int)row;

- (BOOL)ifMissCallFolder:(ChFolder*)pFolder;
- (BOOL)ifResvCallFolder:(ChFolder*)pFolder;
- (BOOL)ifDialCallFolder:(ChFolder*)pFolder;
- (BOOL)inMissCallFolder:(NSTreeNode*)item;
- (BOOL)inResvCallFolder:(NSTreeNode*)item;
- (BOOL)inDialCallFolder:(NSTreeNode*)item;
- (BOOL)ifSelectedFolder:(ChFolder*)pFolder;

- (void)onChCleanCall;
- (void)deleteChildFolder;

@end
