//
//  ImageTextCell.h
//  SofaControl
//
//  Created by Martin Kahr on 10.10.06.
//  Copyright 2006 CASE Apps. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface ImageTextCell : NSTextFieldCell 
{
	NSString* primaryTextKey;

	NSImage *icon;
}

- (void) setIcon: (NSImage*) image;
- (void) setPrimaryTextKey: (NSString*) text;

@end
