//
//  KeypadCtrl.h
//  CocoaTry
//
//  Created by MBJ on 12-5-30.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>


#define KEY_CODE_SEND	'X'
#define KEY_CODE_END	'E'
#define KEY_CODE_FRIEND 'F'
#define KEY_CODE_DEL	'<'
#define KEY_CODE_PLUS	'+'
#define KEY_CODE_P		'P'
#define KEY_CODE_W		'W'

#define KEY_MASK_SEND	0x01
#define KEY_MASK_FRIEND	0x02
#define KEY_MASK_NUMBER	0x04
#define KEY_MASK_DEL	0x08
#define KEY_MASK_EDIT	0x10


@protocol KeyPadClient

- (void)onKeyPressed:(char)keyCode 
		  withString:(NSString*)mmiStr;

@end


@interface KeypadCtrl : NSObject<NSTextViewDelegate>
{
	IBOutlet NSTextField *numEdit;
	
	IBOutlet NSButton *	btnSend;
	IBOutlet NSButton *	btnFriend;	
	
	IBOutlet id<KeyPadClient> theClient;
	
	NSRange numSelRange;
	NSRange oldSelRange;

}

-(IBAction)keyPressed:(id)sender;
-(IBAction)delPressed:(id)sender;

-(IBAction)onSend:(id)sender;
-(IBAction)onEnd:(id)sender;
-(IBAction)onFriend:(id)sender;

-(IBAction)onEnter:(id)sender;

-(void)setText:(NSString*)str;
-(void)setFocus;
-(void)selectAll;
-(void)clear;

-(void)enable:(NSUInteger)mask;

- (void)onSelChanged:(NSNotification*)notification;

@end
