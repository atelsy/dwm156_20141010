//
//  chListData.h
//  DCT
//
//  Created by mtk on 12-6-9.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "utility.h"

@interface chListData : NSObject
{
@private
	NSString *time;
	NSString *name;
	NSString *number;
	PhoneBookItem *pItem;
}

- (id)initWithName:(NSString*)_name Time:(NSString*)_time Number:(NSString*)_number withItem:(PhoneBookItem*)_item;

@property (copy) NSString *name;
@property (copy) NSString *time;
@property (copy) NSString *number;
@property (assign) PhoneBookItem *pItem;

@end
