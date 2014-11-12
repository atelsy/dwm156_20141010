//
//  pbTreeCtrl.h
//  DCT
//
//  Created by mbj on 12-1-13.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "pbTreeView.h"
#import "utility.h"

#define PBTREE_TVN_SELCHANGED @"onSelChangedTreePb"

@interface pbTreeCtrl : NSObject 
{
	
@private
	NSTreeNode *rootNode;
	NSMutableArray *iconImage;
	
	NSTreeNode *PCFolderNode;
	NSTreeNode *TransCanNode;
	
	IBOutlet pbTreeView *outlineView;
}

- (void*)SetTreeFolder:(NSString*)name isHandsetfolder:(bool)isHandset;
- (NSImage*)iconImage:(NSString*)name;
- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary*)dict;
- (NSArray*)childrenForItem:(id)item;
- (BOOL)treeNode:(NSTreeNode*)Node isDescendantOfNode:(NSTreeNode*)parentNode;

// select operation
- (NSTreeNode*)GetSelectedItem;
- (PbFolder*)GetSelectedItemData;
//- (void)SetSelectedItem:(id)item;
- (void)SetSelectedItemWithIndex:(NSInteger)index;
- (PbFolder*)GetItemData:(NSTreeNode*)hItem;

// folder operation
- (BOOL)IfHandsetFolder:(PbFolder*)pFolder;
- (void*)GetFolder:(int)row;
- (BOOL)IfSelectedFolder:(PbFolder*)pFolder;
- (BOOL)InTrashCan:(NSTreeNode*)node;
- (BOOL)GetHandsetRoot:(NSTreeNode**)node forFolder:(PbFolder**)pfolder;
- (BOOL)GetPcRoot:(NSTreeNode**)node forFolder:(PbFolder**)pfolder;
- (BOOL)GetTrashCan:(NSTreeNode**)node forFolder:(PbFolder**)pfolder;
//- (BOOL)Getfolder:(int)index withIndex:(int)index2 isHandset:(BOOL)bHandset forTreeNode:(NSTreeNode**)node andFolder:(PbFolder**)pFolder;
- (NSTreeNode*)GetRootNode:(int)index withIndex:(int)index2;
- (BOOL)GetItem:(int)index1 withIndex:(int)index2 forTreeNode:(NSTreeNode**)node andFolder:(PbFolder**)pfolder;
- (id)GetParent;

//others
- (PbFolder*)NewFolder:(NSTreeNode*)hItem;
- (void)OnPbFolderDelete:(id)sender;
- (void)OnPbFolderNew:(id)sender;
- (void)OnPbFolderRename:(id)sender;
- (void)OnPBFolderSendMsg:(id)sender;
- (BOOL)GetModify:(NSTreeNode**)hItem withFolder:(PbFolder**)pFolder;
- (BOOL)IsPCTreeNode:(NSString*)name;
- (void)MoveFolder:(NSTreeNode*)hItem To:(NSTreeNode*)hItemTarget FromFolder: (PbFolder*)pFolder ToFolder:(PbFolder*)pFolderTarget;
- (void)Delete:(NSTreeNode*)hItem Folder:(PbFolder*)pFolder;
- (NSTreeNode*)TreeInsert:(NSDictionary *)dict Folder:(PbFolder*)pFolder;
-(void)TestFolder:(PbFolder*)pFolder;
- (bool)GetPhoneNumber:(CString &)strNum;

@property (retain)IBOutlet pbTreeView *outlineView;


@end
