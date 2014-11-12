//
//  smsSendController.h
//  DCT
//
//  Created by Fengping Yu on 11/8/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"
#include "ComAgent.h"

@class SMSViewCtrl;

@interface sendStatus : NSObject
{
@private
	NSString *phoneNumber;
	NSString *status;
}

@property (readwrite, copy) NSString *phoneNumber;
@property (readwrite, copy) NSString *status;

@end

@interface smsSendController : NSWindowController<NSWindowDelegate> 
{
@private
	IBOutlet NSProgressIndicator *progress;
	IBOutlet NSButton *cancelButton;
	IBOutlet NSTableView *statusTable;
	IBOutlet NSTextField *ctlStatic;
    
	int m_nIdx;
	int m_npIdx;
	BOOL m_bCancel;
	BOOL m_bFinish;
	
	SmsFolder *m_pFolderOutbox;
	SmsFolder *m_pFolderSentbox;
	std::vector<SMSMessage*> *m_pvMsg;
	std::vector<SMSMessage>m_vMsg;
	std::vector<int> m_vIdx;
	
	CComAgent *m_pComAgent;
	
	// sendstatus array
	NSMutableArray *status;
}

- (IBAction)onCancel:(id)sender;
- (BOOL)initWithMsg:(std::vector<SMSMessage*>*)pvMsg andOutboxFolder:(SmsFolder*)pFolderOutbox SentFoler:(SmsFolder*)pFolderSentbox;

- (void)initCtrl;
- (void)initListStatus:(int)index withStatus:(BOOL)bSuccess;
- (void)initStatic:(int)num;

- (BOOL)doJob;
- (BOOL)getNext;

//message notification
- (void)postNotification:(BOOL)result;
- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
- (void)onSendJobStatus:(NSNotification*)notification;

//others
- (void)setStatus:(NSMutableArray*)_status;
- (SMSViewCtrl*)getSmsViewControl;

@end
