//
//  callHistoryViewController.m
//  DCT
//
//  Created by mtk on 12-6-6.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import "callHistoryViewController.h"
#import "DCTAppDelegate.h"
#import "treeNode.h"

static NSString *tooltips[] = 
{
	@"ID_CH_CALL",
	@"ID_CH_SENDMSG",
	@"ID_CH_SAVENUM",
	@"ID_CH_DELETE",
	@"ID_CH_DOWNLOAD",
};

@implementation callHistoryViewController

@synthesize chTree, chList;
@synthesize searchField;
@synthesize tbMatrix, m_MCNum, m_RCNum, m_LDNum, m_bClean, m_bConn, globalData;
@synthesize m_bDownload, m_bFlags, m_bPromptDown, m_MissCallnum, m_ResvCallnum, m_DialCallnum, m_bSelect, m_CHBEnable, m_bMissCall;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
	if (self) 
	{
		// Initialization code here.
		m_bClean = NO;
		m_bSelect = NO;
		m_bConn = NO;
		m_bDownload = NO;
		m_bPromptDown = NO;
		m_bFlags = NO;
		readDlg = nil;
		m_pMissCallFolder = NULL;
		m_pRecvCallFolder = NULL;
		m_pDialCallFolder = NULL;
		m_nNewCall = 0;
		m_CHBEnable = NO; 
		m_bSavePb = YES;
		m_bSendSms = YES;
		m_bMissCall = NO;
		m_nEnterDlg = 0;
		
		m_MissCallnum = m_ResvCallnum = m_DialCallnum = 0;
		
		globalData = [[NSMutableArray alloc] init];
		
		//register notification 
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onDestroyDialog:) name:WM_CH_DESTROYDIALOG object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSelChangedTreeCh:) name:CHTREE_TVN_SELCHANGED object:nil];
	}
	
	return self;
}

- (void)dealloc
{
	[globalData release];
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	if(readDlg)
	{
		[readDlg release];
		readDlg = nil;
	}
	
	[super dealloc];
}

- (void)awakeFromNib
{
	[self initToolbar];
	[self initSearchMenu];
	
	[self initFolder];
	[self initTreeCtrl];
	
	[self onTBButtonEnabled];
	
	m_bSelect = YES;
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

#pragma mark - init method

- (void)initToolbar
{
	NSInteger count = [[tbMatrix cells] count];
	
	for(NSInteger i = 0; i < count; i++)
	{
		NSButtonCell *cell = [tbMatrix cellAtRow:0 column:i];
		if(cell)
		{
			[tbMatrix setToolTip:LocalizeString(tooltips[i]) forCell:cell];
			[cell setEnabled:NO];
		}
	}
}

- (void)initSearchMenu
{
	NSMenu *menu = [[[NSMenu alloc] initWithTitle:@"Search Menu"] autorelease];
	
	NSMenuItem *item;
	
	item = [[[NSMenuItem alloc] initWithTitle:LocalizeString(@"SEARCH_CATEGORY_NAME") action:@selector(setSearchCategoryName:) keyEquivalent:@""] autorelease];
	
	[item setTarget:self];
	[item setTag: SEARCHNAME];
	[item setState:NSOnState];
	[menu insertItem:item atIndex:0];
	
	item = [[[NSMenuItem alloc] initWithTitle:LocalizeString(@"SEARCH_CATEGORY_NUMBER") action:@selector(setSearchCategoryNumber:) keyEquivalent:@""] autorelease];
	[item setTarget:self];
	[item setTag:SEARCHNUMBER];
	
	[menu insertItem:item atIndex:1];
	
	id searchCell = [searchField cell];
	[searchCell setSearchMenuTemplate:menu];
	
	m_searchCategory = SEARCHNAME;
}

- (void)initTreeCtrl
{
	[[chTree outlineView] expandItem:nil expandChildren:YES];
	[[chTree outlineView] selectRowIndexes:[NSIndexSet  indexSetWithIndex:0] byExtendingSelection:NO];
}

#pragma mark - search method

- (IBAction)setSearchCategoryName:(NSMenuItem *)menuItem
{
	m_searchCategory = (int)[menuItem tag];
	
	NSMenu *menu = [menuItem menu];
	[[menu itemAtIndex:1] setState:NSOffState];
	[menuItem setState: NSOnState];
	
	[self doSearch:nil];
}

- (IBAction)setSearchCategoryNumber:(NSMenuItem *)menuItem
{
	m_searchCategory = (int)[menuItem tag];
	
	NSMenu *menu = [menuItem menu];
	[[menu itemAtIndex:0] setState:NSOffState];
	[menuItem setState:NSOnState];
	
	[self doSearch:nil];
}

- (void)resetSearch
{
	[searchField setStringValue:@""];
	[self doSearch:nil];
	
	[chList.tableView deselectAll:nil];
	
	[chList updateStatusBar];
}

- (IBAction)doSearch:(id)sender
{
	NSPredicate *predicate = nil;
	NSString *searchString = [searchField stringValue];
	
	ChFolder *pFolder = [chTree getSelectedItemData];
	
	if(pFolder == NULL)
		return;
	
	int count = pFolder->GetItemSize();
	
	int nMemoryType = 0;
	
	if([chTree ifMissCallFolder:pFolder])
		nMemoryType = 0;
	else if([chTree ifResvCallFolder:pFolder])
		nMemoryType = 1;
	else if([chTree ifDialCallFolder:pFolder])
		nMemoryType = 2;
	
	if([[chList dataSource] count] != count)
	{
		[chList display:pFolder withMemoryType:nMemoryType];
	}
	
	if(searchString != nil && (![searchString isEqualToString:@""]))
	{
		if(m_searchCategory == SEARCHNAME)
		{
			predicate = [NSPredicate predicateWithFormat:@"CHNameCell contains %@", searchString];
		}
		else if(m_searchCategory == SEARCHNUMBER)
		{
			predicate = [NSPredicate predicateWithFormat:@"CHNumberCell contains %@", searchString];
		}
		
		[globalData removeAllObjects];
		[globalData addObjectsFromArray:chList.dataSource];
		
		[chList.dataSource filterUsingPredicate:predicate];
	}
	
	[chList.tableView reloadData];
	
	[chList.tableView deselectAll:nil];
	
	[chList updateStatusBar];
}

#pragma mark - tree folder

- (void)initFolder
{
	ChFolder *folder = [TheDelegate getChFolder];
	
	int i, j;
	
	if(folder->GetSubfolderSize() == 0)
	{
		for(i = 0, j = 0; i < 3; i++)
		{
			[self insertSubFolder:folder withIndex:i];
		}
	}
	else
	{
		i = j = 0;
		
		SubfolderPos pos = folder->GetFirstSubfolderPos();
		
		while (i < 3 && pos != 0)
		{
			[self setSubFolderName:folder withIndex:i andSubFolderPos:pos];
			i++;
		}
		
		folder->CloseNextSubfolder(pos);
	}
}

- (ChFolder*) insertSubFolder:(ChFolder *)pFolder withIndex:(int&)index
{
	if(!pFolder)
		return NULL;
	
	ChFolder *pSubfolder = [utility newChFolder];
	if(!pSubfolder)
		return NULL;
	
	[self setFolderName:pSubfolder withIndex:index];
	pFolder->InsertSubfolder(pSubfolder);
	
	return pSubfolder;
}

- (ChFolder*)setSubFolderName:(ChFolder *)pFolder withIndex:(int &)index andSubFolderPos:(SubfolderPos &)pos
{
	if(!pFolder || !pos)
		return NULL;
	
	ChFolder *pSubFolder = pFolder->GetNextSubfolder(pos);
	
	[self setFolderName:pFolder withIndex:index];
	
	return pSubFolder;
}

- (void)setFolderName:(ChFolder *)pFolder withIndex:(int)index
{
	if(!pFolder)
		return;
	
	NSString *ID = [@"IDS_CH_FOLDER" stringByAppendingString:[NSString stringWithFormat:@"%d", index]];
	
	NSString *name = LocalizeString(ID);
	
	pFolder->SetFolderName((char*)[name UTF8String]);
}

#pragma mark - IB action

- (BOOL)isEnabled:(NSInteger)tag
{
	NSButtonCell *cell = (NSButtonCell*)[tbMatrix cellWithTag:tag];
	
	return [cell isEnabled];
}

- (IBAction)onEditClear:(id)sender
{	
	if([chList getSelected] == NO && [chList.tableView clickedRow] == -1)
		return;
	
	folder_state_struct state;
	ChFolder *pFolder = [chTree getSelectedItemData];
	
	if(pFolder)
	{
		pFolder->GetFolderState(state);
		
		if(state.nState == -1)
			return;
	}
	
	[chList onChDelete];
}

- (IBAction)onSaveNum:(id)sender
{
	if(([chList getSelected] == NO && [chList.tableView clickedRow] == -1) ||
	   [chList getSelectedCount] > 1 || m_bSavePb == NO) 
		return;
	
	[chList onchSaveas];
}

- (IBAction)onDialCall:(id)sender
{
	if(([chList getSelected] == NO && [chList.tableView clickedRow] == -1) ||
	   [chList getSelectedCount] > 1) 
		return;
	
	[chList onChCall];
}

- (IBAction)onSendMessage:(id)sender
{
	if(([chList getSelected] == NO && [chList.tableView clickedRow] == -1) ||
	   [chList getSelectedCount] > 1 || m_bSendSms == NO) 
		return;
	
	[chList onChSendMessage]; 
}

- (IBAction)onDownload:(id)sender
{
	if([self isEnabled:ID_CH_DOWNLOAD] == NO)
		return;
	
	m_bDownload = NO;
	m_bFlags = NO;
	m_bPromptDown = YES;
	m_MissCallnum = m_DialCallnum = m_ResvCallnum = 0;
	
	[self showReadDlg:nil];
}

- (void)showReadDlg:(id)sender
{
	if(m_nEnterDlg > 0)
		return;
	
	[self enterDlg];
	
	[searchField setStringValue:@""];
	
	NSTreeNode *hItem;
	
	if([chTree getMissCallItem:&hItem andFolder:&m_pMissCallFolder] && [chTree getResvCallItem:&hItem andFolder:&m_pRecvCallFolder] && [chTree getDialCallItem:&hItem andFolder:&m_pDialCallFolder])
	{
		m_pFolder = [chTree getSelectedItemData];
		
		[chList deleteAllItems];
		
		if(m_pMissCallFolder)
			m_pMissCallFolder->DeleteAll(INCLUDE_SUBFOLDER);
		
		if(m_pRecvCallFolder)
			m_pRecvCallFolder->DeleteAll(INCLUDE_SUBFOLDER);
		
		if(m_pDialCallFolder)
			m_pDialCallFolder->DeleteAll(INCLUDE_SUBFOLDER);
		
		if(readDlg == nil)
		{
			readDlg = [chReadDlg alloc];
			
			if([readDlg initMissCallFolder:m_pMissCallFolder ResvCallFolder:m_pRecvCallFolder andDialCallFolder:m_pDialCallFolder withParent:self] == NO)
				[self leaveDlg];
		}
	}
}

#pragma mark - toolbar state and menu item state

- (void)onTBButtonEnabled
{
	if(m_nEnterDlg != 0)
		return;
	
	BOOL bEnable = [chList getSelected];
	
	folder_state_struct state;
	ChFolder *pFolder = [chTree getSelectedItemData];
	
	if(pFolder)
	{
		pFolder->GetFolderState(state);
	}
	else
	{
		state.nState = 0;
	}
	
	if(state.nState == 1)
	{
		[self setToolbarItem:ID_CH_CLEAN State:NO];
	}
	else
	{
		[self setToolbarItem:ID_CH_CLEAN State:bEnable];
	}
	
	if([chList getSelectedCount] > 1)
	{
		[self setToolbarItem:ID_CH_CALL State:NO];
		[self setToolbarItem:ID_CH_SAVENUM State:NO];
		[self setToolbarItem:ID_CH_SENDMSG State:NO];
	}
	else
	{
		if(!m_bSendSms)
			[self setToolbarItem:ID_CH_SENDMSG State:NO];
		else
			[self setToolbarItem:ID_CH_SENDMSG State:bEnable];
		
		if(!m_bSavePb)
			[self setToolbarItem:ID_CH_SAVENUM State:NO];
		else
			[self setToolbarItem:ID_CH_SAVENUM State:bEnable];
		
		[self setToolbarItem:ID_CH_CALL State:bEnable && [self getSysState] > SYS_ATREADY];
	}
	
	[self setToolbarItem:ID_CH_DOWNLOAD State:m_CHBEnable];
}

- (void)setToolbarItem:(int)index State:(signed char)bEnable
{
	NSButtonCell *cell = [tbMatrix cellWithTag:index];
	NSImage *image = nil;
	
	if(cell)
	{
		[cell setEnabled:bEnable];
		
		if(bEnable)
		{
			image = [NSImage imageNamed:chEnableToolbarImages[index]];
		}
		else
		{
			image = [NSImage imageNamed:chDisableToolbarImages[index]];
		}
		
		[cell setImage:image];
	}
}

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	int tag = (int)[menuItem tag];
	
	NSUInteger clickedRow = [chList.tableView clickedRow];
	NSIndexSet *set = [chList.tableView selectedRowIndexes];
	
	if(tag == ID_CH_CALL)
	{
		if(((clickedRow != NSUIntegerMax && !([set containsIndex:clickedRow] && [set count] > 1)) ||
		       (clickedRow == NSUIntegerMax && [set count] == 1))
					&& [self getSysState] > SYS_ATREADY  
					)
			return YES;
		else
			return NO;
	}
	else if(tag == ID_CH_SAVENUM)
	{
		if( ((clickedRow != NSUIntegerMax && !([set containsIndex:clickedRow] && [set count] > 1)) ||
				(clickedRow == NSUIntegerMax && [set count] == 1))
					&& m_bSavePb
					)
			return YES;
		else
			return NO;
	}
	else if(tag == ID_CH_SENDMSG)
	{
		if( ((clickedRow != NSUIntegerMax && !([set containsIndex:clickedRow] && [set count] > 1)) ||
				(clickedRow == NSUIntegerMax && [set count] == 1))
							&& m_bSendSms 
							)
			return YES;
		else
			return NO;
	}
	else if(tag == ID_CH_CLEAN)
	{
		if(clickedRow != NSUIntegerMax || (clickedRow == NSUIntegerMax && [set count] >= 1))
			return YES;
		else
			return NO;
	}
	else if(tag == SEARCHNAME || tag == SEARCHNUMBER)
	{
		if([chList getItemCount] > 0 || [globalData count] > 0)
			return YES;
		else
			return NO;
	}
	
	return YES;
}

- (void)enableAllCtrl:(signed char)bEnable
{
	[[chTree outlineView] setEnabled:bEnable];
	[[chList tableView] setEnabled:bEnable];
	
	[self setToolbarItem:ID_CH_CALL State:bEnable];
	[self setToolbarItem:ID_CH_SENDMSG State:bEnable];
	[self setToolbarItem:ID_CH_SAVENUM State:bEnable];
	[self setToolbarItem:ID_CH_DOWNLOAD State:bEnable];
	[self setToolbarItem:ID_CH_CLEAN State:bEnable];
}

#pragma mark - ComAgent method

- (CComAgent*)getComAgentObj
{
	return [TheDelegate getComAgent];
}

- (BOOL)getOperationStatus:(OPERATION_TYPE)type
{
	switch (type) 
	{
		case OP_SMS:
			return m_bSendSms;
			
		case OP_PB:
			return m_bSavePb;
	}
	
	return 0;
}

- (BOOL)getLastDialedNUmber:(CString &)strNum
{
	ChFolder *pFolder = NULL;
	NSTreeNode *hItem;
	CString strName;
	
	[chTree getDialCallItem:&hItem andFolder:&pFolder];
	
	if(pFolder == NULL)
		return NO;
	
	ItemPos pos = pFolder->GetFirstItemPos();
	
	if(pos == 0)
		return NO;
	
	PhoneBookItem *pItem = pFolder->GetNextItem(pos);
	if(pItem != NULL)
		pItem->GetPhone(strName, strNum);
	
	pFolder->CloseNextItem(pos);
	
	return YES;
}

- (const TCHAR*)getMemType:(PhoneBookItem &)item withCall:(const CCall &)call
{
	static const TCHAR *szLD = _T("LD");
	static const TCHAR *szRC = _T("RC");
	static const TCHAR *szMC = _T("MC");
	
	if(call.IsMoCall())
	{
		item.SetMemType(szLD);
#ifdef _CH_WRITE_LD_
		return szLD;
#else
		return NULL;
#endif
	}
	
	if(call.IsRinging())
	{
		item.SetMemType(szMC);
#ifdef _CH_WRITE_MC_
		return szMC;
#else
		return NULL;
#endif
	}
	
	item.SetMemType(szRC);
#ifdef _CH_WRITE_RC_
	return szRC;
#else
	return NULL;
#endif
}

- (tm)nsDateTimeToTM:(NSDate *)date
{
	NSCalendar *gregorian = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
	NSDateComponents *components = [gregorian components:NSDayCalendarUnit | NSMonthCalendarUnit | 
									NSYearCalendarUnit | NSHourCalendarUnit |
									NSMinuteCalendarUnit | NSSecondCalendarUnit |
									NSWeekdayCalendarUnit fromDate:date];
	
	tm rec_time = {0};
	
	rec_time.tm_year = (int)[components year] - 1900;        
	rec_time.tm_mon = (int)[components month];							
	rec_time.tm_mday = (int)[components day];						
	rec_time.tm_hour = (int)[components hour];								
	rec_time.tm_min = (int)[components minute];								
	rec_time.tm_sec = (int)[components second];	
	
	
	return rec_time;
}

- (void)insertItem:(const CCall &)call
{
	if(call.number.IsEmpty())
		return;
	
	PhoneBookItem *pItem = new PhoneBookItem;
	
	CString strNum = call.number;
	if(NULL != call.GetDtmf())
		strNum += *(call.GetDtmf());
	
	pItem->SetIndex(0);
	pItem->SetPhone(_T(""), strNum, PBCHS_UCS2);
	
	NSDate *today = [NSDate date];
	

	//NSCalendar *gregorian = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
	//NSDateComponents *components = [gregorian components:NSDayCalendarUnit | NSMonthCalendarUnit | 
	//												NSYearCalendarUnit | NSHourCalendarUnit |
	//												NSMinuteCalendarUnit | NSSecondCalendarUnit |
	//												NSWeekdayCalendarUnit fromDate:today];
	
	tm rec_time = {0};
	
	rec_time = [self nsDateTimeToTM:today];
	
	//const CTime t = CTime::GetCurrentTime();
	/*rec_time.tm_year = (int)[components year] - 1900;        //t.GetYear() - 1900;
	rec_time.tm_mon = (int)[components month];							//	t.GetMonth() - 1;
	rec_time.tm_mday = (int)[components day];						//t.GetDay();
	rec_time.tm_hour = (int)[components hour];								//t.GetHour();
	rec_time.tm_min = (int)[components minute];								//t.GetMinute();
	rec_time.tm_sec = (int)[components second];								//t.GetSecond();
	*/
	pItem->SetTime(rec_time);
	
	const TCHAR *szMem = [self getMemType:*pItem withCall:call];
	if(NULL == szMem)
	{
		[self updateRecord:*pItem];
		delete pItem;
		return;
	}
	
	CComAgent *pCom = [self getComAgentObj];
	CAERR err = pCom->WritePBK(self, WM_CH_INSERT, 0, pItem, szMem, (LONG)pItem);
	
	if(CA_OK != err)
	{
		delete pItem;
	}
	else
	{
		
	}
}

#pragma mark - other method

- (void)clearIndication:(int)num
{
	m_nNewCall = num;
}

- (int)getSysState
{
	return [TheDelegate getComAgent]->GetSysState();
}

- (void)enterDlg
{
	m_nEnterDlg++;
	
	[self enableAllCtrl:NO];
}

- (void)leaveDlg
{
	m_nEnterDlg--;
	
	if(m_nEnterDlg <= 0)
	{
		m_nEnterDlg = 0;
		[self enableAllCtrl:YES];
	}
}

#pragma mark - notification method

- (LRESULT)onRefreshList:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	ChFolder *pFolder;
	PhoneBookItem *pItem;
	CString strName, strNumber;
	CString userName;
	int memory = 0;
	BOOL bFlag = YES;
	
	pFolder = [chTree getSelectedItemData];
	
	if(pFolder == NULL)
		return 1;
	
	ItemPos pos = pFolder->GetFirstItemPos();
	
	while (pos != 0)
	{
		if((pItem = pFolder->GetNextItem(pos)))
		{
			pItem->GetPhone(strName, strNumber);
			if(bFlag)
			{
				const CString &memType = pItem->GetMemType();
				bFlag = NO;
				
				if(memType.Compare(_T("MC")) == 0)
					memory = 0;
				else if(memType.Compare(_T("RC")) == 0)
					memory = 1;
				else if(memType.Compare(_T("LD")) == 0)
					memory = 2;
			}
			
			if([chList getContactName:userName andNumber:strNumber])
			{
				if(userName == _T(""))
					pItem->SetPhone(_T("[Unknown]"), strNumber, PBCHS_DEFAULT);
				else
					pItem->SetPhone(userName, strNumber, PBCHS_UCS2);
			}
		}
	}
	
	[chList display:pFolder withMemoryType:memory];
	
	return 0;
}

- (void)onRspCallback:(long)wParam lParm:(long)lparam
{
	WATCH_POINTER(lparam);
	CRespArray *pRespArray;
	
	switch (LOWORD(wParam))
	{
		case RESP_CH_GETMISSCALLNUM:
			pRespArray = (CRespArray*)lparam;
			if(HIWORD(wParam) == ER_OK)
			{
				m_MCNum = pRespArray->Value()[0];
				[readDlg setMissCallNumber:m_MCNum withFlag:YES];
				[readDlg getCallNumber:RESP_CH_GETRECEIVECALLNUM];
			}
			else
			{
				[readDlg setMissCallNumber:0 withFlag:NO];
				
				PostMessage(self, WM_INIT_RESULT, 0, INIT_FAILED);
			}
			
			break;
			
		case RESP_CH_GETRECEIVECALLNUM:
			pRespArray = (CRespArray*)lparam;
			if(HIWORD(wParam) == ER_OK)
			{
				m_RCNum = pRespArray->Value()[0];
				[readDlg setReceiveCallNumber:m_RCNum withFlag:YES];
				[readDlg getCallNumber:RESP_CH_GETDIALCALLNUM];
			}
			else
			{
				[readDlg setMissCallNumber:0 withFlag:NO];
				
				PostMessage(self, WM_INIT_RESULT, 0, INIT_FAILED);
			}
			
			break;
			
		case RESP_CH_GETDIALCALLNUM:
			pRespArray = (CRespArray*)lparam;
			
			if(HIWORD(wParam) == ER_OK)
			{
				m_LDNum = pRespArray->Value()[0];
				[readDlg setDialCallNumber:m_LDNum withFlag:YES];
				
				PostMessage(self, WM_INIT_RESULT, 0, INIT_SUCCESS);
			}
			else
			{
				[readDlg setDialCallNumber:0 withFlag:NO];
				
				PostMessage(self, WM_INIT_RESULT, 0, INIT_FAILED);
			}
			
			break;
			
		default:
			break;
	}
}

- (void)onSelChangedTreeCh:(NSNotification*)notification
{
	if (m_bSelect == NO)
	{
		return;
	}
	
	NSTreeNode *node = [[chTree outlineView] itemAtRow:[chTree.outlineView selectedRow]];
	treeNode *data = [node representedObject];
	
	ChFolder *pFolder = (ChFolder*)[data getTag];
	
	if([chTree ifMissCallFolder:pFolder])
	{
		m_MemoryType = 0;
		m_MissCallnum = 0;
	}
	else if([chTree ifResvCallFolder:pFolder])
	{
		m_MemoryType = 1;
		m_ResvCallnum = 0;
	}
	else if([chTree ifDialCallFolder:pFolder])
	{
		m_MemoryType = 2;
		m_DialCallnum = 0;
	}
	
	if(pFolder)
	{
		[chList display:pFolder withMemoryType:m_MemoryType];
	}
	
	[searchField setStringValue:@""];
	
	[globalData removeAllObjects];
	[globalData addObjectsFromArray:chList.dataSource];
	
	[chList updateStatusBar];
	
	[self onTBButtonEnabled];
}

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	switch (Msg)
	{
		case WM_RSPCALLBACK:
			[self onRspCallback:wparam lParm:lparam];
			break;
			
		case WM_DISPLAY_MISSCALL:
			[self onDisplayMC:wparam andLparam:lparam];
			break;
			
		case WM_INIT_RESULT:
			[self onInitResult:wparam andLparam:lparam];
			break;
			
		case WM_ENABLEOPERATION:
			[self onEnableOperation:wparam andLparam:lparam];
			break;
			
		case WM_CH_STARTDOWNLOAD:
			[self onCHDownload:wparam andLparam:lparam];
			break;
			
		case WM_CH_INSERT:
			[self onCHInsert:wparam andLparam:lparam];
			break;
			
		case WM_REFRESH_LIST:
			[self onRefreshList:wparam andLparam:lparam];
			break;
			
		case WM_CH_DESTROYDLG:
			[self onDestroyDialog:wparam andLparam:lparam];
			break;
			
		case WM_DOWNLOADPROMPT:
			[self onDownloadPrompt:wparam andLparam:lparam];
			break;
			
		default:
			break;
	}
}

- (LRESULT)onDestroyDialog:(long)wParam andLparam:(long)lParam
{
	switch (wParam)
	{
		case CH_DLG_READ:
			[chList display:m_pFolder withMemoryType:m_MemoryType];
			
			if(readDlg != nil)
			{
				[readDlg release];
			}
			
			readDlg = nil;
			
			if(m_bMissCall)
				PostMessage(self, WM_DISPLAY_MISSCALL, 0, 0);
			
			if(m_bDownload || m_bPromptDown)
			{
				m_nNewCall = 0;
				
				//TODO: update call indication
				
				
			}
			
			m_bPromptDown =  NO;
			m_bDownload = YES;
			
			if(m_bFlags)
			{
				//todo:
			}
			
			[self leaveDlg];
			
			[self onTBButtonEnabled];
			
			[chList updateStatusBar];
			
			break;
			
		default:
			break;
	}
	
	return 0;
}

- (LRESULT)onInitResult:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	switch (lParam)
	{
		case INIT_SUCCESS:
			if(readDlg == nil)
				readDlg = [chReadDlg alloc];
			
			[readDlg initWithWindowNibName:@"chRead"];
			
			if(TheDelegate.currentView == CALLHISTORYVIEW)
			{
				[readDlg showWindow:self];
				[[readDlg window] orderFrontRegardless];
			}
			else
			{
				[[readDlg window] orderOut:nil];
			}
			
			readDlg.m_bShowDlg = YES;
			
			break;
			
		case INIT_FAILED:
			[readDlg release];
			readDlg = nil;
			
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_READCALLHISTORY") forType:MB_OK];
			
			[self leaveDlg];
			
			break;
			
		default:
			break;
	}
	
	return 0;
}

- (LRESULT)onDisplayMC:(long)wParam andLparam:(long)lParam
{
	NSTreeNode *hItem;
	ChFolder *pfolder;
	
	[chTree getMissCallItem:&hItem andFolder:&pfolder];
	
	[chList display:pfolder withMemoryType:0];
	
	[[chTree outlineView] selectRowIndexes:[NSIndexSet  indexSetWithIndex:0] byExtendingSelection:NO];
	
	m_bMissCall = NO;
	
	return 0;
}

- (LRESULT)onEnableOperation:(long)wParam andLparam:(long)lParam
{
	switch (wParam)
	{
		case OP_PB:
			m_bSavePb = lParam;
			break;
			
		case OP_SMS:
			m_bSendSms = lParam;
			break;
			
		default:
			break;
	}
	
	[self onTBButtonEnabled];
	
	return 0;
}

- (LRESULT)onCHDownload:(long)wParam andLparam:(long)lParam
{
	if((BOOL)lParam)
	{
		m_CHBEnable = 1;
		[self onTBButtonEnabled];
		
		if([utility getModalCounter] == 0)
		{
			[self showReadDlg:nil];
		}
	}
	else
	{
		m_CHBEnable = 0;
	}
	
	return 0;
}

- (LRESULT)onCHInsert:(long)wParam andLparam:(long)lParam
{
	WATCH_POINTER(lParam);
	
	CRespFlag *pResp = (CRespFlag*)lParam;
	
	PhoneBookItem *pItem = (PhoneBookItem*)(pResp->m_lExt);
	
	if(pResp->Value())
	{
		[self updateRecord:*pItem];
	}
	else
	{
		
	}
	
	delete pItem;
	
	return 0;
}

- (LRESULT)onDownloadPrompt:(long)wParam andLparam:(long)lParam
{
	if(m_nNewCall > 0 && readDlg == nil && [self getSysState] >= SYS_PINREADY)
	{
		m_bDownload = NO;
		m_bFlags = NO;
		m_bPromptDown = YES;
		[self showReadDlg:nil];
		m_bMissCall = YES;
	}
	
	return 0;
}

#pragma mark - help method

- (void)updateRecord:(PhoneBookItem&)pItem
{
	ChFolder *pFolder = NULL;
	NSTreeNode *hItem;
	ChFolder *pTempFolder;
	BOOL bShow = NO;
	BOOL bFinished = NO;
	BOOL bHavePrefix = NO;
	int nCapacity;
	
	const CString& sMemory = pItem.GetMemType();
	
	if(sMemory == _T("MC"))
	{
		nCapacity = m_MCNum;
		[chTree getMissCallItem:&hItem andFolder:&pFolder];
		m_MemoryType = 0;
		m_MissCallnum++;
		m_nNewCall++;
		m_bFlags = YES;
		pTempFolder = [chTree getSelectedItemData];
		
		if([chTree ifMissCallFolder:pTempFolder])
			bShow = YES;
		
		[TheDelegate setMissCallStatus:m_nNewCall];
	}
	else if(sMemory == _T("RC"))
	{
		nCapacity = m_RCNum;
		[chTree getResvCallItem:&hItem andFolder:&pFolder];
		m_MemoryType = 1;
		m_ResvCallnum++;
		pTempFolder = [chTree getSelectedItemData];
		
		if([chTree ifResvCallFolder:pTempFolder])
		{
			bShow = YES;
		}
	}
	else if(sMemory == _T("LD"))
	{
		nCapacity = m_LDNum;
		[chTree getDialCallItem:&hItem andFolder:&pFolder];
		m_MemoryType = 2;
		m_DialCallnum++;
		pTempFolder = [chTree getSelectedItemData];
		if([chTree ifDialCallFolder:pTempFolder])
			bShow = YES;
	}
	
	if(m_bDownload && pFolder)
	{
		m_bFlags = NO;
		
		PhoneBookItem *pCall = [utility NewContact];
		*pCall = pItem;
		
		CString strName, destName;
		CString strNumber, destNumber;
		
		pCall->GetPhone(strName, strNumber);
		if(strNumber.Find('+') == 0)
			bHavePrefix = YES;
		else
			bHavePrefix = NO;
		
		PhoneBookItem *item = NULL;
		ItemPos pos = pFolder->GetFirstItemPos();
		CString strTempNumber;
		
		while (pos)
		{
			if((item = pFolder->GetNextItem(pos)))
			{
				item->GetPhone(destName, destNumber);
				if(bHavePrefix && destNumber.Find('+') != 0)
					strTempNumber = _T("+") + destNumber;
				else if(!bHavePrefix && destNumber.Find('+') == 0)
					strTempNumber = destNumber.Right(destNumber.GetLength() - 1);
				
				if(strNumber.Compare(destNumber) != 0 && strNumber.Compare(strTempNumber) != 0)
					item->SetIndex(item->GetIndex() + 1);
				else
				{
					pFolder->CloseNextItem(pos);
					bFinished = YES;
					pFolder->DeleteItem(item);
					pCall->SetIndex(1);
					pFolder->InsertFirstPos(pCall);
					break;
				}
			}
		}
		
		if(!bFinished)
		{
			pFolder->CloseNextItem(pos);
			pCall->SetIndex(1);
			pFolder->InsertFirstPos(pCall);
		}
		
		if(pFolder->GetItemSize() > nCapacity)
		{
			PhoneBookItem *pItem = NULL;
			int index;
			ItemPos pos = pFolder->GetFirstItemPos();
			
			while (pos)
			{
				if((pItem = pFolder->GetNextItem(pos)))
				{
					index = pItem->GetIndex();
					if(index > nCapacity)
						pFolder->DeleteItem(pItem);
				}
			}
			
			pFolder->CloseNextItem(pos);
		}
		
		if(bShow && pFolder)
		{
			[chList display:pFolder withMemoryType:m_MemoryType];
		}
	}
	else
	{
		m_DialCallnum = m_MissCallnum = m_ResvCallnum = 0;
		if(m_CHBEnable && m_bDownload)
			[self showReadDlg:nil];
	}
}

#pragma mark - get folder method

- (ChFolder*)getMissCallFolder
{
	return m_pMissCallFolder;
}

- (ChFolder*)getResvCallFolder
{
	return m_pRecvCallFolder;
}

- (ChFolder*)getDialCallFolder
{
	return m_pDialCallFolder;
}

@end


