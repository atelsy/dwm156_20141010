//
//  pbListData.h
//  DCT
//
//  Created by mbj on 12-1-31.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"

@interface pbListData : NSObject {
	
@private
	NSString *memory;
	NSString *name;
	NSString *mobile;
	PhoneBookItem *pContact;
	
	NSImage *image;
}

- (id)initWithMemory:(NSString *)_memory Name:(NSString *)_name Mobile:(NSString *)_mobile  withContact:(PhoneBookItem *)_pContact;
- (NSComparisonResult)MemoryCellComparison: (pbListData*)otherListData;
- (NSComparisonResult)NameCellComparison:(pbListData*)otherListData;
- (NSComparisonResult)MobileCellComparison:(pbListData*)otherListData;

@property (copy)NSString *memory;
@property (copy)NSString *name;
@property (copy)NSString *mobile;
@property PhoneBookItem *pContact;
@property (retain)NSImage *image;

@end
