//
//  SMSViewCtrl.h
//  DCT
//
//  Created by Fengping Yu on 11-10-14.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "smsTreeCtrl.h"
#import "smsListCtrl.h"
#import "SmsEditCtrl.h"
#import "smsReaderController.h"
#import "smsReadDlg.h"
#import "smsSendController.h"
#import "smsTextView.h"
#include "SMSMessage.h"
#import "emsEdit.h"
#include "ComAgent.h"
#include "Folder.h"
#import "utility.h"
#import "toolbarViewMatrix.h"

@class smsPopUp;

enum LISTCONTEXTMENUTAG
{
	ID_SMS_OPEN   = 0,
	ID_SMS_SEND,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
	ID_EDIT_DELETE,
	ID_EDIT_CALL,
	ID_EDIT_RESTORE
};

enum SMSFORMVIEWTOOLBAR
{
	ID_SMS_NEW = 0,
	ID_SMS_REPLY,
	ID_SMS_FORWARD,
	ID_SMS_CUT,
	ID_SMS_COPY,
	ID_SMS_PASTE,
	ID_SMS_DELETE,
	ID_SMS_DOWNLOAD
};

typedef struct _newMsgInd
{
	CString strMem;
	int index;
}newMsgInd;

#define WM_SMSINIT_RESULT   @"smsInitResult"

@interface SMSViewCtrl : NSViewController 
{
@private
	
	IBOutlet smsTreeCtrl  *smsTree;
	IBOutlet smsListCtrl  *smsList;
	IBOutlet toolbarViewMatrix *toolbarMatrix;
	IBOutlet NSSearchField *searchField;
	IBOutlet NSMenu *listMenu;
	IBOutlet NSSplitView *verticalSplit;
	IBOutlet NSSplitView *horizontalSplit;
	
	// sms editor control
	IBOutlet NSTextField *number;
	IBOutlet NSTextField *time;
	IBOutlet emsEdit *content;
	
	NSMutableArray *globalData;   //use to record the list data, because when search, it will modify the data, when tree select modify, must reset globalData;
	int searchCategory;
	
	BOOL m_smsEnable;  // also responsible download
	BOOL m_cutcopydeleteEnable;
	   BOOL m_repyforwardEnable;
	BOOL m_pasteEnable;
	BOOL m_bConn;
	BOOL m_bSelFolder;
	BOOL m_bRefList;
	//BOOL m_bMsgFullPrompt;
	int m_nNewMsg;
    
//	int m_numSIM;
//  int m_numME;
	NSInteger m_lastSelectItem;
	
	// sms download controller
	SmsEditCtrl *editorController;
	smsReaderController *readerController;
	smsSendController *sendController;
	smsReadDlg *smsReadController;
	smsPopUp *popupWinController;
	
	//cut, copy and paste
	std::vector<SMSMessage*> m_vMsg;
	BOOL m_bCopy;
	NSTreeNode *m_hItem;
	
	// delete, send;
	std::vector<SMSMessage*> m_vMsgTemp;
	NSTreeNode *m_hTemp;
	
	// new message record
	std::vector<newMsgInd*> m_vNewMsgInd;
	SMSMessage m_Msg;
	int m_nItem;
	int m_nReadItem;
	int m_nNewMsgIndex;
	SmsFolder *m_pNewMsgFolder;
	
	//folder
	SmsFolder *m_pFolder;
	SmsFolder *m_pFolderInbox;
	SmsFolder *m_pFolderOutbox;
	SmsFolder *m_pFolderSentbox;
	
	//others
	CComAgent *pComAgent;
	
	BOOL m_bShowDlg;
	int m_nEnterDlg;
	int m_nIndex;
@public
    int m_numSIM;
    int m_numME;
    BOOL m_nNewMSM;
    BOOL m_nNewSaveMode;
    BOOL m_SavePC;
     BOOL m_MsgFull;
}

//@property (retain)NSMutableArray *toolbarListdata;
@property (retain)NSMutableArray *globalData;
@property (nonatomic ,retain)IBOutlet smsListCtrl *smsList;
@property (nonatomic ,retain)IBOutlet smsTreeCtrl *smsTree;
@property (nonatomic, assign) int m_nEnterDlg;
@property (nonatomic, retain) smsPopUp *popupWinController;
@property (nonatomic, retain) IBOutlet NSSearchField *searchField;

//property access
- (std::vector<SMSMessage*>)getMsgVector;

- (std::vector<newMsgInd*>)getNewMsgIndication;
- (void)resetNewMsgIndication;

- (IBAction)doSearch:(id)sender;
- (IBAction)setSearchCategoryFromTo: (NSMenuItem*)menuItem;
- (IBAction)setSearchCategoryContent: (NSMenuItem*)menuItem;
- (void)resetSearch;

// sms auxiliary function to implement sms operation
- (void)showEditorDlg:(NSString*)strTo messageContent:(NSString*)strMsg item:(int)nItem isLast:(BOOL)bLast withMsg:(SMSMessage*)pMsg andIsReply:(BOOL)bReply;
- (void)showReaderDlg:(NSString*)strFrom withTime:(NSString*)strTime msgContent:(NSString*)strMsg index:(int)_index andMsg:(SMSMessage*)pMsg;
- (void)showSendDlg:(SmsFolder*)pFolderOutbox;
- (void)showReadDlg;

//menu and toolbar action
- (IBAction)smsNew:(id)sender;
- (IBAction)smsOpen:(id)sender;
- (IBAction)smsReply:(id)sender;
- (IBAction)smsForward:(id)sender;
- (IBAction)smsSend:(id)sender;
- (IBAction)smsCut:(id)sender;
- (IBAction)smsCopy:(id)sender;
- (IBAction)smsPaste:(id)sender;
- (IBAction)smsDelete:(id)sender;
- (IBAction)smsDownload:(id)sender;
- (IBAction)smsRestore:(id)sender;
- (IBAction)smsCall:(id)sender;

// Init function
- (void)initTreeCtrl;
- (void)InitToolbar;
- (void)InitSearchMenu;
- (void)setDefaultTreeSelectItem;
-(void)updateFullSms;
// Notification message
- (void)updateListItem;
- (void)onItemchangeListSms:(NSNotification*)notification;
- (void)onSelChangedTreeSms:(NSNotification*)notification;
- (void)onSmsSendOld:(NSNotification*)notification;
- (void)onSmsSendNew:(NSNotification*)notification;
- (void)onSmsInsert:(NSNotification*)notification;
- (void)onSmsUpdate:(NSNotification*)notification;
- (void)onInitResult:(NSDictionary*)dict;
- (void)parseSystemStatus:(CUrcSys *)pUrc;
- (void)onDestroyDialog:(NSNotification*)notification;
- (void) OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
- (void)onURCCallBack:(long)wParam lParam:(long)lparam;
- (void)onRspCallBack:(long)wParam lParam:(long)lparam;

- (void)updateMessageStatusInex:(SMSMessage*)pMsg withItem:(int)nItem;
- (void)updateMessage;//by zhang shao hua
- (void)downloadNewMsg;
- (void)DoJob;
- (void)onJobStatus:(BOOL)result withIndex:(int)index;
- (void)insertMessage:(SmsFolder*)pFolder;
- (void)displayNewMsginFolder:(SmsFolder*)pFrom andInserttoFolder:(SmsFolder*)pFolder;

//folder operation
- (void)setFolderName:(SmsFolder *)pFolder withIndex:(int)index;
- (SmsFolder*)setSubFolderName:(SmsFolder*)pFolder withIndex: (int&)index andSubFolderPos: (SubfolderPos&)pos;
- (void)initFolder;
- (void)reInitFolder;
- (SmsFolder*) insertSubFolder:(SmsFolder*)pFolder withIndex:(int&)index;

// Others
- (BOOL)getSimStatus;
- (BOOL)getConnected;
- (BOOL)getConnStatus;
- (int)getSysState;
- (NSInteger)getLastSelectItem;
- (BOOL)isCopy;
- (void)setCopy:(BOOL)bCopy;
- (BOOL)isPCFolderFull;
- (int)getPCSMSCount;
- (NSTreeNode*)getCopyItem;
- (NSTreeNode*)getDeleteItem;
- (std::vector<SMSMessage*>&)getCopyMessage;
- (std::vector<SMSMessage*>&)getDeleteMessage;
- (CComAgent*)getComAgentObj;
- (bool)checkSupportSMStoInbox;
- (void)enableAllCtrl:(BOOL)bEnable;

// sms display
- (void)display:(SmsFolder*)folder;
- (void)update;
- (void)setEditCtrl:(NSString*)strFrom time:(NSString*)strTime message:(NSString*)strMessage andEmsData:(EMSData*)pEmsData;
- (void)resetEditCtrl;

// clipboard operation
- (void)clearClipboard;
- (bool)checkClipboard:(SMSMessage*)pMsg;
- (bool)checkClipboardforFolder:(SmsFolder*)pfolder;

- (void)setToolbarItem:(int)index state:(BOOL)bEnable;
- (void)onTBButtonEnabled;

- (void)closeSmsEditorDlg;
- (void)closeSmsReadDlg;
- (void)closeReaderDlg;
- (void)closeSendDlg;

- (NSWindow*)getWindow;
- (void)closeWindow;

//debug aid
- (void)showNewMessage;

- (void)enterDlg;
- (void)leaveDlg;
- (int)getSelectIndex;

- (void)showPopupView:(SmsFolder*)pFolder;
- (void)startRing;
- (void)stopRing;

// interface for call history
- (BOOL)sendNewMessage:(NSString*)strNumber;
@end
