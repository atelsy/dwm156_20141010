//
//  chTreeView.h
//  DCT
//  implement for context menu of tree
//  Created by mtk on 12-6-6.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface chTreeView : NSOutlineView
{
	
}

- (void)onChFolderDelete:(id)sender;
- (NSMenu*)defaultMenuForRow:(int)row;

@end
