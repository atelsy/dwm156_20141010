//
//  smsTreeView.h
//  DCT
//
//  Created by Fengping Yu on 12/6/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface smsTreeView : NSOutlineView 
{
@private
	NSMenu* menu;
	
}

- (void)onSmsFolderDelete:(id)sender;
- (NSMenu*)defaultMenuForRow:(int)row;

@property (nonatomic, retain) NSMenu* menu;

@end
