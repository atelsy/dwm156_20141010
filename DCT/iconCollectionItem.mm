//
//  iconCollectionItem.m
//  DCT
//  implement collection item, override nscollectionitem
//  Created by Fengping Yu on 10/31/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "iconCollectionItem.h"

@implementation labelCollectionItem

- (id)init
{
	self = [super init];
	
	if(self)
	{
		
	}
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

@end


@implementation iconCollectionItem

- (id)init
{
	self = [super init];
	if (self) {
		// Initialization code here.
	}
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)click:(id)sender
{
	if([self collectionView] && [[self collectionView] delegate] && 
	   [[[self collectionView] delegate] respondsToSelector:@selector(click:)])
	{
		[[[self collectionView] delegate] performSelector:@selector(click:) withObject:self];
	}
}

@end
