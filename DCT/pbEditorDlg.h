//
//  pbEditorDlg.h
//  DCT
//
//  Created by mbj on 12-1-16.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"
#import "ComAgent.h"
#import "phoneNumberFormatter.h"
#import "smsTextView.h"

#define WM_PB_INSERT	WM_USER + 21
#define WM_PB_UPDATE	WM_USER + 22

#define TO_BREAK	1
#define TO_CONTINUE 2
#define TO_PROMPT	3

@interface pbEditorDlg : NSWindowController<NSOutlineViewDelegate, NSOutlineViewDataSource, NSWindowDelegate, NSTextViewDelegate>
{
	IBOutlet NSOutlineView	*labelList;
	NSTreeNode *rootNode;
	
	IBOutlet NSTextField	*m_ctlEditName;
	IBOutlet NSTextField	*m_ctlEditHandset;
	IBOutlet NSTextField	*m_ctlEditHome;
	IBOutlet NSTextField	*m_ctlEditCompany;
	IBOutlet NSTextField	*m_ctlEditEmail;
	IBOutlet NSTextField	*m_ctlEditFax;
	IBOutlet NSTextField	*m_ctlEditOffice;
	IBOutlet NSTextField	*m_ctlEditBirthday;
	
	IBOutlet NSMatrix		*m_ctlMemType;
	IBOutlet NSButton		*m_ctlButtonOK;
	IBOutlet NSButton		*m_ctlButtonCancel;
	
	smsTextView *customFieldEditor;
	
	phoneNumberFormatter* m_formatter;
	
	CString	m_strName;
	CString	m_strHandset;
	CString	m_strHome;
	CString	m_strCompany;
	CString	m_strEmail;
	CString	m_strFax;
	CString	m_strOffice;	
	CString	m_strBirthday;
	
	
	NSMutableString *m_NSTextName;
	NSMutableString *m_NSTextHandset;
	NSMutableString *m_NSTextHome;
	NSMutableString *m_NSTextCompany;
	NSMutableString *m_NSTextEmail;
	NSMutableString *m_NSTextFax;
	NSMutableString *m_NSTextOffice;	
	NSMutableString *m_NSTextBirthday;
	
	NSRange m_range;
	
	id	m_pParentWnd;
	std::vector<NSTextField*> m_vEdit;
	BOOL m_bShowDlg;
	BOOL m_bModify;
	int m_nMemory;
	int m_nItem;
	PbFolder *m_pFolder;
	PbFolder *m_pFolderHandset;
	PhoneBookItem *m_pContact;
	bool m_bSupportAdvItem;
	int m_nTotal;
	int m_nMaxNum;
	int m_nLen;
	int m_nMax;
	int m_nEncodeClass;
	CString m_str;
	BOOL m_bTarget;
	BOOL m_bSM;
	BOOL m_bClose;
	int m_nMaxName;
	CString m_strMemory;
	CComAgent *m_pComAgent;
	
}

-(IBAction)OnOK:(id)sender;
-(IBAction)OnCancel:(id)sender;
- (IBAction)OnRadioMatrixChange:(id)sender;

- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary*)dict;

- (void)AdjustStringLength:(NSNotification *)obj;
- (void)InitRadio;
- (void)InitEdit;
- (BOOL)step1:(BOOL)bTarget;
- (void)Prompt;
- (short)CheckPhoneName;
- (BOOL)CheckPBNameContinue:(BOOL)bRet;
- (BOOL)ChangeMemory:(PhoneBookItem*)pContact;
- (void)NextEditCtrl;
- (void)PrevEditCtrl;
- (void)EnableEdit:(BOOL)bEnable;
- (CString)GetMemory;
- (void)InitEdit;
- (void)InitRadio;
- (void)InitRadio:(int)idx andState:(BOOL)bEnable;
- (void)OnRadio1;
- (void)OnRadio2;
- (void)UpdateData:(BOOL)flag;
- (void)BtnEnable:(BOOL)bEnable;
- (void)CloseDialog;
- (PhoneBookItem*)InitContact:(int)encodeClass;
- (void)Init:(LPCTSTR)strName Handset:(LPCTSTR)strHandset Home:(LPCTSTR)strHome Company:(LPCTSTR)strCompany 
	   Email:(LPCTSTR)strEmail Office:(LPCTSTR)strOffice Fax:(LPCTSTR)strFax Birthday:(LPCTSTR)strBirthday
	 withIdx:(int)nItem andFolder:(PbFolder*)pFolder andHandset:(PbFolder*)pFolderHandset
  andContact:(PhoneBookItem*)pContact withAdv:(bool) bSupportAdvItem andParentwnd:pParent;

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
- (LRESULT) OnCheckPhoneName:(WPARAM)wParam andLparam:(LPARAM)lParam;


@end
