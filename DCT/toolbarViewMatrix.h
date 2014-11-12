//
//  toolbarViewMatrix.h
//  DCT
//  implement mouse click toolbar event, it is delegate for the toolbar
//  Created by Fengping Yu on 10/31/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface toolbarViewMatrix : NSMatrix
{
@private
	
}

- (void)updateImage:(BOOL)bEnter withEvent:(NSEvent*)theEvent;

@end
