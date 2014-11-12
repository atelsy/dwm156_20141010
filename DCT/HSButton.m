//
//  HSButton.m
//  HStarEdit
//
//  Created by Zhuwei on 12-2-8.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "HSButton.h"
@implementation HSButton
@synthesize enteredImage = _enteredImage;
@synthesize exitedImage = _exitedImage;
@synthesize entered = _entered;
- (void)awakeFromNib {
    _enteredImage = nil;
    _exitedImage = nil;
    if(self.image) _exitedImage = [self.image retain];
    if(self.alternateImage) _enteredImage = [self.alternateImage retain];
    
    [self addTrackingRect:NSMakeRect(5,5, self.frame.size.width - 10, self.frame.size.height - 10) owner:self userData:Nil assumeInside:YES];
}

- (void)dealloc {
    if(_enteredImage) [_enteredImage release];
    if(_exitedImage) [_exitedImage release];
    [super dealloc];
}

- (void)mouseEntered:(NSEvent *)theEvent {
    _entered = YES;
    [self setImage:_enteredImage];
      [self setToolTip:(self.title)];
    [self display];
    
}

- (void)mouseExited:(NSEvent *)theEvent {
    _entered = NO;
    [self setImage:_exitedImage];
    [self display];    
}

@end
