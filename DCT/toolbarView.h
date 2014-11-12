//
//  toolbarView.h
//  DCT
//
//  Created by Fengping Yu on 12/26/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>


extern NSString* enableToolbarImages[];

extern NSString *disableToolbarImages[];

extern NSString *hotToolbarImages[];

extern NSString* pbEnableToolbarImages[];

extern NSString *pbDisableToolbarImages[];

extern NSString *pbHotToolbarImages[];

extern NSString *chEnableToolbarImages[];

extern NSString *chDisableToolbarImages[];

@interface toolbarView : NSCollectionView
{
@private
	NSMutableArray *trackingAreas;
	
	NSInteger m_trackID;
	BOOL m_trackEntered;
}

- (void)removeTrackingAreas;
- (void)setupTrackingAreas;
- (int)getTrackIDFromDictionary:(NSDictionary*)dict;
- (void)updateToolbarImage:(BOOL)bEnter withEvent:(NSEvent*)theEvent;

@property (retain)NSMutableArray *trackingAreas;

@end
