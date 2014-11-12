//
//  callHistoryViewController.h
//  DCT
//
//  Created by mtk on 12-6-6.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"
#import "chTreeControl.h"
#import "chListCtrl.h"
#import "chTbMatrix.h"
#import "ComAgent.h"
#import "chReadDlg.h"

enum CHFORMVIEWTOOLBAR
{
	ID_CH_CALL  = 0,
	ID_CH_SENDMSG,
	ID_CH_SAVENUM,
	ID_CH_CLEAN,
	ID_CH_DOWNLOAD,
};

#define _CH_WRITE_LD_
#define _CH_WRITE_MC_
#define _CH_WRITE_RC_

@interface callHistoryViewController : NSViewController<NSOutlineViewDelegate, NSOutlineViewDataSource, NSTableViewDelegate, NSTableViewDataSource>
{
	IBOutlet chTreeControl *chTree;
	IBOutlet chListCtrl	*chList;
	IBOutlet chTbMatrix *tbMatrix;
	IBOutlet NSSearchField *searchField;
	NSMutableArray *globalData;
	
	ChFolder *m_pMissCallFolder;
	ChFolder *m_pRecvCallFolder;
	ChFolder *m_pDialCallFolder;
	
	int m_MemoryType;
	int m_nNewCall;
	int m_nEnterDlg;
	BOOL m_CHBEnable;
	BOOL m_bFlags;
	BOOL m_bPromptDown;
	BOOL m_bClean;
	BOOL m_bSelect;
	
	BOOL m_bMissCall;
	
	int m_MCNum;
	int m_RCNum;
	int m_LDNum;
	int m_MissCallnum;
	int m_ResvCallnum;
	int m_DialCallnum;
	
	BOOL m_bSendSms;
	BOOL m_bSavePb;
	BOOL m_bDownload;
	BOOL m_bConn;
	ChFolder *m_pFolder;
	
	int m_searchCategory;
	
	CComAgent *pComAgent;
	
	chReadDlg *readDlg;
}

- (void)initToolbar;
- (void)initSearchMenu;
- (void)initTreeCtrl;

- (ChFolder*)getMissCallFolder;
- (ChFolder*)getResvCallFolder;
- (ChFolder*)getDialCallFolder;

- (void)initFolder;
- (ChFolder*) insertSubFolder:(ChFolder *)pFolder withIndex:(int&)index;
- (ChFolder*)setSubFolderName:(ChFolder *)pFolder withIndex:(int &)index andSubFolderPos:(SubfolderPos &)pos;
- (void)setFolderName:(ChFolder *)pFolder withIndex:(int)index;

- (IBAction)onEditClear:(id)sender;
- (IBAction)onSaveNum:(id)sender;
- (IBAction)onDialCall:(id)sender;
- (IBAction)onSendMessage:(id)sender;
- (IBAction)onDownload:(id)sender;
- (void)showReadDlg:(id)sender;

- (BOOL)isEnabled:(NSInteger)tag;
- (void)onTBButtonEnabled;
- (void)setToolbarItem:(int)index State:(BOOL)bEnable; 
- (void)enableAllCtrl:(BOOL)bEnable;

- (IBAction)doSearch:(id)sender;
- (IBAction)setSearchCategoryName:(NSMenuItem *)menuItem;
- (IBAction)setSearchCategoryNumber:(NSMenuItem *)menuItem;
- (void)resetSearch;

- (void)clearIndication:(int)num;
- (int)getSysState;
- (void)enterDlg;
- (void)leaveDlg;

- (CComAgent*)getComAgentObj;
- (BOOL)getOperationStatus:(OPERATION_TYPE)type;
- (BOOL)getLastDialedNUmber:(CString&)strNum;
- (const TCHAR*)getMemType:(PhoneBookItem&)item withCall:(const CCall&)call;

- (void)insertItem:(const CCall &)call;
- (tm)nsDateTimeToTM:(NSDate*)date;

- (void)updateRecord:(PhoneBookItem&)pItem;

- (void)onRspCallback:(long)wParam lParm:(long)lparam;
- (void)onSelChangedTreeCh:(NSNotification*)notification;
- (LRESULT)onDisplayMC:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)onInitResult:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)onEnableOperation:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)onCHDownload:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)onCHInsert:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)onRefreshList:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)onDestroyDialog:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)onDownloadPrompt:(WPARAM)wParam andLparam:(LPARAM)lParam;

@property (nonatomic, retain) IBOutlet chTreeControl *chTree;
@property (nonatomic, retain) IBOutlet chListCtrl *chList;
@property (nonatomic, retain) IBOutlet chTbMatrix *tbMatrix;
@property (nonatomic, retain) IBOutlet NSSearchField *searchField;
@property (nonatomic, retain) NSMutableArray *globalData;

@property (nonatomic, assign) BOOL m_bConn;
@property (nonatomic, assign) BOOL m_bDownload;
@property (nonatomic, assign) BOOL m_bFlags;
@property (nonatomic, assign) BOOL m_bPromptDown;
@property (nonatomic, assign) BOOL m_bClean;
@property (nonatomic, assign) BOOL m_bSelect;
@property (nonatomic, assign) BOOL m_CHBEnable;
@property (nonatomic, assign) BOOL m_bMissCall;

@property (nonatomic, assign) int m_MCNum;
@property (nonatomic, assign) int m_RCNum;
@property (nonatomic, assign) int m_LDNum;
@property (nonatomic, assign) int m_MissCallnum;
@property (nonatomic, assign) int m_ResvCallnum;
@property (nonatomic, assign) int m_DialCallnum;

@end
