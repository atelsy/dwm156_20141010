//
//  pbSelectListCtrl.h
//  DCT
//
//  Created by Fengping Yu on 12/13/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "utility.h"
#import "PhoneBookItem.h"

#define NAMECELL	@"NameCell"
#define PBNUMCELL   @"PhoneNumberCell"

@interface pbSelectData : NSObject
{
@private
	NSString *strName;
	NSString *strNumber;
}

@property(readwrite, retain)NSString *strName;
@property(readwrite, retain)NSString *strNumber;

@end


@interface pbSelectListCtrl : NSView<NSTableViewDelegate, NSTableViewDataSource> 
{
@private
	IBOutlet NSTableView *tableView;
	NSMutableArray *dataSource;
	
}

//- (void)setDelete:(BOOL)bDelete;
- (BOOL)getData:(int)nItem name:(NSString**)strName number:(NSString**)strNumber;
- (void)display:(PbFolder*)pFolder;
- (void)insertItem:(PhoneBookItem*)pItem item:(int)nItem;
- (void)insertItem:(NSString*)strName number:(NSString*)strNumber item:(int)nItem;
- (void)insertItems:(NSArray*)objects;
- (int)getSelectCount;
- (int)getCount;
- (void)deleteItem;
- (void)selectAll;
- (void)selectLastItem;
- (NSArray*)getSelectItems;
- (void)addRow:(pbSelectData*)data;

@property(readwrite, retain)NSTableView *tableView;
@property(readwrite, retain)NSMutableArray *dataSource;

@end
