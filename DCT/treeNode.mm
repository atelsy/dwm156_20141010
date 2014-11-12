//
//  treeNode.m
//  DCT
//
//  Created by Fengping Yu on 10/18/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "treeNode.h"


@implementation treeNode

- (id)init
{
	self = [super init];
	if (self) 
	{
		// Initialization code here.
		self.name = @"Untitled";
		self.selectable = YES;
		self.expandable = YES;
		self.container = YES;
		self.bEdit = NO;
	}
	
	return self;
}

- (id)initWithName:(NSString *)aName
{
	self = [self init];
	self.name = aName;
	
	return self;
}

- (void)setTag:(void *)_tag
{
	tag = _tag;
}

- (void*)getTag
{
	return tag;
}

+ (treeNode*)nodeDataWithName:(NSString *)aName
{
	return [[[treeNode alloc] initWithName:aName] autorelease];
}

- (void)dealloc
{
	[name release];
	[image release];
	
	[super dealloc];
}

@synthesize name, image, expandable, selectable, container,bEdit;

- (NSString *)description {
	return [NSString stringWithFormat:@"%@ - '%@' expandable: %d, selectable: %d, container: %d", [super description], self.name, self.expandable, self.selectable, self.container];
}

@end
