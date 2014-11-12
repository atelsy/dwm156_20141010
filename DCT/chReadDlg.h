//
//  chReadDlg.h
//  DCT
//
//  Created by mtk on 12-6-8.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"
#import "ComAgent.h"

@class callHistoryViewController;

#define WM_CH_DESTROYDIALOG @"onDestroyDialog"

@interface chReadDlg : NSWindowController
{
	IBOutlet NSProgressIndicator *m_ctlProgress;
	IBOutlet NSButton *m_ctlButton;
	
	ChFolder *m_pMissCallFolder;
	ChFolder *m_pResvCallFolder;
	ChFolder *m_pDialCallFolder;
	BOOL  m_bDownload;
	BOOL m_bShowDlg;
	PhoneBookItem m_Call;
	CComAgent *pComAgent;
	
	int m_nIdx;
	int m_nItem;
	BOOL m_bCancel;
	std::vector<int> m_vTotal;
	std::vector<CString> m_vMemory;
	std::vector<int> m_vIndex;
	
	callHistoryViewController *m_pParent;
}

- (BOOL)initMissCallFolder:(ChFolder*)pMissCallFolder ResvCallFolder:(ChFolder*)pResvCallFolder andDialCallFolder:(ChFolder*)pDialCallFolder withParent:(callHistoryViewController*)pParent;
- (void)initCtrl;

- (void)setMissCallNumber:(int)nMissTotal withFlag:(BOOL)bFlag;
- (void)setReceiveCallNumber:(int)nRecvTotal withFlag:(BOOL)bFlag;
- (void)setDialCallNumber:(int)nDialTotal withFlag:(BOOL)bFlag;
- (void)getCallNumber:(int)nStep;

- (BOOL)insertMissCall:(ChFolder*)pFolder withIndex:(int)nIdx;
- (BOOL)insertResvCall:(ChFolder*)pFolder withIndex:(int)nIdx;
- (BOOL)insertDialCall:(ChFolder*)pFolder withIndex:(int)nIdx;
- (BOOL)insertCall:(ChFolder*)pFolder withIndex:(int)nIdx;

- (BOOL)getNext;
- (void)doJob;

- (LRESULT)onJobStatus:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)onRspCallBack:(WPARAM)wParam andLparam:(LPARAM)lParam;

- (IBAction)onCancel:(id)sender;

- (CComAgent*)getComAgent;

@property (nonatomic, assign) BOOL m_bShowDlg;

@end
