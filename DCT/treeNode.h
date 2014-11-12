//
//  treeNode.h
//  DCT
//  implement tree node data representation
//  Created by Fengping Yu on 10/18/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

#define NAME_KEY		@"Name"
#define CHILDREN_KEY	@"Children"

@interface treeNode : NSObject
{
@private
	NSString *name;
	NSImage *image;
	BOOL expandable;
	BOOL selectable;
	BOOL container;
	BOOL bEdit;
	void* tag;
		
	//todo: add member of sms folder information
}

- (id)initWithName:(NSString*)name;
+ (treeNode*)nodeDataWithName:(NSString*)name;
- (void)setTag:(void*)_tag;
- (void*)getTag;

@property(readwrite, copy) NSString *name;
@property(readwrite, retain)NSImage *image;
@property(readwrite, getter = isExpandable)BOOL expandable;
@property(readwrite, getter = isSelectable)BOOL selectable;
@property(readwrite, getter = isContainer)BOOL container;
@property(readwrite, getter = getParent)BOOL bEdit;

@end
