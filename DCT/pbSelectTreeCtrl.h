//
//  pbSelectTreeCtrl.h
//  DCT
//
//  Created by Fengping Yu on 12/14/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "utility.h"

#define PBSELECT_CHANGE @"OnSelChangedTreePb"

@interface pbSelectTreeCtrl : NSObject<NSOutlineViewDataSource, NSOutlineViewDelegate> 
{
@private
	
	NSTreeNode *rootNode;
	NSMutableArray *iconImage;
	
	IBOutlet NSOutlineView *outlineView;
	PbFolder *m_pFolder;
	
}

@property(readwrite, assign) PbFolder *m_pFolder;
@property(readwrite, retain)NSOutlineView *outlineView;
@property(nonatomic, retain)NSTreeNode *rootNode;
@property(nonatomic, retain)NSMutableArray *iconImage;

- (NSImage*)iconImage:(NSString*)name;
- (void*)setFolder:(NSString*)name;
- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary*)dict;
- (void)initWithDictionaryFile:(NSString*)fileName andFolder:(PbFolder*)pFolder;
- (PbFolder*)getItemData:(NSTreeNode*)hItem;
- (PbFolder*)getSelectItemData;
- (NSTreeNode*)TreeInsert:(NSDictionary *)dict Folder:(PbFolder*)pFolder;

@end
