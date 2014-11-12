//
//  listData.h
//  DCT
//
//  Created by Fengping Yu on 10/19/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "utility.h"

@interface listData : NSObject 
{
@private
	NSString *memory;
	NSString *fromTo;
	NSString *time;
	NSString *content;
	SMSMessage *pMsg;
	
	NSImage *image;
}

- (id)initWithMemory:(NSString*)_memory FromTo: (NSString*)_frmoTo Time:(NSString*)_time Content:(NSString*)_content withMsg:(SMSMessage*)_pMsg;

- (NSComparisonResult)MemoryCellComparison: (listData*)otherListData;
- (NSComparisonResult)FromToCellComparison:(listData*)otherListData;
- (NSComparisonResult)TimeCellComparison:(listData*)otherListData;
- (NSComparisonResult)ContentCellComparison:(listData*)otherListData;

@property (copy)NSString *memory;
@property (copy)NSString *fromTo;
@property (copy)NSString *time;
@property (copy)NSString *content;
@property SMSMessage *pMsg;
@property (retain)NSImage *image;

@end

/*
 @interface listData(sorting) 
 - (NSComparisonResult)MemoryCellComparison: (listData*)otherListData;
 - (NSComparisonResult)FromToCellComparison:(listData*)otherListData;
 - (NSComparisonResult)TimeCellComparison:(listData*)otherListData;
 - (NSComparisonResult)ContentCellComparison:(listData*)otherListData;
 @end
 */