//
//  ListCtrl.h
//  DCT
//
//  Created by Fengping Yu on 11-10-14.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "listData.h"
#import "utility.h"
#import "smsJobThreadDlg.h"
#import "smsReadDlg.h"
#include "ComAgent.h"
#define MEMORYCELL  @"MemoryCell"
#define FROMTOCELL  @"FromToCell"
#define TIMECELL	@"TimeCell"
#define CONTENTCELL @"ContentCell"
#define ICONCELL	@"IconCell"
#define MESSAGE		@"Message"

#define LVN_ITEMCHANGED @"smsListItemChanged"


@interface smsListCtrl : NSView<NSTableViewDelegate, NSTableViewDataSource> 
{
@private
	
	IBOutlet NSTableView *tableView;
	
	NSMutableArray *iconImage;
	NSMutableArray *dataSource; // sms message data source
	
	NSTableColumn *lastColumn;  //track last column chosen
	
	NSMutableArray *cutSelectedItem;
	
	BOOL bAscending;
	BOOL m_bRedraw;
	BOOL m_bOpenDlg;
	
	SmsFolder *m_pFolder;
	SmsFolder *m_pFolderFrom;
	NSTreeNode *m_hCopyItem;
        //folder
        //SmsFolder *m_pFolder1;
        //SmsFolder *m_pFolderInbox1;
        //SmsFolder *m_pFolderOutbox1;
        //smsReadDlg *smsReadController;

	int m_nNumSim;
	int m_nNumMe;
	CComAgent *m_pCom;
	smsJobThreadDlg *m_dlg;
	
	NSSortDescriptor *sortDescriptor;
}

- (BOOL)deleteSms;
- (IBAction)cutSms;
- (IBAction)openSms;
- (void)smsCall;

- (void)repaintTree;
- (void)display:(SmsFolder*)pFolder;
- (int)getItemCount;
- (int)getSelectedCount;

- (void)addRow:(listData*)pData;
- (NSImage*)iconImage:(NSString*)imageName;

- (SMSMessage*)getInfo:(BOOL&)bReceived withAddress:(NSString**)strFrom Time:(NSString**)strTime andMsgContent:(NSString**)strMessage andEmsData:(EMSData**)pEmsData isConvert:(BOOL)bConvert;
- (SMSMessage*)getInfo:(BOOL&)bReceived withAddress:(NSString**)strFrom Time:(NSString**)strTime andMsgContent:(NSString**)strMessage isConvert:(BOOL)bConvert;
- (SMSMessage*)getInfo:(int)nItem isReceived:(BOOL&)bReceived withAddress:(NSString**)strFrom Time:(NSString**)strTime msgContent:(NSString**)strMsg andEMSData:(EMSData**)pEmsData isConvert:(BOOL)bConvert;
- (BOOL)getReceived:(SMSMessage*)pMsg;
- (BOOL)getReceived;
- (BOOL)getOneReceived:(int&)nItem isReceived:(BOOL)bReceived;

// other operation
- (void)insertItem:(SMSMessage*)pMsg withIndex:(int)index;
- (void)insertItem:(SmsFolder*)pFolder withMessage:(SMSMessage*)pMsg;
- (void)insertItem:(SmsFolder*)pFolder withMessage:(SMSMessage*)pMsg andMsgVector:(std::vector<SMSMessage*>&)vMsg;
- (BOOL)insertMessage:(SmsFolder*)pFolder withMessage:(SMSMessage*)pMsg andMsgVector:(std::vector<SMSMessage*>&)vMsg;
- (void)deleteItem:(SmsFolder*)pFolder withMessage:(SMSMessage*)pMsg;
- (void)deleteAllItems;
- (SMSMessage*)updateItem:(SMSMessage*)pMsg withItem:(int)nItem andMsgs:(std::vector<SMSMessage*>&)vMsg;
- (BOOL)updateMessage:(int)nItem withMessage:(SMSMessage*)pMsg andMsgVector:(std::vector<SMSMessage*>&)vMsg;
- (BOOL)getClipboard;
- (void)updateStatusBar;

// select operation
- (bool)getSelected;
- (int)getFirstSelected;
- (void)getSelectItem:(NSTreeNode**)hItem withMsg:(std::vector<SMSMessage*>&)vec;
- (SMSMessage*)getItemData:(int)nItem;
- (void)selectLastItem:(SmsFolder*)pFolder;
- (void)selectItem:(int)nItem;

// implement mfc method
- (void)setRedraw:(BOOL)bRedraw;
- (id)getParent;
- (id)getsmsJobThreadDlg;

NSInteger dataSort(id data1, id data2, void *columnIdentifier);

//cut, copy and paste operation
- (void)moveMessageFrom:(NSTreeNode*)hFrom To:(NSTreeNode*)hTo forMessages:(std::vector<SMSMessage*>&)vec whetherRestore:(BOOL)isRestore;
- (void)copyMessageFrom:(NSTreeNode*)hFrom To:(NSTreeNode*)hTo forMessages:(std::vector<SMSMessage*>&)vec;
- (void)copyMessageFrom:(std::vector<SMSMessage*>&)vecFrom To:(std::vector<SMSMessage*>&)vec withFolder:(SmsFolder*)pFolder isCopy:(BOOL)bCopy;
- (void)pasteMessage:(NSTreeNode*)hItem;
- (void)copyMessage;
- (void)cutMessage;
- (void)deleteMessage:(NSTreeNode*)hItem;
- (BOOL)onSmsDelete;
- (DIRECTION)getDirection:(NSTreeNode*)hFrom to:(NSTreeNode*)hTo;
- (void)moveItemFrom:(SmsFolder*)pFolderFrom to:(SmsFolder*)pFolderTo withMsg:(std::vector<SMSMessage*>&)vec index:(int)nIdx isCheck:(BOOL)bCheck;

//SMS job thread message
- (void)onSmsThreadUpdate:(NSNotification*)notification;
- (void)launchSmsJobThread:(SmsJobPara*)para;
- (void)launchSmsJobThread:(SmsJobPara *)para withFolder:(SmsFolder*)pFolder message:(SMSMessage*)pMsg andIndex:(int)index;

- (void)onDestroySmsDialog:(NSNotification*)notification;
- (void)closeThreadDlg;
- (void)onSmsThreadSave:(NSNotification*)notification;

- (void)updateMsgStatusIndex:(SmsFolder*)pFolder withMessage:(SMSMessage*)pMsg;

@property(assign)NSMutableArray *dataSource;
@property(assign)NSTableView *tableView;
@property(assign)NSMutableArray *iconImage;
@property(nonatomic, assign) BOOL m_bOpenDlg;

@end
