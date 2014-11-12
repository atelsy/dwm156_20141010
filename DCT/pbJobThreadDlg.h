//
//  pbJobThreadDlg.h
//  DCT
//
//  Created by mbj on 12-1-16.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"
#include "ComAgent.h"

#define WM_PBTHREAD_DELETE		WM_USER + 16
#define WM_PBTHREAD_INSERT		WM_USER + 17
#define WM_PBTHREAD_UPDATE		WM_USER + 18
#define WM_PBTHREAD_DELETEMOVE  WM_USER + 19
#define WM_PBTHREAD_INSERTMOVE	WM_USER + 20

enum CTLTYPE {
	CTL_NULL = 0,
	CTL_HSTOPC,
	CTL_MOVETOHS,
	CTL_COPYTOHS
};

typedef struct tagPbFolders
{
	PbFolder *pFolderFrom;
	PbFolder *pFolderTo;
	std::vector<PhoneBookItem*> *pvContact;
}PbFolders;

typedef struct tagPbMemory
{
	int		nIdx;
	CString sMemory;
}PbMemory;

typedef struct tagPbJobPara
{
	NSString* strTitle;
	std::vector<PhoneBookItem*>* pvContact;
	int job;
	PbFolder *pFolderFrom;
	PbFolder *pFolder;
	int nCount;
	CString s;
	CTLTYPE nCtrlType;
	id hParent;
}PbJobPara;

@interface pbJobThreadDlg : NSWindowController 
{
	IBOutlet NSProgressIndicator *m_ctlProgress;
	IBOutlet NSButton *m_ctlButton;
	
	std::vector<PhoneBookItem*>	m_pvResultContact;
	id	m_pParentWnd;
	CComAgent *m_pComAgent;
	CTLTYPE m_nCtlType;
	BOOL m_bShowDlg;	
	
	enum PBOPERATIONTYPE {
		JOB_PBDELETE = 0,
		JOB_PBINSERT, 
		JOB_PBUPDATE, 
		JOB_PBCOPY, 
		JOB_PBDELETEMOVE, 
		JOB_PBINSERTMOVE
	};
	
	int		m_nItem;
	int		m_nCount;
	bool	m_bFinish;
	bool	m_bCancel;
	//	HWND	m_hDlgWnd;
	PBOPERATIONTYPE	m_nType;
	CString m_sMemory;
	std::vector<PhoneBookItem*> m_vContact;
	std::vector<PhoneBookItem*> *m_pvJobContact;
	PbFolder *m_pJobFolder;
	PbFolder *m_pJobFolderFrom;
	PhoneBookItem *m_pContact;
	PhoneBookItem *m_pContactNew;
	PhoneBookItem m_Contact;
	
	HANDLE	m_hThread;
	DWORD	m_ThreadId;
	
	/////async	
	CString m_strMem;
	bool	m_bNotify;
	int m_nID;
	bool	m_bSame;
	
	CString m_strTitle;
	std::vector<PhoneBookItem*> *m_pvContact;
	int m_Job;
	PbFolder *m_pFolderFrom;
	PbFolder *m_pFolder;
	int m_Count;
	CString m_Memory;
}

-(IBAction)OnCancel:(id)sender;

- (void)initWithParameter:(PbJobPara*)para;
- (std::vector<PhoneBookItem*>)GetResultContactVec;
- (void)DisableButton;

/*CPbJobThreadDlg(CString strTitle, std::vector<PhoneBookItem*>* pvContact, int job, 
 PbFolder *pFolderFrom, PbFolder *pFolder, int nCount, CString s, CTLTYPE nCtrlType, CWnd* pParent = NULL);   // standard constructor
 CPbJobThreadDlg(int job, CTLTYPE nCtrlType, CWnd* pParent = NULL);*/
- (void) InitCtrl;
- (void) DoJob;
- (void) MoveItem:(PbFolder*)pFolderFrom withFolder:(PbFolder*)pFolder andVector:(std::vector<PhoneBookItem*>&)vec andIndex:(int)nIdx andCheck:(BOOL)bCheck;
- (void) InsertPbIndex:(std::vector<PhoneBookItem*>)vec withIndex:(int)nIndex;

- (void) PbInsertMove;
- (void) PbDeleteMove;
- (void) PbUpdate;	
- (BOOL) WriteContact:(PhoneBookItem*)pContact withMem:(CString)s andIndex:(int)nIndex andNotify:(bool)bNotify;
- (BOOL) UpdateContact:(PhoneBookItem*)pContact withNew:(PhoneBookItem*)pContactNew;
- (void) PbInsert;
- (void) PbCopy;
- (PbFolders*) NewFolders;
- (PbMemory*) NewMemory:(PhoneBookItem *)pContact;

- (BOOL) Start:(id)hWnd withFolder:(PbFolder*)pFolder andContact:(PhoneBookItem*)pContact andMem:(CString)sMemory;	// Insert
- (BOOL) Start:(id)hWnd withFolder:(PbFolder*)pFolder withIndex:(int)nItem andContact:(PhoneBookItem*)pContact andNew:(PhoneBookItem*)pContactNew;  // Update
- (BOOL)Start:(id)hWnd withFolder:(PbFolder*)pFolder withVector:(std::vector<PhoneBookItem*>)vContact andCount:(int)nCount;	// Copy
- (BOOL)Start:(id)hWnd withFrom:(PbFolder*)pFolderFrom andFolder:(PbFolder*)pFolder andVector:(std::vector<PhoneBookItem*>*)pvContact andCount:(int)nCount andMem:(CString)sMemory;	// Move
- (BOOL) GetFinish;
- (void)DoCancel;
- (void) NotifyDlg;
- (DWORD) Func;

//- (void) CopyEnd:(CRespFlag*)pResp;
- (void) CopyEnd:(int) nNum;
- (void) CopyNext:(int) nNum;
- (void) InsertMoveEnd:(int) nNum;
- (void) InsertMoveNext:(int) nNum;
- (void) DeleteEnd;
- (void) DeleteNext:(int) nNum;

- (BOOL) GetSIMStatus;
- (CAERR)WritePBK:(id)hWnd withMsg:(UINT)uMsg andStep:(short)step 
	   andContact:(PhoneBookItem*)pPbk andMem:(const CString&) strMem andExt:(LONG)lExt; 
- (CAERR)ErasePBK:(id)hWnd withMsg:(UINT)uMsg andStep:(short)step 
	   andContact:(PhoneBookItem*)pPbk andExt:(LONG)lExt;
- (CAERR)ReplacePBK:(id)hWnd withMsg:(UINT)uMsg andStep:(short)step 
		 andContact:(PhoneBookItem*)pPbk andNew:(const PhoneBookItem&)newItem andExt:(LONG)lExt; 

- (LRESULT) OnPbThreadInsert:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT) OnPbThreadDeleteMove:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT) OnPbThreadInsertMove:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT) OnProcessContact:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT) OnPbDeleteMove:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT) OnPbInsertMove:(int)step Success:(BOOL)bSuccess andNum:(int)nNum;
- (LRESULT) OnPbCopy:(int)step Success:(BOOL)bSuccess andNum:(int)nNum;
//- (LRESULT) OnPbCopy:(WPARAM)wParam andLparam:(LPARAM)lParam;
//- (LRESULT) OnPbInsertMove:(WPARAM)wParam andLparam:(LPARAM)lParam;

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;

@end
