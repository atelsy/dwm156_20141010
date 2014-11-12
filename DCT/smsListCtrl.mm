//
//  ListCtrl.m
//  DCT
//
//  Created by Fengping Yu on 11-10-14.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsListCtrl.h"
#import "imageAndTextCell.h"
#import "ImageTextCell.h"
#import "DCTAppDelegate.h"
#import "smsCocoa.h"
#import "nsstrex.h"

@implementation smsListCtrl

#pragma mark--
#pragma mark construct and destruct
#pragma mark--

- (id)init
{
	self = [super init];
	if (self) 
	{
		// register notification message
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSmsThreadUpdate:) name:WM_SMSTHREAD_UPDATE object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onDestroySmsDialog:) name:WM_DESTROY_DIALOG object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSmsThreadSave:) name:WM_SMSTHREAD_SAVE object:nil];
		
		m_pFolder = NULL;
		m_pFolderFrom = NULL;
		m_nNumMe = m_nNumSim = 0;
		
		dataSource = [[NSMutableArray alloc] init];   
		
		sortDescriptor = [[NSSortDescriptor alloc] init];
		
		m_bOpenDlg = NO;
	}
	
	return self;
}

- (void)awakeFromNib
{
	NSTableColumn *column = [[tableView tableColumns] objectAtIndex:0];
	
	ImageTextCell *cell = [[[ImageTextCell alloc] init] autorelease];
	
	[column setDataCell:cell];
	
	NSTableColumn *timeColumn = [tableView tableColumnWithIdentifier:TIMECELL];
	
	if([NSSortDescriptor respondsToSelector:@selector(sortDescriptorWithKey:ascending:selector:)])
		[tableView setIndicatorImage:[NSImage imageNamed:@"NSDescendingSortIndicator"] inTableColumn:timeColumn];
	
	bAscending = NO;
	
	lastColumn = timeColumn;
	
	[tableView reloadData];
}

- (void)dealloc
{
	[dataSource release];
	[iconImage release];
	[sortDescriptor release];
	
	[super dealloc];
}

@synthesize dataSource, tableView, iconImage, m_bOpenDlg;

#pragma mark -- 
#pragma mark sms operation
#pragma mark--

- (BOOL)deleteSms
{
	if([self onSmsDelete])
	{
        return YES;
    }
    else
       return NO;
}

- (IBAction)cutSms
{
	NSIndexSet *indexs = [tableView selectedRowIndexes];
	
	cutSelectedItem = [[NSMutableArray alloc] init];
	
	NSUInteger index = [indexs firstIndex];
	
	while (index != NSNotFound)
	{
		[cutSelectedItem addObject:[dataSource objectAtIndex:index]];
		index = [indexs indexGreaterThanIndex:index];
	}
}

- (IBAction)openSms
{
	[[(DCTAppDelegate*)TheDelegate getSmsViewController] smsOpen:nil];
}

- (void)smsCall
{
	NSUInteger index = [tableView clickedRow];
	
	if(index == NSUIntegerMax)
		index = [self getFirstSelected];
	
	NSMutableDictionary *dict = [dataSource objectAtIndex:index];
	
	NSString * number = [dict objectForKey:FROMTOCELL];

//dwm156
//	DCTAppDelegate *delegate = [NSApp delegate];
	
//	[delegate.callCtrl makeCall:number];
}

- (void)addRow:(listData *)pData
{
	NSMutableDictionary *dict = nil;
	
	smsCocoa *sms = [[[smsCocoa alloc] initWithMsg: pData.pMsg] autorelease];
	
	eMemStatus status = pData.pMsg->GetStatus();
	
	eSendStatus es = pData.pMsg->GetMsgSendStatus();
	
	NSString *iconName = nil;
	
	if([pData.memory caseInsensitiveCompare:@"SIM"] == NSOrderedSame)
	{
		if(status == REC_UNREAD)
			iconName = @"UnRead_SIM";
		else if(status == REC_READ)
			iconName = @"Read_SIM";
		else
			iconName = @"sim";
	}
	else if([pData.memory caseInsensitiveCompare:@"ME"] == NSOrderedSame)
	{
		if(status == REC_UNREAD)
			iconName = @"UnRead";
		else if(status == REC_READ)
			iconName = @"Read";
		else
			iconName = @"me";
	}
	else
		iconName = @"pc";
	
	if(es == SMS_SEND_SUCESSFUL)
		iconName = @"Send_successful";
	else if(es == SMS_SEND_FAILED)
		iconName = @"Send_failed";
	
	dict = [[NSMutableDictionary alloc] initWithObjectsAndKeys:pData.memory, MEMORYCELL, pData.fromTo, FROMTOCELL, pData.time, TIMECELL,pData.content, CONTENTCELL, sms, MESSAGE, iconName, ICONCELL, nil];
	
	[self.dataSource addObject:dict];
	
	[dict release];
	
	if(m_bRedraw == YES)
		[tableView reloadData];
	
	[pData release];
}

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
	
	if([identifier isEqualToString:MEMORYCELL])
	{
		NSCell *cell = [tableColumn dataCell];
		ImageTextCell *imageCell = (ImageTextCell*)cell;
		
		NSString *memory = [dict objectForKey:MEMORYCELL];
		[imageCell setPrimaryTextKey:memory];
		
		NSString *imageName = [dict objectForKey:ICONCELL];
		NSImage *image = [self iconImage: imageName];
		
		[imageCell setIcon:image];
		
		return imageCell;
	}
	else if([identifier isEqualToString:FROMTOCELL])
	{
		returnValue = [dict objectForKey:FROMTOCELL]; // data.frmoTo;
	}
	else if([identifier isEqualToString:TIMECELL])
	{
		returnValue = [dict objectForKey:TIMECELL]; //data.time;
	}
	else if([identifier isEqualToString:CONTENTCELL])
	{
		returnValue = [dict objectForKey:CONTENTCELL]; //data.content;
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
		[[NSNotificationCenter defaultCenter] postNotificationName:LVN_ITEMCHANGED object:self userInfo:nil];
		
	}
	else
	{
		//todo: change the content show in the below reader dialog
		NSInteger sel = [tableView selectedRow];
		NSDictionary * dict = [dataSource objectAtIndex:sel];
		
		[[NSNotificationCenter defaultCenter] postNotificationName:LVN_ITEMCHANGED object:self userInfo:dict];
	}
}

NSInteger dataSort(id data1, id data2, void* columnIdentifier)
{
	listData *listData1 = (listData*)data1;
	listData *listData2 = (listData*)data2;
	
	NSComparisonResult result = NSOrderedAscending;
	
	if([(NSString*)columnIdentifier isEqualToString:MEMORYCELL])
	{
		result = [listData1.memory localizedCompare:listData2.memory];
	}
	else if([(NSString*)columnIdentifier isEqualToString:TIMECELL])
	{
		result = [listData1.time localizedCompare: listData2.time];
	}
	else if([(NSString*)columnIdentifier isEqualToString:FROMTOCELL])
	{
		result = [listData1.fromTo localizedCompare:listData2.fromTo];
	}
	else if([(NSString*)columnIdentifier isEqualToString:CONTENTCELL])
	{
		result = [listData1.content localizedCompare:listData2.content];
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
		
		array = [NSArray arrayWithObjects:@"me.png", @"pc.png", @"sim.png", @"Read.png", @"UnRead.png", 
				 @"UnRead_SIM.png", @"Read_SIM.png", @"Send_failed.png",@"Send_successful.png", nil];
		
		for(NSString *str in array)
		{
			NSImage *image = [NSImage imageNamed:str];
			
			if(image != nil)
				[iconImage addObject:image];
		}
	}
	
	imageName = [imageName stringByDeletingPathExtension];
	
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

#pragma mark--
#pragma mark get operation
#pragma mark--

- (SMSMessage*)getInfo:(BOOL&)bReceived withAddress:(NSString **)strFrom Time:(NSString **)strTime andMsgContent:(NSString **)strMessage isConvert:(BOOL)bConvert
{
	int item = [self getFirstSelected];
	
	return [self getInfo:item isReceived:bReceived withAddress:strFrom Time:strTime msgContent:strMessage andEMSData:NULL isConvert:NO];
}

- (SMSMessage*)getInfo:(BOOL&)bReceived withAddress:(NSString **)strFrom Time:(NSString **)strTime andMsgContent:(NSString **)strMessage andEmsData:(EMSData **)pEmsData isConvert:(signed char)bConvert
{
	int nItem = [self getFirstSelected];
	
	return [self getInfo:nItem isReceived:bReceived withAddress:strFrom Time:strTime msgContent:strMessage andEMSData:pEmsData isConvert:bConvert];
}

- (SMSMessage*)getInfo:(int)nItem isReceived:(BOOL&)bReceived withAddress:(NSString **)strFrom Time:(NSString **)strTime msgContent:(NSString **)strMsg andEMSData:(EMSData **)pEmsData isConvert:(BOOL)bConvert
{
	if(nItem < 0 || [dataSource count] <= 0)
	{
		bReceived = YES;
		*strFrom = nil;
		*strTime = nil;
		*strMsg = nil;
		
		if(pEmsData)
			*pEmsData = NULL;
		
		return NULL;
	}
	
	NSDictionary *dict = [dataSource objectAtIndex:nItem];
	smsCocoa *sms = [dict objectForKey:MESSAGE];
	SMSMessage *pMsg = sms->msg;
	CString from;
	int nLen;
	
	if(pMsg)
	{
		if(pEmsData)
		{
			pMsg->GetMsg(pEmsData);
			unsigned short *spbuf = [utility getEmsText:*pEmsData withLength:&nLen];
			if(spbuf)
				*strMsg = [[NSString alloc] initWithCharacters:spbuf length:nLen];
			else
				*strMsg = @"";
			delete  spbuf;
		}
		
		bReceived = [self getReceived:pMsg];
		pMsg->GetAddressNumber(from);
		*strTime = [utility getTime:pMsg];
		
		if(bConvert)
		{
			*strFrom = [TheDelegate convertNumber:from];
		}
		else
			*strFrom = [NSString getStringFromWchar:(LPCTSTR)from];
	}
	else
	{
		bReceived = YES;
		*strFrom = nil;
		*strTime = nil;
		*strMsg = nil;
		
		if(pEmsData)
			*pEmsData = NULL;
	}
	
	return pMsg;
}

- (BOOL)getOneReceived:(int&)nItem isReceived:(BOOL)bReceived
{
	NSIndexSet *selects = [tableView selectedRowIndexes];
	
	NSUInteger currentIndex = [selects firstIndex];
	
	while(currentIndex != NSNotFound)
	{
		SMSMessage *pMsg = [self getItemData:(int)currentIndex];
		
		if(pMsg && ([self getReceived:pMsg] == bReceived))
			return YES;
		
		currentIndex = [selects indexGreaterThanIndex:currentIndex];
	}
	
	return NO;
}

- (BOOL)getReceived
{
	int nItem = [self getFirstSelected];
	
	if(nItem < 0)
		return NO;
	
	SMSMessage *pMsg = [self getItemData:nItem];
	
	return [self getReceived:pMsg];
}

- (BOOL)getReceived:(SMSMessage *)pMsg
{
	if(pMsg)
	{
		eMemStatus status = pMsg->GetStatus();
		
		if(status == REC_READ || status == REC_UNREAD)
			return YES;
	}
	
	return NO;
}

- (void)deleteAllItems
{
	[dataSource removeAllObjects];
	
	[tableView reloadData];
}

#pragma mark --
#pragma mark select operation
#pragma mark--

- (int)getSelectedCount
{
	NSIndexSet *set = [tableView selectedRowIndexes];
	
	return (int)[set count];
}



- (int)getFirstSelected
{
	NSIndexSet *set = [tableView selectedRowIndexes];
	
	if([set count] <= 0)
	{
		if(m_bOpenDlg)
		{
			
			return [[self getParent] getSelectIndex];
		}
		else
			return -1;
	}
	
	return (int)[set firstIndex];
}

- (void)getSelectItem:(NSTreeNode **)hItem withMsg:(std::vector<SMSMessage *> &)vec
{
	vec.clear();
	
	*hItem = [[TheDelegate getSmsViewController].smsTree getSelectedItem];
	
	SMSMessage *pMsg;
	smsCocoa *sms;
	NSDictionary *dict;
	
	NSIndexSet *set = [tableView selectedRowIndexes];
	NSUInteger clickRow = [tableView clickedRow];
	
	NSUInteger current = [set firstIndex];
	
	if(clickRow == NSUIntegerMax || (clickRow != NSUIntegerMax && [set containsIndex:clickRow]))
	{	
		while(current != NSNotFound)
		{
			dict = [dataSource objectAtIndex:current];
			sms = (smsCocoa*)[dict objectForKey:MESSAGE];
			pMsg = sms->msg;
			if(pMsg)
				vec.push_back(pMsg);
			
			current = [set indexGreaterThanIndex:current];
		}
	}
	else
	{
		dict = [dataSource objectAtIndex:clickRow];
		sms = (smsCocoa*)[dict objectForKey:MESSAGE];
		pMsg = sms->msg;
		if(pMsg)
			vec.push_back(pMsg);
	}
}

- (bool)getSelected
{
	return ([[tableView selectedRowIndexes] count] > 0);
}

- (SMSMessage*)getItemData:(int)nItem
{
	if(nItem >= 0 && nItem < [self getItemCount])
	{
		NSDictionary *dict = [dataSource objectAtIndex:nItem];
		
		if(dict != nil)
		{
			smsCocoa *sms = [dict objectForKey:MESSAGE];
			return sms->msg;
		}
	}
	
	return NULL;
}

- (void)selectLastItem:(SmsFolder *)pFolder
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	
	if([tree ifSelectedFolder:pFolder] == YES)
		[self selectItem:([self getItemCount] - 1)];
}

- (void)selectItem:(int)nItem
{
	[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:nItem] byExtendingSelection:NO];
	
	[tableView scrollRowToVisible:nItem];
	
	[self tableViewSelectionDidChange:nil];
}

#pragma mark--
#pragma mark display operation
#pragma mark--

- (void)repaintTree
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	
	smsTreeCtrl *tree = pView.smsTree;
	
	SmsFolder *pFolder = [tree getSelectedItemData];
	
	[self display:pFolder];
	
	[self updateStatusBar];
}

- (BOOL)getClipboard
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	
	std::vector<SMSMessage*>vec = [pView getMsgVector];
	int nState = 0;
	
	for(int i = 0; i < vec.size(); i++)
	{
		if(vec[i])
			nState |= vec[i]->GetStatus();
	}
	
	SmsFolder *pFolder = [tree getSelectedItemData];
	if(pFolder)
	{
		if(([tree ifHandsetInbox:pFolder] || [tree ifHandsetOutbox:pFolder]) && 
		   (![pView getSimStatus] || ![pView getConnStatus]))
			return NO;
		
		folder_state_struct state;
		pFolder->GetFolderState(state);
		
		if(state.nState & nState && ![tree ifHandsetOutbox:pFolder])
			return YES;
	}
	
	return NO;
}

- (int)getItemCount
{
	//todo: maybe modify as [dataSouce count];
	return (int)[tableView numberOfRows];
}

- (BOOL)updateMessage:(int)nItem 
		  withMessage:(SMSMessage *)pMsg 
		 andMsgVector:(std::vector<SMSMessage *> &)vMsg
{
	if(!pMsg)
		return NO;
	
	vMsg.clear();
	
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	
	NSTreeNode *hOutbox;
	SmsFolder *pFolderOutbox;
	
	if([tree getHandsetOutbox:&hOutbox forFolder:&pFolderOutbox] && 
	   [tree ifSelectedFolder:pFolderOutbox])
	{
		CString s;
		pMsg->GetMemType(s);
		
		SmsJobPara para;
		para.sMemory = [NSString getStringFromWchar:(LPCTSTR)s];
		para.job = -1;
		para.nCtrlType = CTL_SMSNULL;
		
		[self launchSmsJobThread:&para 
					  withFolder:pFolderOutbox 
						 message:pMsg andIndex:nItem];
	}
	else
	{
		[self updateItem:pMsg withItem:nItem andMsgs:vMsg];
	}
	
	return YES;
}

- (SMSMessage*)updateItem:(SMSMessage *)pMsg 
				 withItem:(int)nItem 
				  andMsgs:(std::vector<SMSMessage *> &)vMsg
{
	if(!pMsg)
		return NULL;
	
	SMSMessage *pMsgSel = [self getItemData:nItem];;
	if(pMsgSel)
	{
		*pMsgSel = *pMsg;
		vMsg.push_back(pMsgSel);
		
		[self insertItem:pMsgSel withIndex:nItem];
		[self selectItem:nItem];
	}
	
	delete pMsg;
	
	return pMsgSel;
}

- (void)insertItem:(SMSMessage *)pMsg withIndex:(int)index
{
	if(pMsg == NULL)
		return;
	
	CString strMemory, strNumber;
	NSString *strName = nil, *strTime, *strMessage;
	EMSData *pEmsData;
	NSString *strOriMemory;
	int nLen;
	NSDictionary *dict;
	
	if(index >= 0)
	{
		dict = [dataSource objectAtIndex:index];
		strOriMemory = [dict objectForKey:MEMORYCELL];
	}
	
	pMsg->GetMemType(strMemory);
	
	//modify the message number
	if(strMemory == _T("SM"))
	{
		strMemory = _T("SIM");
		
		if(index >= 0)
		{
			if([strOriMemory caseInsensitiveCompare:@"ME"] == NSOrderedSame)
			{
				m_nNumMe--;
				m_nNumSim++;
			}
		}
		else
			m_nNumSim++;
	}
	else if(strMemory == _T("ME"))
	{
		if(index >= 0)
		{
			if([strOriMemory caseInsensitiveCompare:@"SIM"] == NSOrderedSame)
			{
				m_nNumSim--;
				m_nNumMe++;
			}
		}
		else
			m_nNumMe++;
	}
	
	pMsg->GetAddressNumber(strNumber);
	
	strName = [TheDelegate convertNumber:strNumber];
	
	if(strName == nil)
		strName = [NSString CStringToNSString:(LPCTSTR)strNumber];
	
	strTime = [utility getTime:pMsg];
	
	pMsg->GetMsg(&pEmsData);
	
	unsigned short *pbuf = [utility getEmsText:pEmsData withLength:&nLen];
	
	if(pbuf)
	{
		strMessage = [[NSString alloc] initWithCharacters:pbuf length:nLen];
		
		delete pbuf;
	}
	else
		strMessage = @"";
	
	listData *data = [[listData alloc] initWithMemory:[NSString getStringFromWchar:(LPCTSTR)strMemory] 
											   FromTo:strName 
												 Time:strTime 
											  Content:strMessage 
											  withMsg:pMsg];
	
	// for sms update operation
	if(index >= 0)
		[dataSource removeObject:dict];
	
	[self addRow:data];
}

- (void)display:(SmsFolder *)pFolder
{
	if(!pFolder)
		return;
	
	[self setRedraw:NO];
	
	[self deleteAllItems];
	m_nNumSim = m_nNumMe = 0;
	
	SMSMessage *pMsg = NULL;
	ItemPos pos = pFolder->GetFirstItemPos();
	
	while(pos != 0)
	{
		pMsg = pFolder->GetNextItem(pos);
		if(pMsg != NULL)
			[self insertItem:pMsg withIndex:-1];
	}
	pFolder->CloseNextItem(pos);
	
	//todo: add sort operation
	[self tableView: tableView mouseDownInHeaderOfTableColumn:lastColumn];
	[dataSource sortUsingDescriptors:[tableView sortDescriptors]];
	
	[tableView reloadData];
	
	[self setRedraw:YES];
	
	[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:0]
		   byExtendingSelection:NO];
}

#pragma mark--
#pragma mark insert, delete
#pragma mark--

- (void)deleteItem:(SmsFolder *)pFolder withMessage:(SMSMessage *)pMsg
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	
	if(!pFolder || !pMsg)
		return;
	
	if([tree ifSelectedFolder:pFolder] == YES)
	{
		for(int i = 0; i < [self getItemCount]; i++)
		{
			if([self getItemData:i] == pMsg)
			{
				[dataSource removeObjectAtIndex:i];
				break;
			}
		}
		
		[tableView reloadData];
		
		[tableView deselectAll:nil];
		
		if([self getItemCount] == 0)
			[pView resetEditCtrl];
		
		[self updateStatusBar];
		
		return;
	}
}

- (BOOL)insertMessage:(SmsFolder *)pFolder 
		  withMessage:(SMSMessage *)pMsg 
		 andMsgVector:(std::vector<SMSMessage *> &)vMsg
{
	if(!pFolder || !pMsg)
		return NO;
	
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	vMsg.clear();
	
	if([tree ifHandsetOutbox:pFolder])
	{
		CString s;
		pMsg->GetMemType(s);
		pMsg->SetStatus(STO_UNSENT);
		
		SmsJobPara para;
		para.job = -1;
		para.nCtrlType = CTL_SMSNULL;
		para.sMemory = [NSString getStringFromWchar:(LPCTSTR)s];
		
		[self launchSmsJobThread:&para 
					  withFolder:pFolder 
						 message:pMsg
						andIndex: -1];
	}
	else if([tree ifHandsetInbox:pFolder])
	{
		CString s;
		pMsg->GetMemType(s);
		pMsg->SetStatus(REC_READ);
		
		SmsJobPara para;
		para.job = -1;
		para.nCtrlType = CTL_SMSNULL;
		para.sMemory = [NSString getStringFromWchar:(LPCTSTR)s];
		
		[self launchSmsJobThread:&para 
					  withFolder:pFolder 
						 message:pMsg
						andIndex: -1];
	}
	else
	{
		[self insertItem:pFolder withMessage:pMsg andMsgVector:vMsg];
		[self selectLastItem:pFolder];
	}
	
	return YES;
}

- (void)insertItem:(SmsFolder *)pFolder
	   withMessage:(SMSMessage *)pMsg
	  andMsgVector:(std::vector<SMSMessage *> &)vMsg
{
	if(!pFolder || !pMsg)
		return;
	
	vMsg.push_back(pMsg);
	pFolder->InsertItem(pMsg);
	
	[self insertItem:pFolder withMessage:pMsg];
}

- (void)insertItem:(SmsFolder *)pFolder withMessage:(SMSMessage *)pMsg
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	
	if(!pFolder || !pMsg)
		return;
	
	if([tree ifSelectedFolder:pFolder] == YES)
		[self insertItem:pMsg withIndex:-1];
}

#pragma mark --
#pragma mark cut, copy and paste
#pragma mark--

- (DIRECTION)getDirection:(NSTreeNode *)hFrom to:(NSTreeNode *)hTo
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	
	SmsFolder *pFolderInbox, *pFolderOutbox;
	NSTreeNode *hInbox, *hOutbox;
	
	[tree getHandsetInbox:&hInbox forFolder:&pFolderInbox];
	[tree getHandsetOutbox:&hOutbox forFolder:&pFolderOutbox];
	
	if((hFrom == hInbox) || (hFrom == hOutbox))
	{
		if((hTo == hInbox) || (hTo == hOutbox))
			return HStoHS;
		else
			return HStoPC;
	}
	else if((hTo == hInbox) || (hTo == hOutbox))
	{
		return PCtoHS;
	}
	else
		return PCtoPC;
}

- (void)moveItemFrom:(SmsFolder *)pFolderFrom to:(SmsFolder *)pFolderTo withMsg:(std::vector<SMSMessage *> &)vec index:(int)nIdx isCheck:(signed char)bCheck
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	
	SMSMessage *pMsg = vec[nIdx];
	
	if(!pFolderFrom || !pFolderTo || !pMsg)
		return;
	
	pFolderFrom->MoveItemtoFolder(pMsg, pFolderTo);
	
	if(bCheck == YES)
		[pView checkClipboard:pMsg];
	
	[self deleteItem:pFolderFrom withMessage:pMsg];
	
	[self insertItem:pFolderTo withMessage:pMsg];
	
	[self selectLastItem:pFolderTo];
	
	vec[nIdx] = NULL;
}

- (void)moveMessageFrom:(NSTreeNode *)hFrom To:(NSTreeNode *)hTo forMessages:(std::vector<SMSMessage *> &)vec whetherRestore:(signed char)isRestore
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	
	if(hFrom == hTo)
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_MOVEMESSAGE") forType:MB_OK];
		return;
	}
	
	SmsFolder *pFolderFrom = [tree getItemData:hFrom];
	SmsFolder *pFolder = [tree getItemData:hTo];
	
	m_pFolder = pFolder;
	m_pFolderFrom = pFolderFrom;
	
	if(!pFolder || !pFolderFrom)
		return;
	
	eDelMsgOrigin eo = SMS_DELETE_FROM_NONE;
	
	if([tree ifHandsetInbox:pFolderFrom])
		eo = SMS_DELETE_FROM_HANDSET_INBOX;
	else if([tree ifHandsetOutbox:pFolderFrom])
		eo = SMS_DELETE_FROM_HANDSET_OUTBOX;
	else if([tree ifPcInbox:pFolderFrom])
		eo = SMS_DELETE_FROM_PC_INBOX;
	else if([tree ifPcOutbox:pFolderFrom])
		eo = SMS_DELETE_FROM_PC_OUTBOX;
	else if([tree ifPcDraft:pFolderFrom])
		eo = SMS_DELETE_FROM_PC_DRAFT;
	else if([tree ifPcPersonalFolder:pFolderFrom])
		eo = SMS_DELETE_FROM_PERSONALFOLDER;
	
	int i = 0;
	
	if([tree ifTrashCan:pFolder])
	{
		for(i = 0; i < vec.size(); i++)
			vec[i]->SetMsgDeleteOrigin(eo);
	}
	
	if(isRestore == NO && [tree ifTrashCan:pFolder] == NO)
	{
		for(i = 0; i < vec.size(); i++)
			vec[i]->SetMsgDeleteOrigin(SMS_DELETE_FROM_NONE);
	}
	
	std::vector<SMSMessage*>msg = [pView getMsgVector];
	
	bool bCheck;
	if(&vec == &msg)
		bCheck = false;
	else
		bCheck = true;
	
	//get direction
	DIRECTION dir = [self getDirection:hFrom to:hTo];
    
    
    int m_numSIM1=[(DCTAppDelegate*)TheDelegate smsController]->m_numSIM;
    
    /************ 20131109 by Zhuwei **********************************************/
    NSString * s = @"SM";
    BOOL isSIMFull = m_numSIM1 >= 50;
    SMSMessage *pMsg = [utility newMessage];
    if(isSIMFull||(isSIMFull && pMsg)) {
        pMsg->SetMemType(_T("ME"));
        s = @"ME";
    } else {
        pMsg->SetMemType(_T("SM"));
    }
    /******************************************************************************/
	
	BOOL bSuccess = YES;
	std::vector<SMSMessage*>vMsg;
	std::vector<CString> vAddress;
	
	SmsFolder *pFolderInbox, *pFolderOutbox;
	NSTreeNode *hInbox, *hOutbox;
	bool isInbox = false;
	
	if(dir == PCtoPC)
	{
		for(int i = 0; (i < vec.size() && bSuccess); i++)
		{
			if([utility getState:vec[i] withFolder:pFolder] == YES)
				[self moveItemFrom:pFolderFrom to:pFolder withMsg:vec index:i isCheck:bCheck];
			
			[pView resetSearch];
		}
	}
	else if(dir == HStoPC)
	{
		SmsJobPara para;
		para.strTitle = LocalizeString(@"IDS_SMS_DEL");
		para.pFolder = pFolder;
		para.pFolderFrom = pFolderFrom;
		para.bIsFromPC = true;
		para.bIsInbox = true;
		para.nCtrlType = CTL_SMSHSTOPC;
		para.pvMessage = &vec;
		para.job = 0;
		para.sMemory = @"";
		
		[self launchSmsJobThread:&para];
        m_pCom = [TheDelegate getComAgent];
        
        SMSViewCtrl *smsView = [TheDelegate getSmsViewController];
        
        m_pCom->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "SM", READ_SM);
        
        m_pCom->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "ME",READ_ME);
        
	}
	else if(dir == PCtoHS)
	{
		if([tree getHandsetInbox:&hInbox forFolder:&pFolderInbox] && [tree getHandsetOutbox:&hOutbox forFolder:&pFolderOutbox])
		{
			if(hInbox == hTo)
				isInbox = true;
			else
				isInbox = false;
		}
		
		SmsJobPara para;
		para.strTitle = LocalizeString(@"IDS_SMS_COPY");
		para.pvMessage = &vec;
		para.job = 0;
		para.pFolderFrom = pFolderFrom;
		para.pFolder = pFolder;
		para.bIsInbox = isInbox;
		para.bIsFromPC = true;
		para.sMemory = s;
		para.nCtrlType = CTL_SMSMOVEPCTOHS;
		
		[self launchSmsJobThread:&para];
	}
	else if(dir == HStoHS)
	{
		if([tree getHandsetInbox:&hInbox forFolder:&pFolderInbox] && [tree getHandsetOutbox:&hOutbox forFolder:&pFolderOutbox])
		{
			if(hInbox == hTo)
				isInbox = true;
			else
				isInbox = false;
		}
		
		SmsJobPara para;
		para.strTitle = LocalizeString(@"IDS_SMS_COPY");
		para.pvMessage = &vec;
		para.job = 0;
		para.pFolderFrom = pFolderFrom;
		para.pFolder = pFolder;
		para.bIsInbox = isInbox;
		para.bIsFromPC = false;
		para.sMemory = s;
		para.nCtrlType = CTL_SMSHSTOHS;
		
		[self launchSmsJobThread:&para];
	}
	
	[self updateStatusBar];
    

}
- (void)copyMessageFrom:(NSTreeNode *)from To:(NSTreeNode *)to forMessages:(std::vector<SMSMessage *> &)vec
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	
	SmsFolder *pFolderFrom = [tree getItemData:from];
	m_pFolder = [tree getItemData:to];
	
	if(!pFolderFrom || !m_pFolder)
		return;
	
	SmsFolder *pFolderInbox;
    
	NSTreeNode *hInbox;
	bool isInbox = false;
	
	DIRECTION dir = [self getDirection:from to:to];
    
    
    
    int m_numSIM1=[(DCTAppDelegate*)TheDelegate smsController]->m_numSIM;
    int m_numME1=[(DCTAppDelegate*)TheDelegate smsController]->m_numME;
   	
    /************  **********************************************/
    NSString * s = @"SM";
    BOOL isSIMFull = m_numSIM1 >= 50;
    SMSMessage *pMsg = [utility newMessage];
    if(isSIMFull||(isSIMFull && pMsg)) {
        pMsg->SetMemType(_T("ME"));
        s = @"ME";
    } else {
        pMsg->SetMemType(_T("SM"));
    }
    
    /******************************************************************************/
	
	std::vector<SMSMessage*>vMsg;
	BOOL bSuccess = YES;
	
	switch (dir)
	{
		case PCtoPC:
		case HStoPC:
			for(int i = 0; (i < vec.size() && bSuccess == YES); i++)
			{
				if([utility getState:vec[i] withFolder:m_pFolder])
				{
					SMSMessage *pMsg = [utility newMessage];
					if(!pMsg)
						break;
					
					*pMsg = *vec[i];
					pMsg->SetMemType(_T("PC"));
					[self insertItem:m_pFolder withMessage:pMsg andMsgVector:vMsg];
				}
			}
			
			[pView resetSearch];
			
			[tableView reloadData];
			
			[self selectLastItem:m_pFolder];
			
			[self updateStatusBar];
			
			break;
			
		case HStoHS:
		case PCtoHS:
            if(m_numSIM1+m_numME1==99)
            {
                [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_MEMORYFULL") forType:MB_OK];
                return ;
            }else
            {
                if([tree getHandsetInbox:&hInbox forFolder:&pFolderInbox] )
                {
                    if(hInbox == to)
                    {
                        isInbox = true;
                    }
                    else
                    {
                        isInbox = false;
                    }
                }
                
                SmsJobPara para;
                para.strTitle = LocalizeString(@"IDS_SMS_COPY");
                para.pvMessage = &vec;
                para.job = 1;
                para.pFolderFrom = NULL;
                para.pFolder = m_pFolder;
                para.bIsInbox = isInbox;
                para.bIsFromPC = true;
                para.sMemory = s;
                para.nCtrlType = CTL_SMSCOPYPCTOHS;
                [self launchSmsJobThread:&para];
                m_pCom = [TheDelegate getComAgent];
                SMSViewCtrl *smsView = [TheDelegate getSmsViewController];
                m_pCom->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "SM", READ_SM);
                m_pCom->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "ME",READ_ME);
                
                [self updateStatusBar];
            }
			break;
			
		default:
			break;
	}
    // by zhang shao hua update SMS
    
    
    
}

- (void)copyMessageFrom:(std::vector<SMSMessage *> &)vecFrom To:(std::vector<SMSMessage *> &)vec withFolder:(SmsFolder *)pFolder isCopy:(signed char)bCopy
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	
	if(!pFolder)
		return;
	
	vec.clear();
	
	if(bCopy)
	{
		SmsFolder *pFolderSel = [tree getSelectedItemData];
		if(pFolderSel != NULL && (pFolderSel == pFolder))
			bCopy = NO;
	}
	
	for(int i = 0; i < vecFrom.size(); i++)
	{
		if([utility getState:vecFrom[i] withFolder:pFolder])
		{
			if(bCopy)
			{
				SMSMessage *pMsg = [utility newMessage];
				if(!pMsg)
					break;
				
				*pMsg = *vecFrom[i];
				pMsg->SetMemType(_T("PC"));
				pFolder->InsertItem(pMsg);
				
				vec.push_back(pMsg);
			}
			else
				vec.push_back(vecFrom[i]);
		}
	}
}

- (BOOL)onSmsDelete
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	
	NSTreeNode *hTrash;
	SmsFolder *pFolderTrash;
	NSTreeNode *hDelete = [pView getDeleteItem];
	std::vector<SMSMessage*>& deleSmsVec = [pView getDeleteMessage];
	
	BOOL result = NO;
	
	if([self getSelectedCount] == 0 && [tableView clickedRow] == -1)
	{
		return NO;
	}
	
	if(![tree getTrashCan:&hTrash forFolder:&pFolderTrash])
		return NO;
	
	[self getSelectItem:&hDelete withMsg: deleSmsVec];
	
	
	if([tree inTrashCan:hDelete])
	{
		if([utility showMessageBox: LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_FOLDER_TRASHDELETE") forType:MB_YESNO] == NSAlertDefaultReturn)
		{
			[self deleteMessage: hDelete];
			[pView resetSearch];
			result = YES;
		}
		
		return result;
	}
	
	if([utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_MESSAGE_DELETE") forType:MB_YESNO] == NSAlertDefaultReturn)
	{
		[self moveMessageFrom:hDelete To:hTrash forMessages:deleSmsVec whetherRestore:NO];
		return YES;
	}
	else
	{
		return NO;
	}
}

- (void)deleteMessage:(NSTreeNode*)hItem
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	std::vector<SMSMessage*>& vec = [pView getDeleteMessage];
	
	smsTreeCtrl *tree = pView.smsTree;
	//   NSTreeNode *hFrom = [pView getDeleteItem];
	
	SmsFolder *pFolderFrom = [tree getItemData:hItem];
	if(!pFolderFrom)
		return;
	
	DIRECTION dir = [self getDirection:hItem to:NULL];
    
	
	if(dir == HStoPC)
	{
		;
	}
	else
	{
		for(int i = 0; i < vec.size(); i++)
		{
			[pView checkClipboard:vec[i]];
			
			[self deleteItem:pFolderFrom withMessage:vec[i]];
			
			pFolderFrom->DeleteItem(vec[i]);
		}
	}
	
	vec.clear();
	
	[pView resetEditCtrl];
	
	[pView resetSearch];
	
	[self updateStatusBar];
    //by zhang shao hua 20131203 update SMS count
    m_pCom = [TheDelegate getComAgent];
	
	SMSViewCtrl *smsView = [TheDelegate getSmsViewController];
    
    m_pCom->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "SM", READ_SM);
    
    m_pCom->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "ME",READ_ME);
    [self updateStatusBar];
   
    
}

- (void)cutMessage
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	m_hCopyItem = [pView getCopyItem];
	
	[self getSelectItem:&m_hCopyItem withMsg:[pView getCopyMessage]];
	
	[pView setCopy:NO];
	
	[self updateStatusBar];
}

- (void)copyMessage
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	
	m_hCopyItem = [pView getCopyItem];
	[self getSelectItem: &m_hCopyItem withMsg:[pView getCopyMessage]];
	
	[pView setCopy:YES];
}

- (void)pasteMessage:(NSTreeNode *)hItem
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	
	if([pView isCopy])
	{
		[self copyMessageFrom:m_hCopyItem To:hItem forMessages:[pView getCopyMessage]];
	}
	else
	{
		[self moveMessageFrom:m_hCopyItem To:hItem forMessages:[pView getCopyMessage] whetherRestore:NO];
	}
}

#pragma mark -- 
#pragma mark sms job thread message
#pragma mark--

- (void)onSmsThreadUpdate:(NSNotification *)notification
{
	NSDictionary *dict = [notification userInfo];
	
	int nItem = [[dict objectForKey:ITEM_INDEX] intValue];
	classToObject *msgObject = [dict objectForKey:ITEM_MESSAGE];
	
	if(nItem >= 0 && msgObject != nil)
	{
		SMSMessage *pMsg = msgObject->pMsg;
		std::vector<SMSMessage*> vMsg;
		
		[self updateItem:pMsg withItem:nItem andMsgs:vMsg];
	}
	else
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_UPDATEMESSAGE") forType:MB_OK];
	}
}

- (void)onSmsThreadSave:(NSNotification*)notification
{
	NSDictionary *dict = [notification userInfo];
	classToObject *object = [dict objectForKey:ITEM_FOLDER];
	SmsFolder *pFolder = object->pFolder;
	classToObject *msgObject = [dict objectForKey:ITEM_MESSAGE];
	SMSMessage *pMsg = NULL;
	
	if(msgObject)
		pMsg = msgObject->pMsg;
	
	if(pFolder && pMsg)
	{
		[self updateMsgStatusIndex:pFolder withMessage:pMsg];
		
		std::vector<SMSMessage*>vMsg;
		[self insertItem:pFolder withMessage:pMsg andMsgVector:vMsg];
		[self selectLastItem:pFolder];
		[self updateStatusBar];
	}
	else
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_WRITEMESSAGE") forType:MB_OK];
	}
}

- (void)launchSmsJobThread:(SmsJobPara *)para
{
	SMSViewCtrl* pView = (SMSViewCtrl*)[self getParent];
	
	if(m_dlg == nil)
		m_dlg = [smsJobThreadDlg alloc];
	
	[m_dlg initWithParameter:para];
	
	[m_dlg initWithWindowNibName:@"smsJobThreadDlg"];
	
	[pView enterDlg];
	
    [m_dlg showWindow:nil];
    [m_dlg  close];
    
}

- (void)launchSmsJobThread:(SmsJobPara *)para withFolder:(SmsFolder *)pFolder message:(SMSMessage *)pMsg andIndex:(int)index
{
	SMSViewCtrl* pView = (SMSViewCtrl*)[self getParent];
	
	if(m_dlg == nil)
		m_dlg = [smsJobThreadDlg alloc];
	
	[m_dlg initWithParameter:para];
	
	[m_dlg initWithWindowNibName:@"smsJobThreadDlg"];
	
	[pView enterDlg];
	
	[m_dlg showWindow:nil];
	
	[m_dlg start:pFolder message:pMsg withMemory:para->sMemory andIndex:index];
}

#pragma mark--
#pragma mark common function
#pragma mark--

- (void)updateStatusBar
{
	DCTAppDelegate *delegate = TheDelegate;
	
	NSString *info; 
	
	if(([((SMSViewCtrl*)[self getParent]).smsTree getSelectedItemData] != NULL) && delegate.currentView == SMSVIEW)
    {
        SmsFolder *pFolder = [((SMSViewCtrl*)[self getParent]).smsTree getSelectedItemData];
        
        
        if([((SMSViewCtrl*)[self getParent]).smsTree ifHandsetInbox:pFolder])
        {
            
            info = [NSString stringWithFormat:@"%@ %d", LocalizeString(@"IDS_STATUS_USED"),([(DCTAppDelegate*)TheDelegate smsController]->m_numSIM+[(DCTAppDelegate*)TheDelegate smsController]->m_numME)];
        }
        
        if([((SMSViewCtrl*)[self getParent]).smsTree ifHandsetOutbox:pFolder])
        {
            
            info = [NSString stringWithFormat:@"%@ %d", LocalizeString(@"IDS_STATUS_USED"),0];
            
        }
        if([((SMSViewCtrl*)[self getParent]).smsTree ifPcInbox:pFolder])
        {
            
            info = [NSString stringWithFormat:@"%d %@", [dataSource count],LocalizeString(@"IDS_STATUS_MESSAGES")];
            
        }
        
        if([((SMSViewCtrl*)[self getParent]).smsTree ifPcOutbox:pFolder])
        {
            
            info = [NSString stringWithFormat:@"%d %@", [dataSource count],LocalizeString(@"IDS_STATUS_MESSAGES")];
            
        }
        if([((SMSViewCtrl*)[self getParent]).smsTree ifPcSentbox:pFolder])
        {
            
            info = [NSString stringWithFormat:@"%d %@", [dataSource count],LocalizeString(@"IDS_STATUS_MESSAGES")];
            
        }
        
        if([((SMSViewCtrl*)[self getParent]).smsTree ifPcDraft:pFolder])
        {
            
            info = [NSString stringWithFormat:@"%d %@", [dataSource count],LocalizeString(@"IDS_STATUS_MESSAGES")];
            
        }
        
        if([((SMSViewCtrl*)[self getParent]).smsTree ifPcPersonalFolder:pFolder])
        {
            
            info = [NSString stringWithFormat:@"%d %@", [dataSource count],LocalizeString(@"IDS_STATUS_MESSAGES")];
            
        }
        if([((SMSViewCtrl*)[self getParent]).smsTree ifTrashCan:pFolder])
        {
            
            info = [NSString stringWithFormat:@"%d %@", [dataSource count],LocalizeString(@"IDS_STATUS_MESSAGES")];
            
        }
        
		//info = [NSString stringWithFormat:@"%d %@", [dataSource count],LocalizeString(@"IDS_STATUS_MESSAGES")];
    }
    
    
	else
        //info = @"Ready";
        info = LocalizeString(@"IDS_STATUS_READY");
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
	
	return [delegate getSmsViewController];
}

- (id)getsmsJobThreadDlg
{
	return m_dlg;
}

- (void)onDestroySmsDialog:(NSNotification *)notification
{
	NSDictionary *dict = [notification userInfo];
	
	int wParam = [[dict objectForKey:ITEM_MSGID] intValue];
	
	std::vector<SMSMessage*>vec = [m_dlg getResultMsgVec];
	
	switch(wParam)
	{
		case CTL_SMSHSTOPC:
		{
			for(int i = 0; i < vec.size(); i++)
			{
				[self deleteItem:m_pFolderFrom withMessage:vec[i]];
				[self insertItem:m_pFolder withMessage:vec[i]];
			}
			
			[self selectLastItem:m_pFolder];
		}
			break;
			
		case CTL_SMSMOVEPCTOHS:
		case CTL_SMSCOPYPCTOHS:
		case CTL_SMSHSTOHS:
		{
			std::vector<SMSMessage*>vMsg;
			eDelMsgOrigin eo;
			NSTreeNode *hItem;
			SmsFolder *pFolder;
			smsTreeCtrl *tree = [[self getParent] smsTree];
			
			for(int i = 0; i < vec.size(); i++)
			{
				eo = vec[i]->GetMsgDeleteOrigin();
				
				if(eo == SMS_DELETE_FROM_HANDSET_INBOX)
				{
					[tree getHandsetInbox:&hItem forFolder:&pFolder];
					[self insertItem:pFolder withMessage:vec[i] andMsgVector:vMsg];
				}
				else if(eo == SMS_DELETE_FROM_HANDSET_OUTBOX)
				{
					[tree getHandsetOutbox:&hItem forFolder:&pFolder];
					[self insertItem:pFolder withMessage:vec[i] andMsgVector:vMsg];
				}
				else
					[self insertItem:m_pFolder withMessage:vec[i] andMsgVector:vMsg];
			}
		}
			break;
	}
	
	[self updateStatusBar];
	
	[self closeThreadDlg];
}

- (void)closeThreadDlg
{
	SMSViewCtrl* pView = (SMSViewCtrl*)[self getParent];
	
	if(m_dlg != nil)
	{
		[m_dlg.window performClose:nil];
		[m_dlg release];
		m_dlg = nil;
	}
	
	[pView leaveDlg];
	
	[pView resetSearch];
	
	[self selectLastItem:m_pFolder];
	
	m_pFolder = NULL;
}

- (void)updateMsgStatusIndex:(SmsFolder *)pFolder withMessage:(SMSMessage *)pMsg
{
	SMSViewCtrl *pView = TheDelegate.smsController;
	smsTreeCtrl *tree = pView.smsTree;
	
	if([tree ifHandsetInbox:pFolder] == NO)
		return;
	
	std::list<int> indexList = pMsg->GetMsgIndexList();
	std::list<int>::iterator iter = indexList.begin();
	
	if(iter != indexList.end())
		pMsg->SetMsgIndex(*iter);
	
	NSMutableDictionary *dict = TheDelegate.m_indexMap;
	NSString *key = [NSString stringWithFormat:@"%d", pMsg->GetMsgIndex()];
	
	if([dict objectForKey:key] == nil)
		[dict setObject:[NSNumber numberWithBool:YES] forKey:key];
}

@end
