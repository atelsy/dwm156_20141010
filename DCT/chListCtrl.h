//
//  chListCtrl.h
//  DCT
//
//  Created by mtk on 12-6-7.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "utility.h"
#import "chJobThreadDlg.h"
#import "chListData.h"

#define CHNAMECELL		@"CHNameCell"
#define CHNUMBERCELL	@"CHNumberCell"
#define CHTIMECELL		@"CHTimeCell"
#define CHITEM			@"CHItem"

@interface chListCtrl : NSView<NSTableViewDelegate, NSTableViewDataSource>
{
	NSMutableArray *dataSource;
	chJobThreadDlg *m_dlg;
	NSTableColumn *lastColumn;
	IBOutlet NSTableView *tableView;
	int m_numRC;
	int m_numLD;
	int m_numMC;
	BOOL m_bAscending;
	
	std::vector<PhoneBookItem*> m_vChCache;
	ChFolder *m_pFolderDelete;
}

@property (nonatomic, retain) IBOutlet NSTableView *tableView;
@property (nonatomic, retain) NSMutableArray *dataSource;
@property (nonatomic, assign) int m_numMC;
@property (nonatomic, assign) int m_numRC;
@property (nonatomic, assign) int m_numLD;
@property (nonatomic, assign) BOOL m_bAscending;

- (void)onChDelete;
- (void)onchSaveas;
- (void)onChSendMessage;
- (void)onChCall;

- (BOOL)getSelected;
- (NSInteger)getSelectedCount;
- (NSInteger)getItemCount;
- (void)getSelectItem:(NSTreeNode**)node withItems:(std::vector<PhoneBookItem*>&) vec;

- (void)selectLastItem:(ChFolder*)pFolder;
- (void)selectItem:(NSInteger)nItem;

- (BOOL)getPhoneNumber:(NSString**)strNum withName:(NSString**)strName;
- (BOOL)getContactName:(CString&)name andNumber:(CString&)number;
- (id)getParent;
- (BOOL)getItem:(PhoneBookItem*)pItem time:(CString&)sTime;
- (PhoneBookItem*)getItemData:(int)nItem;

- (void)deleteAllItems;
- (void)deleteCall:(NSTreeNode*)hDelete withItems:(std::vector<PhoneBookItem*>&)vec andID:(NSString*)nID;
- (void)deleteItem:(PhoneBookItem*)pItem inFolder:(ChFolder*)pFolder;
- (void)updateListCtrl;

- (void)display:(ChFolder*)pFolder withMemoryType:(int)memoryType;
- (void)insertItem:(PhoneBookItem*)pItem withMemory:(int)memoryType andIndex:(int)nItem;
- (void)addRow:(chListData*)data;
- (void)updateStatusBar;
- (void)closeThreadDlg;

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
- (LRESULT)onDestroyCallDlg:(WPARAM)wParam andLparam:(LPARAM)lParam;

@end
