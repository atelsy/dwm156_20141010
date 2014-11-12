//
//  chListCtrl.m
//  DCT
//
//  Created by mtk on 12-6-7.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import "chListCtrl.h"
#import "ImageTextCell.h"
#import "callHistoryViewController.h"
#import "chTreeControl.h"
#import "DCTAppDelegate.h"
#import "pbCocoa.h"
#import "SMSViewCtrl.h"
#import "pbViewControl.h"
#import "callControl.h"
#import "nsstrex.h"

@implementation chListCtrl

@synthesize tableView, dataSource;
@synthesize m_numMC, m_numRC, m_numLD, m_bAscending;

- (id)init
{
	self = [super init];
	
	if(self)
	{
		m_numLD = m_numRC = m_numMC = 0;
		m_dlg = nil;
		
		dataSource = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (void)awakeFromNib
{
	NSTableColumn *column = [tableView tableColumnWithIdentifier:CHNAMECELL];
	
	if([NSSortDescriptor respondsToSelector:@selector(sortDescriptorWithKey:ascending:selector:)])
		[tableView setIndicatorImage:[NSImage imageNamed:@"NSDescendingSortIndicator"] inTableColumn:column];
	
	m_bAscending = NO;
	
	lastColumn = column;
	
	[tableView reloadData];
}

- (void)dealloc
{
	[dataSource release];
	
	[super dealloc];
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

#pragma mark - table view datasource and delegate method

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return [dataSource count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	NSDictionary *dict = [dataSource objectAtIndex:row];
	
	id returnValue = nil;
	
	NSString *identifier = [tableColumn identifier];
	
	if([identifier isEqualToString:CHNAMECELL])
	{
		NSString *name = [dict objectForKey:CHNAMECELL];
		
		if(name == nil)
			returnValue = LocalizeString(@"IDS_CH_UNKNOWN");
		else
			returnValue = [dict objectForKey:CHNAMECELL];
	}
	else if([identifier isEqualToString:CHNUMBERCELL])
	{
		returnValue =  [dict objectForKey:CHNUMBERCELL];
	}
	else if([identifier isEqualToString:CHTIMECELL])
	{
		returnValue = [dict objectForKey:CHTIMECELL];
	}
	
	return returnValue;
}

- (void)tableView:(NSTableView*)tableView mouseDownInHeaderOfTableColumn:(NSTableColumn *)tableColumn
{
	lastColumn = tableColumn;
	
	[self.tableView deselectAll:nil];
	
	if([NSSortDescriptor respondsToSelector:@selector(sortDescriptorWithKey:ascending:selector:)])
	{
		NSSortDescriptor *sort = [NSSortDescriptor sortDescriptorWithKey:[tableColumn identifier] ascending:m_bAscending selector:@selector(localizedCompare:)];
		
		[tableColumn setSortDescriptorPrototype:sort];
	}
}

- (void)tableView:(NSTableView *)_tableView sortDescriptorsDidChange:(NSArray *)oldDescriptors
{
	[dataSource sortUsingDescriptors:[_tableView sortDescriptors]];
	
	[_tableView reloadData];
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	callHistoryViewController *pView = [self getParent];
	
	[pView onTBButtonEnabled];
}

#pragma mark - ib action message

- (void)onChDelete
{
	callHistoryViewController *pView = [self getParent];
	chTreeControl *tree = pView.chTree;
	NSString *strMsg = nil;
	NSString *nID = nil;
	ChFolder *pFolder;
	PhoneBookItem *pItem;
	NSTreeNode *node = [tree getSelectedItem];
	
	if([tree inMissCallFolder:node])
	{
		strMsg = LocalizeString(@"IDS_CH_DELETE_MISSCALL");
		nID = LocalizeString(@"IDS_MISSCALL_DEL");
		if(pView.m_bClean)
		{
			m_vChCache.clear();
			[tree getMissCallItem:&node andFolder:&pFolder];
			ItemPos pos = pFolder->GetFirstItemPos();
			
			while (pos != 0)
			{
				if((pItem = pFolder->GetNextItem(pos)))
				{
					m_vChCache.push_back(pItem);
				}
			}
		}
	}
	else if([tree inResvCallFolder:node])
	{
		strMsg = LocalizeString(@"IDS_CH_DELETE_RESVCALL");
		nID = LocalizeString(@"IDS_RESVCALL_DEL");
		if(pView.m_bClean)
		{
			m_vChCache.clear();
			[tree getResvCallItem:&node andFolder:&pFolder];
			ItemPos pos = pFolder->GetFirstItemPos();
			
			while (pos != 0)
			{
				if((pItem = pFolder->GetNextItem(pos)))
				{
					m_vChCache.push_back(pItem);
				}
			}
		}
	}
	else if([tree inDialCallFolder:node])
	{
		strMsg = LocalizeString(@"IDS_CH_DELETE_DIALCALL");
		nID = LocalizeString(@"IDS_DIALCALL_DEL");
		
		if(pView.m_bClean)
		{
			m_vChCache.clear();
			[tree getDialCallItem:&node andFolder:&pFolder];
			ItemPos pos = pFolder->GetFirstItemPos();
			
			while (pos != 0)
			{
				if((pItem = pFolder->GetNextItem(pos)))
				{
					m_vChCache.push_back(pItem);
				}
			}
		}
	}
	
	if(pView.m_bClean)
	{
		[self deleteCall:node withItems:m_vChCache andID:nID];
	}
	else 
	{
		[self getSelectItem:&node withItems:m_vChCache];
		
		if([utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:strMsg forType:MB_YESNO] == NSAlertDefaultReturn)
			[self deleteCall:node withItems:m_vChCache andID:nID];
	}
}

- (void)onchSaveas
{
	NSString *strNum, *strName;
	
	if([self getPhoneNumber:&strNum withName:&strName])
	{
		pbViewControl *pbView = [TheDelegate pbController];
		
		[pbView AddNewContact:[NSString NSStringToCString:strNum] Home:_T("")];
	}
}

- (void)onChSendMessage
{
	NSString *strNum, *strName;
	
	if([self getPhoneNumber:&strNum withName:&strName])
	{
		SMSViewCtrl *pSms = [TheDelegate smsController];
		
		if([pSms sendNewMessage:strNum])
		{
			
		}
	}
}

- (void)onChCall
{	
	NSString *name,*number;
	
	if([self getPhoneNumber:&number withName:&name])
	{
		callControl *pCall = TheDelegate.callCtrl;
		
		[pCall makeCall:number withName:name];
	}
}

#pragma mark - selected method

- (void)getSelectItem:(NSTreeNode **)node withItems:(std::vector<PhoneBookItem *> &)vec
{
	callHistoryViewController *pView = [self getParent];
	chTreeControl *tree = pView.chTree;
	
	vec.clear();
	*node = [tree getSelectedItem];
	
	ChFolder *pFolder = [tree getItemData:*node];
	
	PhoneBookItem *pCall;
	pbCocoa *ch;
	NSDictionary *dict;
	
	NSIndexSet *set = [tableView selectedRowIndexes];
	NSUInteger current = [set firstIndex];
	NSUInteger clickIndex = [tableView clickedRow];
	
	if(clickIndex == NSUIntegerMax || [set containsIndex:clickIndex])
	{
		while (current != NSNotFound)
		{
			dict = [dataSource objectAtIndex:current];
			ch = [dict objectForKey:CHITEM];
			pCall = ch->contact;
			if(pCall)
			{
				if([tree ifMissCallFolder:pFolder])
					pCall->SetMemType(_T("MC"));
				else if([tree ifResvCallFolder:pFolder])
					pCall->SetMemType(_T("RC"));
				else if([tree ifDialCallFolder:pFolder])
					pCall->SetMemType(_T("LD"));
				
				vec.push_back(pCall);
			}
			
			current = [set indexGreaterThanIndex:current];
		}
	}
	else
	{
		dict = [dataSource objectAtIndex:clickIndex];
		ch = [dict objectForKey:CHITEM];
		pCall = ch->contact;
		if(pCall)
		{
			if([tree ifMissCallFolder:pFolder])
				pCall->SetMemType(_T("MC"));
			else if([tree ifResvCallFolder:pFolder])
				pCall->SetMemType(_T("RC"));
			else if([tree ifDialCallFolder:pFolder])
				pCall->SetMemType(_T("LD"));
			
			vec.push_back(pCall);
		}
	}
}

- (BOOL)getSelected
{
	return ([[tableView selectedRowIndexes] count] > 0);
}

- (NSInteger)getSelectedCount
{
	return [[tableView selectedRowIndexes] count];
}

- (NSInteger)getItemCount
{
	return [tableView numberOfRows];
}

- (PhoneBookItem*)getItemData:(int)nItem
{
	if(nItem >= 0 && nItem < [self getItemCount])
	{
		NSDictionary *dict = [dataSource objectAtIndex:nItem];
		
		if(dict != nil)
		{
			pbCocoa *data = [dict objectForKey:CHITEM];
			
			return data->contact;
		}
	}
	
	return NULL;
}

#pragma mark - selected method

- (void)selectLastItem:(ChFolder *)pFolder
{
	callHistoryViewController *pView = [TheDelegate chController];
	chTreeControl *tree = pView.chTree;
	
	if([tree ifSelectedFolder:pFolder])
		[self selectItem:([self getItemCount] - 1)];
}

- (void)selectItem:(NSInteger)nItem
{
	[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:nItem] byExtendingSelection:NO];
	
	[tableView scrollRowToVisible:nItem];
}

#pragma mark - delete method

- (void)deleteCall:(NSTreeNode *)hDelete withItems:(std::vector<PhoneBookItem *> &)vec andID:(NSString *)nID
{
	callHistoryViewController *pView = [self getParent];
	chTreeControl *tree = pView.chTree;
	
	m_pFolderDelete = [tree getItemData:hDelete];
	
	if(!m_pFolderDelete)
		return;
	
	int jobType = 0;
	
	if([tree inMissCallFolder:hDelete])
		jobType = 0;
	else if([tree inResvCallFolder:hDelete])
		jobType = 1;
	else if([tree inDialCallFolder:hDelete])
		jobType = 2;
	
	if(m_dlg == nil)
		m_dlg = [chJobThreadDlg alloc];
	else
		return;
	
	[m_dlg initWithTitle:nID items:&vec jobType:jobType fromFolder:m_pFolderDelete withNumber:0 andParent:self];
	
	[m_dlg initWithWindowNibName:@"chJobThreadDlg"];
	
	[m_dlg showWindow:nil];
}

- (void)deleteAllItems
{
	[dataSource removeAllObjects];
	
	[tableView reloadData];
}

- (void)deleteItem:(PhoneBookItem *)pItem inFolder:(ChFolder *)pFolder
{
	callHistoryViewController *pView = [self getParent];
	chTreeControl *tree = pView.chTree;
	
	if(!pFolder || !pItem)
		return;
	
	if([tree ifSelectedFolder:pFolder])
	{
		for(int i = 0; i < [self getItemCount]; i++)
		{
			if([self getItemData:i] == pItem)
			{
				[dataSource removeObjectAtIndex:i];
				
				[tableView reloadData];
				[tableView deselectAll:nil];
				
				[self updateStatusBar];
				
				return;
			}
		}
	}
}

- (void)updateListCtrl
{
	std::vector<PhoneBookItem*> pvResult = [m_dlg getResultCall];
	
	for (int i = 0; i < pvResult.size(); i++)
	{
		[self deleteItem:pvResult[i] inFolder:m_pFolderDelete];
	}
}

#pragma mark - display method

- (void)display:(ChFolder *)pFolder withMemoryType:(int)memoryType
{
	int nShowNum = 0;
	
	if(!pFolder)
		return;
	
	[self deleteAllItems];
	m_numLD = m_numRC = m_numMC = 0;
	
	PhoneBookItem *pItem = NULL;
	callHistoryViewController *pView = (callHistoryViewController*)[self getParent];
	
	if(memoryType == 0)
		nShowNum = pView.m_MCNum;
	else if(memoryType == 1)
		nShowNum = pView.m_RCNum;
	else if(memoryType == 2)
		nShowNum = pView.m_LDNum;
	
	if(pFolder->GetItemSize() > nShowNum)
	{
		ChFolder *pTempFolder = new ChFolder;
		
		ItemPos pos = pFolder->GetFirstItemPos();
		
		while (pos != 0)
		{
			pItem = pFolder->GetNextItem(pos);
			if(pItem->GetIndex() <= nShowNum)
				pTempFolder->InsertItem(pItem);
		}
		
		pFolder->CloseNextItem(pos);
		
		pos = pTempFolder->GetFirstItemPos();
		
		while (pos != 0)
		{
			if((pItem = pTempFolder->GetNextItem(pos)))
				[self insertItem:pItem withMemory:memoryType andIndex:-1];
		}
		
		pTempFolder->CloseNextItem(pos);
		
		delete pTempFolder;
	}
	else
	{
		ItemPos pos = pFolder->GetFirstItemPos();
		
		while (pos != 0)
		{
			if((pItem = pFolder->GetNextItem(pos)))
				[self insertItem:pItem withMemory:memoryType andIndex:-1];
		}
		
		pFolder->CloseNextItem(pos);
	}
	
	[self tableView:tableView mouseDownInHeaderOfTableColumn:lastColumn];
	
	[dataSource sortUsingDescriptors:[tableView sortDescriptors]];
	
	[tableView reloadData];
	
	[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
}

- (void)insertItem:(PhoneBookItem *)pItem withMemory:(int)memoryType andIndex:(int)nItem
{
	if(!pItem)
		return;
	
	CString sTime, sName, sNumber;
	
	pItem->GetPhone(sName, sNumber);
	
	[self getItem:pItem time:sTime];
	
	[self getContactName:sName andNumber:sNumber];
	
	if(sName.IsEmpty())
	{
		ssize_t nPos = sNumber.FindOneOf(_T("pPwW"));
		
		if(nPos > 0)
		{
			CString number = sNumber.Left(nPos);
			[self getContactName:sName andNumber:number];
		}
	}
	
	if(nItem > 0)
	{
		[dataSource removeObjectAtIndex:nItem];
	}
	
	NSString *name = nil;
	
	if(!sName.IsEmpty())
		name = [NSString CStringToNSString:sName];
	
	NSString *number = [NSString CStringToNSString:sNumber];
	NSString *time = [NSString CStringToNSString:sTime];
	
	chListData *data = [[chListData alloc] initWithName:name
												   Time:time
												 Number:number
											   withItem:pItem];
	
	[self addRow:data];
	
	[data release];
}

- (void)addRow:(chListData *)data
{
	NSMutableDictionary *dict = nil;
	
	pbCocoa *item = [[[pbCocoa alloc] initWithPbk:data.pItem] autorelease];
	
	if(data.name != nil)
		dict = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
												data.name, CHNAMECELL, 
												data.time, CHTIMECELL,
												data.number, CHNUMBERCELL, 
												item, CHITEM, 
														nil];
	else
		dict = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
										data.time, CHTIMECELL,
										data.number, CHNUMBERCELL, 
										item, CHITEM, 
										nil];
	
	[self.dataSource addObject:dict];
	
	[dict release];
}

#pragma mark - help method

- (BOOL)getPhoneNumber:(NSString**)strNum withName:(NSString**)strName
{
	NSUInteger index = [tableView clickedRow];
	
	if(index == NSUIntegerMax)
	{
		 index = [tableView selectedRow];
	}
	
	NSDictionary *dict = [dataSource objectAtIndex:index];
	
	*strName = [dict objectForKey:CHNAMECELL];
	*strNum = [dict objectForKey:CHNUMBERCELL];
	
	return [*strNum length] > 0;
}

- (BOOL)getContactName:(CString &)name andNumber:(CString &)number
{
	DCTAppDelegate *delegate = TheDelegate;
	
	name.Empty();
	
	return [delegate searchName:name withNumber:number];
}

- (BOOL)getItem:(PhoneBookItem *)pItem time:(CString &)sTime
{
	sTime.Empty();
	
	if(pItem == NULL)
		return NO;
	
	tm time;
	pItem->GetTime(time);
	
	if(time.tm_year < 0)
		return NO;
	
	if(time.tm_mon < 0 || time.tm_mon > 11)
		return NO;
	
	if(time.tm_mday < 1 || time.tm_mday > 31)
		return NO;
	
	if(time.tm_hour < 0 || time.tm_hour > 23)
		return NO;
	
	if(time.tm_min < 0 || time.tm_min > 59)
		return NO;
	
	if(time.tm_sec < 0 || time.tm_sec > 59)
		return NO;
	
	NSDateComponents *components = [[NSDateComponents alloc] init];
	[components setYear:time.tm_year + 1900];
	[components setMonth:time.tm_mon];
	[components setDay:time.tm_mday];
	[components setHour:time.tm_hour];
	[components setMinute:time.tm_min];
	[components setSecond:time.tm_sec];
	
	NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
	NSDate *date = [calendar dateFromComponents:components];
	
	time = [[self getParent] nsDateTimeToTM:date];
	
	sTime.Format(_T("%d/%02d/%02d %02d:%02d:%02d"), time.tm_year + 1900, time.tm_mon, time.tm_mday,
				 time.tm_hour, time.tm_min, time.tm_sec);
	
	return YES;
}

- (id)getParent
{
	DCTAppDelegate *delegate = TheDelegate;
	
	return delegate.chController;
}

- (void)updateStatusBar
{
	DCTAppDelegate *delegate = TheDelegate;
	
	NSString *info = nil; 
	
	chTreeControl *tree = ((callHistoryViewController*)[self getParent]).chTree;
	ChFolder *pFolder = [tree getSelectedItemData];
	
	if((pFolder != NULL) && delegate.currentView == CALLHISTORYVIEW)
	{
		if([tree ifMissCallFolder:pFolder])
			info = [[NSString stringWithFormat:@"%ld ",[dataSource count]] stringByAppendingString:LocalizeString(@"IDS_CH_MISSCALL")];
		else if([tree ifDialCallFolder:pFolder])
			info = [[NSString stringWithFormat:@"%ld ",[dataSource count]] stringByAppendingString:LocalizeString(@"IDS_CH_DIALCALL")];
		else if([tree ifResvCallFolder:pFolder])
			info = [[NSString stringWithFormat:@"%ld ",[dataSource count]] stringByAppendingString:LocalizeString(@"IDS_CH_RECECALL")];
	}
	else
		info = @"Ready";
	
	[delegate modifyMsgStatusBarData:info];
}

#pragma mark - message 

- (void) OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	switch (Msg)
	{
		case WM_CH_DESTROYCALLDLG:
			[self onDestroyCallDlg:wparam andLparam:lparam];
			break;
			
		default:
			break;
	}
}

- (LRESULT)onDestroyCallDlg:(long)wParam andLparam:(long)lParam
{
	[self updateListCtrl];
	[self closeThreadDlg];
	
	return 0;
}

- (void)closeThreadDlg
{
	if(m_dlg != nil)
	{
		[m_dlg release];
		m_dlg = nil;
	}
}

@end
