//
//  cellData.h
//  DCT
//  implement save view toolbar data: image and tooltip
//  Created by Fengping Yu on 10/31/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum _appTag 
{
	PHONEBOOK = 0,
	SMS = 1
}appTag;

@interface iconViewBox : NSBox
{
@private
	IBOutlet id delegate;
}
@end

@interface cellData : NSObject 
{
@private
	NSImage *image;
	NSString *tooltip;
	BOOL bEnable;
	int tag;
	appTag appTag;
}

@property(retain)NSImage *image;
@property(retain)NSString *tooltip;
@property(readwrite, assign)int tag;
@property(readwrite, assign)BOOL bEnable;
@property(readwrite, assign)appTag appTag;

@end

@interface labelCellData : NSObject
{
@private
	NSString *info;
}

@property(retain, readwrite)NSString *info;

@end
