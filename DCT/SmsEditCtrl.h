//
//  SmsEditCtrl.h
//  DCT
//
//  Created by Fengping Yu on 10/24/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "smsSaveController.h"
#import "pbSelectController.h"
#import "utility.h"
#import "emsEdit.h"
#import "smsTextView.h"
#import "phoneNumberFormatter.h"

#define WM_SMS_SENDOLD			@"OnSmsSendOld"
#define WM_SMS_SENDNEW			@"OnSmsSendNew"
#define WM_SMS_INSERT			@"OnSmsInsert"
#define WM_SMS_UPDATE			@"OnSmsUpdate"
#define WM_SMS_SAVE				@"OnSmsSave"
#define WM_SMS_DESTROYDIALOG	@"onDestroyDlg"

enum toolbarTag 
{
	SENDITEM= 0,
	SAVEITEM= 1,
	CUTITEM = 2,
	COPYITEM= 3,
	PASTEITEM   = 4,
};

@interface SmsEditCtrl : NSWindowController<NSWindowDelegate, NSTextViewDelegate> 
{
@private
	
	IBOutlet NSButton *toButton;
	IBOutlet NSTextField *pbNumber;
	IBOutlet NSTextField *counter;
	smsTextView *customFieldEditor;
	
	smsSaveController *saveController;
	pbSelectController *pbSelectCtrl;
	
	NSPasteboard *defaultPasteboard;
	
	BOOL m_bModify;
	NSInteger m_saveDlgResult;
	
	BOOL m_bLast;
	BOOL m_bReply;
	int m_nItem;
	NSMutableString *m_strTo;
	NSString *m_strMessage;
	SmsFolder *m_pFolder;
	SmsFolder *m_pFolderInbox;
	SmsFolder *m_pFolderOutbox;
	PbFolder *m_pFolderPb;
	SMSMessage *m_pMsg;
	
	IBOutlet emsEdit *m_ctlEditMessage;
	
	NSMutableString* m_lastValidRecipients;
}

@property(nonatomic, retain)IBOutlet emsEdit *m_ctlEditMessage;
@property(nonatomic, retain)IBOutlet NSTextField *pbNumber;
@property(nonatomic, retain)IBOutlet NSTextField *counter;
@property(nonatomic, retain)IBOutlet NSButton *toButton;
@property(nonatomic, retain) NSString* m_lastValidRecipients;

- (IBAction)onPbSelect:(id)sender;
- (IBAction)onSmsSave:(id)sender;
- (IBAction)onSmsSend:(id)sender;
- (IBAction)onEditCut:(id)sender;
- (IBAction)onEditCopy:(id)sender;
- (IBAction)onEditPaste:(id)sender;

- (void)initwithAddress:(NSString*)strto msgContent:(NSString*)strMsg inItem:(int)item isLast:(BOOL)bLast withSmsfolder:(SmsFolder*)pFolder outboxFolder:(SmsFolder*)pFolderOutbox inboxFolder:(SmsFolder*)pFolderInbox andPhonebookfolder:(PbFolder*)pFolderPb withMsg:(SMSMessage*)pmsg replyOrNot:(BOOL)bReply;
- (void)updateInfo:(BOOL)bCheck;
- (void)closeDialog;
- (BOOL)checkRecipients:(NSString*)str;
- (SMSMessage*)initMessage;
- (id)getFocus;

- (void)cutString:(NSTextView*)textView;
- (void)copyString:(NSTextView*)textView;
- (void)pasteString:(NSTextView*)textView;

- (BOOL)enableCutCopy;
- (BOOL)enablePaste;

- (void)setModify:(BOOL)bModify;

@end
