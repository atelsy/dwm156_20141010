//
//  commonTreeControl.h
//  DCT
//  implement common tree control behavior
//  Created by Fengping Yu on 11/5/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "utility.h"

#define OnSelChangedSaveSmsTree @"OnSelChangeTreeSms"

@interface smsSaveTreeControl : NSObject 
{
@private
	NSTreeNode *rootNode;
	NSMutableArray *iconImages;
	
	IBOutlet NSOutlineView *outlineView;
	
	SmsFolder *pFolder;
}

@property (readwrite) SmsFolder *pFolder;
@property (readonly, retain) NSOutlineView *outlineView;

- (NSImage*)iconImage:(NSString*)name;
- (void*)setTreeFolder:(NSString*)name isHandsetfolder:(bool)isHandset;
- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary*)dict;
- (void)initWithDictionaryFile:(NSString*)dictFile andFolder:(SmsFolder*)_pFolder;
- (SmsFolder*)getItemData:(NSTreeNode *)hItem;
- (NSTreeNode*)getSelectedItem;
- (SmsFolder*)getSelectedItemData;
- (void)setSelectedItem:(id)item;

@end
