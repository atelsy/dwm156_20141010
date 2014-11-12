//
//  smsReadDlg.h
//  DCT
//
//  Created by Fengping Yu on 12/17/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"
#include "ComAgent.h"

//#define WM_SMSREAD_STATUS @"onJobStatus"

enum sms_read
{
	READ_SM = 0,
	READ_ME = 1,
	READ_RET,
};

@interface smsReadDlg : NSWindowController 
{
@private
	
	IBOutlet NSProgressIndicator *m_ctlProgress;
	IBOutlet NSButton *m_ctlButton;
	IBOutlet NSTextField *m_ctlStatic;
	
	SmsFolder *m_pFolderInbox;
	SmsFolder *m_pFolderOutbox;
	SMSMessage m_Msg;
	CComAgent *m_pCom;
	
	int m_nIdx;
	int m_nItem;
	int m_nReadItem;
	int m_nSimTotal;
	BOOL m_bCancel;
	BOOL m_bMissed;
	
	NSMutableArray *m_vMemory;
	NSMutableArray *m_vCurMemType;
	
	//   std::vector<CString> m_vMemory;
	//std::vector<CString> m_vCurMemType;
	std::vector<int> m_vUsed;
	std::vector<int> m_vBgn;
	std::vector<int> m_vEnd;
}

- (BOOL)init:(SmsFolder*)pFolderInbox andOutbox:(SmsFolder*)pFolderOutbox;
- (void)initCtrl;
- (void)doJob;
- (void)setTotalNumber:(BOOL)flag 
				memory:(NSString*)memory 
			withVector:(const std::vector<int>&)vec;
- (void)setRcvMsgFlag:(const CMsgInd&)urc;
- (BOOL)getRcvMsgFlag;

- (void)onJobStatus:(NSDictionary*)dict;
- (BOOL)getNext;
- (void)insertMessage:(SmsFolder*)pFolder;

- (void)postSmsReadMessage:(int)nIndex status:(BOOL)status;
- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;

- (IBAction)onCancel:(id)sender;
- (void)disableButton;

//- (void)updateMessageStatusInex;

@end;
