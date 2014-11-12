//
//  chJobThreadDlg.h
//  DCT
//
//  Created by mtk on 12-6-9.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"
#import "ComAgent.h"

#define WM_CHTHREAD_DELETE  WM_USER + 21

@class chListCtrl;

typedef struct tagCHFolders
{
	ChFolder *pFolderDelete;
	std::vector<PhoneBookItem*>* pvCall;
}CHFoldres;

@interface chJobThreadDlg : NSWindowController
{
	IBOutlet NSProgressIndicator *m_ctlProgress;
	IBOutlet NSButton *m_ctlButton;
	
	std::vector<PhoneBookItem*> m_pvResultCall;
	chListCtrl *m_pParent;
	
	CComAgent* pComAgent;
	BOOL m_bShowDlg;
	
	BOOL m_bFinish;
	BOOL m_bCancel;
	int m_nCount;
	std::vector<PhoneBookItem*>m_vCall;
	std::vector<PhoneBookItem*>* m_pvJobCall;
	ChFolder *m_pJobFolderDelete;
	
	NSString *m_strTitle;
	int m_Job;
	ChFolder *m_pFolderDelete;
	int m_Count;
	std::vector<PhoneBookItem*>* m_pvCall;
	std::vector<PhoneBookItem*>* m_pvMissCall;
	std::vector<PhoneBookItem*>* m_pvResvCall;
	std::vector<PhoneBookItem*>* m_pvDialCall;
}

- (std::vector<PhoneBookItem*>)getResultCall;

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
//- (LRESULT)onURCCallBack:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)onCHDelete:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)onChThreadDelete:(WPARAM)wParam andLparam:(LPARAM)lParam;

- (IBAction)onCancel:(id)sender;

- (void)initWithTitle:(NSString*)strTitle items:(std::vector<PhoneBookItem*>*)pvCall jobType:(int)job fromFolder:(ChFolder*)pFolderFrom withNumber:(int)ncount andParent:(chListCtrl*)parent;
- (void)initCtrl;
- (void)doJob;
- (DWORD)func;
- (BOOL)start:(ChFolder*)pFolderDelete withItem:(std::vector<PhoneBookItem*>*)pvCall andCount:(int)nCount;
- (void)chDeleteMissCall;
- (void)chDeleteResvCall;
- (void)chDeleteDialCall;
- (void)chDeleteCall;
- (void)chDeleteNext:(int)nNum;
- (void)deleteEnd:(CRespFlag*)pResp;
- (void)notifyDlg;
- (void)moveItem:(ChFolder*)pFolderDelete withItem:(std::vector<PhoneBookItem*>&)vec andIndex:(int)nIdx;
- (CHFoldres*)newFolders;
- (CAERR)erasePBK:(void*)hWnd msg:(UINT)uMsg step:(short)nStep andItem:(PhoneBookItem*)pPbk exten:(long)lExt;

@property (nonatomic, copy) NSString* m_strTitle;

@end
