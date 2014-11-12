//
//  DialupSettingsController.h
//  DCT
//
//  Created by MBJ on 11-12-23.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DialupMgr.h"

enum {
	STAT_NOITEM,	//	no item selected. enable new
	STAT_SELECTED,	//	one selected, enable all except save
	STAT_NEW,		//	creating new one, clear contents, enable save, cancel
	STAT_EDIT,		//	editing, same as above, except clear contents.
};

@interface DialupSettingsController : NSViewController {
	IBOutlet NSTextField*	serviceLabel;
	IBOutlet NSComboBox*	servicesBox;
	IBOutlet NSTextField*	serviceNameLabel;
	IBOutlet NSTextField*	serviceName;
	IBOutlet NSTextField*	telNumberLabel;
	IBOutlet NSTextField*	telNumber;
	IBOutlet NSTextField*	accountNameLabel;
	IBOutlet NSTextField*	accountName;
	IBOutlet NSTextField*	passWordLabel;
	IBOutlet NSTextField*	passWord;
	IBOutlet NSTextField*	APN;
	
	IBOutlet NSButton*		btnNew;
	IBOutlet NSButton*		btnEdit;
	IBOutlet NSButton*		btnDelete;
	IBOutlet NSButton*		btnSave;
	IBOutlet NSButton*		btnCancel;
	
	NSInteger		m_iLastSel;
	NSMutableArray*	m_EntryInfoList;

@public
	int		m_iStat;
}
- (void)SetCtrlStat;
- (void)SetBtnStat;
- (void)FillEntryParam:(ENTRYINFO*)info;
- (void)FillCtrls:(ENTRYINFO*)info;
- (void)SaveChanges;
- (NSInteger)UpdateServList:(NSString*)selname;

- (IBAction)OnBtnNewClick:(id)sender;
- (IBAction)OnBtnEditClick:(id)sender;
- (IBAction)OnBtnDeleteClick:(id)sender;
- (IBAction)OnBtnSaveClick:(id)sender;
- (IBAction)OnBtnCancelClick:(id)sender;
- (IBAction)OnSelServChange:(id)sender;

@end
