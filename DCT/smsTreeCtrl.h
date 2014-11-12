//
//  TreeCtrl.h
//  DCT
//
//  Created by Fengping Yu on 11-10-14.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "utility.h"
#import "smsTreeView.h"

#define SMSTREE_TVN_SELCHANGED  @"onSelChangedTreeSms"

@interface smsTreeCtrl : NSObject 
{
@private
	NSTreeNode *rootNode;
	NSMutableArray *iconImage;
	
	IBOutlet smsTreeView *outlineView;
	
}

- (void*)setTreeFolder:(NSString*)name isHandsetfolder:(bool)isHandset;
- (NSImage*)iconImage:(NSString*)name;
- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary*)dict;

// select operation
- (NSTreeNode*)getSelectedItem;
- (SmsFolder*)getSelectedItemData;
- (void)setSelectedItem:(id)item;
- (void)setSelectedItemWithIndex:(NSInteger)index;
- (id)getParent;
- (SmsFolder*)getFolderFromName:(NSString*)name;

// folder operation

- (void*)getFolder:(int)row;
- (BOOL)ifSelectedFolder:(SmsFolder*)pFolder;
- (BOOL)ifPcOutbox;
- (BOOL)ifSentbox;
- (BOOL)ifTrashCan;
- (BOOL)inTrashCan:(NSTreeNode*)node;
- (BOOL)ifHandsetOutbox;
- (BOOL)ifHandsetOutbox:(SmsFolder*)pFolder;
- (BOOL)ifHandsetInbox:(SmsFolder*)pFolder;
- (BOOL)ifPcInbox:(SmsFolder*)pFolder;
- (BOOL)ifPcOutbox:(SmsFolder*)pFolder;
- (BOOL)ifPcSentbox:(SmsFolder*)pfolder;
- (BOOL)ifPcDraft:(SmsFolder*)pFolder;
- (BOOL)ifPcPersonalFolder:(SmsFolder*)pFolder;
- (BOOL)ifTrashCan:(SmsFolder*)pFolder;
- (BOOL)getHandsetRoot:(NSTreeNode**)node forFolder:(SmsFolder**)pfolder;
- (BOOL)getPcRoot:(NSTreeNode**)node forFolder:(SmsFolder**)pfolder;
- (BOOL)getHandsetInbox:(NSTreeNode**)node forFolder:(SmsFolder**)pfolder;
- (BOOL)getHandsetOutbox:(NSTreeNode**)node forFolder:(SmsFolder**)pfolder;
- (BOOL)getPcInbox:(NSTreeNode**)node forFolder:(SmsFolder**)pFolder;
- (BOOL)getPcOutbox:(NSTreeNode**)node forFolder:(SmsFolder**)pfolder;
- (BOOL)getSentbox:(NSTreeNode**)node forFolder:(SmsFolder**)pfolder;
- (BOOL)getPcDraft:(NSTreeNode**)node forFolder:(SmsFolder**)pfolder;
- (BOOL)getPersonal:(NSTreeNode**)node forFolder:(SmsFolder**)pfolder;
- (BOOL)getTrashCan:(NSTreeNode**)node forFolder:(SmsFolder**)pfolder;
- (BOOL)getfolder:(int)index withIndex:(int)index2 isHandset:(BOOL)bHandset forTreeNode:(NSTreeNode**)node andFolder:(SmsFolder**)pFolder;
- (NSTreeNode*)getRootNode:(int)index withIndex:(int)index2;
- (BOOL)getItem:(int)index1 withIndex:(int)index2 forTreeNode:(NSTreeNode**)node andFolder:(SmsFolder**)pfolder;
- (SmsFolder*)getItemData:(NSTreeNode*)hItem;

//others
- (void)onSmsFolderDelete:(id)sender;
- (BOOL)getModify:(NSTreeNode**)hItem withFolder:(SmsFolder**)pFolder;

@property (retain)IBOutlet smsTreeView *outlineView;

@end
