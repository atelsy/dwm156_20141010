//
//  pbListCtrl.h
//  DCT
//
//  Created by mbj on 12-1-13.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "pbListData.h"
#import "utility.h"
#import "pbJobThreadDlg.h"

#define PBMEMORYCELL	@"PBMemoryCell"
#define PBNAMECELL		@"PBNameCell"
#define PBMOBILECELL	@"PBMobileCell"
#define PBITEM			@"PBItem"

#define PBLIST_LVN_ITEMCHANGED @"onItemchangedListPb"

#define WM_PBLIST_SETINDEX	WM_USER + 300
#define WM_MOVETOHANDDSET   WM_USER + 301
#define WM_COPYTOHANDDSET   WM_USER + 302

enum PBJOBTYPE {	
	JOB_NONE = 0,
	JOB_MOVE, 
	JOB_COPY
};

enum OPERATION{
	PB_INSERT = 0,
	PB_COPY,
	PB_MOVE	
};

@interface pbListCtrl : NSView<NSTableViewDelegate, NSTableViewDataSource> 
{
	IBOutlet NSTableView *tableView;
	
	NSMutableArray *iconImage;
	NSMutableArray *dataSource; // phonebook contact data source
	SEL columnSortSelector; // holds a method pointer
	NSTableColumn *lastColumn;  //track last column chosen
	
	NSMutableArray *cutSelectedItem;
	
	BOOL bAscending;
	BOOL m_bRedraw;
	
	NSString *searchString;
	
	std::vector<PhoneBookItem*> m_vContact;	// InsertMove, Copy to Handset
	std::vector<PhoneBookItem*> *m_vec;
	std::vector<PhoneBookItem*> m_vecTemp;
	std::vector<PhoneBookItem> m_ReDownloadvec;
	PhoneBookItem *m_pContact;
	int m_numSIM, m_numME;
	
	PBJOBTYPE m_nType;
	
	CString m_str;
	int m_nMaxName;
	int m_nEncodeClass;
	BOOL  m_bYesToAll;
	int m_nLen;
	int m_nMax;
	PbFolder *m_pFolderFrom;
	PbFolder *m_pFolder;
	pbJobThreadDlg *m_dlg;
	BOOL m_bCopy;
	CString m_strMemType;
	OPERATION m_Operation;
}

// other operation
- (BOOL)GetClipboard;
- (void)UpdateStatusBar;
- (void)Display:(PbFolder *)pFolder;
- (void)AddRow:(pbListData*)pData;
- (NSImage*)iconImage:(NSString*)imageName;

// implement mfc method
- (void)setRedraw:(BOOL)bRedraw;
- (id)getParent;

- (void)GetAnotherMemory:(CString&)s;
- (void)GetMemery:(CString&)str;
- (void)InsertPbIndex:(PhoneBookItem*)pContact;
- (void)WriteContact:(PbFolder*)pFolder withItem:(PhoneBookItem*)pContact andMem:(CString&)s withIdx:(int)nIdx;
- (void)WriteContact:(PbFolder*)pFolder withPbVector:(std::vector<PhoneBookItem*>&)vec;
- (void)WriteContact:(PbFolder *)pFolderFrom toFolder:(PbFolder*)pFolder withPbVector:(std::vector<PhoneBookItem*>&)vec;
- (int)SetIndex:(std::vector<PhoneBookItem*>&)vec withCopy:(BOOL)bCopy;
- (BOOL)ReDownload:(OPERATION)op;
- (int)GetMemery:(PhoneBookItem*)pContact withMem:(CString&)sMem;
- (int)GetFirstSelected;
- (void)ResetStr:(CString&)str withId:(UINT)nID;
- (void)SaveContact;
- (void)PasteContact:(NSTreeNode*)hItem;
- (void)CopyContact;
- (void)CutContact;
- (void)DeleteContact;
- (BOOL)OnPbDelete;
- (void)SelectItem:(int)nItem;
- (void)InsertItem:(PbFolder*)pFolder withContact:(PhoneBookItem*)pContact;
- (void)InsertItem:(PbFolder*)pFolder withContact:(PhoneBookItem*)pContact andVector:(std::vector<PhoneBookItem*>&)vContact;
- (void)MoveContact:(NSTreeNode*)hFrom To:(NSTreeNode*)hItem withVector:(std::vector<PhoneBookItem*>&)vec;
- (void)CopyContact:(NSTreeNode*)hFrom To:(NSTreeNode*)hItem withVector:(std::vector<PhoneBookItem*>&)vec;
- (void)MoveItem:(PbFolder*)pFolderFrom To:(PbFolder*)pFolder withVector:(std::vector<PhoneBookItem*>&)vec andIdx:(int)nIdx andChecked:(BOOL)bCheck;
- (void)DeleteItem:(PbFolder*)pFolder withItem:(PhoneBookItem*)pContact;
- (void)SelectLastItem:(PbFolder*)pFolder;
- (void)GetSelectedItem:(BOOL)bCopyDrag;
- (void)GetSelectedItem:(NSTreeNode**)hItem withVector:(std::vector<PhoneBookItem*>&)vec;
- (void)InsertItem:(PhoneBookItem*)pItem withIndex:(int)nItem;

- (BOOL)GetMemoryIdx:(CString)str withMem:(CString&)strMemory andIdx:(int&)nIdx;
- (BOOL)GetMemoryIdx:(PhoneBookItem*)pContact isCopy:(BOOL)bCopy withMem:(CString&)strMemory andIdx:(int&)nIdx isShow:(BOOL)bShowMsg;
- (BOOL)UpdateContact:(int)nItem withItem:(PhoneBookItem*)pContact andVector:(std::vector<PhoneBookItem*>&)vContact;
- (BOOL)InsertContact:(PbFolder*)pFolder withItem:(PhoneBookItem*)pContact andVector:(std::vector<PhoneBookItem*>&)vContact;
- (BOOL)CheckPhoneName:(CString&)str withMem:(const CString&)strMem andEncode:(int&)encodeClass andMaxName:(int&)nMaxName; 

- (DIRECTION)GetDirection:(NSTreeNode*)hFrom To:(NSTreeNode*)hTo;
// CImageList* CreateDragImageEx(LPPOINT lpPoint);

- (PhoneBookItem*)GetItemData:(int)nItem;
- (PhoneBookItem*)UpdateItem:(int)nItem;
- (PhoneBookItem*)UpdateItem:(PhoneBookItem*)pContact withItem:(int)nItem andVector:(std::vector<PhoneBookItem*>&)vContact;
- (PhoneBookItem*)GetInfo:(CString&)strName andHandset:(CString&)strHandset andHome:(CString&)strHome andCompany:(CString&)strCompany andEmail:(CString&)strEmail andOffice:(CString&)strOffice andFax:(CString&)strFax andBirthday:(CString&)strBirthday;
- (PhoneBookItem*)GetInfo:(int)nItem andName:(CString&)strName andHandset:(CString&)strHandset andHome:(CString&)strHome andCompany:(CString&)strCompany andEmail:(CString&)strEmail andOffice:(CString&)strOffice andFax:(CString&)strFax andBirthday:(CString&)strBirthday;

- (void)CloseThreadDlg;
- (void)UpdateListCtrl;
- (void)InsertPbIndex:(std::vector<PhoneBookItem*>)vec withIdx:(int)nIndex;
// CWnd * GetPbJobDlgWnd();

//PB job thread message
- (void)LaunchPbJobThread:(PbJobPara*)para;
- (void)LaunchPbJobThread:(PbJobPara *)para withFolder:(PbFolder *)pFolder andContact:(PhoneBookItem*)pContact andMem:(CString&)s;
- (void)LaunchPbJobThread:(PbJobPara *)para withFolder:(PbFolder *)pFolderHandset withItem:(int)nItem andSelContact:(PhoneBookItem*)pContactSel andContact:(PhoneBookItem*)pContact;
- (void)CloseThreadDlg;

- (void)deleteAllItems;
- (int)GetItemCount;
- (int)GetSelectedCount;
- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
- (void)ModifyContactCount:(CString)sMem;

- (LRESULT) OnPbThreadInsert:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT) OnPbThreadUpdate:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT) OnDestroyDlg:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT) OnCopyToHandset:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT) OnMoveToHandset:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT) OnAfterReDownload:(WPARAM)wParam andLparam:(LPARAM)lParam;

- (bool)getSelected;


NSInteger PbdataSort(id data1, id data2, void *columnIdentifier);

@property (assign)NSMutableArray *dataSource;
@property (assign)NSTableView *tableView;
@property (assign)NSMutableArray *iconImage;

@end
