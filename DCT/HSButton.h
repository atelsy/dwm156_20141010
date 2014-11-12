//
//  HSButton.h
//  HStarEdit
//
//  Created by Zhuwei on 12-2-8.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <AppKit/AppKit.h>


@interface HSButton : NSButton {
@private
    NSImage*                _enteredImage;
    NSImage*                _exitedImage;
    BOOL                    _entered;

}
@property (nonatomic,assign) BOOL       entered;
@property (nonatomic,retain) NSImage*   enteredImage;
@property (nonatomic,retain) NSImage*   exitedImage;
@end
