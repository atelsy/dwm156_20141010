//
//  smsPopUp.h
//  DCT
//
//  Created by mtk on 12-6-13.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol popupWindowDelegate 

- (void)windowWillStartTimer:(NSNotification*)aNotification;

@end

@interface smsPopUp : NSWindowController
{
	IBOutlet id delegate;
	
	NSString *strAddress;
	NSString *strContent;
	
	IBOutlet NSTextField *address;
	IBOutlet NSTextView *content;
}

@property (nonatomic, assign, readwrite) IBOutlet id delegate;
@property(nonatomic, retain) IBOutlet NSTextField *address;
@property(nonatomic, retain) IBOutlet NSTextView *content;

- (id)delegate;
- (void)setDelegate:(id)anObject;
- (void)startTimer;
- (void)initwithAddress:(NSString *)_address andContent:(NSString *)_content;

extern NSString *windowWillStartTimerNotification;

@end
