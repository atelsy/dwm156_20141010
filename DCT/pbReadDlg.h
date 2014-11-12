//
//  pbReadDlg.h
//  DCT
//
//  Created by mbj on 12-1-16.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"
#import "ComAgent.h"

@interface pbReadDlg : NSWindowController {
	
	IBOutlet NSProgressIndicator *m_ctlProgress;
	IBOutlet NSButton *m_ctlButton;
	IBOutlet NSTextField *m_ctlStatic;
	
	BOOL m_bShowDlg;
	PbFolder		*m_pFolder;	
	PhoneBookItem	m_Contact;
	CComAgent		*m_pComAgent;
	int m_nIdx;
	int m_nItem;
	int m_nReadItem;
	BOOL m_bCancel;
	id	m_pParentWnd;
	
	std::vector<int> m_vTotal;
	
}

- (void)DisableButton;
- (BOOL)GetNext;
- (BOOL)InsertContact:(PbFolder*)pFolder;
- (void)DoJob;
- (void)InitCtrl;
- (BOOL)Init:(PbFolder*)pFolder andParentWnd:(id)pParent;
//	CPbReadDlg(PbFolder *pFolder, CWnd* pParent = NULL);   // standard constructor
- (void)SetTotalNumber:(BOOL)flag andTotal:(int)nTotal;
- (CString)GetMemory:(int)idx;
- (size_t)GetMemorySize;

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
- (LRESULT)OnJobStatus:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)OnRspCallBack:(WPARAM)wParam andLparam:(LPARAM)lParam;
//- (LRESULT)OnURCCallBack:(WPARAM)wParam andLparam:(LPARAM)lParam;

- (IBAction)OnCancel:(id)sender;

@end
