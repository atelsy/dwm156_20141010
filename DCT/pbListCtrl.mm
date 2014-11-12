//
//  pbListCtrl.m
//  DCT
//
//  Created by mbj on 12-1-13.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "pbListCtrl.h"
#import "imageAndTextCell.h"
#import "ImageTextCell.h"
#import "DCTAppDelegate.h"
#import "pbCocoa.h"
#import "pbViewControl.h"
#import "pbTreeCtrl.h"
#import "pbYesNoAllDlg.h"
#import "nsstrex.h"

@implementation pbListCtrl

#pragma mark--
#pragma mark construct and destruct
#pragma mark--

- (id)init
{
	self = [super init];
	if (self) 
	{
		m_numSIM = m_numME = 0;
		m_dlg = nil;
		m_nType = JOB_NONE;
		m_pFolder = nil;
		m_bCopy = FALSE;
		
		dataSource = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (void)awakeFromNib
{
	NSTableColumn *column = [[tableView tableColumns] objectAtIndex:0];
	
	ImageTextCell *cell = [[[ImageTextCell alloc] init] autorelease];
	
	[column setDataCell:cell];
	
	NSTableColumn *nameColumn = [tableView tableColumnWithIdentifier:PBNAMECELL];
	
	if([NSSortDescriptor respondsToSelector:@selector(sortDescriptorWithKey:ascending:selector:)])
		[tableView setIndicatorImage:[NSImage imageNamed:@"NSDescendingSortIndicator"] inTableColumn:nameColumn];
	
	bAscending = NO;
	
	lastColumn = nameColumn;
	
	[tableView reloadData];
}

- (void)dealloc
{
	[dataSource release];
	[iconImage release];
	
	[super dealloc];
}

@synthesize dataSource, tableView, iconImage;

#pragma mark -- 
#pragma mark tableview delegate
#pragma mark--

- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView
{
	return [dataSource count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	//get the row content, need modify depend on the dataSource object
	// listData *data = [dataSource objectAtIndex:row];
	NSDictionary *dict = [dataSource objectAtIndex:row];
	
	id returnValue = nil;
	
	NSString *identifier = [tableColumn identifier];
	
	if([identifier isEqualToString:PBMEMORYCELL])
	{
		NSCell *cell = [tableColumn dataCell];
		ImageTextCell *imageCell = (ImageTextCell*)cell;
		
		NSString *memory = [dict objectForKey:PBMEMORYCELL];
		NSImage *image = [self iconImage: memory];
		
		[imageCell setIcon:image];
		[imageCell setPrimaryTextKey:memory];
		
		return imageCell;
	}
	else if([identifier isEqualToString:PBNAMECELL])
	{
		returnValue = [dict objectForKey:PBNAMECELL]; // data.frmoTo;
	}
	else if([identifier isEqualToString:PBMOBILECELL])
	{
		returnValue = [dict objectForKey:PBMOBILECELL]; //data.time;
	}
	
	return returnValue;
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	int number = (int)[tableView numberOfSelectedRows];
	//   NSLog(@"select number is %d", number);
	
	if(number == 0 || number > 1)
	{
		//todo: clear the message reader dialog
		[[NSNotificationCenter defaultCenter] postNotificationName:PBLIST_LVN_ITEMCHANGED
															object:self
														  userInfo:nil];
		
	}
	else
	{
		//todo: change the content show in the below reader dialog
		NSInteger sel = [tableView selectedRow];
		NSDictionary * dict = [dataSource objectAtIndex:sel];
		
		[[NSNotificationCenter defaultCenter] postNotificationName:PBLIST_LVN_ITEMCHANGED
															object:self
														  userInfo:dict];
	}
}

NSInteger PbdataSort(id data1, id data2, void* columnIdentifier)
{
	pbListData *listData1 = (pbListData*)data1;
	pbListData *listData2 = (pbListData*)data2;
	
	NSComparisonResult result = NSOrderedAscending;
	
	if([(NSString*)columnIdentifier isEqualToString:PBMEMORYCELL])
	{
		result = [listData1.memory localizedCompare:listData2.memory];
	}
	else if([(NSString*)columnIdentifier isEqualToString:PBNAMECELL])
	{
		result = [listData1.name localizedCompare: listData2.name];
	}
	else if([(NSString*)columnIdentifier isEqualToString:PBMOBILECELL])
	{
		result = [listData1.mobile localizedCompare:listData2.mobile];
	}
	
	return result;
}

- (void)tableView:(NSTableView *)tableView mouseDownInHeaderOfTableColumn:(NSTableColumn *)tableColumn
{
	lastColumn = tableColumn;
	
	[self.tableView deselectAll:nil];
	
	if([NSSortDescriptor respondsToSelector:@selector(sortDescriptorWithKey:ascending:selector:)])
	{
		NSSortDescriptor *sort = [NSSortDescriptor sortDescriptorWithKey:[tableColumn identifier] ascending:bAscending selector:@selector(localizedCompare:)];
		[tableColumn setSortDescriptorPrototype:sort];
	}
}

- (void)tableView:(NSTableView*)_tableView sortDescriptorsDidChange:(NSArray *)oldDescriptors
{
	[dataSource sortUsingDescriptors:[_tableView sortDescriptors]];
	
	[_tableView reloadData];
}

- (NSImage*)iconImage:(NSString *)imageName
{
	NSArray *array = nil;
	
	if(iconImage == nil)
	{
		iconImage = [[NSMutableArray alloc] init];
		
		array = [NSArray arrayWithObjects:@"me.png", @"pc.png", @"sim.png", nil];
		
		for(NSString *str in array)
		{
			NSImage *image = [NSImage imageNamed:str];
			
			if(image != nil)
				[iconImage addObject:image];
		}
	}
	
	for(NSImage *image in iconImage)
	{
		if([[image name] caseInsensitiveCompare:imageName] == NSOrderedSame)
			return image;
	}
	
	return nil;
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

#pragma mark -- 
#pragma mark pb job thread message
#pragma mark--

- (LRESULT) OnPbThreadInsert:(WPARAM)wParam andLparam:(LPARAM)lParam;
{
	if(wParam && lParam)
	{
		PbFolder *pFolder = (PbFolder*)wParam;
		PhoneBookItem *pContact = (PhoneBookItem*)lParam;
		
		std::vector<PhoneBookItem*> vContact;
		[self InsertItem:pFolder withContact:pContact andVector:vContact];
		[self SelectLastItem:pFolder];
		[self UpdateStatusBar];
		//pView->UpateSmsInfo();
	}
	else
	{
		
		if(lParam)
		{
			PhoneBookItem *pContact = (PhoneBookItem*)lParam;
			[self InsertPbIndex:pContact];
			delete pContact;
		}
		
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
						withMsg:LocalizeString(@"IDS_ERROR_WRITECONTACT")
						forType:MB_OK];
		[pbViewControl SetDownloadState:FALSE];
	}
	
	return 0;
	
}

- (LRESULT) OnPbThreadUpdate:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	if(lParam)
	{
		PbMemory *pMemory = (PbMemory*)lParam;
		[pbViewControl InsertFreeIdx:pMemory->sMemory withIdx:pMemory->nIdx];
		delete pMemory;
	}
	
	int nItem = (int)wParam;
	if(nItem >= 0)
	{
		[self UpdateItem:nItem];
		//pView->UpdateSmsInfo();
	}
	else
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
						withMsg:LocalizeString(@"IDS_ERROR_UPDATECONTACT")
						forType:MB_OK];
	
	return 0;
	
}

- (void)LaunchPbJobThread:(PbJobPara *)para
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	
	if(m_dlg == nil)
		m_dlg = [pbJobThreadDlg alloc];
	else 
		return;
	
	[m_dlg initWithParameter:para];
	
	[m_dlg initWithWindowNibName:@"pbJobThreadDlg"];
	
	[pView EnterDlg];
	
	[m_dlg showWindow:nil];
    [m_dlg close];
}

- (void)LaunchPbJobThread:(PbJobPara *)para
			   withFolder:(PbFolder *)pFolder
			   andContact:(PhoneBookItem*)pContact
				   andMem:(CString&)s
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	
	if(m_dlg == nil)
		m_dlg = [pbJobThreadDlg alloc];
	else
		return;
	
	[m_dlg initWithParameter:para];
	
	[m_dlg initWithWindowNibName:@"pbJobThreadDlg"];
	
	[m_dlg showWindow:nil];
	
	[pView EnterDlg];
	
	[m_dlg Start:self withFolder:pFolder andContact:pContact andMem:s];
}

- (void)LaunchPbJobThread:(PbJobPara *)para
			   withFolder:(PbFolder *)pFolderHandset
				 withItem:(int)nItem
			andSelContact:(PhoneBookItem*)pContactSel
			   andContact:(PhoneBookItem*)pContact
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	
	if(m_dlg == nil)
		m_dlg = [pbJobThreadDlg alloc];
	else 
		return;
	
	[m_dlg initWithParameter:para];
	
	[m_dlg initWithWindowNibName:@"pbJobThreadDlg"];
	
	[pView EnterDlg];
	
	[m_dlg showWindow:nil];
	
	[m_dlg Start:self
	  withFolder:pFolderHandset
	   withIndex:nItem
	  andContact:pContactSel
		  andNew:pContact];
}

- (void)CloseThreadDlg
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	
	if(m_dlg != nil)
	{
		[m_dlg release];
		m_dlg = nil;
	}
	
	PostMessage([TheDelegate getCallhistoryViewController], WM_REFRESH_LIST, 0, 0);
	[pView LeaveDlg];
	[pView ResetSearch];
	[self SelectLastItem:m_pFolder];
	
	m_pFolder = NULL;
}

- (void) OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	switch(Msg)
	{
		case WM_MOVETOHANDDSET:
			[self OnMoveToHandset:wparam andLparam:lparam];
			break;
		case WM_COPYTOHANDDSET:
			[self OnCopyToHandset:wparam andLparam:lparam];
			break;
		case WM_PB_DESTROYDLG:
			[self OnDestroyDlg:wparam andLparam:lparam];
			break;
		case WM_PB_AFTERREDOWNLOAD:
			[self OnAfterReDownload:wparam andLparam:lparam];
			break;
		case WM_PBTHREAD_INSERT:
			[self OnPbThreadInsert:wparam andLparam:lparam];
			break;
		case WM_PBTHREAD_UPDATE:
			[self OnPbThreadUpdate:wparam andLparam:lparam];
			break;
	}
}

- (LRESULT) OnDestroyDlg:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	std::vector<PhoneBookItem*>vec = [m_dlg GetResultContactVec];
	
	switch(wParam)
	{
		case CTL_HSTOPC:
		{
			[self UpdateListCtrl];
			PostMessage([TheDelegate getCallhistoryViewController], WM_REFRESH_LIST, 0, 0);
		}
			break;
		case CTL_MOVETOHS:
			[self UpdateListCtrl];
			break;	
		case CTL_COPYTOHS:
		{
			std::vector<PhoneBookItem*> vContact;
			for(int i = 0 ; i < vec.size() ; i++)
			{
				[self InsertItem:m_pFolder withContact:vec[i] andVector:vContact];
			}
		}
			break;	
	}
	
	[self CloseThreadDlg];
	return 0;
}

- (LRESULT) OnCopyToHandset:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	NSString *str;
	int nMiss = LOWORD(lParam);
	int nCount = HIWORD(lParam);
	
	if (nCount <= 0)
	{
		if(nMiss > 0)
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_PBFULL")
							forType:MB_OK];
		return 0;
	}
	else if(nMiss > 0)
	{
		str = LocalizeString(@"IDS_ERROR_PBFULL_COPY");
		
		NSString *strTmp = [NSString localizedStringWithFormat:str, nCount];
		
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
						withMsg:strTmp
						forType:MB_OK];	
	}
	
	PbJobPara para;
	para.strTitle = LocalizeString(@"IDS_PB_COPY");
	para.pvContact = &m_vContact;
	para.job = 1;
	para.pFolderFrom = NULL;
	para.pFolder = m_pFolder;
	para.nCount = nCount;
	para.s = _T("");
	para.nCtrlType = CTL_COPYTOHS;
	para.hParent = self;
	
	[self LaunchPbJobThread:&para];
	
	return 0;
}

- (LRESULT) OnMoveToHandset:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	NSString *str;
	CString s = _T("SM");
	
	int nMiss = LOWORD(lParam);
	int nCount = HIWORD(lParam);
	
	if (nCount <= 0)
	{
		if(nMiss > 0)
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_PBFULL")
							forType:MB_OK];
		return 0;
	}
	else if(nMiss > 0)
	{		
		str = LocalizeString(@"IDS_ERROR_PBFULL_MOVE");
		
		NSString *strTmp = [NSString localizedStringWithFormat:str, nCount];
		
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
						withMsg:strTmp
						forType:MB_OK];	
	}
	
	PbJobPara para;
	para.strTitle = LocalizeString(@"IDS_PB_COPY");
	para.pvContact = &m_vContact;
	para.job = 0;
	para.pFolderFrom = m_pFolderFrom;
	para.pFolder = m_pFolder;
	para.nCount = nCount;
	para.s = s;
	para.nCtrlType = CTL_MOVETOHS;
	para.hParent = self;
	
	[self LaunchPbJobThread:&para];
	
	return 0;
}

- (LRESULT) OnAfterReDownload:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	int nIdx;
	//	CPBFormView *pView = (CPBFormView*)GetParent();
	
	if (!lParam)
		return 0;
	
	switch(m_Operation)
	{
		case PB_INSERT:
		{
			if (((BOOL)lParam) && ([pbViewControl GetFreeIdx:m_strMemType withIdx:nIdx]))
			{
				[self WriteContact:m_pFolder withItem:m_pContact andMem:m_strMemType withIdx:nIdx];
			}				
			else
			{
				delete m_pContact;
				if([pbViewControl GetDownloadState])
					[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
									withMsg:LocalizeString(@"IDS_ERROR_PBFULL")
									forType:MB_OK];
			}
		}
			break;
		case PB_COPY:
		{
			if([pbViewControl GetDownloadState])
			{
				m_vecTemp.clear();
				for(int i=0; i<m_ReDownloadvec.size(); i++)
				{
					m_vecTemp.push_back( &m_ReDownloadvec[i]);
				}
				[self WriteContact:m_pFolder withPbVector:m_vecTemp];
			}
			
		}
			break;
		case PB_MOVE:
		{
			if([pbViewControl GetDownloadState])
			{
				m_vecTemp.clear();
				for(int i=0; i<m_ReDownloadvec.size(); i++)
				{
					m_vecTemp.push_back( &m_ReDownloadvec[i]);
				}
				[self WriteContact:m_pFolderFrom toFolder:m_pFolder withPbVector:m_vecTemp];
			}
		}
			break;	
	}
	
	return 0;
}


#pragma mark --
#pragma mark select operation
#pragma mark--
- (void)SelectLastItem:(PbFolder *)pFolder
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	pbTreeCtrl *tree = pView.pbTree;
	
	if([tree IfSelectedFolder:pFolder] == YES)
		[self SelectItem:([self GetItemCount] - 1)];
}

- (void)SelectItem:(int)nItem
{
	[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:nItem] byExtendingSelection:NO];
	
	[tableView scrollRowToVisible:nItem];
}

- (bool)getSelected
{
	return ([[tableView selectedRowIndexes] count] > 0);
}

#pragma mark--
#pragma mark common function
#pragma mark--

- (void)deleteAllItems
{
	[dataSource removeAllObjects];
	
	[tableView reloadData];
}

- (void)AddRow:(pbListData*)pData
{
	NSMutableDictionary *dict = nil;
	
	pbCocoa *pbItem = [[[pbCocoa alloc] initWithPbk: pData.pContact] autorelease];
	
	dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:pData.memory, PBMEMORYCELL, pData.name, PBNAMECELL, pData.mobile, PBMOBILECELL, pbItem, PBITEM, nil] autorelease];
	
	[self.dataSource addObject:dict];
	
	[tableView reloadData];
}

- (PhoneBookItem*)UpdateItem:(int)nItem
{
	PhoneBookItem *pContactSel = [self GetItemData:nItem];
	if(pContactSel)
	{
		[self InsertItem:pContactSel withIndex:nItem];
		[self SelectItem:nItem];
	}
	return pContactSel;
}

- (PhoneBookItem*)UpdateItem:(PhoneBookItem*)pContact withItem:(int)nItem andVector:(std::vector<PhoneBookItem*>&)vContact
{
	if(!pContact)
		return nil;
	
	PhoneBookItem *pContactSel = [self GetItemData:nItem];
	if (pContactSel)
	{
		*pContactSel = *pContact;
		vContact.push_back(pContactSel);
		
		[self InsertItem:pContactSel withIndex:nItem];
		[self SelectItem:nItem];
	}
	
	delete pContact;
	return pContactSel;
}

- (PhoneBookItem*)GetInfo:(CString&)strName andHandset:(CString&)strHandset andHome:(CString&)strHome andCompany:(CString&)strCompany andEmail:(CString&)strEmail andOffice:(CString&)strOffice andFax:(CString&)strFax andBirthday:(CString&)strBirthday
{
	//int nItem = [self GetFirstSelected];
	int nItem = (int)[tableView clickedRow];
	if(nItem < 0)
		nItem = [self GetFirstSelected];
	
	if(nItem >= 0)
		return [self GetInfo:nItem andName:strName andHandset:strHandset andHome:strHome andCompany:strCompany andEmail:strEmail andOffice:strOffice andFax:strFax andBirthday:strBirthday];
	
	return nil;	
}

- (PhoneBookItem*)GetInfo:(int)nItem andName:(CString&)strName andHandset:(CString&)strHandset andHome:(CString&)strHome andCompany:(CString&)strCompany andEmail:(CString&)strEmail andOffice:(CString&)strOffice andFax:(CString&)strFax andBirthday:(CString&)strBirthday
{
	Advanced_PBItem_Struct adv_pb_item;
	PhoneBookItem *pContact = [self GetItemData:nItem];	
	
	if(pContact)
	{
		pContact->GetPhone(strName, strHandset);		
		pContact->GetAdvPBItem(adv_pb_item);
		
		strHome = adv_pb_item.m_home_num;
		strCompany = adv_pb_item.m_company_name;
		strEmail = adv_pb_item.m_email;
		strOffice = adv_pb_item.m_office_num;
		strFax = adv_pb_item.m_fax_num;
		if((adv_pb_item.m_birth_year > 0) &&
		   (adv_pb_item.m_birth_month > 0) &&
		   (adv_pb_item.m_birth_day > 0))
		{
			strBirthday.Format(_T("%04d/%02d/%02d"), 
							   adv_pb_item.m_birth_year, 
							   adv_pb_item.m_birth_month, 
							   adv_pb_item.m_birth_day);
		}
		else
		{
			strBirthday.Empty();
		}
	}
	else
	{
		strName.Empty();
		strHome.Empty();
		strHandset.Empty();
		strCompany.Empty();
		strEmail.Empty();
		strOffice.Empty();
		strFax.Empty();
		strBirthday.Empty();
	}
	return pContact;
}

#pragma mark--
#pragma mark display operation
#pragma mark--

- (void)repaintTree
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	
	pbTreeCtrl *tree = pView.pbTree;
	
	PbFolder *pFolder = [tree GetSelectedItemData];
	
	[self Display:pFolder];
	
	[self UpdateStatusBar];
}

- (BOOL)GetClipboard
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	pbTreeCtrl *tree = pView.pbTree;
	
	// Get Status
	std::vector<PhoneBookItem*> vec = [pView getPhbVector];
	
	// Get Selected Folder
	PbFolder *pFolder = [tree GetSelectedItemData];
	if(pFolder)
	{
		// If Handset Folder && !Connected
		
		if([tree IfHandsetFolder:pFolder] && (![pView getConnStatus] || ![pView getSimStatus]))
			return NO;
		
		for(int i=0;i<vec.size();i++)
		{
			if(vec[i])
				return YES;
		}
	}
	
	return NO;
}

- (int)GetItemCount
{
	//todo: maybe modify as [dataSouce count];
	return (int)[tableView numberOfRows];
}

- (void)ModifyContactCount:(CString)sMem
{
	if(sMem == _T("SM"))
		m_numSIM--;
	else
		m_numME--;
}

- (void)Display:(PbFolder *)pFolder
{
	if(!pFolder)
		return;
	
	[self setRedraw:NO];
	
	[self deleteAllItems];
	m_numSIM = m_numME = 0;
	
	PhoneBookItem *pItem = NULL;
	ItemPos pos = pFolder->GetFirstItemPos();
	
	while(pos != 0)
	{
		pItem = pFolder->GetNextItem(pos);
		if(pItem != NULL)
			[self InsertItem:pItem withIndex:-1];
	}
	pFolder->CloseNextItem(pos);
	
	[self tableView: tableView mouseDownInHeaderOfTableColumn:lastColumn];
	[dataSource sortUsingDescriptors:[tableView sortDescriptors]];
	
	[tableView reloadData];
	
	[self setRedraw:YES];
	
	[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
}

#pragma mark--
#pragma mark common function
#pragma mark--

- (void)UpdateStatusBar
{
	DCTAppDelegate *delegate = TheDelegate;
	
	NSString *info; 
	
	if(([((pbViewControl*)[self getParent]).pbTree GetSelectedItemData] != NULL) && delegate.currentView == PHONEBOOKVIEW)
		info = [NSString stringWithFormat:@"%ld contact(s)", [dataSource count]];
	else
		info = @"Ready";
	
	
	[delegate modifyMsgStatusBarData:info];
}

- (void)setRedraw:(signed char)bRedraw
{
	m_bRedraw = bRedraw;
	
	if(bRedraw == YES)
		[tableView reloadData];
}

- (id)getParent
{
	DCTAppDelegate *delegate = TheDelegate;
	
	return [delegate getPbViewController];
}


- (void)GetAnotherMemory:(CString&)s
{
	if(s == CString("SM"))
		s = CString("ME");
	else
		s = CString("SM");
	
}

- (void)GetMemery:(CString&)str
{
	int nItem = [self GetFirstSelected];
	str.Empty();
	
	PhoneBookItem *pContact = [self GetItemData:nItem];	
	if(pContact)
	{
		str = pContact->GetMemType();
	} 
}

- (PhoneBookItem*)GetItemData:(int)nItem
{
	if(nItem >= 0 && nItem < [self GetItemCount])
	{
		NSDictionary *dict = [dataSource objectAtIndex:nItem];
		
		if(dict != nil)
		{
			pbCocoa *cocoa = (pbCocoa*)[dict objectForKey:PBITEM];
			return cocoa->contact;
		}
	}
	
	return NULL;
}

- (void)InsertPbIndex:(PhoneBookItem*)pContact
{
	if(pContact != NULL)
		[pbViewControl InsertFreeIdx:pContact->GetMemType() withIdx:pContact->GetIndex()];
}

- (void)InsertPbIndex:(std::vector<PhoneBookItem*>)vec withIdx:(int)nIndex
{
	while(nIndex < vec.size())
	{
		[pbViewControl InsertFreeIdx:vec[nIndex]->GetMemType() withIdx:vec[nIndex]->GetIndex()];
		nIndex++;
	}
}

#pragma mark--
#pragma mark write contact function
#pragma mark--

- (void)WriteContact:(PbFolder*)pFolder
			withItem:(PhoneBookItem*)pContact
			  andMem:(CString&)s
			 withIdx:(int)nIdx
{
	
	pContact->SetIndex(nIdx);
	
	PbJobPara para;
	para.job = -1;
	para.nCtrlType = CTL_NULL;
	para.hParent = self;
	para.strTitle = LocalizeString(@"IDS_PB_COPY");
	
	[self LaunchPbJobThread:&para withFolder:pFolder andContact:pContact andMem:s];
}

- (void)WriteContact:(PbFolder*)pFolder
		withPbVector:(std::vector<PhoneBookItem*>&)vec
{	
	m_nType = JOB_COPY;
	m_pFolder = pFolder;
	
	if(vec.size() > 0)
	{
		//TRACE(_T("copy contact vec size = %d\n"), vec.size());
		[self SetIndex:vec withCopy:TRUE];
	}
}

- (void)WriteContact:(PbFolder *)pFolderFrom
			toFolder:(PbFolder*)pFolder
		withPbVector:(std::vector<PhoneBookItem*>&)vec
{
	m_nType = JOB_MOVE;	
	m_pFolderFrom  = pFolderFrom;
	m_pFolder = pFolder;
	
	if(vec.size() > 0)
	{
		
		//TRACE(_T("cut contact vec size = %d\n"), vec.size());
		[self SetIndex:vec withCopy:FALSE];
	}
}

- (int)SetIndex:(std::vector<PhoneBookItem*>&)vec withCopy:(BOOL)bCopy
{
	m_vec = &vec;
	m_vContact.clear();
	m_bYesToAll = FALSE;
	
	int nIdx;
	PhoneBookItem *pContact;
	CString sMemory, sName, sNumber;
	int nMiss = 0;
	NSInteger uOption = 0;
	
	for(int i=0; i<vec.size(); i++)
	{
		if(vec[i])
		{
			if(![self GetMemoryIdx:vec[i] isCopy:bCopy withMem:sMemory andIdx:nIdx isShow:FALSE])
				break;
			
			(vec[i])->GetPhone(sName, sNumber);
			m_str = sName;
			
			if(![self CheckPhoneName:m_str withMem:sMemory andEncode:m_nEncodeClass andMaxName:m_nMaxName])
			{
				if (0 == (uOption & (OPT_YESALL|OPT_NOALL)))
				{
					
					pbYesNoAllDlg *dlg = [[pbYesNoAllDlg alloc] retain];
					NSString *strBefore = [NSString CStringToNSString:sName];
					NSString *strEnd = [NSString CStringToNSString:m_str];
					NSString *strErr = LocalizeString(@"IDS_ERROR_PBNAME_MAX");										  
					NSString *strTmp = [NSString localizedStringWithFormat:strErr, strBefore, strEnd];
					
					[dlg Init:strTmp withMask:OPT_YESNOALL];
					[dlg initWithWindowNibName:@"pbYesNoAllDlg"];
					
					uOption = [NSApp runModalForWindow:[dlg window]];
					[dlg close];
					[dlg release];
				}
				
				if (0 != (uOption & (OPT_NO|OPT_NOALL)))
				{
					[pbViewControl InsertFreeIdx:sMemory withIdx:nIdx];
					nMiss ++;
					continue;
				}
				
				sName = m_str;
			}
			
			// New Contact
			if(bCopy)
			{
				pContact = [utility NewContact];
				if(!pContact)
					break;
				
				*pContact = *vec[i];
			}
			else
			{
				pContact = vec[i];
			}
			
			// Set Index & Memory
			pContact->SetIndex(nIdx);
			pContact->SetMemType(sMemory);
			pContact->SetPhone(sName, sNumber, m_nEncodeClass);			
			m_vContact.push_back(pContact);	
		}
	}
	
	DWORD dwSize = (DWORD)MAKELONG(vec.size() - m_vContact.size() - nMiss, m_vContact.size());
	if (m_nType == JOB_MOVE )
		PostMessage(self, WM_MOVETOHANDDSET, 0, LPARAM(dwSize));
	else if (m_nType == JOB_COPY)
		PostMessage(self, WM_COPYTOHANDDSET, 0, LPARAM(dwSize));
	
	return (int)m_vContact.size();
}

- (BOOL)ReDownload:(OPERATION)op
{
	m_Operation = op;
	
	[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
					withMsg:LocalizeString(@"IDS_PB_REDOWNLOAD")
					forType:MB_OK];
	
	pbViewControl *pView = [TheDelegate getPbViewController];
	[pView ShowReadDlg:REDOWNLOAD];
	return 0;
}

- (int)GetMemery:(PhoneBookItem*)pContact withMem:(CString&)sMem
{
	int nImage = 0;
	if(pContact) 
	{
		sMem = pContact->GetMemType();
		
		// Memory		
		if(sMem == _T("PC"))
			nImage = 0;
		else if(sMem == _T("SM"))
			nImage = 1;
		else if(sMem == _T("ME"))
			nImage = 2;
		else
			nImage = 3;
	}
	
	return nImage;
}

- (int)GetSelectedCount
{
	NSIndexSet *set = [tableView selectedRowIndexes];
	
	return (int)[set count];
}

- (int)GetFirstSelected
{
	NSIndexSet *set = [tableView selectedRowIndexes];
	
	if([set count] <= 0)
		return -1;
	
	return (int)[set firstIndex];
}

- (void)ResetStr:(CString&)str withId:(UINT)nID
{
	str.Empty();	
}

- (void)SaveContact
{
	
}

- (void)PasteContact:(NSTreeNode*)hItem
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	
	if(m_bCopy)
		[self CopyContact:pView->m_hItem To:hItem withVector:[pView GetCopyContacts]];
	else
		[self MoveContact:pView->m_hItem To:hItem withVector:[pView GetCopyContacts]];
}

- (void)CopyContact
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	
	[self GetSelectedItem:&pView->m_hItem withVector:[pView GetCopyContacts]];
	m_bCopy = TRUE;
}

- (void)CutContact
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	
	[self GetSelectedItem:&pView->m_hItem withVector:[pView GetCopyContacts]];
	
	m_bCopy = FALSE;
	
	[self UpdateStatusBar];
}

- (void)DeleteContact
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	std::vector<PhoneBookItem*> &vec = [pView GetDeleteContacts];
	pbTreeCtrl *tree = pView.pbTree;
	
	// Get Folder
	PbFolder *pFolderFrom = [tree GetItemData:pView->m_hTemp];
	if(!pFolderFrom) return;
	
	// Get Direction
	DIRECTION dir = [self GetDirection:pView->m_hTemp To:NULL];
	
	// Delete All Messages
	
	if(dir == HStoPC)	// Handset Folders
	{
		;
	}
	else				// PC Folders
	{
		for(int i=0;i<vec.size();i++)
		{
			// Check Clipboard
			[pView CheckClipboard:vec[i]];
			
			// Delet List Item
			[self DeleteItem:pFolderFrom withItem:vec[i]];
			
			// Delete Item
			pFolderFrom->DeleteItem(vec[i]);
		}
	}
	
	vec.clear();
	
	// Repaint
	[pView ResetEditCtrl];
	[pView onTBButtonEnabled];
	[pView ResetSearch];
	[self UpdateStatusBar];
}

- (BOOL)OnPbDelete
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	
	pbTreeCtrl *tree = pView.pbTree;
	
	NSTreeNode *hTrash;
	PbFolder *pFolderTrash;
	
	// Check Selected Item
	if([self GetSelectedCount] == 0 && [tableView clickedRow] == -1)
		return FALSE;
	
	// Can get the Trash Can folder	
	if(![tree GetTrashCan:&hTrash forFolder:&pFolderTrash])
		return FALSE;
	
	// Get Selected Item
	[self GetSelectedItem:&pView->m_hTemp withVector:[pView GetDeleteContacts]];
	
	// Check if the message is in Trash Can
	if([tree InTrashCan:pView->m_hTemp])
	{
		if([utility showMessageBox: LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_FOLDER_TRASHDELETE") forType:MB_YESNO] == NSAlertDefaultReturn)
		{
			[self DeleteContact];
			[pView ResetSearch];
			PostMessage([TheDelegate getCallhistoryViewController], WM_REFRESH_LIST, 0, 0);
			return TRUE;
		}
		else
			return FALSE;
	}
	
	// Prompt users if move to Trash Can
	if([utility showMessageBox: LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_PB_DELETE_PROMPT") forType:MB_YESNO] == NSAlertDefaultReturn)
	{
		[self MoveContact:pView->m_hTemp To:hTrash withVector:[pView GetDeleteContacts]];
		return TRUE;
	}
	
	return FALSE;
	
}

- (void)OnPbOpen
{
	
}

- (void)OnEditCut
{
	
}

- (void)OnEditCopy
{
	
}

- (void)OnEditPaste
{
	
}

- (void)OnEditDelete
{
	
}

- (DIRECTION)GetDirection:(NSTreeNode*)hFrom To:(NSTreeNode*)hTo
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	pbTreeCtrl *tree = pView.pbTree;
	
	// Get Handset Folder
	PbFolder *pFolderHandset;
	NSTreeNode* hHandset;
	
	[tree GetHandsetRoot:&hHandset forFolder:&pFolderHandset];
	// Check Tree Item
	if(hFrom == hHandset) 
	{
		if(hTo == hHandset)
			return HStoHS;
		else
			return HStoPC;
	}
	else if(hTo == hHandset)
		return PCtoHS;
	else
		return PCtoPC;
}

- (void)InsertItem:(PbFolder*)pFolder withContact:(PhoneBookItem*)pContact
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	pbTreeCtrl *tree = pView.pbTree;
	
	if(!pFolder || !pContact) 
		return;
	
	// Insert List Item
	if([tree IfSelectedFolder:pFolder])
		[self InsertItem:pContact withIndex:-1];
}

- (void)InsertItem:(PbFolder*)pFolder withContact:(PhoneBookItem*)pContact andVector:(std::vector<PhoneBookItem*>&)vContact
{
	if(!pFolder || !pContact) return;
	
	// Insert Contact
	vContact.push_back(pContact);
	pFolder->InsertItem(pContact);
	
	// Insert List Item
	[self InsertItem:pFolder withContact:pContact];
}

- (void)MoveContact:(NSTreeNode*)hFrom To:(NSTreeNode*)hItem withVector:(std::vector<PhoneBookItem*>&)vec
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	pbTreeCtrl *tree = pView.pbTree;
	
	//use member variable
	m_ReDownloadvec.clear();
	for(int i=0; i< vec.size(); i++)
	{
		if (!vec[i])
			continue;
		m_ReDownloadvec.push_back(*vec[i]);
	}
	
	// Check if move to self
	if(hFrom == hItem) 
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_MOVECONTACT") forType:MB_OK];
		return;
	}
	
	m_pFolderFrom = [tree GetItemData:hFrom];
	m_pFolder = [tree GetItemData:hItem];
	if(!m_pFolderFrom || !m_pFolder) return;
	
	// Check Contact Vector
	BOOL bCheck;
	if(&vec == &pView->m_vContact)	// Move from clipboard
		bCheck = FALSE;
	else
		bCheck = TRUE;
	
	// Get Direction
	DIRECTION dir = [self GetDirection:hFrom To:hItem];
	
	// Move All Contacts
	BOOL bSuccess = TRUE;
	
	// Check Direction
	switch (dir)
	{
		case PCtoPC:
		{
			// Move Contact
			for(int i=0;(i<vec.size() && bSuccess);i++)
				[self MoveItem:m_pFolderFrom To:m_pFolder withVector:vec andIdx:i andChecked:bCheck];
			[pView ResetSearch];
			[pView onTBButtonEnabled];
		}
			break;
			
		case HStoPC:	// Delete Contact
		{	
			if(m_dlg != nil)
				return;
			PbJobPara para;
			para.strTitle = LocalizeString(@"IDS_PB_DEL");
			para.pvContact = &vec;
			para.job = 0;
			para.pFolderFrom = m_pFolderFrom;
			para.pFolder = m_pFolder;
			para.nCount = 0;
			para.s = _T("");
			para.nCtrlType = CTL_HSTOPC;
			para.hParent = self;
			
			[self LaunchPbJobThread:&para];
		}
			break;
			
		case PCtoHS:	// Write Contact
		{
			if(m_dlg != nil)
				return;
			
			if(![pbViewControl GetDownloadState])			
			{
				[self ReDownload:PB_MOVE];
				return;
			}			
			
			if([pbViewControl GetDownloadState])
				[self WriteContact:m_pFolderFrom toFolder:m_pFolder withPbVector:vec];
		}
			break;
			
		default:
			break;
	}
	
	
	[self UpdateStatusBar];
	PostMessage([TheDelegate getCallhistoryViewController], WM_REFRESH_LIST, 0, 0);
}

- (void)CopyContact:(NSTreeNode*)hFrom To:(NSTreeNode*)hItem withVector:(std::vector<PhoneBookItem*>&)vec
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	pbTreeCtrl *tree = pView.pbTree;
	
	// Get Folder
	PbFolder *pFolderFrom = [tree GetItemData:hFrom];
	PbFolder *pFolder = [tree GetItemData:hItem];
	if(!pFolderFrom || !pFolder) return;
	
	// Get Direction
	DIRECTION dir = [self GetDirection:hFrom To:hItem];
	
	// Copy All Messages
	// 	int nIdx;
	// 	CString s = _T("SM");
	std::vector<PhoneBookItem*> vContact;
	
	//use member variable
	// 	m_strMemType = s;
	m_ReDownloadvec.clear();
	for(int i=0; i< vec.size(); i++)
	{
		if (!vec[i])
			continue;
		m_ReDownloadvec.push_back(*vec[i]);
	}
	m_pFolder = pFolder;
	// Check Direction
	switch (dir)
	{
		case PCtoPC:
		case HStoPC:
		{
			// New Contact
			for(int i=0;i<vec.size();i++)
			{
				PhoneBookItem *pContact = [utility NewContact]; 
				if(!pContact) break;
				
				if (!vec[i])
				{
					delete pContact;
					continue;
				}
				*pContact = *vec[i];
				pContact->SetMemType(_T("PC"));
				[self InsertItem:pFolder withContact:pContact andVector:vContact];
			}
			
			// Select Item
			[pView ResetSearch];
			[self SelectLastItem:pFolder];
		}
			break;
			
		case HStoHS:	// Write Contact
		case PCtoHS:
		{					
			if(m_dlg != nil)
				return;
			
			if(![pbViewControl GetDownloadState])
			{
				[self ReDownload:PB_COPY];
				break;
			}
			
			if([pbViewControl GetDownloadState])
				[self WriteContact:pFolder withPbVector:vec];
		}
			break;
	}
}

- (void)MoveItem:(PbFolder*)pFolderFrom To:(PbFolder*)pFolder withVector:(std::vector<PhoneBookItem*>&)vec andIdx:(int)nIdx andChecked:(BOOL)bCheck
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	
	PhoneBookItem *pContact = vec[nIdx];
	if(!pFolderFrom || !pFolder || !pContact) return;
	
	// Move Item
	pFolderFrom->MoveItemtoFolder(pContact, pFolder);
	
	// Check Clipboard
	if(bCheck)
		[pView CheckClipboard:pContact];
	
	// Clear Vector
	vec[nIdx] = NULL;
	
	// Delete List Item
	[self DeleteItem:pFolderFrom withItem:pContact];
	
	// Insert List Item
	[self InsertItem:pFolder withContact:pContact];
	
	// Select Item
	[self SelectLastItem:pFolder];
	
}

- (void)DeleteItem:(PbFolder*)pFolder withItem:(PhoneBookItem*)pContact
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	pbTreeCtrl *tree = pView.pbTree;
	
	if(!pFolder || !pContact) return;
	
	if([tree IfSelectedFolder:pFolder])
	{
		for(int i=0;i<[self GetItemCount];i++)
		{
			if([self GetItemData:i] == pContact)
			{	
				// Delete List Item
				[dataSource removeObjectAtIndex:i];
				
				// Clear Edit Data
				if([self GetItemCount] == 0)
					[pView ResetEditCtrl];
				
				[tableView reloadData];
				[tableView deselectAll:nil];
				
				[self UpdateStatusBar];
				return;
			}
		}
	}
}

- (void)GetSelectedItem:(BOOL)bCopyDrag
{
	
}

- (void)GetSelectedItem:(NSTreeNode**)hItem withVector:(std::vector<PhoneBookItem*>&)vec
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	pbTreeCtrl *tree = pView.pbTree;
	
	// Reset
	vec.clear();
	
	// Get Selected Folder
	*hItem = [tree GetSelectedItem];
	
	// Get Selected Contacts
	PhoneBookItem *pContact;
	pbCocoa *phb;
	NSDictionary *dict;
	
	NSIndexSet *set = [tableView selectedRowIndexes];
	NSUInteger clickRow = [tableView clickedRow];
	NSUInteger current = [set firstIndex];

	if(clickRow == NSUIntegerMax || (clickRow != NSUIntegerMax && [set containsIndex:clickRow]))
	{
		while(current != NSNotFound)
		{
			dict = [dataSource objectAtIndex:current];
			phb = (pbCocoa*)[dict objectForKey:PBITEM];
			pContact = phb->contact;
			if(pContact)
				vec.push_back(pContact);
		
			current = [set indexGreaterThanIndex:current];
		}
	}
	else 
	{
		dict = [dataSource objectAtIndex:clickRow];
		phb = (pbCocoa*)[dict objectForKey:PBITEM];
		pContact = phb->contact;
		if(pContact)
			vec.push_back(pContact);		
	}

}

- (void)InsertItem:(PhoneBookItem*)pItem withIndex:(int)nItem
{
	if(!pItem) return;
	
	CString sMem, sName, sNumber;
	NSDictionary *dict;
	pItem->GetPhone(sName, sNumber);
	
	if(nItem >= 0)
	{
		dict = [dataSource objectAtIndex:nItem];
	}
	// Memory
	
	sMem = pItem->GetMemType();
	CString strMemory;
	
	if(sMem == _T("SM"))
	{
		strMemory = "SIM";
	}
	else if(sMem == _T("ME"))
	{
		strMemory = sMem;
	}
	else
	{
		strMemory = sMem;
	}
	
	if(nItem < 0)
	{
		if(sMem == _T("SM"))
		{
			m_numSIM++;
		}
		else if(sMem == _T("ME"))
		{
			m_numME++;
		}
	}				
	
	NSString *strName, *strMobile;
	strName = [NSString CStringToNSString:sName];
	strMobile = [NSString CStringToNSString:sNumber];
	
	pbListData *data = [[pbListData alloc] initWithMemory:[NSString CStringToNSString:strMemory]
													 Name:strName
												   Mobile:strMobile
											  withContact:pItem];
	
	// for sms update operation
	if(nItem >= 0)
		[dataSource removeObject:dict];
	
	[self AddRow:data];
	
	[data release];
}

- (BOOL)GetMemoryIdx:(CString)str withMem:(CString&)strMemory andIdx:(int&)nIdx
{
	strMemory = str;
	return [pbViewControl GetFreeIdx:strMemory withIdx:nIdx];
}

- (BOOL)GetMemoryIdx:(PhoneBookItem*)pContact
			  isCopy:(BOOL)bCopy
			 withMem:(CString&)strMemory
			  andIdx:(int&)nIdx
			  isShow:(BOOL)bShowMsg
{
	if([self GetMemoryIdx:_T("SM") withMem:strMemory andIdx:nIdx])
		return TRUE;
	
	if(bShowMsg)
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
						withMsg:LocalizeString(@"IDS_ERROR_PBFULL")
						forType:MB_OK];
	
	return FALSE;
}

- (BOOL)UpdateContact:(int)nItem
			 withItem:(PhoneBookItem*)pContact
			andVector:(std::vector<PhoneBookItem*>&)vContact
{
	vContact.clear();
	
	pbViewControl *pView = [TheDelegate getPbViewController];
	pbTreeCtrl *tree = pView.pbTree;
	PhoneBookItem* pContactSel = [self GetItemData:nItem];
	
	if(!pContact || !pContactSel) 
		return FALSE;
	
	// Check if Handset Folder
	NSTreeNode *hHandSet;
	PbFolder *pFolderHandset;
	if([tree GetHandsetRoot:&hHandSet forFolder:&pFolderHandset] && 
	   [tree IfSelectedFolder:pFolderHandset])
	{
		// Compare Memory
		const CString &sOld = pContactSel->GetMemType();
		const CString &sNew = pContact->GetMemType();
		
		// Get New Index
		int nIdx;
		if(sNew != sOld)
		{
			if(![pbViewControl GetFreeIdx:sNew withIdx:nIdx])
			{
				[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
								withMsg:LocalizeString(@"IDS_ERROR_PBFULL")
								forType:MB_OK];
				delete pContact;
				return FALSE;
			}
			if(sOld == _T("SM"))
				m_numSIM--;
			else if(sOld == _T("ME"))
				m_numME--;
			
			if(sNew == _T("SM"))
				m_numSIM++;
			else if(sNew == _T("ME"))
				m_numME++;
			pContact->SetIndex(nIdx);
		}
		
		PbJobPara para;
		para.job = -1;
		para.nCtrlType = CTL_NULL;
		para.hParent = self;
		para.strTitle = LocalizeString(@"IDS_PB_COPY");
		
		[self LaunchPbJobThread:&para
					 withFolder:pFolderHandset
					   withItem:nItem
				  andSelContact:pContactSel
					 andContact:pContact];
	}
	else
		[self UpdateItem:pContact withItem:nItem andVector:vContact];
	
	return TRUE;
}

- (BOOL)InsertContact:(PbFolder*)pFolder
			 withItem:(PhoneBookItem*)pContact
			andVector:(std::vector<PhoneBookItem*>&)vContact
{
	if(!pFolder || !pContact) return FALSE;
	
	//use member variable 
	m_pFolder = pFolder;
	m_pContact  = pContact;
	
	pbViewControl *pView = [TheDelegate getPbViewController];
	pbTreeCtrl *tree = pView.pbTree;
	vContact.clear();
	
	// Check if Handset Outbox
	if([tree IfHandsetFolder:pFolder])
	{
		// Get Memory
		m_strMemType = pContact->GetMemType();
		
		// Write Contact
		int nIdx;
		if([pbViewControl GetDownloadState])
		{
			if([pbViewControl GetFreeIdx:m_strMemType withIdx:nIdx])
			{
				[self WriteContact:pFolder withItem:pContact andMem:m_strMemType withIdx:nIdx];
			}
			else
			{
				delete pContact;
				[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
								withMsg:LocalizeString(@"IDS_ERROR_PBFULL")
								forType:MB_OK];
			}
		}
		else
		{
			[self ReDownload:PB_INSERT];
		}
	}
	else
	{
		[self InsertItem:pFolder withContact:pContact andVector:vContact];
		[self SelectLastItem:pFolder];
	}
	
	return TRUE;
}

- (BOOL)CheckPhoneName:(CString&)str
			   withMem:(const CString&)strMem
			 andEncode:(int&)encodeClass
			andMaxName:(int&)nMaxName
{
	if ((strMem != _T("ME") && strMem != _T("SM")))
		return TRUE;
	
	int nLen, nMax;
	
	encodeClass = [utility CheckEncodeClass:str andLen:nLen];
	
	const Connection *pConn = CComAgent::GetConnection();
	if(!pConn->CheckSupport0x81CharSet())
	{
		if(encodeClass == PBCHS_0x81)
			encodeClass = PBCHS_UCS2;
	}
	
	// Get Maximum Length
	pbViewControl *pView = [TheDelegate getPbViewController];
	nMaxName = [pView GetNameLimit:strMem];
	if(nMaxName <= 0)
		nMaxName = 14;
	
	if(encodeClass == PBCHS_UCS2) // UCS2
		nMax = (nMaxName-1)/2;
	else if(encodeClass == PBCHS_0x81) // 0x81
		nMax = nMaxName - 3;
	else
		nMax = nMaxName;
	
	// Check Length
	if(nLen <= nMax)
		return TRUE;
	else if(encodeClass == PBCHS_DEFAULT)
	{ // 7-bit
		str = str.Left(nMax);	// WRONG!
		return FALSE;
	}
	else
	{
		// Shorten Name		
#if defined(UNICODE) || defined(_UNICODE)
		str = str.Left(nMax);
#else
		unsigned short *pBuf;
		try
		{
			pBuf = new unsigned short[nMax + 1];
		}
		catch (...)
		{
			pBuf = NULL;
			str = str.Left(nMax);			
		}
		
		if(pBuf)
		{
			MultiByteToWideChar(CP_ACP, 0, str, -1, pBuf, nMax);	
			pBuf[nMax] = '\0';
			
			str = CString(pBuf);
			delete [] pBuf;
		}
		else
		{
			str = str.Left(nMax);
		}
#endif				
	}
	return FALSE;
}

- (void)UpdateListCtrl
{
	std::vector<PhoneBookItem*> vec = [m_dlg GetResultContactVec];
	for(int i = 0 ; i < vec.size() ; i++)
	{
		// Delete List Item
		[self DeleteItem:m_pFolderFrom withItem:vec[i]];
		
		// Insert List Item
		[self InsertItem:m_pFolder withContact:vec[i]];
	}
}

@end
