//
//  smsReaderController.h
//  DCT
//
//  Created by Fengping Yu on 10/25/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"
#import "smsTextView.h"
#import "emsEdit.h"

enum smsReadToolbarTag
{
	NEW_TAG = 0,
	REPLY_TAG   = 1,
	FORWARD_TAG,
};


@interface smsReaderController : NSWindowController<NSWindowDelegate>
{
@private
	NSString *m_strFrom;
	NSString *m_strTime;
	NSString *m_strMessage;
	SMSMessage *m_pMsg;
	int m_nIndex;
	IBOutlet emsEdit *m_ctlEditMessage;
	IBOutlet NSTextField *m_number;
	IBOutlet NSTextField *m_time;
}

- (SMSMessage*)initMessage;
- (void)initWithAddress:(NSString*)from time:(NSString*)time andMessageContent:(NSString*)message forMsg:(SMSMessage*)pmsg andIndex:(int)index;

- (IBAction)onSmsNew:(id)sender;
- (IBAction)onSmsReply:(id)sender;
- (IBAction)onSmsForward:(id)sender;

@end
