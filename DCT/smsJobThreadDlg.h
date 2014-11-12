//
//  smsJobThreadDlg.h
//  DCT
//
//  Created by Fengping Yu on 12/10/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"
#include "ComAgent.h"

@class SMSViewCtrl;

enum TYPE
{
	JOB_SMSDELETE = 0,
	JOB_SMSINSERT = 1,
	JOB_SMSUPDATE,
	JOB_SMSCOPY,
	JOB_SMSDELETEMOVE,
	JOB_SMSINSERTMOVE
};

enum CTLSMSTYPE
{
	CTL_SMSNULL = 0,
	CTL_SMSHSTOPC,
	CTL_SMSMOVEPCTOHS,
	CTL_SMSCOPYPCTOHS,
	CTL_SMSHSTOHS
};

typedef struct tagSmsFolders
{
	SmsFolder *pFolderFrom;
	SmsFolder *pFolderTo;
	std::vector<SMSMessage*> *pvMsg;
}SmsFolders;

typedef struct tagSmsJobPara
{
	NSString *strTitle;
	std::vector<SMSMessage*> *pvMessage;
	int job;
	SmsFolder *pFolderFrom;
	SmsFolder *pFolder;
	bool bIsInbox;
	bool bIsFromPC;
	NSString *sMemory;
	CTLSMSTYPE nCtrlType;
}SmsJobPara;

#define WM_SMSTHREAD_DELETE @"smsThreadDelete"
#define WM_SMSTHREAD_INSERT @"smsThreadInsert"
#define WM_SMSTHREAD_UPDATE @"smsThreadUpdate"
#define WM_SMSTHREAD_DELETEMOVE @"smsThreadDeleteMove"
#define WM_SMSTHREAD_INSERTMOVE @"smsThreadInsertMove"
#define WM_DESTROY_DIALOG   @"smsDestroyDialog"
#define WM_SMSTHREAD_SAVE   @"smsThreadSave"

@interface smsJobThreadDlg : NSWindowController<NSWindowDelegate> 
{
@private
	
	IBOutlet NSButton *m_ctlButton;
	IBOutlet NSProgressIndicator *m_ctlProgress;
	
	std::vector<SMSMessage*> m_pvResultMessage;
	CComAgent *m_pComAgent;
	BOOL m_bFinish;
	BOOL m_bCancel;
	int m_nItem;
	TYPE m_nType;
	NSString *m_sMemory;
	std::vector<SMSMessage*> m_vMsg;
	std::vector<SMSMessage*> *m_pvMsg;
	SMSMessage* m_pMsg;
	SMSMessage m_Msg;
	NSArray *m_vAddress;
	NSString *m_nID;
	SMSMessage *m_pCurMsg;
	
	NSString *m_strTitle;
	int m_Job;
	std::vector<SMSMessage*> *m_pvMessage;
	SmsFolder *m_pFolderFrom;
	SmsFolder *m_pFolder;
	NSString *m_Memory;
	BOOL m_bIsInbox;
	BOOL m_bIsFromPC;
	BOOL m_bNotify;
	CTLSMSTYPE m_nCtlType;
	SMSMessage *m_pInsertMsg;
	SMSMessage *m_pInsertNewMsg;
}

- (IBAction)onButtonCancel:(id)sender;
- (void)disableButton;

- (void)initWithParameter:(SmsJobPara*)para;
- (void)initCtrl;
- (void)doJob;

- (bool)start:(SmsFolder*)pFolder message:(SMSMessage*)pMsg withMemory:(NSString*)sMemory andIndex:(int)nItem;  //update and insert
- (bool)start:(SmsFolder*)pFolder messages:(std::vector<SMSMessage*>)vMsg WithMemory:(NSString*)sMemory isInbox:(BOOL)bIsInbox;   //copy
- (bool)start:(SmsFolder*)pFolderFrom folder:(SmsFolder*)pFolder messages:(std::vector<SMSMessage*>*)pvMsg isInbox:(BOOL)bIsInbox andisFromPC:(BOOL)bIsFromPC withMemory:(NSString*)sMemory;

- (void)func;
- (void)smsInsert;
- (void)smsUpdate;
- (void)smsCopy;
- (void)smsDeleteMove;
- (void)smsInsertMove;

- (bool)writeMessage:(SMSMessage*)pMsg withMemory:(NSString*)memory index:(int)nIndex notify:(bool)bNotify;
- (bool)updateMessage:(SMSMessage*)pMsg;
- (CAERR)writeSMS:(int)uMsg step:(int)nStep message:(SMSMessage*)pMsg memory:(NSString*)strMem extendValue:(long)lExt;
- (CAERR)ReplaceSMS:(int)uMsg step:(int)nStep message:(SMSMessage*)pMsg extendValue:(long)lExt;
- (CAERR)eraseSMS:(int)uMsg step:(int)nStep message:(SMSMessage*)pMsg extendValue:(long)lExt;

- (void)copyNext:(int)nItem;
- (void)copyEnd:(int)nNum;
- (LRESULT)onSmsCopy:(int)step status:(BOOL)bSuccess andIndex:(int)nNum;

- (void)deleteNext:(int)nItem;
- (void)deleteEnd:(CRespFlag*)pResp;
- (LRESULT)onSmsDeleteMove:(WPARAM)wParam andLParam:(LPARAM)lParam;

- (void)moveItem:(SmsFolder*)pFolderFrom toFolder:(SmsFolder*)pFolderTo withMsgs:(std::vector<SMSMessage*>&)vec index:(int)nIdx isCheck:(BOOL)bCheck;


- (LRESULT)onSmsThreadInsertMove:(WPARAM)wParam andLParam:(LPARAM)lParam;
- (LRESULT)onSMsThreadDeleteMove:(WPARAM)wParam andLParam:(LPARAM)lParam;
- (LRESULT)onProcessSMS:(WPARAM)wParam andLParam:(LPARAM)lParam;

- (void)insertMoveNext:(int)nItem;
- (void)insertMoveEnd:(int)nNum;
- (LRESULT)onSmsInsertMove:(int)step status:(BOOL)bSucess andIndex:(int)nNum;


- (void)onSmsThreadInsert:(NSNotification*)notification;

- (BOOL)getFinish;
- (BOOL)getSimStatus;
- (SMSViewCtrl*)getSMSViewControl;
- (SmsFolders*)newFolders;
- (std::vector<SMSMessage*>)getResultMsgVec;

- (void)sendMessage:(NSString*)msgID folderParam:(SmsFolder*)pFolder andMsgParam:(SMSMessage*)pMsg;
- (void)notifyDlg;

// add for distingush sms read and unread
- (void)updateMsgIndex:(SmsFolder*)pFolder withMessages:(std::vector<SMSMessage*>&)vec andIndex:(int)index;
- (void)updateMsgIndex:(SmsFolder*)pFolder withMessage:(SMSMessage*)pMsg;

@end
