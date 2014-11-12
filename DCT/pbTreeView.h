//
//  pbTreeView.h
//  DCT
//
//  Created by mbj on 12-1-13.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>



@interface pbTreeView : NSOutlineView
{
	
}

- (void)onPbFolderDelete:(id)sender;
- (void)onPbFolderNew:(id)sender;
- (void)onPbFolderRename:(id)sender;
- (void)onPbFolderSendMsg:(id)sender;

- (NSMenu*)defaultMenuForRow:(int)row;
- (NSMenu*)pbFolderMenu:(int)row;
@end
