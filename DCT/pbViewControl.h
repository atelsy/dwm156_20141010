//
//  pbViewControl.h
//  DCT
//
//  Created by Fengping Yu on 11/29/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//


#import <Cocoa/Cocoa.h>
#import "pbListCtrl.h"
#import "pbTreeCtrl.h"
#include "ComAgent.h"
#include "Folder.h"
#import "toolbarView.h"
#import "utility.h"
#import "pbReadDlg.h"
#import "pbEditorDlg.h"
#import "pbJobThreadDlg.h"
#import "Str2IntList.h"
#import "toolbarView.h"
#import "pbEditorDlg.h"
#import "pbCSVDlg.h"
#import "pbToolbarViewMatrix.h"

#define SEARCHNUMBER	8
#define SEARCHNAME		9

enum PBFORMVIEWTOOLBAR
{
	ID_PB_NEW = 0,
	ID_PB_CUT,
	ID_PB_COPY,
	ID_PB_PASTE,
	ID_PB_DELETE,
	ID_PB_DOWNLOAD,
	ID_PB_FILE,
    //ID_PB_NEWSMS,
	ID_PB_OPEN
};

@interface pbViewControl : NSViewController<NSOutlineViewDataSource, NSOutlineViewDelegate, NSTableViewDelegate, NSTableViewDataSource>
{
@public
	IBOutlet pbTreeCtrl  *pbTree;
	IBOutlet pbListCtrl  *pbList;
	IBOutlet toolbarView *toolbar;
	IBOutlet NSSearchField *searchField;
	IBOutlet NSMenu *listMenu;
	IBOutlet NSSplitView *verticalSplit;
	IBOutlet NSSplitView *horizontalSplit;
	IBOutlet NSOutlineView *labelList;
	IBOutlet NSTableView *infoList;
	IBOutlet pbToolbarViewMatrix *toolbarMatrix;
	
	NSMutableArray *toolbarListdata;
	NSMutableArray *globalData;   //use to record the list data, because when search, it will modify the data, when tree select modify, must reset globalData;
	
	PhoneBookItem *m_PBItem;
	std::vector<PhoneBookItem*>	m_vContactDrag;
	BOOL			m_bNotify;
	
	// Cut, Copy, Paste (Clipboard)
	NSTreeNode *			m_hItem;
	std::vector<PhoneBookItem*>	m_vContact;
	BOOL					m_bCopy;
	
	// Delete
	NSTreeNode *			m_hTemp;
	std::vector<PhoneBookItem*>	m_vContactTemp;
	
	int m_LastLoadIndex;
	CComAgent *m_pComAgent;
	DOWNLOAD_TYPE m_DownloadType;
	
	pbReadDlg   *m_dlgPbRead;
	pbEditorDlg *m_dlgPbEditor;
	pbCSVDlg    * m_dlgPbCsv;
	
	PbFolder *m_pFolderHandset;
	PbFolder *m_pFolder;
	
	int	m_nWidth;
	int	m_nHeight;
	BOOL m_bCreated;
	BOOL m_bShowDlg;
	BOOL m_bDownload;
	BOOL b_Select;	
	BOOL m_bConn;
	BOOL m_PHBEnable;
	
	int m_pbkMeTotal;
	int m_pbkSmTotal;
	float m_tview_height;
	float m_oview_height;
	
	NSTreeNode *rootNode;
	
	int m_nSimTotal;
    int  m_nSimUsed;
	std::vector<CString> m_vMemory;
	std::vector<int> m_vLenLimit;
	
	int m_searchCategory;
	
	IBOutlet NSBox *box;
	
}

@property (retain)NSMutableArray *toolbarListdata;
@property (retain)NSMutableArray *globalData;
@property (retain)IBOutlet pbListCtrl *pbList;
@property (retain)IBOutlet pbTreeCtrl *pbTree;

// set/get static value
+ (BOOL)GetDownloadState;
+ (void)SetDownloadState:(BOOL)state;
+ (BOOL)GetFreeIdx:(const CString&)str withIdx:(int&)idx;
+ (void)InsertFreeIdx:(const CString&)str withIdx:(const int)idx;
+ (void)ClearAllIdx;
//by Get Telephoto Number
- (bool)GetPhoneNumber:(CString &)strNum;
// Init function
- (void)InitPbList;
- (void)InitTreeCtrl;
- (void)InitToolbar;
- (void)setToolbarItem:(int)index state:(BOOL)bEnable;
- (void)onTBButtonEnabled;
- (void)setDefaultTreeSelectItem;
- (void)EnableAllCtrl:(BOOL)bEnable;
- (void)InitSearchMenu;

//menu and toolbar action
- (IBAction)OnPBFolderSendMsg:(id)sender;
- (IBAction)OnPbNew:(id)sender;
- (IBAction)OnPbOpen:(id)sender;
- (IBAction)OnEditCut:(id)sender;
- (IBAction)OnEditCopy:(id)sender;
- (IBAction)OnEditPaste:(id)sender;
- (IBAction)OnEditClear:(id)sender;

- (IBAction)setSearchCategoryName:(NSMenuItem *)menuItem;
- (IBAction)setSearchCategoryNumber:(NSMenuItem *)menuItem;
- (IBAction)doSearch:(id)sender;
- (void)ResetSearch;

// Notification message
- (void)onItemchangeListPb:(NSNotification*)notification;
- (void)onSelChangedTreePb:(NSNotification*)notification;


- (void)onTBButtonEnabled;


//folder operation
- (void)setFolderName:(PbFolder *)pFolder withIndex:(int)index;
- (PbFolder*)setSubFolderName:(PbFolder*)pFolder withIndex: (int&)index andSubFolderPos: (SubfolderPos&)pos;
- (void)InitFolder;
- (PbFolder*) insertSubFolder:(PbFolder*)pFolder withIndex:(int&)index;

//get and set function
- (std::vector<PhoneBookItem*>)getPhbVector;
- (BOOL)getSimStatus;
- (BOOL)getConnected;
- (BOOL)getConnStatus;
- (CComAgent*)getComAgentObj;
- (int)getSysState;
- (NSInteger)getLastSelectItem;
- (NSTreeNode*)GetCopyItem;
- (NSTreeNode*)GetDeleteItem;
- (std::vector<PhoneBookItem*>&)GetCopyContacts;
- (std::vector<PhoneBookItem*>&)GetDeleteContacts;

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
- (LRESULT)OnURCCallBack:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)OnRspCallBack:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)OnDestroyDlg:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)OnInitResult:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)OnPbRefresh:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)OnPbInsert:(WPARAM)wParam andLparam:(LPARAM)lParam;
- (LRESULT)OnPbUpdate:(WPARAM)wParam andLparam:(LPARAM)lParam;

//other
-(void)ResetEditCtrl;
- (void)display:(PbFolder *)folder;
- (void)ParseSystemStatus:(CUrcSys*)pUrc;
- (void)ClearClipboard;
- (void)ClearContactList;
- (void)ShowReadDlg:(DOWNLOAD_TYPE)type;
- (void)UpdateMemory;
- (void)UpdateInfo;
- (void)Update;
- (void)OnPbCSV;

- (CString)GetMemory:(int)idx;
- (int)GetLimit:(const CString&)strMem;
- (size_t)GetMemorySize;
- (int)GetNameLimit:(const CString&)strMem;
- (int)GetNumberLimit:(const CString&)strMem;
- (void)SetPbkTotal:(const CString&)memtype andTotal:(int)total;
- (bool)CheckClipboard:(const PhoneBookItem*)pContact;
- (bool)CheckClipboardFolder:(PbFolder *)pFolder;
- (int)GetPbkSmTotal;
- (int)GetPbkMeTotal;

- (void)EnterDlg;
- (void)LeaveDlg;
- (void)AddNewContact:(LPCTSTR)strHandset Home:(LPCTSTR)strHome;
- (void)ShowEditorDlg:(LPCTSTR)strName Handset:(LPCTSTR)strHandset Home:(LPCTSTR)strHome Company:(LPCTSTR)strCompany Email:(LPCTSTR)strEmail
			   Office:(LPCTSTR)strOffice Fax:(LPCTSTR)strFax Birthday:(LPCTSTR)strBirthday withIdx:(int)nItem andContact:(PhoneBookItem*)pContact;


- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary*)dict;

- (void)NotifyDownloadCH;
- (NSWindow*)getWindow;
- (void)closeWindow;
- (void)closeEditorWindow;
- (void)closeReaderWindow;
- (void)closeCSVWindow;

@end
