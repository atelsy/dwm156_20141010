//
//  pbCocoa.h
//  DCT
//
//  Created by mbj on 12-1-31.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PhoneBookItem.h"

@interface pbCocoa : NSObject {

@public
	PhoneBookItem *contact;
}

- (id)initWithPbk:(PhoneBookItem*)pContact;

@end