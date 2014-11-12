//
//  SMSViewCtrl.m
//  DCT
//
//  Created by Fengping Yu on 11-10-14.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "SMSViewCtrl.h"
#import "DCTAppDelegate.h"
#import "smsListCtrl.h"
#import "listData.h"
#import "treeNode.h"
#import "smsReadDlg.h"
#import "SmsEditCtrl.h"
#import "iconCollectionItem.h"
#import "utility.h"
#import "callHistoryViewController.h"
#import "smsPopUp.h"
#import "smsCocoa.h"
#import "nsstrex.h"

#pragma mark--
#pragma mark constant define
#pragma mark--

#define FROMTO  8
#define CONTENT 9
#define NO_COPY FALSE
#define STATE_NONE  0
#define STATE_OUTBOX	STO_UNSENT | STO_SENT | PC_SENT | UNKNOW
#define STATE_INBOX		REC_UNREAD | REC_READ
#define STATE_ALL		STATE_INBOX | STATE_OUTBOX
#define MAX_PCFOLDER_MSG_NUMBER		3000

static folder_state_struct folders[] = 
{
	// Type,  state   img,   subfr  modify  msgenable  save
	{TYPE_FROM, STATE_NONE,		0,0,  false,  false,  false,  false,  false},// SIM Card
    {TYPE_FROM, STATE_NONE,		3,3,  false,  false,  true,   false,  false},
	{TYPE_TO,   STATE_OUTBOX,	4,4,  false,  false,  true,   false,  true},
	{TYPE_FROM, STATE_NONE,		1,1,  false,  false,  false,  true,   false},
	{TYPE_FROM, STATE_INBOX,	3,3,  true,   false,  true,   true,   false},
	{TYPE_TO,   STATE_OUTBOX,	4,4,  false,  false,  false,  true,   true},
	{TYPE_TO,   PC_SENT,		5,5,  true,   false,  true,   true,   false},
	{TYPE_TO,   STATE_OUTBOX,	6,6,  true,   false,  true,   true,   true},
	{TYPE_BOTH, STATE_ALL,		7,7,  true,   false,  true,   true,   true},
	{TYPE_BOTH, STATE_ALL,		2,2,  true,   false,  true,   true,   false},
};

static int numbers[] = 
{
	2,  //handset
	5,  // pc
	0,  // trash
};

static NSString *tooltips[] = 
{
	@"ID_SMS_NEW",
	@"ID_SMS_REPLY",
	@"ID_SMS_FORWARD",
	@"ID_SMS_CUT",
	@"ID_SMS_COPY",
	@"ID_SMS_PASTE",
	@"ID_SMS_DELETE",
	@"ID_SMS_DOWNLOAD",
};

@implementation SMSViewCtrl

#pragma mark--
#pragma mark construct and destruct 
#pragma mark--

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
	if (self) 
	{
		// Initialization code here.
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onItemchangeListSms:) name:LVN_ITEMCHANGED object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSelChangedTreeSms:) name:SMSTREE_TVN_SELCHANGED object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSmsSendOld:) name:WM_SMS_SENDOLD object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSmsSendNew:) name:WM_SMS_SENDNEW object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSmsInsert:) name:WM_SMS_INSERT object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSmsUpdate:) name:WM_SMS_UPDATE object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onInitResult:) name:WM_SMSINIT_RESULT object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onDestroyDialog:) name:WM_SMS_DESTROYDIALOG object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onTBButtonEnabled) name:WM_TOOLBARSTATE object:nil];
		
		m_bConn = NO;
		m_bSelFolder = NO;
		m_smsEnable = NO;
		m_cutcopydeleteEnable = NO;
		m_pasteEnable = NO;
		  m_repyforwardEnable = NO;
		m_bShowDlg = NO;
		m_nEnterDlg = 0;
		
		m_bRefList = NO;
		
		m_nNewMsg = 0;
		m_numME = 0;
		m_numSIM = 0;
		m_lastSelectItem = -1;
		m_nIndex = -1;
		m_nNewMsgIndex = -1;
		
		m_hItem = [[NSTreeNode alloc] init];
		m_hTemp = [[NSTreeNode alloc] init];
		
		editorController = nil;
		readerController = nil;
		sendController = nil;
		smsReadController = nil;
        m_nNewSaveMode = FALSE;  // A new Message Sava Mode flag
        m_SavePC = FALSE ;
         m_MsgFull=TRUE;  // Message full flag
		//register URC
		pComAgent = [self getComAgentObj];
		if(pComAgent)
		{
			pComAgent->RegisterURC(URC_SYS, self, WM_URCCALLBACK);
			pComAgent->RegisterURC(URC_CMTI, self, WM_URCCALLBACK);
			pComAgent->RegisterURC(URC_CIEV, self, WM_URCCALLBACK);
		}
		
		globalData = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[globalData release];
	
	[m_hItem release];
	[m_hTemp release];
	
	[self resetNewMsgIndication];
	
	[super dealloc];
}

@synthesize globalData, smsList, smsTree, m_nEnterDlg, popupWinController, searchField;

- (void)awakeFromNib
{
	// construct the search field category menu item
	[self InitSearchMenu];
	
	[self InitToolbar];
	[self initFolder];
	
	[self initTreeCtrl];
	
	[smsList.tableView setTarget: self];
	[smsList.tableView setDoubleAction:@selector(smsOpen:)];
	
	//set init toolbar status
	[self onTBButtonEnabled];
}

#pragma mark--
#pragma mark toolbar operation
#pragma mark--

- (void)InitToolbar
{
	NSInteger count = [[toolbarMatrix cells] count];
	
	for(NSInteger i = 0; i < count; i++)
	{
		NSButtonCell *btnCell = [toolbarMatrix cellAtRow:0 column:i];
		if(btnCell)
		{
			[toolbarMatrix setToolTip:LocalizeString(tooltips[i]) forCell:btnCell];
			if(i == 0)
				[btnCell setEnabled:YES];
			else
				[btnCell setEnabled:NO];
		}
	}
}

- (void)setToolbarItem:(int)index state:(BOOL)bEnable
{
	NSButtonCell *btnCell = [toolbarMatrix cellWithTag:index];
	NSImage *image = nil;
	
	if(btnCell)
	{
		[btnCell setEnabled:bEnable];
		if(bEnable)
			image = [NSImage imageNamed:enableToolbarImages[index]];
		else 
			image = [NSImage imageNamed:disableToolbarImages[index]];
		
		[btnCell setImage:image];
		
	}
}

- (void)onTBButtonEnabled
{
	
	if(m_nEnterDlg > 0)
		return;
    
	m_repyforwardEnable = [smsList getReceived] && m_smsEnable;
    
	BOOL bEnable = [smsList getSelected];
	
	[self setToolbarItem:ID_SMS_CUT state:bEnable];
	[self setToolbarItem:ID_SMS_COPY state:bEnable];
	[self setToolbarItem:ID_SMS_DELETE state:bEnable];
	
	m_cutcopydeleteEnable = bEnable;
	
	bEnable = [smsList getReceived] && ([smsList getSelectedCount] == 1);
	
	[self setToolbarItem:ID_SMS_REPLY state:bEnable && m_smsEnable];
	[self setToolbarItem:ID_SMS_FORWARD state:bEnable && m_smsEnable];
	
    //m_repyforwardEnable = bEnable && m_smsEnable;
	
	[self setToolbarItem:ID_SMS_DOWNLOAD state:m_smsEnable];
	
	bEnable = [smsList getClipboard];
	
	[self setToolbarItem:ID_SMS_PASTE state:bEnable];
	
	m_pasteEnable = bEnable;
}

#pragma mark--
#pragma mark init sms list and tree operation
#pragma mark--

// init tree control
- (void)initTreeCtrl
{
	[[smsTree outlineView] expandItem:nil expandChildren:YES];
	[[smsTree outlineView] selectRowIndexes:[NSIndexSet indexSetWithIndex:1] byExtendingSelection:NO];
	
	//display handset inbox folder messages in right list
	[self display:(SmsFolder*)[smsTree getFolder:1]];
}

- (void)setDefaultTreeSelectItem
{
	NSTreeNode *hInbox;
	SmsFolder *pFolderInbox = NULL;
	
	[smsTree getHandsetInbox:&hInbox forFolder:&pFolderInbox];
	
	[smsList display:pFolderInbox];
	
	[[smsTree outlineView] expandItem:nil expandChildren:YES];
	[[smsTree outlineView] selectRowIndexes:[NSIndexSet indexSetWithIndex:1] byExtendingSelection:NO];
}

#pragma mark--
#pragma mark search menu
#pragma mark--

- (void)InitSearchMenu
{
	NSMenu *menu = [[[NSMenu alloc] initWithTitle:@"Search Menu"] autorelease];
	
	NSMenuItem *item;
	
	item = [[[NSMenuItem alloc] initWithTitle:LocalizeString(@"SEARCH_CATEGORY_NUMBER") action:@selector(setSearchCategoryFromTo:) keyEquivalent:@""] autorelease];
	
	[item setTarget:self];
	[item setTag: FROMTO];
	[item setState:NSOnState];
	[menu insertItem:item atIndex:0];
	
	item = [[[NSMenuItem alloc] initWithTitle:LocalizeString(@"SEARCH_CATEGORY_MESSAGE") action:@selector(setSearchCategoryContent:) keyEquivalent:@""] autorelease];
	[item setTarget:self];
	[item setTag:CONTENT];
	
	[menu insertItem:item atIndex:1];
	
	id searchCell = [searchField cell];
	[searchCell setSearchMenuTemplate:menu];
	
	searchCategory = FROMTO;
}

- (IBAction)setSearchCategoryFromTo:(NSMenuItem *)menuItem
{
	searchCategory = (int)[menuItem tag];
	
	NSMenu *menu = [menuItem menu];
	[[menu itemAtIndex:1] setState:NSOffState];
	[menuItem setState: NSOnState];
	
	[self doSearch:nil];
}

- (IBAction)setSearchCategoryContent:(NSMenuItem *)menuItem
{
	searchCategory = (int)[menuItem tag];
	
	NSMenu *menu = [menuItem menu];
	[[menu itemAtIndex:0] setState:NSOffState];
	[menuItem setState:NSOnState];
	
	[self doSearch:nil];
}

- (void)resetSearch
{
	[searchField setStringValue:@""];
	[self doSearch:nil];
	[smsList.tableView deselectAll:nil];
}

- (IBAction)doSearch:(id)sender
{
	NSPredicate *predicate = nil;
	NSString *searchString = [searchField stringValue];
	
	SmsFolder* pFolder = [smsTree getSelectedItemData];
	
	if(pFolder == NULL)
		return;
	
	int count = pFolder->GetItemSize();
	
	// restore smslist datasouce, because when using predicate to filter data, it will modify the datasouce 
	// consider add item or delete item, especially for delete item, in this case, cannot replace data souce with
	// global data, as globaldata still exist the remove item.
	/*   if ([globalData count] >= [smsList.dataSource count] && [globalData count] != 0 && [globalData count] == count)
	 {
	 [smsList.dataSource removeAllObjects];
	 [smsList.dataSource addObjectsFromArray:globalData];
	 }*/
	
	if([smsList.dataSource count] != count)
	{
		[smsList display:pFolder];
	}
	
	if((searchString != nil) && (![searchString isEqualToString:@""]))
	{
		if(searchCategory == FROMTO)
		{
			predicate = [NSPredicate predicateWithFormat:@"FromToCell contains %@", searchString];
		}
		else if(searchCategory == CONTENT)
		{
			predicate = [NSPredicate predicateWithFormat:@"ContentCell contains %@", searchString];
		}
		
		//back up smslist datasource
		[globalData removeAllObjects];
		[globalData addObjectsFromArray:smsList.dataSource];
		
		[smsList.dataSource filterUsingPredicate:predicate];
	}
	
	[smsList.tableView reloadData];
	
	[smsList.tableView deselectAll:nil];
	
	[smsList updateStatusBar];
}

- (BOOL)control:(NSControl*)control textView:(NSTextView *)textView doCommandBySelector:(SEL)commandSelector
{
	BOOL result = NO;
	
	if([textView respondsToSelector:commandSelector])
	{
		[textView performSelector:commandSelector withObject:nil];
		
		result = YES;
	}
	
	return result;
}

#pragma mark--
#pragma mark sms operation
#pragma mark--

- (void)enterDlg
{
	m_bShowDlg = YES;
	
	m_nEnterDlg++;
	
	[self enableAllCtrl:NO];
	
	callHistoryViewController *pWnd = [TheDelegate chController];
	
	if(pWnd)
		SendMessage(pWnd, WM_ENABLEOPERATION, OP_SMS, 0);
}

- (void)leaveDlg
{
	m_bShowDlg = NO;
	
	m_nEnterDlg--;
	
	if(m_nEnterDlg <= 0)
	{
		m_nEnterDlg = 0;
		[self enableAllCtrl:YES];
		
		callHistoryViewController *pWnd = [TheDelegate chController];
		
		if(pWnd)
			SendMessage(pWnd, WM_ENABLEOPERATION, OP_SMS, 1);
	}
}

- (void)showEditorDlg:(NSString *)strTo messageContent:(NSString *)strMsg item:(int)nItem isLast:(BOOL)bLast withMsg:(SMSMessage *)pMsg andIsReply:(BOOL)bReply
{   
	SmsFolder *pFolderInbox = NULL, *pFolderOutbox = NULL;
	NSTreeNode *hInbox, *hOutbox;
	
	[smsTree getHandsetInbox:&hInbox forFolder:&pFolderInbox];
	[smsTree getHandsetOutbox:&hOutbox forFolder:&pFolderOutbox];
	
	if(editorController == nil)
		editorController = [SmsEditCtrl alloc];
	
	if([self checkSupportSMStoInbox])
	{
		[editorController initwithAddress:strTo msgContent:strMsg inItem:nItem isLast:bLast withSmsfolder:[(DCTAppDelegate*)TheDelegate getSmsFolder]  outboxFolder:pFolderOutbox inboxFolder:pFolderInbox andPhonebookfolder:[(DCTAppDelegate*)TheDelegate getPbFolder] withMsg:pMsg replyOrNot:bReply];
	}
	else
	{
		[editorController initwithAddress:strTo msgContent:strMsg inItem:nItem isLast:bLast withSmsfolder:[(DCTAppDelegate*)TheDelegate getSmsFolder]  outboxFolder:pFolderOutbox inboxFolder:pFolderInbox andPhonebookfolder:[(DCTAppDelegate*)TheDelegate getPbFolder] withMsg:pMsg replyOrNot:NO];
	}
	
	[editorController initWithWindowNibName:@"smsEditor"];
	
	[self enterDlg];
	
	[editorController showWindow:nil];
}

- (void)showReaderDlg:(NSString *)strFrom withTime:(NSString *)strTime msgContent:(NSString *)strMsg index:(int)_index andMsg:(SMSMessage *)pMsg
{
	if(readerController == nil)
	{
		readerController = [smsReaderController alloc];
	}
	
	[readerController initWithAddress:strFrom time:strTime andMessageContent:strMsg forMsg:pMsg andIndex:_index];
	
	[readerController initWithWindowNibName:@"smsReader"];
	
	[self enterDlg];
	
	[readerController showWindow:nil];
}

- (void)showSendDlg:(SmsFolder *)pFolderOutbox
{
	NSTreeNode *hSentbox;
	SmsFolder *pFolderSentbox;
	
	if(!pFolderOutbox || [smsTree getSentbox:&hSentbox forFolder:&pFolderSentbox] == NO)
		return;
	
	m_pFolderOutbox = pFolderOutbox;
	m_pFolderSentbox = pFolderSentbox;
	
	if(sendController == nil)
		sendController = [smsSendController alloc];
	
	if([sendController initWithMsg:&m_vMsgTemp andOutboxFolder:pFolderOutbox SentFoler:pFolderSentbox] == NO)
	{
		[sendController release];
		sendController = nil;
		
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_SENDMESSAGE") forType:MB_OK];
		
		[self resetSearch];
		
		return;
	}
	
	[sendController initWithWindowNibName:@"smsSend"];
	
	[self enterDlg];
	
	[sendController showWindow:nil];
}

- (void)showReadDlg
{
	SmsFolder *pFolderInbox, *pFolderOutbox;
	SmsFolder *pFolder;
	NSTreeNode *hInbox, *hOutbox;
	
	m_vMsg.clear();
	
	if([smsTree getHandsetInbox:&hInbox forFolder:&pFolderInbox] && [smsTree getHandsetOutbox:&hOutbox forFolder:&pFolderOutbox])
	{
		pFolder = [smsTree getSelectedItemData];
		
		if(pFolder == pFolderInbox || pFolder == pFolderOutbox)
		{
			[smsList deleteAllItems];
		}
		
		pFolderInbox->DeleteAll(INCLUDE_SUBFOLDER);
		pFolderOutbox->DeleteAll(INCLUDE_SUBFOLDER);
		
		m_pFolder = pFolder;
		m_pFolderInbox = pFolderInbox;
		m_pFolderOutbox = pFolderOutbox;
		
		if(smsReadController == nil)
		{
			smsReadController = [smsReadDlg alloc];
			
			if([smsReadController init:pFolderInbox andOutbox:pFolderOutbox] == NO)
				[self leaveDlg];
		}
	}
}

#pragma mark --
#pragma mark sms toolbar operation
#pragma mark--
- (IBAction)smsNew:(id)sender
{
	NSButtonCell *cell = [toolbarMatrix cellWithTag:ID_SMS_NEW];
	if([cell isEnabled] == NO)
		return;
	
	[self showEditorDlg:nil messageContent:nil item:-1 isLast:YES withMsg:nil andIsReply:NO];
}

- (IBAction)smsOpen:(id)sender
{
	smsList.m_bOpenDlg = NO;
	
	int nItem = (int)[smsList.tableView clickedRow];
	
	if(nItem < 0)
	{
		nItem = [smsList getFirstSelected];
		
		if(nItem < 0)
			return;
	}
	
	smsList.m_bOpenDlg = YES;
	
	BOOL bReceived;
	NSString *strFrom, *strTime, *strMsg;
	
	SMSMessage *pMsg = [smsList getInfo:nItem isReceived:bReceived withAddress:&strFrom Time:&strTime msgContent:&strMsg andEMSData:NULL isConvert:NO];
	
	if([smsTree ifSentbox])
		nItem = -1;
	
	m_nIndex = nItem;
	
	if(pMsg)
	{
		if(bReceived)
			[self showReaderDlg:strFrom withTime:strTime msgContent:strMsg index:nItem andMsg:pMsg];
		else
			[self showEditorDlg:strFrom messageContent:strMsg item:nItem isLast:YES withMsg:pMsg andIsReply:NO];
	}
}

- (IBAction)smsReply:(id)sender
{
	if(m_repyforwardEnable==NO ||m_nEnterDlg > 0 || [smsList getSelectedCount] > 1)
		return;
	
	BOOL bReceived;
	NSString *strFrom, *strTime, *strMessage;
	
	SMSMessage *pMsg = [smsList getInfo:bReceived withAddress:&strFrom Time:&strTime andMsgContent:&strMessage isConvert:NO];
	
	if(pMsg)
		[self showEditorDlg:strFrom messageContent:strMessage item:-1 isLast:YES withMsg:pMsg andIsReply:YES];
}

- (IBAction)smsForward:(id)sender
{
	if(m_repyforwardEnable==NO ||m_nEnterDlg > 0 || [smsList getSelectedCount] > 1)
		return;
	
	BOOL bReceived;
	NSString *strFrom, *strTime, *strMessage;
	
	SMSMessage *pMsg = [smsList getInfo:bReceived withAddress:&strFrom Time:&strTime andMsgContent:&strMessage isConvert:NO];
	
	if(pMsg)
		[self showEditorDlg:nil messageContent:strMessage item:-1 isLast:YES withMsg:pMsg andIsReply:NO];
}

- (IBAction)smsSend:(id)sender
{
	NSTreeNode *hOutbox;
	SmsFolder *pOutboxFolder;
	NSTreeNode *hItem;
	vector<SMSMessage*>vMsg;
	
	if([smsTree getPcOutbox:&hOutbox forFolder:&pOutboxFolder])
	{
		[smsList getSelectItem:&hItem withMsg:vMsg];
		[smsList copyMessageFrom:vMsg To:m_vMsgTemp withFolder:pOutboxFolder isCopy:YES];
		
		for(int i = 0; i < m_vMsgTemp.size(); i++)
			m_vMsgTemp[i]->SetTimeStamp();
		
		[self showSendDlg:pOutboxFolder];
	}
}

- (IBAction)smsCut:(id)sender
{
	if((m_cutcopydeleteEnable == NO || m_nEnterDlg > 0) && [smsList.tableView clickedRow] == -1)
		return;
	
	
	[smsList cutMessage];
	
	[self onTBButtonEnabled];
}

- (IBAction)smsCopy:(id)sender
{
	if((m_cutcopydeleteEnable == NO || m_nEnterDlg > 0) && [smsList.tableView clickedRow] == -1)
		return;
    
	[smsList copyMessage];
    [self onTBButtonEnabled];
}

- (IBAction)smsPaste:(id)sender
{
   
	if(m_pasteEnable == NO || m_nEnterDlg > 0)
		return;
	
	if([smsList getClipboard] == NO)
		return;
	
	NSTreeNode *node = [smsTree getSelectedItem];
	[smsList pasteMessage:node];
	[self onTBButtonEnabled];
}

- (IBAction)smsDelete:(id)sender
{
   
	if(m_cutcopydeleteEnable == NO && [smsList.tableView clickedRow] == -1)
		return;
	
	if(m_nEnterDlg > 0)
		return;
	
     smsList.m_bOpenDlg = NO;
    if ([smsList deleteSms])    {
    
        [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_DEL_SUCCESS")forType:MB_OK];
    }
    else
        {
        
        [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_DEL_FAIL")forType:MB_OK];
        
        }
    [self onTBButtonEnabled];
    
}
- (void)updateMessage
{
    
	if(m_smsEnable == NO && m_nEnterDlg > 0)
		return;
	
	[searchField setStringValue:@""];
	
		
	NSLog(@"the download button cell is enabled.\n");
	
	[self enterDlg];
	
	smsList.m_bOpenDlg = NO;
	
	SmsFolder *pFolderInbox, *pfolderOutbox;
	SmsFolder *pFolder;
	NSTreeNode *hInbox, *hOutbox;
	
	m_vMsg.clear();
	
	if([smsTree getHandsetInbox:&hInbox forFolder:&pFolderInbox] && [smsTree getHandsetOutbox:&hOutbox forFolder:&pfolderOutbox])
        {
		pFolder = [smsTree getSelectedItemData];
		
		if((pFolder == pFolderInbox) || (pFolder == pfolderOutbox))
            {
                 [smsList deleteAllItems];
                 [self update];
            }
		
            pFolderInbox->DeleteAll(INCLUDE_SUBFOLDER);
            pfolderOutbox->DeleteAll(INCLUDE_SUBFOLDER);
		
		m_pFolder = pFolder;
		m_pFolderInbox = pFolderInbox;
		m_pFolderOutbox = pfolderOutbox;
		
		if(smsReadController == nil)
			smsReadController = [smsReadDlg alloc];
		
		[smsReadController init:pFolderInbox andOutbox:pfolderOutbox];
        }





}
- (IBAction)smsDownload:(id)sender
{
	if(m_smsEnable == NO && m_nEnterDlg > 0)
		return;
	
	[searchField setStringValue:@""];
	
	NSButtonCell *cell = (NSButtonCell*)[toolbarMatrix cellWithTag:ID_SMS_DOWNLOAD];
	if([cell isEnabled] == NO)
		return;
	
	NSLog(@"the download button cell is enabled.\n");
	
	[self enterDlg];
	
	smsList.m_bOpenDlg = NO;
	
	SmsFolder *pFolderInbox, *pfolderOutbox;
	SmsFolder *pFolder;
	NSTreeNode *hInbox, *hOutbox;
	
	m_vMsg.clear();
	
	if([smsTree getHandsetInbox:&hInbox forFolder:&pFolderInbox] && [smsTree getHandsetOutbox:&hOutbox forFolder:&pfolderOutbox])
	{
		pFolder = [smsTree getSelectedItemData];
		
		if((pFolder == pFolderInbox) || (pFolder == pfolderOutbox))
		{
			[smsList deleteAllItems];
			[self update];
		}
		
		pFolderInbox->DeleteAll(INCLUDE_SUBFOLDER);
		pfolderOutbox->DeleteAll(INCLUDE_SUBFOLDER);
		
		m_pFolder = pFolder;
		m_pFolderInbox = pFolderInbox;
		m_pFolderOutbox = pfolderOutbox;
		
		if(smsReadController == nil)
			smsReadController = [smsReadDlg alloc];
		
		[smsReadController init:pFolderInbox andOutbox:pfolderOutbox]; 
	}
}
-(void)updateFullSms
{
    
	if(m_smsEnable == NO && m_nEnterDlg > 0)
		return;
	
	[searchField setStringValue:@""];
	
	
	
	[self enterDlg];
	
	smsList.m_bOpenDlg = NO;
	
	SmsFolder *pFolderInbox, *pfolderOutbox;
	SmsFolder *pFolder;
	NSTreeNode *hInbox, *hOutbox;
	
	m_vMsg.clear();
	
	if([smsTree getHandsetInbox:&hInbox forFolder:&pFolderInbox] && [smsTree getHandsetOutbox:&hOutbox forFolder:&pfolderOutbox])
	{
		pFolder = [smsTree getSelectedItemData];
		
		if((pFolder == pFolderInbox) || (pFolder == pfolderOutbox))
		{
			[smsList deleteAllItems];
			[self update];
		}
		
		pFolderInbox->DeleteAll(INCLUDE_SUBFOLDER);
		pfolderOutbox->DeleteAll(INCLUDE_SUBFOLDER);
		
		m_pFolder = pFolder;
		m_pFolderInbox = pFolderInbox;
		m_pFolderOutbox = pfolderOutbox;
		
		if(smsReadController == nil)
			smsReadController = [smsReadDlg alloc];
		
		[smsReadController init:pFolderInbox andOutbox:pfolderOutbox];
	}




}

- (IBAction)smsCall:(id)sender 
{
	[smsList smsCall];
}

- (IBAction)smsRestore:(id)sender 
{
	if([smsList getSelectedCount] == 0)
		return;
	
	SmsFolder *pFolderTrash, *pFolder;
	NSTreeNode *hTrash, *hFolderItem;
	
	if([smsTree getTrashCan:&hTrash forFolder:&pFolderTrash] == NO)
		return;
	
	NSTreeNode *hItem;
	std::vector<SMSMessage*>vec, vecHandsetInbox;
	
	[smsList getSelectItem:&hItem withMsg:vec];
	
	eDelMsgOrigin eo;
	
	for(int i = 0; i <  vec.size(); i++)
	{
		eo = vec[i]->GetMsgDeleteOrigin();
		
		switch (eo)
		{
			case SMS_DELETE_FROM_HANDSET_INBOX:
				vecHandsetInbox.push_back(vec[i]);
				
				break;
				
			case SMS_DELETE_FROM_HANDSET_OUTBOX:
				vecHandsetInbox.push_back(vec[i]);
				
				break;
				
			case SMS_DELETE_FROM_PC_INBOX:
				if([self isPCFolderFull] == YES)
				{
					[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_PCFOLDER_FULL") forType:MB_OK];
					return;
				}
				
				[smsTree getPcInbox:&hFolderItem forFolder:&pFolder];
				vec[i]->SetMsgDeleteOrigin(SMS_DELETE_FROM_NONE);
				
				[smsList moveItemFrom:pFolderTrash to:pFolder withMsg:vec index:i isCheck:NO];
				
				break;
				
			case SMS_DELETE_FROM_PC_OUTBOX:
				if([self isPCFolderFull] == YES)
				{
					[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_PCFOLDER_FULL") forType:MB_OK];
					return;
				}
				
				[smsTree getPcOutbox:&hFolderItem forFolder:&pFolder];
				
				vec[i]->SetMsgDeleteOrigin(SMS_DELETE_FROM_NONE);
				
				[smsList moveItemFrom:pFolderTrash to:pFolder withMsg:vec index:i isCheck:NO];
				
				break;
				
			case SMS_DELETE_FROM_SENTBOX:
				if([self isPCFolderFull] == YES)
				{
					[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_PCFOLDER_FULL") forType:MB_OK];
					return;
				}
				
				[smsTree getSentbox:&hFolderItem forFolder:&pFolder];
				vec[i]->SetMsgDeleteOrigin(SMS_DELETE_FROM_NONE);
				
				[smsList moveItemFrom:pFolderTrash to:pFolder withMsg:vec index:i isCheck:NO];
				
				break;
				
			case SMS_DELETE_FROM_PC_DRAFT:
				if([self isPCFolderFull] == YES)
				{
					[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_PCFOLDER_FULL") forType:MB_OK];
					return;
				}
				
				[smsTree getPcDraft:&hFolderItem forFolder:&pFolder];
				vec[i]->SetMsgDeleteOrigin(SMS_DELETE_FROM_NONE);
				
				[smsList moveItemFrom:pFolderTrash to:pFolder withMsg:vec index:i isCheck:NO];
				
				break;
				
			case SMS_DELETE_FROM_PERSONALFOLDER:
				if([self isPCFolderFull] == YES)
				{
					[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_PCFOLDER_FULL") forType:MB_OK];
					return;
				}
				
				[smsTree getPersonal:&hFolderItem forFolder:&pFolder];
				vec[i]->SetMsgDeleteOrigin(SMS_DELETE_FROM_NONE);
				
				[smsList moveItemFrom:pFolderTrash to:pFolder withMsg:vec index:i isCheck:NO];
				
				break;
				
			default:
				break;
		}
	}
	
	if(vecHandsetInbox.size() > 0)
	{
		[smsTree getHandsetInbox:&hFolderItem forFolder:&pFolder];
		
		[smsList moveMessageFrom:hTrash To:hFolderItem forMessages:vecHandsetInbox whetherRestore:YES];
	}
}

#pragma mark--
#pragma mark menu and toolbar 
#pragma mark--

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	int tag = (int)[menuItem tag];
	int nItem;
	
	NSUInteger clickRow = [smsList.tableView clickedRow];
	NSIndexSet *set = [smsList.tableView selectedRowIndexes];
	
	if(tag == ID_SMS_OPEN || tag == ID_EDIT_CALL)
	{
		if( (clickRow != NSUIntegerMax && !([set containsIndex:clickRow] && [set count] > 1)) || 
			(clickRow == NSUIntegerMax && [set count] == 1))
		{
			return YES;
		}
		else
			return NO;
	}
	else if(tag == ID_SMS_SEND)
	{
		if((clickRow == NSUIntegerMax && [set count] != 1)
							|| ([set containsIndex:clickRow] && [set count] > 1) 
							|| [self getSimStatus] == NO 
							||[smsList getOneReceived: nItem isReceived:NO] == NO)
			return NO;
		else
			return YES;
	}
	else if(tag == ID_EDIT_CUT || tag == ID_EDIT_COPY || tag == ID_EDIT_DELETE)
	{
		if(clickRow == NSUIntegerMax && [set count] < 1)
			return NO;
		else
		{
			return YES;
		}
		
	}
	else if(tag == ID_EDIT_PASTE)
	{
		if([smsList getClipboard] == NO || m_nEnterDlg > 0)
			return NO;
		else
		{
			return YES;
		}
	}
	else if(tag == ID_EDIT_RESTORE)
	{
		if((clickRow == NSUIntegerMax && [set count] < 1) || [smsTree ifTrashCan] == NO)
			return NO;
	}
	else if(tag == FROMTO || tag == CONTENT)
	{
		if([smsList getItemCount] > 0 || [globalData count] > 0)
			return YES;
		else
			return NO;
	}
	
	return YES;
}

-(void)enableAllCtrl:(BOOL)bEnable
{
	[smsTree.outlineView setEnabled:bEnable];
	[smsList.tableView setEnabled:bEnable];
	
	[smsList.tableView deselectAll:nil];
	int count = [smsList getSelectedCount];
	
	for (NSInteger i = 0; i < [listMenu numberOfItems]; i++)
	{
		[[listMenu itemAtIndex:i] setEnabled:bEnable];
		
		if(count == 0 && i != 5)
			[[listMenu itemAtIndex:i] setEnabled:NO];
		
		if( i == 5 && [smsList getClipboard] == NO)
			[[listMenu itemAtIndex:5] setEnabled:NO];
	}
	
	for(NSInteger k = 0; k < [smsTree.outlineView.menu numberOfItems]; k++)
		[[smsTree.outlineView.menu itemAtIndex:k] setEnabled: bEnable];
	
	// toolbar button
	NSButtonCell *btnCell = [toolbarMatrix cellWithTag:ID_SMS_NEW];
	[btnCell setEnabled:bEnable];
	NSImage *image = bEnable? [NSImage imageNamed:enableToolbarImages[ID_SMS_NEW]] : [NSImage imageNamed:disableToolbarImages[ID_SMS_NEW]];
	[btnCell setImage:image];
	
	for(NSInteger i = ID_SMS_REPLY; i <= ID_SMS_DOWNLOAD;  i++)
	{
		btnCell = [toolbarMatrix cellWithTag:i];
		image = bEnable? [NSImage imageNamed:enableToolbarImages[i]] : [NSImage imageNamed:disableToolbarImages[i]];
		[btnCell setImage:image];
		[btnCell setEnabled:bEnable];
		
		if(count == 0 && i != ID_SMS_PASTE && i != ID_SMS_DOWNLOAD)
		{
			[btnCell setImage:[NSImage imageNamed:disableToolbarImages[i]]];
			[btnCell setEnabled:NO];
		}
		
		if( i == ID_SMS_PASTE && [smsList getClipboard] == NO)
		{
			[btnCell setImage:[NSImage imageNamed:disableToolbarImages[ID_SMS_PASTE]]];
			[btnCell setEnabled:NO];
		}
		
		if(i == ID_SMS_DOWNLOAD && m_smsEnable == NO)
		{
			[btnCell setImage:[NSImage imageNamed:disableToolbarImages[ID_SMS_DOWNLOAD]]];
			[btnCell setEnabled:NO];
		}
	}
	
	[searchField setEnabled:bEnable];
}

#pragma mark -- 
#pragma mark notification message
#pragma mark--

- (void)onItemchangeListSms:(NSNotification *)notification
{
	NSDictionary *userInfo = [notification userInfo];
	
	if(userInfo == nil)
	{
		[number setStringValue:@""];
		[time setStringValue:@""];
		[content setString:@""];
	}
	else
	{
		[number setStringValue:[userInfo objectForKey:FROMTOCELL]];
		[time setStringValue:[userInfo objectForKey:TIMECELL]];
		[content setString:[userInfo objectForKey:CONTENTCELL]];
	}
	
	
	[self updateListItem];
	[self onTBButtonEnabled];
}

- (void)onSelChangedTreeSms:(NSNotification*)notification
{
	if([[smsTree.outlineView selectedRowIndexes] count] == 1)
	{
		NSTreeNode *node = [smsTree.outlineView itemAtRow:[smsTree.outlineView selectedRow]];
		treeNode *data = [node representedObject];
		
		SmsFolder *pFolder = (SmsFolder*)[data getTag];
		
		if(pFolder == NULL)
		{
			pFolder = [smsTree getFolderFromName:data.name];
		}
		
		if(pFolder)
		{
			CString folderName;
			pFolder->GetFolderName(folderName);
			
			NSString *name = [NSString getStringFromWchar:folderName]; 
			
			if([name caseInsensitiveCompare:LocalizeString(@"IDS_SMS_FOLDER0")] == NSOrderedSame || 
			   [name caseInsensitiveCompare:LocalizeString(@"IDS_SMS_FOLDER3")] == NSOrderedSame)
				m_bSelFolder = YES;
			else
				m_bSelFolder = NO;
			
			[content setSelectedRange:NSMakeRange(0, 0)];
			
			[self display:pFolder];
		}
		
		m_lastSelectItem = [smsTree.outlineView selectedRow];
		
		[searchField setStringValue:@""];
		
		[globalData removeAllObjects];
		[globalData addObjectsFromArray:smsList.dataSource];
		
		[smsList updateStatusBar];
		
		[self onTBButtonEnabled];
		
		[smsList.tableView becomeFirstResponder];
		[smsTree.outlineView resignFirstResponder];
	}
}

- (void)onSmsSendNew:(NSNotification *)notification
{
	NSDictionary *dict = [notification userInfo];
	
	SMSMessage *pMsg;
	classToObject *object = [dict objectForKey:ITEM_MESSAGE];
	pMsg = object->pMsg;
	
	NSTreeNode *hOutbox;
	SmsFolder *pFolderOutbox;
	
	if ([smsTree getPcOutbox:&hOutbox forFolder:&pFolderOutbox])
	{
		std::vector<SMSMessage*> vMsg;
		if([smsList insertMessage:pFolderOutbox withMessage:pMsg andMsgVector:m_vMsgTemp])
		{
			[self showSendDlg:pFolderOutbox];
			
			return;
		}
	}
	else
	{
		delete pMsg;
	}
	
	[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
					withMsg:LocalizeString(@"IDS_ERROR_SAVEMESSAGE")
					forType:MB_OK];
}

- (void)onSmsSendOld:(NSNotification *)notification
{
	NSDictionary *dict = [notification userInfo];
	int nItem = (int)[[dict objectForKey:ITEM_INDEX] intValue];
	
	SMSMessage *pMsg;
	classToObject *object = [dict objectForKey:ITEM_MESSAGE];
	pMsg = object->pMsg;
	
	NSTreeNode *hOutbox;
	SmsFolder *pFolderOutbox;
	std::vector<SMSMessage*>vMsg;
	
	if([smsTree getPcOutbox:&hOutbox forFolder:&pFolderOutbox])
	{
		if([smsTree ifHandsetOutbox])
		{
			pMsg->SetMemType(_T("PC"));
			pFolderOutbox->InsertItem(pMsg);
			
			m_vMsgTemp.clear();
			m_vMsgTemp.push_back(pMsg);
			[self showSendDlg:pFolderOutbox];
			return;
		}
		
		if([smsList updateMessage:nItem withMessage:pMsg andMsgVector:vMsg])
		{
			[smsList copyMessageFrom:vMsg To:m_vMsgTemp withFolder:pFolderOutbox isCopy:YES];
			
			[self showSendDlg:pFolderOutbox];
			return;
		}
	}
	else
	{
		delete pMsg;
		
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
						withMsg:LocalizeString(@"IDS_ERROR_SAVEMESSAGE")
						forType:MB_OK];
	}
}

- (void)onSmsInsert:(NSNotification *)notification
{
	NSDictionary *dict = [notification userInfo];
	SmsFolder *pFolder;
	SMSMessage *pMsg;
	
	classToObject *object = [dict objectForKey:ITEM_FOLDER];
	
	pFolder = object->pFolder;
	
	object = [dict objectForKey:ITEM_MESSAGE];
	
	pMsg = object->pMsg;
	
	[smsList insertMessage:pFolder withMessage:pMsg andMsgVector:m_vMsgTemp];
}

- (void)onSmsUpdate:(NSNotification *)notification
{
	NSDictionary *dict = [notification userInfo];
	
	int nItem = [[dict objectForKey:ITEM_INDEX] intValue];
	classToObject *msgObject = [dict objectForKey:ITEM_MESSAGE];
	
	SMSMessage *pMsg = msgObject->pMsg;
	
	[smsList updateMessage:nItem withMessage:pMsg andMsgVector:m_vMsgTemp];
}

- (void)parseSystemStatus:(CUrcSys *)pUrc
{
	if(pUrc == NULL)
		return;
	
	if(pUrc->Value().alpha >= SYS_PINREADY)
	{
		if(!m_bConn && pComAgent)
		{
			pComAgent->RegisterURC(URC_SMS, self, WM_URCCALLBACK);
		}
		m_bConn = YES;
		
		[self clearClipboard];
	}
	else
	{
		if(m_bConn && pComAgent)
		{
			pComAgent->DeregisterURC(URC_SMS, self);
		}
		
		m_bConn = NO;
		m_smsEnable = NO;
		[self onTBButtonEnabled];
		m_nNewMsg = 0;
	}
}

- (void)onInitResult:(NSDictionary *)dict
{
	int ret = [[dict objectForKey:ITEM_NUMBER] intValue];
	
	switch (ret)
	{
		case INIT_SUCCESS:
			if(smsReadController == nil)
				smsReadController = [smsReadDlg alloc];
			
			[smsReadController initWithWindowNibName:@"smsRead"];
			
			if(TheDelegate.currentView == SMSVIEW)
			{
				[smsReadController showWindow: self];
				[[smsReadController window] orderFrontRegardless];
			}
			else
				[[smsReadController window] orderOut:nil];
			
			break;
			
		case INIT_FAILED:
			[smsReadController release];
			smsReadController = nil;
			
			[self leaveDlg];
			
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_READMESSAGE")
							forType:MB_OK];
			
			break;
			
		default:
			break;
	}
}

- (void)onDestroyDialog:(NSNotification*)notification
{
	NSDictionary *dict = [notification userInfo];
	
	NSInteger dlg = [[dict objectForKey:ITEM_MSGID] intValue];
	
	switch (dlg)
	{
		case SMS_DLG_EDITOR:
			[self closeSmsEditorDlg];
			[self resetSearch];
			break;
			
		case SMS_DLG_READ:
			[self closeSmsReadDlg];
			[self resetSearch];
			break;
			
		case SMS_DLG_READER:
			[self closeReaderDlg];
			[self resetSearch];
			break;
			
		case SMS_DLG_SEND:
			[self closeSendDlg];
			[self resetSearch];
			break;
			
		default:
			break;
	}
}

- (void)onURCCallBack:(long)wParam lParam:(long)lparam
{
	WATCH_POINTER(lparam);
	
	switch (wParam)
	{
		case URC_SYS:
		{
			CUrcSys *pUrc = (CUrcSys*)lparam;
			[self parseSystemStatus:pUrc];
			break;
		}
			
		case URC_SMS:
		{
			CUrcFlag *pUrc = (CUrcFlag*)lparam;
			m_smsEnable = pUrc->Value();
			//[self onTBButtonEnabled];
			if (m_smsEnable)
			{
				CComAgent *pCom = [self getComAgentObj];
				pCom->ConfigSMS(self, WM_RSPCALLBACK, RESP_SMS_CONFIG);
			}
			
			break;
		}
			
		case URC_CMTI:
		{
			CUrcMsg *pUrc = (CUrcMsg*)lparam;
			const CMsgInd &ind = pUrc->Value();
			
			newMsgInd *indication = new newMsgInd;
			
			indication->strMem = ind.mem;
			indication->index = ind.index;
			
			m_vNewMsgInd.push_back(indication);
			
			if(m_smsEnable == NO)
				return;
			
			if(smsReadController)
				[smsReadController setRcvMsgFlag:pUrc->Value()];
			
			m_nNewMsg++;
			
			[self downloadNewMsg];
			
			break;
		}
		
		case URC_CIEV:
			{
				CUrcMsgStatus *pUrc = (CUrcMsgStatus*)lparam;

				const CMsgStatusInd &pInd = pUrc->Value();

				if(pInd.value.Compare(_T("3")) == 0 &&   m_MsgFull)
				{
					
					
					[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_MEMORYFULL") forType:MB_OK];
					m_MsgFull=FALSE;
				}
				else if(pInd.value.Compare(_T("0")) == 0)
				{
					m_MsgFull=TRUE;
				}
			}
			break;
			
		default:
			break;
	}
}

- (void)onRspCallBack:(long)wParam lParam:(long)lparam
{
	WATCH_POINTER(lparam);
	
	if(LOWORD(wParam) == RESP_SMS_READ)
	{
		CRespFlag *pResp = (CRespFlag*)lparam;
		
		if(pResp->Value())
		{
			m_nReadItem++;
			[self onJobStatus:YES withIndex:m_nNewMsgIndex];
		}
		else
		{
			[self onJobStatus:NO withIndex:m_nNewMsgIndex];
		}
		
		return;
	}
	
	CRespArray *pResp = (CRespArray*)lparam;
	CString str;
	
	switch (LOWORD(wParam))
	{
		case RESP_SMS_GETTOTALNUM:
			if(HIWORD(wParam) == ER_OK)
			{
				if(pResp->m_lExt == READ_SM)
				{
					[smsReadController setTotalNumber:YES 
											   memory:LocalizeString(@"SM") 
										   withVector:pResp->Value()];
					m_numSIM = pResp->Value()[2] - pResp->Value()[0] ;
                     NSLog(@"m_numSIM:%d",m_numSIM);
				}
				else if(pResp->m_lExt == READ_ME)
				{
					[smsReadController setTotalNumber:YES 
											   memory:LocalizeString(@"ME") 
										   withVector:pResp->Value()];
					m_numME = pResp->Value()[2] - pResp->Value()[0] ;
                    NSLog(@"m_numME:%d",m_numME);
				}
				else if(pResp->m_lExt == READ_RET)
				{
					NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithObject:[NSNumber numberWithInt:INIT_SUCCESS] forKey:ITEM_NUMBER];
					[dict setObject:WM_SMSINIT_RESULT forKey:ITEM_MSGID];
					
					[self performSelectorOnMainThread:@selector(onInitResult:) withObject:dict waitUntilDone:NO];
				}
                
                 [smsList updateStatusBar];
			}
			else
			{
				std::vector<int> vec;
				for(int i = 0; i < 3; i ++)
					vec.push_back(0);
				
				if(pResp->m_lExt == READ_SM)
					[smsReadController setTotalNumber:NO memory:LocalizeString(@"SM") withVector:vec];
				else if(pResp->m_lExt == READ_ME)
					[smsReadController setTotalNumber:NO memory:LocalizeString(@"ME") withVector:vec];
				else if(pResp->m_lExt == READ_RET)
				{
					NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithObject:[NSNumber numberWithInt:INIT_FAILED] forKey:ITEM_NUMBER];
					[self performSelectorOnMainThread:@selector(onInitResult:) withObject:dict waitUntilDone:NO];
				}
                
                [smsList updateStatusBar];
			}
			
			break;
			
		case RESP_SMS_CONFIG:
		{
			CComAgent *pcom = [self getComAgentObj];
			pcom->QueryIMSI(self, WM_RSPCALLBACK, RESP_SMS_IMSI);
		}
			
			break;
			
		case RESP_SMS_IMSI:
		{
			if(ER_OK == HIWORD(wParam) && 0 !=  lparam)
			{
				CRespText *p = (CRespText*)lparam;
				DCTAppDelegate *delegate = TheDelegate;
				
				NSMutableDictionary *dict = delegate.m_indexMap;
				
				NSString *imsi = [dict objectForKey:IMSIKEY];
				
				if(imsi == nil || [NSString getCStringFromNSString:imsi].Compare(p->Value()) != 0)
				{
					[dict removeAllObjects];
					
					CString str = p->Value();
					wchar_t *chars = str.GetBuffer(str.GetLength());					
					NSString *value = [NSString getStringFromWchar:chars];
					
					[dict setObject:value forKey:IMSIKEY];
				}
			}
		}
			NSLog(@"response sms config message.\n"); 
			
			if([smsList getsmsJobThreadDlg] != nil)
				return;
			;
			[self reInitFolder];
			
			if([utility getModalCounter] == 0)
			{
				if(editorController)
				{
					[editorController setModify:NO];
					[editorController.window performClose:nil];
				}
				
				if(readerController)
					[readerController.window performClose:nil];
				
				if(sendController)
					[sendController.window performClose:nil];
				
				[self showReadDlg];
			}
			break;
			
		default:
			break;
	}
}

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	if(Msg == WM_URCCALLBACK)
	{
		[self onURCCallBack:wparam lParam:lparam];
	}
	else if(Msg == WM_RSPCALLBACK)
	{
		[self onRspCallBack:wparam lParam:lparam];
	}
}

#pragma mark--
#pragma mark new message download
#pragma mark--

- (void)updateMessageStatusInex:(SMSMessage*)pMsg withItem:(int)nItem
{
	DCTAppDelegate *delegate = TheDelegate;
	
	eMemStatus status = pMsg->GetStatus();
	//	pMsg->SetMsgIndex(nItem);
	
	NSMutableDictionary *dict = delegate.m_indexMap;
	
	NSString *key = [NSString stringWithFormat:@"%d", nItem];
	
	NSNumber *result = [dict objectForKey:key];
	
	NSUInteger count = [dict count];
	
	if(result != nil && count > 0)
	{
		if([result boolValue] == YES)
			pMsg->SetStatus(REC_READ);
		else if([result boolValue] == NO)
			pMsg->SetStatus(REC_UNREAD);
	}
	else
	{
		if(status == REC_READ)
			[dict setObject:[NSNumber numberWithBool:YES] forKey: key];
		else if(status == REC_UNREAD)
			[dict setObject:[NSNumber numberWithBool:NO] forKey: key];
	}
}

- (void)showNewMessage
{
	DCTAppDelegate *delegate = TheDelegate;
	
	[utility processSMSFolder:m_pNewMsgFolder];
	
	NSTreeNode *hInbox;
	SmsFolder *pFolderInbox;
	
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSNumber *value = (NSNumber*)[defaults valueForKey:SHOWNOTIFICATION];
	if((value != nil && [value integerValue] == 1) || value == nil)
	{
		[self showPopupView:m_pNewMsgFolder];
	}
	
	value = (NSNumber*)[defaults valueForKey:PLAYSOUND];
	
	if((value != nil && [value integerValue] == 1) || value == nil)
	{
		[self startRing];
	}
	

    if(m_nNewSaveMode)
    {
        [smsTree getPcInbox:&hInbox forFolder:&pFolderInbox];
        m_SavePC = TRUE;
    }
    else
    {
        [smsTree getHandsetInbox:&hInbox forFolder:&pFolderInbox];
        m_SavePC = FALSE;
    }
   	[self displayNewMsginFolder:m_pNewMsgFolder andInserttoFolder:pFolderInbox];
	
	if(delegate.currentView == SMSVIEW && m_SavePC)
        {
		[[smsTree outlineView] expandItem:nil expandChildren:YES];
		[[smsTree outlineView] selectRowIndexes:[NSIndexSet indexSetWithIndex:4] byExtendingSelection:NO];
		
		[smsList updateStatusBar];
        }
    
	if(delegate.currentView == SMSVIEW &&  !m_SavePC)
        {
		[[smsTree outlineView] expandItem:nil expandChildren:YES];
		[[smsTree outlineView] selectRowIndexes:[NSIndexSet indexSetWithIndex:1] byExtendingSelection:NO];
		
		[smsList updateStatusBar];
        }

	
        //NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
        //NSNumber *value = (NSNumber*)[defaults valueForKey:SHOWNOTIFICATION];
	
		if(m_pNewMsgFolder)
	{
		delete m_pNewMsgFolder;
		
		m_pNewMsgFolder = NULL;
	}
}

- (void)showPopupView:(SmsFolder*)pFolder
{
	NSRect visibleFrame = [[NSScreen mainScreen] visibleFrame];
	
	SMSMessage *pMsg = NULL;
	ItemPos pos = pFolder->GetFirstItemPos();
	CString strNumber;
	NSString *strContent = nil, *strAddress = nil;
    NSString *strName=nil;
	
	while(pos != 0)
	{
		pMsg = pFolder->GetNextItem(pos);
		
		pMsg->GetAddressNumber(strNumber);
        strName = [TheDelegate convertNumber:strNumber];
        if(strName == nil)
            strName = [NSString CStringToNSString:(LPCTSTR)strNumber];
		EMSData *pEmsData;
		pMsg->GetMsg(&pEmsData);
		strContent = [utility getEmsText:pEmsData];
        strAddress=strName;
	}
	
	pFolder->CloseNextItem(pos);
	
	popupWinController = nil;
	
	if(popupWinController == nil)
	{
		popupWinController = [[[smsPopUp alloc] initWithWindowNibName:@"smsPopUp"] autorelease];
	}
	
	if(strContent == nil)
		strContent = @"";
	
	if(strAddress == nil)
		strAddress = @"";
	
	[popupWinController initwithAddress:strAddress andContent:strContent];
	
	NSPoint origin = NSMakePoint(visibleFrame.size.width * 0.6, 418);
	[popupWinController.window setFrameOrigin:origin];
	
	[popupWinController.window orderFront:self];
	
	[popupWinController showWindow:nil];
	
	[popupWinController startTimer];
}

- (void)startRing
{
	NSSound* sound = nil;
	
	NSString *path = [[NSBundle mainBundle] pathForResource:@"message" ofType:@"mp3"];
	
	NSLog(@"%@\n", path);
	
	sound = [[[NSSound alloc] initWithContentsOfFile:path 
										 byReference:NO] retain];
	
	if(sound != nil)
		[sound play];
	
	[sound release];
}

- (void)stopRing
{
	// todo
}

- (void)downloadNewMsg
{
	 m_nItem = 0;
   
	[self DoJob];
}

- (void)DoJob
{
	if(m_nItem >= m_vNewMsgInd.size())
    {
		NSLog(@"Error, crash!");
    }
	
	CString memory = m_vNewMsgInd[m_nItem]->strMem;
	int index = m_vNewMsgInd[m_nItem]->index;
	m_nNewMsgIndex = index;
	std::vector<newMsgInd*>::iterator iter = m_vNewMsgInd.begin();
	m_vNewMsgInd.erase(iter);
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSNumber *value = (NSNumber*)[defaults valueForKey:SMSSAVE];
	
    if((value != nil && [value integerValue] == 1) )
    {
          m_nNewSaveMode=TRUE;
        if(CA_OK != pComAgent->ReadSMS(self, WM_RSPCALLBACK, RESP_SMS_READ, &m_Msg, memory, index, (long)&m_Msg))
        {
            [self onJobStatus:NO withIndex:index];
        }
       
        if(CA_OK == pComAgent->EraseSMS(self, WM_SMSDELETEMOVE, 1, &m_Msg, (long)&m_Msg))
        {   
            
            [smsTree setSelectedItemWithIndex:4];
            m_MsgFull=FALSE;
            
        }
        pComAgent = [TheDelegate getComAgent];
        SMSViewCtrl *smsView = [TheDelegate getSmsViewController];
        pComAgent->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "SM", READ_SM);
        pComAgent->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "ME",READ_ME);
    }
    else
    {
        
        m_nNewSaveMode=FALSE;
        /***********************Update   New Message ***************************/
        pComAgent = [TheDelegate getComAgent];
        SMSViewCtrl *smsView = [TheDelegate getSmsViewController];
        pComAgent->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "SM", READ_SM);
        pComAgent->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "ME",READ_ME);
        /***********************Update hua   New Message ***************************/
        
        int m_numSIM1=[(DCTAppDelegate*)TheDelegate smsController]->m_numSIM;
        int m_numME1=[(DCTAppDelegate*)TheDelegate smsController]->m_numME;
        if ((m_numSIM1+m_numME1)==99 && m_MsgFull)
        {
            if(CA_OK != pComAgent->ReadSMS(self, WM_RSPCALLBACK, RESP_SMS_READ, &m_Msg, memory, index, (long)&m_Msg))
            {
                [self onJobStatus:NO withIndex:index];
            }
            
            [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_MEMORYFULL") forType:MB_OK];
            if([utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_MESSAGE_DELETE") forType:MB_YESNO] == NSAlertDefaultReturn)
            {
                pComAgent = [TheDelegate getComAgent];
                pComAgent->EraseSMS(self, WM_SMSDELETEMOVE, 1, &m_Msg, (long)&m_Msg);
                SMSViewCtrl *smsView = [TheDelegate getSmsViewController];
                pComAgent->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "SM", READ_SM);
                pComAgent->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "ME",READ_ME);
                [smsTree setSelectedItemWithIndex:1];
                [self updateFullSms];
                [smsList updateStatusBar];
                 m_MsgFull=FALSE;
            }
            else
            {
                [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_DEL_MEMORYFULL") forType:MB_OK];
                
               // m_nNewSaveMode=FALSE;
                [smsTree setSelectedItemWithIndex:1];
                m_MsgFull=TRUE;
            }
            
            
        }
        else
        {
            if(CA_OK != pComAgent->ReadSMS(self, WM_RSPCALLBACK, RESP_SMS_READ, &m_Msg, memory, index, (long)&m_Msg))
            {
                [self onJobStatus:NO withIndex:index];
            }
            
            m_nNewSaveMode=FALSE;
            [smsTree setSelectedItemWithIndex:1];
            m_MsgFull=TRUE;
            
            
        }
        
    }
    

}

- (void)onJobStatus:(signed char)result withIndex:(int)index
{
	if(result == YES)
	{
		if(m_pNewMsgFolder == NULL)
			m_pNewMsgFolder = new SmsFolder;
		
		m_Msg.SetMsgIndex(index);
		
		[self updateMessageStatusInex:&m_Msg withItem:index];
		
		[self insertMessage:m_pNewMsgFolder];
	}
}

- (void)insertMessage:(SmsFolder *)pFolder
{
	SMSMessage *pMsg = [utility newMessage];
	
	if(pMsg && pFolder)
	{
		*pMsg = m_Msg;
		
		pFolder->InsertItem(pMsg);
		
		long cs = 0;
		pMsg->GetConcatenateIDbuf(cs);
		
		char *pc = (char*)&cs;
		
		if(pc[2] > pc[3])
		{
			NSLog(@"message not complete yet.\n");
		}
		else
		{
			pFolder->ProcessSMSItems();
			
			[TheDelegate newMessageNotification];
		}
	}
}

- (void)displayNewMsginFolder:(SmsFolder *)pFrom andInserttoFolder:(SmsFolder *)pFolder
{
	SMSMessage *pMsg = NULL;
	
	ItemPos pos = pFrom->GetFirstItemPos();
	
	while(pos != 0)
	{
		pMsg = pFrom->GetNextItem(pos);
       if(m_SavePC)
        {
          pMsg->SetMemType(_T("PC"));//  A new Message save in PC
        }
		[smsList insertItem:pMsg withIndex:-1];
		
		pFolder->InsertItem(pMsg);
	}
	
	pFrom->CloseNextItem(pos);
	
	[smsList.tableView reloadData];
	
	//   [smsList.tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:[smsList.tableView numberOfRows] - 1]byExtendingSelection:NO];
}

#pragma mark--
#pragma mark get and set function
#pragma mark--

- (int)getSelectIndex
{
	return m_nIndex;
}

- (BOOL)isPCFolderFull
{
	return [self getPCSMSCount] > MAX_PCFOLDER_MSG_NUMBER ? YES : NO; 
}

- (int)getPCSMSCount
{
	NSTreeNode *hItem;
	SmsFolder *pFolder;
	int result = 0;
	
	[smsTree getPcRoot:&hItem forFolder:&pFolder];
	
	if(pFolder)
	{
		SmsFolder *pSubFolder;
		SubfolderPos pos = pFolder->GetFirstSubfolderPos();
		
		while (pos != 0)
		{
			pSubFolder = pFolder->GetNextSubfolder(pos);
			if(pSubFolder)
				result += pSubFolder->GetItemSize();
		}
		
		pFolder->CloseNextSubfolder(pos);
	}
	
	return result;
}

- (BOOL)isCopy
{
	return m_bCopy;
}

- (void)setCopy:(signed char)bCopy
{
	m_bCopy = bCopy;
}

- (NSTreeNode*)getCopyItem
{
	return m_hItem;
}

- (NSTreeNode*)getDeleteItem
{
	return m_hTemp;
}

- (std::vector<SMSMessage*>&)getCopyMessage
{
	return m_vMsg;
}

- (std::vector<SMSMessage*>&)getDeleteMessage
{
	return m_vMsgTemp;
}

- (std::vector<SMSMessage*>)getMsgVector
{
	return m_vMsg;
}

- (BOOL)getSimStatus
{
	return m_smsEnable;
}

- (BOOL)getConnected
{
	DCTAppDelegate *delegate = (DCTAppDelegate*)TheDelegate;
	
	return [delegate getConnected];
}

- (BOOL)getConnStatus
{
	return m_bConn;
}

- (CComAgent*)getComAgentObj
{
	return [TheDelegate getComAgent];
}

- (int)getSysState
{
	return [self getComAgentObj]->GetSysState();
}

- (NSInteger)getLastSelectItem
{
	return m_lastSelectItem;
}

- (bool)checkSupportSMStoInbox
{
	return CComAgent::GetConnection()->CheckSupportSMStoInbox();
}

- (std::vector<newMsgInd*>)getNewMsgIndication
{
	return m_vNewMsgInd;
}

- (void)resetNewMsgIndication
{
	for(int i = 0; i < m_vNewMsgInd.size(); i++)
	{
		newMsgInd *ind = m_vNewMsgInd[i];
		delete ind;
	}
	
	m_vNewMsgInd.clear();
}

- (NSWindow*)getWindow
{
	if(editorController)
		return editorController.window;
	else if(readerController)
		return readerController.window;
	else if(sendController)
		return sendController.window;
	else if(smsReadController)
		return smsReadController.window;
	
	return nil;
}

- (void)closeWindow
{
	if(editorController)
		[self closeSmsEditorDlg];
	else if(readerController)
		[self closeReaderDlg];
	else if(smsReadController)
		[self closeSmsReadDlg];
	else if(sendController)
		[self closeSendDlg];
}

#pragma mark--
#pragma mark folder operation
#pragma mark--

- (void)setFolderName:(SmsFolder *)pFolder withIndex:(int)index
{
	if(!pFolder)
		return ;
	
	NSString *ID = [@"IDS_SMS_FOLDER" stringByAppendingString:[NSString stringWithFormat:@"%d", index]];
	
	NSString *name = [[NSBundle mainBundle] localizedStringForKey:ID value:nil table:@"InfoPlist"];
	
	char *cname = (char*)[name UTF8String];
	
	pFolder->SetFolderName(cname);
}

- (SmsFolder*)setSubFolderName:(SmsFolder *)pFolder withIndex:(int &)index andSubFolderPos:(SubfolderPos &)pos
{
	if(!pFolder || !pos)
		return NULL;
	
	SmsFolder *pSubFolder = pFolder->GetNextSubfolder(pos);
	[self setFolderName:pSubFolder withIndex:index];
	index++;
	
	return pSubFolder;
}

- (SmsFolder*) insertSubFolder:(SmsFolder *)pFolder withIndex:(int&)index
{
	if(!pFolder)
		return NULL;
	
	SmsFolder *pSubfolder = [utility newSmsFolder];
	if(!pSubfolder)
		return NULL;
	
	
	[self setFolderName:pSubfolder withIndex:index];
	
	pSubfolder->SetFolderState(folders[index++]);
	pFolder->InsertSubfolder(pSubfolder);
	
	return pSubfolder;
}

- (void)initFolder
{
	SmsFolder *p = [TheDelegate getSmsFolder];
	SmsFolder *pFolder;
	int i, j, k , num = 3;
	
    
    
	BOOL bInsertIntoInbox = [self checkSupportSMStoInbox];
	
	if(p->GetSubfolderSize() == 0)
	{
		for(i = 0, k = 0; i < num; i++)
		{
			pFolder = [self insertSubFolder:p withIndex:k];
			
			for(j = 0; j < numbers[i]; j++)
			{
				SmsFolder *pSubFolder = [self insertSubFolder:pFolder withIndex:k];
				
				if(bInsertIntoInbox)
				{
					// adjust folder state for new feature: insert into inbox
					if((k - 1== 1) || (k - 1) == 2 || (k - 1 == 4) || (k - 1 == 5) || (k - 1== 7))
					{
						folder_state_struct state;
						pSubFolder->GetFolderState(state);
						state.nState = STATE_ALL;
						if(k - 1 == 1)
						{
							state.bSave = true;
						}
						pSubFolder->SetFolderState(state);
					}
				}
				else
				{
					folder_state_struct state;
					pSubFolder->GetFolderState(state);
					switch(k - 1)
					{
						case 1:
							state.nState = STATE_NONE;
							state.bSave = false;
							break;
							
						case 2:
						case 5:
						case 7:
							state.nState = STATE_OUTBOX;
							break;
							
						case 4:
							state.nState = STATE_INBOX;
							break;
					}
					
					pSubFolder->SetFolderState(state);
				}
			}
		}
	}
	else
	{
		i = k = 0;
		
		SubfolderPos pos = p->GetFirstSubfolderPos();
		while((i < num) && (pos != 0))
		{
			pFolder = [self setSubFolderName:p withIndex:k andSubFolderPos:pos];
			
			// sub sub folder
			SubfolderPos subPos = pFolder->GetFirstSubfolderPos();
			for(j = 0; j < numbers[i] && subPos; j++)
			{
				SmsFolder *pSubFolder = [self setSubFolderName:pFolder withIndex:k andSubFolderPos:subPos];
				
				if(bInsertIntoInbox)
				{
					if((k - 1== 1) || (k - 1) == 2 || (k - 1 == 4) || (k - 1 == 5) || (k - 1== 7))
					{
						folder_state_struct state;
						pSubFolder->GetFolderState(state);
						state.nState = STATE_ALL;
						if(k - 1 == 1)
						{
							state.bSave = true;
						}
						pSubFolder->SetFolderState(state);
					}
				}
				else
				{
					folder_state_struct state;
					pSubFolder->GetFolderState(state);
					switch(k - 1)
					{
						case 1:
							state.nState = STATE_NONE;
							state.bSave = false;
							break;
							
						case 2:
						case 5:
						case 7:
							state.nState = STATE_OUTBOX;
							break;
							
						case 4:
							state.nState = STATE_INBOX;
							break;
					}
					
					pSubFolder->SetFolderState(state);
				}
			}  //end of for loop
			
			i++;
			
		} // end of while loop
	}
	
	[self display:p];
}

- (void)reInitFolder
{
	SmsFolder *p = [TheDelegate getSmsFolder];
	SmsFolder *pFolder;
	int i= 0, j, k = 0 , num = 3;
	
	BOOL bInsertIntoInbox = [self checkSupportSMStoInbox];
	
	SubfolderPos pos = p->GetFirstSubfolderPos();
	
	while(i < num && pos != 0)
	{
		pFolder = p->GetNextSubfolder(pos);
		k++;
		
		SubfolderPos subPos = pFolder->GetFirstSubfolderPos();
		for(j = 0; j < numbers[i] && subPos; j++)
		{
			SmsFolder *pSubFolder = [self setSubFolderName:pFolder withIndex:k andSubFolderPos:subPos];
			
			if(bInsertIntoInbox)
			{
				if((k - 1== 1) || (k - 1) == 2 || (k - 1 == 4) || (k - 1 == 5) || (k - 1== 7))
				{
					folder_state_struct state;
					pSubFolder->GetFolderState(state);
					state.nState = STATE_ALL;
					if(k - 1 == 1)
					{
						state.bSave = true;
					}
					pSubFolder->SetFolderState(state);
				}
			}
			else
			{
				folder_state_struct state;
				pSubFolder->GetFolderState(state);
				switch(k - 1)
				{
					case 1:
						state.nState = STATE_NONE;
						state.bSave = false;
						break;
						
					case 2:
					case 5:
					case 7:
						state.nState = STATE_OUTBOX;
						break;
						
					case 4:
						state.nState = STATE_INBOX;
						break;
				}
				
				pSubFolder->SetFolderState(state);
			}
		}
		i++;
	}
}

#pragma mark--
#pragma mark display operation
#pragma mark--

- (void)resetEditCtrl
{
	[self setEditCtrl:@"" time:@"" message:@"" andEmsData:NULL];
}

- (void)display:(SmsFolder *)folder
{
	[smsList display:folder];
	
	if([smsList getItemCount] == 0)
		[self resetEditCtrl];
}

- (void)setEditCtrl:(NSString *)strFrom time:(NSString *)strTime message:(NSString *)strMessage andEmsData:(EMSData *)pEmsData
{
	[number setStringValue:strFrom];
	[time setStringValue:strTime];
	
	[content setSelectedRange:NSMakeRange(0, 0)];
	
	if(pEmsData)
		[content setEmsData:pEmsData withSelFolder:m_bSelFolder];
}

- (void)update
{
	BOOL bReceived;
	NSString *strFrom, *strTime, *strMessage;
	EMSData *pEmsData;
	
	SMSMessage *pMsg = [smsList getInfo:bReceived withAddress:&strFrom Time:&strTime andMsgContent:&strMessage andEmsData:&pEmsData isConvert:YES];
	
	if(pMsg)
	{
		[self setEditCtrl:strFrom time:strTime message:strMessage andEmsData:pEmsData];
	}
	
	[smsList updateStatusBar];
}

- (void)updateListItem
{
	BOOL bReceived;
	NSString *strFrom, *strTime, *strMessage;
	EMSData *pEmsData;
	
	SMSMessage *pMsg = [smsList getInfo:bReceived withAddress:&strFrom Time:&strTime andMsgContent:&strMessage andEmsData:&pEmsData isConvert:YES];
	
	if(pMsg && pMsg->GetStatus() == REC_UNREAD)
	{
		pMsg->SetStatus(REC_READ);
		DCTAppDelegate *delegate = TheDelegate;
		
		NSMutableDictionary *dict = delegate.m_indexMap;
		NSString *key = [NSString stringWithFormat:@"%d",  pMsg->GetMsgIndex()];
		NSNumber *result = [dict objectForKey:key];
		
		if(result != nil)
		{
			[dict setObject:[NSNumber numberWithBool:YES] forKey:key];
		}
		
		int index = [smsList getFirstSelected];
		if(index >= 0)
		{
			NSMutableDictionary *dict = [[smsList dataSource] objectAtIndex:index];
			NSString* memory = [dict objectForKey:MEMORYCELL];
			
			if([memory caseInsensitiveCompare:@"SIM"] == NSOrderedSame)
				[dict setObject:@"Read_SIM.png" forKey:ICONCELL];
			else if([memory caseInsensitiveCompare:@"ME"] == NSOrderedSame)
				[dict setObject:@"Read.png" forKey:ICONCELL];
			
			[[smsList tableView] reloadData];
		}
	}
}

#pragma mark--
#pragma mark clipboard operation
#pragma mark--

- (void)clearClipboard
{
	CString str = _T("");
	NSString *memType;
	
	if(m_vMsg.empty())
		return;
	
	for(int i = 0; i < m_vMsg.size(); i++)
	{
		if(m_vMsg[i] != NULL)
		{
			m_vMsg[i]->GetMemType(str);
			memType = [NSString getStringFromWchar:str.GetBuffer(str.GetLength())];
			if([memType caseInsensitiveCompare:LocalizeString(@"PC")] != NSOrderedSame)
			{
				m_vMsg.clear();
				return;
			}
		}
	}
}

- (bool)checkClipboard:(SMSMessage *)pMsg
{
	for(int i = 0; i < m_vMsg.size(); i++)
	{
		if(m_vMsg[i] == pMsg)
		{
			m_vMsg.erase(m_vMsg.begin() + i);
			return true;
		}
	}
	
	return false;
}

- (bool)checkClipboardforFolder:(SmsFolder *)pfolder
{
	if(m_vMsg.empty())
		return false;
	
	SMSMessage *pItem = NULL;
	ItemPos pos = pfolder->GetFirstItemPos();
	
	while(pos != 0)
	{
		pItem = (SMSMessage*)pfolder->GetNextItem(pos);
		if(pItem != NULL)
		{
			for(int i = 0; i < m_vMsg.size(); i++)
			{
				if(m_vMsg[i] == pItem)
				{
					m_vMsg.clear();
					break;
				}
			}
		}
	}
	
	pfolder->CloseNextItem(pos);
	
	return true;
}

#pragma mark--
#pragma mark splitview delegate method
#pragma mark--

- (CGFloat)splitView:(NSSplitView*)splitView constrainMaxCoordinate:(CGFloat)proposedMaximumPosition ofSubviewAt:(NSInteger)dividerIndex
{
	if(splitView == verticalSplit)
	{
		return proposedMaximumPosition - kMinVerticalRightSplit; 
	}
	else if(splitView == horizontalSplit)
	{
		return proposedMaximumPosition - kMinHorizontalDownSplit;  
	}
	
	return proposedMaximumPosition;
}

- (CGFloat)splitView:(NSSplitView*)splitView constrainMinCoordinate:(CGFloat)proposedMinimumPosition ofSubviewAt:(NSInteger)dividerIndex
{
	if(splitView == verticalSplit)
	{
		return proposedMinimumPosition + kMinVerticalLeftSplit;
	}
	else if(splitView == horizontalSplit)
	{
		return proposedMinimumPosition + kMinHorizontalUpSplit; 
	}
	
	return proposedMinimumPosition;
}

#pragma mark--
#pragma mark close windows
#pragma mark--

- (void)closeSmsEditorDlg
{
	if(editorController != nil)
	{
		[editorController release];
		
		editorController = nil;
	}
	
	[self leaveDlg];
	
	PostMessage([TheDelegate getCallhistoryViewController], WM_REFRESH_LIST, 0, 0);
	
	[self onTBButtonEnabled];
}

- (void)closeSmsReadDlg
{
	if(m_pFolder == m_pFolderInbox || m_pFolder == m_pFolderOutbox)
	{
		[smsList display:m_pFolder];
		[smsList updateStatusBar];
	}
	
	if(smsReadController != nil)
	{
		if([smsReadController getRcvMsgFlag] == NO)
			m_nNewMsg = 0;
		
		[smsReadController release];
	}
	
	smsReadController = nil;
	
	[self leaveDlg];
	
	[self onTBButtonEnabled];
}

- (void)closeReaderDlg
{
	if(readerController != nil)
	{
		[readerController release];
	}
	
	readerController = nil;
	
	[self leaveDlg];
	
	[self onTBButtonEnabled];
}

- (void)closeSendDlg
{
	SmsFolder *pFolder = [smsTree getSelectedItemData];
	
	if(pFolder == m_pFolderOutbox || pFolder == m_pFolderSentbox)
	{
		[smsList display:pFolder];
		
		if([smsList getItemCount] == 0)
			[self resetEditCtrl];
	}
	
	if(sendController != nil)
	{
		[sendController release];
		sendController = nil;
	}
	
	[self leaveDlg];
	
	[self onTBButtonEnabled];
	
	m_nIndex = -1;
}

#pragma mark- interface for call hsitory

- (BOOL)sendNewMessage:(NSString *)strNumber
{
	if(sendController != nil)
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_SENDMESSAGE_FROMCH") forType:MB_OK];
		
		return NO;
	}
	
	
	[self showEditorDlg:strNumber messageContent:nil item:-1 isLast:YES withMsg:nil andIsReply:NO];
	
	[TheDelegate loadSmsView:nil];
	
	return YES;
}

@end
