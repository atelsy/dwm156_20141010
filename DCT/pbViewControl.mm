//
//  pbViewControl.m
//  DCT
//
//  Created by Fengping Yu on 11/29/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "pbViewControl.h"
#import "DCTAppDelegate.h"
#import "listData.h"
#import "cellData.h"
#import "treeNode.h"
#import "iconCollectionItem.h"
#import "ImageTextCell.h"
#import "imageAndTextCell.h"
#import "pbCocoa.h"
#import "callHistoryViewController.h"
#import "nsstrex.h"

static Str2IntList	m_mIndex;
static BOOL			m_bDownloadComplete = FALSE;

static folder_state_struct pb_folders[] =
{
	// Type   , State, Img , SubFr, Modif, Msg  , Enable, Save
	{TYPE_FROM, 0, 0, 0, false, false, true , false, true },	// Handset Folder   <----
	{TYPE_FROM, 0, 1, 1, true , false, true , true , true },	// PC Folder<----
	{TYPE_FROM, 0, 2, 2, true , false, true , true , true },	// Trash Can		<----
};

static NSString* pb_folderids[] = 
{
	LocalizeString(@"IDS_PB_FOLDER") ,
	LocalizeString(@"IDS_PB_FOLDER2") ,
	LocalizeString(@"IDS_PB_FOLDER3") ,
};

static NSString *tooltips[] = 
{
	@"IDS_PB_NEW",
	@"IDS_PB_CUT",
	@"IDS_PB_COPY",
	@"IDS_PB_PASTE",
	@"IDS_PB_DELETE",
	@"IDS_PB_DOWNLOAD",
	@"IDS_PB_FILE",
};

@implementation pbViewControl

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
	if (self) 
	{
		// Initialization code here.
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onItemchangeListPb:) name:PBLIST_LVN_ITEMCHANGED object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSelChangedTreePb:) name:PBTREE_TVN_SELCHANGED object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onTBButtonEnabled) name:WM_TOOLBARSTATE object:nil];
		
		
		NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"contactInfoLabel" ofType:@"dict"]];
		
		rootNode = [[self treeNodeFromDictionary:dict] retain];
		
		m_LastLoadIndex = -1;
		m_bConn = NO;
		m_dlgPbEditor = nil;
		m_dlgPbRead   = nil;
		m_dlgPbCsv    = nil;
		
		m_bNotify = NO;
		m_PHBEnable = NO;
		m_DownloadType = DOWNLOAD;
		//	m_bFromCH = FALSE;
		
		m_nSimTotal = 0;
		m_nSimUsed=0;
		m_pFolderHandset = NULL;
		
		m_pbkMeTotal = -1;
		m_pbkSmTotal = -1;
		m_oview_height = 0;
		m_tview_height = 0;
		
		m_hItem = [[NSTreeNode alloc] init];
		m_hTemp = [[NSTreeNode alloc] init];
		
		//register URC
		m_pComAgent = [self getComAgentObj];
		if (m_pComAgent != NULL)
		{
			m_pComAgent->RegisterURC(URC_SYS, self, WM_URCCALLBACK);
		}
		
		globalData = [[NSMutableArray alloc] init];		
		
	}
       
	
	return self;
}

- (void)dealloc
{
	[rootNode release];
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[globalData release];
	[toolbarListdata release];
	
	[m_hItem release];
	[m_hTemp release];
	
	[super dealloc];
}

#pragma mark--
#pragma mark get/set static value
#pragma mark--

+ (BOOL)GetDownloadState
{
	return m_bDownloadComplete;
}

+ (void)SetDownloadState:(BOOL)state
{
	m_bDownloadComplete = state;
}

+ (BOOL)GetFreeIdx:(const CString&)str withIdx:(int&)idx
{
	return m_mIndex.Get(str, idx);
}

+ (void)InsertFreeIdx:(const CString&)str withIdx:(const int)idx
{
	m_mIndex.Insert(str, idx);
}

+ (void)ClearAllIdx
{
	m_mIndex.clear();
}

@synthesize toolbarListdata, globalData, pbList, pbTree;

#pragma mark--
#pragma mark Initialization
#pragma mark--



- (void)awakeFromNib
{
	[self InitToolbar];
	[self InitPbList];
	
	//init tree control
//	[self InitFolder];
	[self InitTreeCtrl];
	
	[self InitSearchMenu];
	
	[pbList.tableView setTarget: self];
	[pbList.tableView setDoubleAction:@selector(OnPbOpen:)];
	
	//set init toolbar status
	[self onTBButtonEnabled];
}

/************ 20131115 by Zhuwei **********************************************/


/******************************************************************************/

// create toolbar
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
			image = [NSImage imageNamed:pbEnableToolbarImages[index]];
		else 
			image = [NSImage imageNamed:pbDisableToolbarImages[index]];
		
		[btnCell setImage:image];
		
	}
}

- (void)onTBButtonEnabled
{
	if(m_bShowDlg)
		return;
	
	BOOL bEnable = [pbList getSelected];
	
	folder_state_struct state;
	PbFolder *pFolder = [pbTree GetSelectedItemData];
	
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
		[self setToolbarItem:ID_PB_CUT state:NO];
		[self setToolbarItem:ID_PB_DELETE state:NO];
	}
	else
	{
		[self setToolbarItem:ID_PB_CUT state:bEnable];
		[self setToolbarItem:ID_PB_DELETE state:bEnable];
	}
	
	[self setToolbarItem:ID_PB_COPY state:bEnable];
	
	if(state.nState == 1)
		[self setToolbarItem:ID_PB_PASTE state:NO];
	else
		[self setToolbarItem:ID_PB_PASTE state:[pbList GetClipboard]];
	
	[self setToolbarItem:ID_PB_DOWNLOAD state:m_PHBEnable];
	
	[self setToolbarItem:ID_PB_NEW state:YES];
	[self setToolbarItem:ID_PB_FILE state:YES];
}

- (void)InitPbList
{
	
}

- (void)InitTreeCtrl
{
	[[pbTree outlineView] expandItem:nil expandChildren:YES];
	[[pbTree outlineView] selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
	
	//display handset folder contacts in right list
	[self display:(PbFolder*)[pbTree GetFolder:0]];
	
}

- (void)setDefaultTreeSelectItem
{
	NSTreeNode *hRoot;
	PbFolder *pFolder = NULL;
	
	b_Select = TRUE;
	
	[pbList Display:pFolder];
	[pbTree GetHandsetRoot:&hRoot forFolder:&pFolder];
	[[pbTree outlineView] selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
	
}

- (void)onItemchangeListPb:(NSNotification *)notification
{
	NSDictionary *userInfo = [notification userInfo];
	
	if(userInfo == nil)
	{
		m_PBItem = nil;
	}
	else 
	{
		pbCocoa *Cocoa = (pbCocoa*)[userInfo objectForKey:PBITEM];
		
		if(Cocoa != nil)
		{
			m_PBItem = Cocoa->contact;
		}
		
	}
	
	[infoList reloadData];
	[self Update];
	
	[self onTBButtonEnabled];
}

- (void)onSelChangedTreePb:(NSNotification*)notification
{	
	//reset edit ctrl text
	[self ResetEditCtrl];
	
	NSTreeNode *node = [pbTree.outlineView itemAtRow:[pbTree.outlineView selectedRow]];
	treeNode *data = [node representedObject];
	
	PbFolder *pFolder = (PbFolder*)[data getTag];
	if (pFolder != NULL)
	{
		m_LastLoadIndex = -1;
		[self display:pFolder];
	}
	
	if([pbList GetSelectedCount] <= 0)
		[self UpdateInfo];
	
	[pbList UpdateStatusBar];
	
	[searchField setStringValue:@""];
	
	[globalData removeAllObjects];
	[globalData addObjectsFromArray:pbList.dataSource];
}

- (void) ResetEditCtrl
{
	//update preview windows, set all item to empty string.
	m_PBItem = nil;
}

#pragma mark--
#pragma mark folder operation
#pragma mark--

- (void)setFolderName:(PbFolder *)pFolder withIndex:(int)index
{
	if(!pFolder)
		return;
	
	NSString *ID = [@"IDS_PB_FOLDER" stringByAppendingString:[NSString stringWithFormat:@"%d", index]];
	
	NSString *name = LocalizeString(ID);
	
	char *cname = (char*)[name UTF8String];
	
	pFolder->SetFolderName(cname);
	
}

- (PbFolder*)setSubFolderName:(PbFolder *)pFolder withIndex:(int &)index andSubFolderPos:(SubfolderPos &)pos
{
	if(!pFolder || !pos)
		return nil;
	
	PbFolder *pSubfolder = pFolder->GetNextSubfolder(pos);
	
	[self setFolderName:pFolder withIndex:index];
	
	return pSubfolder;
}

- (PbFolder*) insertSubFolder:(PbFolder *)pFolder withIndex:(int&)index
{
	// Check Folder
	if(!pFolder) 
		return NULL;
	
	// New Subfolder
	PbFolder *pSubfolder = [utility newPbFolder];
	if(!pSubfolder) return NULL;
	
	// Set Attribute
	[self setFolderName:pSubfolder withIndex:index];
	pSubfolder->SetFolderState(pb_folders[index]);
	pFolder->InsertSubfolder(pSubfolder);
	
	return pSubfolder;
}
/***********20131120  by zhang shao hua GetPhoneNumber ********/
- (bool)GetPhoneNumber:(CString &)strNum
{
		
	folder_state_struct state;
	PbFolder *pFolder = [pbTree GetSelectedItemData];
	if(pFolder)
	{
		pFolder->GetFolderState(state);
		if(state.nState == 1)
			return false;
		if([pbTree IfHandsetFolder:pFolder] && ![self getSimStatus])
			return false;
	}
	
	// Get Selected
	//int nItem = [pbList GetFirstSelected];
	int nItem = (int)[pbList.tableView clickedRow];
	if(nItem < 0)
	{
		nItem = [pbList GetFirstSelected];
		if(nItem < 0)
			return false;
	}
	
	// Get Info
	CString strName, strHandset, strHome, strCompany, strEmail, strOffice, strFax, strBirthday;
	PhoneBookItem *pContact = [pbList GetInfo:strName andHandset:strHandset andHome:strHome andCompany:strCompany andEmail:strEmail andOffice:strOffice andFax:strFax andBirthday:strBirthday];
	
	if(pContact)
    {
        strNum=strHandset;
        strNum += _T(";");
    return true;
        
    }
	return false;
}
- (void)InitFolder
{
	PbFolder *folder = [TheDelegate getPbFolder];
	
	int i, j;
	
	if(folder->GetSubfolderSize() == 0)
	{
		for(i=0, j=0;i<3;i++)
		{
			[self insertSubFolder:folder withIndex:i];
		}
	}
	else	// Update Default Folder Name
	{
		i = j = 0;
		
		SubfolderPos pos = folder->GetFirstSubfolderPos();		
		while((i < 3) && (pos != 0))
		{
			// Sub Folder
			[self setSubFolderName:folder withIndex:i andSubFolderPos:pos];
			i++;
		}
		folder->CloseNextSubfolder(pos);
	}
}

#pragma mark --
#pragma mark PhoneBook toolbar operation
#pragma mark--


- (IBAction)OnEditCut:(id)sender
{
	if(![pbList getSelected] && [pbList.tableView clickedRow] == -1)
		return;
	
	folder_state_struct state;
	PbFolder *pFolder = [pbTree GetSelectedItemData];
	if(pFolder){
		pFolder->GetFolderState(state);
		if(state.nState == 1)
			return;
	}
	[pbList CutContact];
	[self onTBButtonEnabled];
}

- (IBAction)OnEditCopy:(id)sender
{
	if(![pbList getSelected] && [pbList.tableView clickedRow] == -1)
		return;
	
	[pbList CopyContact];
	[self onTBButtonEnabled];
}

- (IBAction)OnEditPaste:(id)sender
{
	if (![pbList GetClipboard])
		return;
	
	folder_state_struct state;
	PbFolder *pFolder = [pbTree GetSelectedItemData];
	if(pFolder){
		pFolder->GetFolderState(state);
		if(state.nState == 1)
			return;
	}
	NSTreeNode* hItem = [pbTree GetSelectedItem];
	[pbList PasteContact:hItem];
}

- (IBAction)OnEditClear:(id)sender
{
	if(![pbList getSelected] && [pbList.tableView clickedRow] == -1)
		return;
	
	folder_state_struct state;
	PbFolder *pFolder = [pbTree GetSelectedItemData];
	if(pFolder)
	{
		pFolder->GetFolderState(state);
		if(state.nState == 1)
			return;
	}
	if([pbList OnPbDelete])
        {
         [self ResetEditCtrl];
          [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:@"Delete PbNumber Success!"forType:MB_OK];
        }
        else
        {
             [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:@"Delete PbNumber Fail!"forType:MB_OK];
            
        }
	
	PostMessage([TheDelegate getCallhistoryViewController], WM_REFRESH_LIST, 0, 0);
}

- (IBAction)OnPbNew:(id)sender
{
	[self ShowEditorDlg:_T("") Handset:_T("") Home:_T("") Company:_T("") Email:_T("") Office:_T("") Fax:_T("") Birthday:_T("") withIdx:-1 andContact:nil];
	
}
/**********by shao hua 20131120 SendMessage Action *********/
- (IBAction)OnPBFolderSendMsg:(id)sender
{
	CString strNum;
	if([self GetPhoneNumber:(CString&)strNum])
	{
		
        [[TheDelegate getSmsViewController] showEditorDlg:[NSString CStringToNSString:strNum] messageContent:nil item:-1 isLast:YES withMsg:nil andIsReply:NO];
	}
}
- (IBAction)OnPbOpen:(id)sender
{
	folder_state_struct state;
	PbFolder *pFolder = [pbTree GetSelectedItemData];
	if(pFolder)
	{
		pFolder->GetFolderState(state);
		if(state.nState == 1)
			return;
		if([pbTree IfHandsetFolder:pFolder] && ![self getSimStatus])
			return;
	}
	
	// Get Selected
	//int nItem = [pbList GetFirstSelected];
	int nItem = (int)[pbList.tableView clickedRow];
	if(nItem < 0)
	{
		nItem = [pbList GetFirstSelected];
		if(nItem < 0)
			return;
	}
	
	// Get Info	
	CString strName, strHandset, strHome, strCompany, strEmail, strOffice, strFax, strBirthday;
	PhoneBookItem *pContact = [pbList GetInfo:strName andHandset:strHandset andHome:strHome andCompany:strCompany andEmail:strEmail andOffice:strOffice andFax:strFax andBirthday:strBirthday];
	
	if(pContact)
		[self ShowEditorDlg:strName Handset:strHandset Home:strHome Company:strCompany Email:strEmail 
					 Office:strOffice Fax:strFax Birthday:strBirthday withIdx:nItem andContact:pContact];	
}

#pragma mark--
#pragma mark display operation
#pragma mark--

- (void)display:(PbFolder *)folder
{
	[pbList Display:folder];
	
	if([pbList GetItemCount] == 0)
		[self ResetEditCtrl];
}

#pragma mark--
#pragma mark handle message
#pragma mark--

- (LRESULT) OnURCCallBack:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	WATCH_POINTER(lParam);
	CUrcFlag *pUrc = (CUrcFlag*)lParam;
	switch (wParam)
	{
		case URC_SYS:
		{
			CUrcSys *pInt = (CUrcSys*)pUrc;
			[self ParseSystemStatus:pInt];
		}
			break;
		case URC_PBK:
			m_bNotify = TRUE;
			m_PHBEnable = pUrc->Value();
			if (m_PHBEnable)
			{
				if([utility getModalCounter] != 0)
					break;
				
				if(m_dlgPbEditor != nil)
				{
					[m_dlgPbEditor release];			
					m_dlgPbEditor = nil;
				}
				
				if(m_dlgPbCsv != nil)
				{
					if([m_dlgPbCsv IsModelDlg])
						return 0;
					[m_dlgPbCsv release];			
					m_dlgPbCsv = nil;
				}
				
                //#ifndef DEBUG
				[self ShowReadDlg:DOWNLOAD];
                //#endif
			}
			break;
		default:
			break;
	}
	return 0;
}

- (LRESULT) OnRspCallBack:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	WATCH_POINTER(lParam);
	CRespArray *pRespArray;
	
	switch(LOWORD(wParam))
	{
		case RESP_PB_GETTOTALNUM:
		{
			long idx = 0;
			pRespArray = (CRespArray*)lParam;
			if (HIWORD(wParam) == ER_OK)
			{
				
				if (NULL != pRespArray)
				{
					idx = pRespArray->m_lExt;
					[m_dlgPbRead SetTotalNumber:TRUE 
									   andTotal:MAKELONG(pRespArray->Value()[3], pRespArray->Value()[0])];
                    int a=MAKELONG(pRespArray->Value()[3], pRespArray->Value()[0]);
					m_nSimTotal = pRespArray->Value()[0];
					m_vLenLimit.push_back(MAKELONG(pRespArray->Value()[1], pRespArray->Value()[2]));
					m_nSimUsed=pRespArray->Value()[3];
                    NSLog(@"m_nSimTotal: %d",m_nSimTotal);
                    NSLog(@"m_nSimUsed: %d",m_nSimUsed);
                    NSLog(@"a: %d",a);
					[self SetPbkTotal:m_vMemory[idx-1] andTotal:pRespArray->Value()[0]];
				}
			}
			else
			{
				if (NULL != pRespArray)
				{
					idx = pRespArray->m_lExt;
					[m_dlgPbRead SetTotalNumber:TRUE andTotal: MAKELONG(0, 0)];
					
					m_nSimTotal = 0;
					m_vLenLimit.push_back(MAKELONG(0, 0));
					
					[self SetPbkTotal:m_vMemory[idx-1] andTotal:-1];
				}
			}
			if (idx >= m_vMemory.size())
				PostMessage(self, WM_INIT_RESULT, 0, INIT_SUCCESS);	
			else
			{
				CComAgent *pCom = [TheDelegate getComAgent];
				
				CAERR err = pCom->GetPBKNumber(self, WM_RSPCALLBACK, 
											   RESP_PB_GETTOTALNUM, m_vMemory[idx], idx+1);
				
				if (CA_OK != err)
					PostMessage(self, WM_INIT_RESULT, 0, INIT_FAILED);
			}
		}
			break;
		default:
			break;
	}
	return 0;
}

- (LRESULT) OnDestroyDlg:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	switch(wParam)
	{
		case PB_DLG_READ:
			[self closeReaderWindow];
			break;
		case PB_DLG_EDITOR:
			[self closeEditorWindow];
			break;
		case PB_DLG_CSV:
			[self closeCSVWindow];
			break;
	}
	
	[self LeaveDlg];
	
	[self ResetSearch];
	return 0;
}

- (void)closeEditorWindow
{
	if(m_dlgPbEditor != nil)
		[m_dlgPbEditor release];			
	m_dlgPbEditor = nil;
	
	PostMessage([TheDelegate getCallhistoryViewController], WM_REFRESH_LIST, 0, 0);
}

- (void)closeReaderWindow
{
	if(m_pFolder == m_pFolderHandset)
		[pbList Display:m_pFolder];
	
	if(m_dlgPbRead != nil)
		[m_dlgPbRead release];			
	m_dlgPbRead = nil;
	
	[self NotifyDownloadCH];
	if (m_DownloadType == REDOWNLOAD)
		[pbList OnAfterReDownload:0 andLparam:TRUE];
}

- (void)closeCSVWindow
{
	if(m_dlgPbCsv != nil)
		[m_dlgPbCsv release];			
	m_dlgPbCsv = nil;
}

- (LRESULT) OnInitResult:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	switch(lParam)
	{
		case INIT_SUCCESS:
		{
			if(m_dlgPbRead == nil)
				m_dlgPbRead = [pbReadDlg alloc];
			
			[m_dlgPbRead initWithWindowNibName:@"pbReadDlg"];
			
			if(TheDelegate.currentView == PHONEBOOKVIEW)
			{
				[m_dlgPbRead showWindow: self];
				[[m_dlgPbRead window] orderFrontRegardless];
			}
			else
				[[m_dlgPbRead window] orderOut:nil];
		}	
			break;
			
		case INIT_FAILED:
			[m_dlgPbRead release];
			m_dlgPbRead = nil;
			[self LeaveDlg];
			[self NotifyDownloadCH];
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_READPHONEBOOK") forType:MB_OK];
			
			break;
	}
	
	return 0;
}

- (LRESULT) OnPbRefresh:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	return 0;
}

- (LRESULT) OnPbInsert:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	PbFolder *pFolder = (PbFolder*)wParam;
	PhoneBookItem *pContact = (PhoneBookItem*)lParam;
	
	[pbList InsertContact:pFolder withItem:pContact andVector:m_vContactTemp];
	
	return 0;
}

- (LRESULT) OnPbUpdate:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	int nItem = (int)wParam;
	PhoneBookItem *pContact = (PhoneBookItem*)lParam;
	
	[pbList UpdateContact:nItem withItem:pContact andVector:m_vContactTemp];
	
	return 0;
}

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	switch (Msg) 
	{
		case WM_URCCALLBACK:
			[self OnURCCallBack:wparam andLparam:lparam];
			break;
		case WM_RSPCALLBACK:
			[self OnRspCallBack:wparam andLparam:lparam];
			break;
		case WM_PB_DESTROYDLG:
			[self OnDestroyDlg:wparam andLparam:lparam];
			break;
		case WM_INIT_RESULT:
			[self OnInitResult:wparam andLparam:lparam];
			return;
		case WM_PB_INSERT:
			[self OnPbInsert:wparam andLparam:lparam];
			break;
		case WM_PB_UPDATE:
			[self OnPbUpdate:wparam andLparam:lparam];
			break;
		case WM_PB_REFRESH:
			[self OnPbRefresh:wparam andLparam:lparam];
			break;
		default:
			break;
	}
}

#pragma mark--
#pragma mark get and set function
#pragma mark--

- (std::vector<PhoneBookItem*>)getPhbVector
{
	return m_vContact;
}

- (BOOL)getSimStatus
{
	return m_PHBEnable;
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
	return m_LastLoadIndex;
}

#pragma mark--
#pragma mark outlineview datasource and delegate
#pragma mark--

- (NSArray*)childrenForItem:(id)item
{
	if(item == nil)
	{
		return [rootNode childNodes];
	}
	else
	{
		return [item childNodes];
	}
}

- (id)outlineView:(NSOutlineView*)outlineView child:(NSInteger)index ofItem:(id)item
{
	NSArray *children = [self childrenForItem:item];
	
	return [children objectAtIndex:index];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	return NO;
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	NSArray *children = [self childrenForItem:item];
	
	return [children  count];
}

- (id)outlineView:(NSOutlineView*)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
	id object;
	
	treeNode *node = [item representedObject];
	
	object = node.name;
	
	return object;
}

- (void)outlineView:(NSOutlineView*)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	treeNode *node = [item representedObject];
	
	if(node.image == nil)
		node.image = [NSImage imageNamed:node.name]; 
	
	imageAndTextCell *imageCell = (imageAndTextCell*)cell;
	
	[imageCell setImage:node.image];
}

- (CGFloat)outlineView:(NSOutlineView*)outlineView heightOfRowByItem:(id)item
{
	NSRect rect = [self.view convertRect:[labelList bounds] toView:self.view];
	
	if(m_oview_height == 0)
		m_oview_height = rect.size.height/8.8;
	
	return m_oview_height;
}

- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary*)dict
{
	NSString *nodeName = [dict objectForKey:NAME_KEY];
	treeNode *nodeData = [treeNode nodeDataWithName:nodeName];
	
	NSTreeNode *result = [NSTreeNode treeNodeWithRepresentedObject:nodeData];
	
	NSArray *children = [dict objectForKey:CHILDREN_KEY];
	
	for(id item in children)
	{
		NSTreeNode *childTreeNode;
		
		treeNode *childNodeData = [[treeNode alloc] initWithName:item];
		
		childTreeNode = [NSTreeNode treeNodeWithRepresentedObject:childNodeData];
		[childNodeData release];
		
		[[result mutableChildNodes] addObject:childTreeNode];
	}
	
	return result;
}

#pragma mark--
#pragma mark table view delegate and datasource protocol
#pragma mark--

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return 8;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	NSString *strRow = @"";
	
	if(m_PBItem != NULL)
	{
		Advanced_PBItem_Struct pbAdv;
		CString strName, strMobile;
		
		m_PBItem->GetPhone(strName, strMobile);
		m_PBItem->GetAdvPBItem(pbAdv);
		
		switch(row)
		{	
			case 0:
				if(!strName.IsEmpty())
					strRow = [NSString CStringToNSString:strName];
				break;
			case 1:
				if(!strMobile.IsEmpty())
					strRow = [NSString CStringToNSString:strMobile];
				break;
			case 2:
				if(!pbAdv.m_home_num.IsEmpty())
					strRow = [NSString CStringToNSString:pbAdv.m_home_num];
				break;
			case 3:
				if(!pbAdv.m_company_name.IsEmpty())
					strRow = [NSString CStringToNSString:pbAdv.m_company_name];
				break;
			case 4:
				if(!pbAdv.m_email.IsEmpty())
					strRow = [NSString CStringToNSString:pbAdv.m_email];
				break;
			case 5:
				if(!pbAdv.m_office_num.IsEmpty())
					strRow = [NSString CStringToNSString:pbAdv.m_office_num];
				break;
			case 6:
				if(!pbAdv.m_fax_num.IsEmpty())
					strRow = [NSString CStringToNSString:pbAdv.m_fax_num];
				break;
			case 7:
				if(pbAdv.m_birth_year>0 && pbAdv.m_birth_month>0 && pbAdv.m_birth_month>0)
					strRow = [NSString localizedStringWithFormat:@"%04d/%02d/%02d", pbAdv.m_birth_year, pbAdv.m_birth_month, pbAdv.m_birth_day];
				break;
		}
	}
	
	//  NSLog(@"strRow = %@, row = %d\n", strRow, row);
	return strRow;
}

- (CGFloat)tableView:(NSTableView *)tableView heightOfRow:(NSInteger)row
{
	NSRect rect = [self.view convertRect:[labelList bounds] toView:self.view];
	
	if(m_tview_height == 0)
		m_tview_height = rect.size.height/8.8;
	
	return m_tview_height;
}


#pragma mark--
#pragma mark splitview delegate method
#pragma mark--

- (void)InitSearchMenu
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

- (void)ResetSearch
{
	[searchField setStringValue:@""];
	[self doSearch:nil];
	[pbList.tableView deselectAll:nil];
}

- (IBAction)doSearch:(id)sender
{
	NSPredicate *predicate = nil;
	NSString *searchString = [searchField stringValue];
	
	PbFolder* pFolder = [pbTree GetSelectedItemData];
	int count = pFolder->GetItemSize();
	
	
	if([pbList.dataSource count] != count)
	{
		[self display:pFolder];
	}
	
	if((searchString != nil) && (![searchString isEqualToString:@""]))
	{
		if(m_searchCategory == SEARCHNAME)
		{
			predicate = [NSPredicate predicateWithFormat:@"PBNameCell contains %@", searchString];
		}
		else if(m_searchCategory == SEARCHNUMBER)
		{
			predicate = [NSPredicate predicateWithFormat:@"PBMobileCell contains %@", searchString];
		}
		
		//back up smslist datasource
		[globalData removeAllObjects];
		[globalData addObjectsFromArray:pbList.dataSource];
		
		[pbList.dataSource filterUsingPredicate:predicate];
	}
	
	[pbList.tableView reloadData];
	
	[pbList.tableView deselectAll:nil];
	
	[pbList UpdateStatusBar];
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
		return [box frame].size.height - 6; 
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
		return [box frame].size.height; 
	}
	
	return proposedMinimumPosition;
}

- (CGFloat)splitView:(NSSplitView*)splitView constrainSplitPosition:(CGFloat)proposedPosition ofSubviewAt:(NSInteger)dividerIndex
{
	return 210;
}

#pragma mark--
#pragma mark menu status
#pragma mark--

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	int tag = (int)[menuItem tag];
	
	NSUInteger clickRow = [pbList.tableView clickedRow];
	NSIndexSet *set = [pbList.tableView selectedRowIndexes];
	
	folder_state_struct state;
	PbFolder *pFolder = [pbTree GetSelectedItemData];
	
	if(pFolder)
	{
		pFolder->GetFolderState(state);
		if(tag == ID_PB_OPEN)
		{
			if([pbTree IfHandsetFolder:pFolder] && ![self getSimStatus])
				return NO;
		}
		
	}
	else
	{		
		state.nState = 0;
	}
	
	if(tag == SEARCHNAME || tag == SEARCHNUMBER)
	{
		if([pbList GetItemCount] > 0)
			return YES;
		else
			return NO;
	}
	else 
	{		
/*		if(state.nState == 1)
			return NO;
		
		if(![pbList getSelected])
			return NO;
*/		
		if(tag == ID_PB_PASTE)
		{
			if(![pbList GetClipboard])
				return NO;
		}
		else if(tag == ID_PB_OPEN)
		{
			if ((clickRow != NSUIntegerMax && !([set containsIndex:clickRow] && [set count] > 1))||
			   (clickRow == NSUIntegerMax && [set count] == 1))
			{
				return YES;
			}
			else
				return NO;
		}
		else if(tag == ID_PB_CUT || tag == ID_PB_COPY || tag == ID_PB_DELETE)
		{
			if((clickRow == NSUIntegerMax) && ([set count] < 1))
				return NO;
			else
			{
				return YES;
			}
			
		}

	}
	
	return YES;
}

#pragma mark--
#pragma mark other function
#pragma mark--
- (NSTreeNode*)GetCopyItem
{
	return m_hItem;
}

- (NSTreeNode*)GetDeleteItem
{
	return m_hTemp;
}

- (std::vector<PhoneBookItem*>&)GetCopyContacts
{
	return m_vContact;
}

- (std::vector<PhoneBookItem*>&)GetDeleteContacts
{
	return m_vContactTemp;
}

- (int)GetPbkSmTotal
{
	return m_pbkSmTotal;
}

- (int)GetPbkMeTotal
{
	return m_pbkMeTotal;
}

- (void)ParseSystemStatus:(CUrcSys*)pUrc
{
	if (pUrc == nil)
		return;
	
	if (pUrc->Value().alpha >= SYS_PINREADY)
	{
		if (!m_bConn)
		{
			m_pComAgent = [self getComAgentObj];
			if (m_pComAgent != nil)
			{
				m_pComAgent->RegisterURC(URC_PBK, self, WM_URCCALLBACK);
			}
			
		}
		m_bConn = TRUE;
		[self ClearClipboard];
	}
	else
	{
		if (m_bConn)
		{
			m_pComAgent = [self getComAgentObj];
			m_pComAgent->DeregisterURC(URC_PBK, self);
		}
		m_bConn = FALSE;
		m_PHBEnable = FALSE;
		
		m_pbkMeTotal = -1;
		m_pbkSmTotal = -1;
		
		[self onTBButtonEnabled];
		
		DebugOut(_T("\n(URC_SYS)Current PBK status m_PHBEnable = %d\n"), m_PHBEnable);
	}
}

-(void) ClearClipboard
{
	if(m_vContact.empty())
		return;
	
	for(int i=0; i<m_vContact.size(); i++)
	{
		if (m_vContact[i] != NULL)
		{
			if (m_vContact[i]->GetMemType() != _T("PC"))
			{
				m_vContact.clear();
				return;
			}
		}
	}
}

- (void)ClearContactList
{
	m_vContact.clear();
	m_vContactTemp.clear();	
	m_vContactDrag.clear();
}

- (void)Update
{
	[self UpdateInfo];
	[pbList UpdateStatusBar];
	[self UpdateMemory];
	//	[self ResetEditCtrl];
}

- (void)UpdateInfo
{
	
}

- (void)UpdateMemory
{
	
}

- (void)OnPbCSV
{
	NSTreeNode *hHandset, *hPC;
	PbFolder *pFolderHandset = NULL;
	PbFolder *pPCFolder = NULL;
	
	[pbTree GetHandsetRoot:&hHandset forFolder:&pFolderHandset];
	[pbTree GetPcRoot:&hPC forFolder:&pPCFolder];
	
	if(m_dlgPbCsv == nil)
		m_dlgPbCsv = [pbCSVDlg alloc];
	
	[m_dlgPbCsv Init:pFolderHandset withPcFolder:pPCFolder andParent:self];
	
	[m_dlgPbCsv initWithWindowNibName:@"pbCSVDlg"];
	
	[m_dlgPbCsv showWindow:nil];
	[self EnterDlg];
}

- (void)ShowReadDlg:(DOWNLOAD_TYPE)type
{
	if(m_PHBEnable == NO)
		return;
	
	[searchField setStringValue:@""];
	
	[self EnterDlg];
	m_DownloadType = type;
	
	// Clear Flag
	[self ClearContactList];
	m_LastLoadIndex = -1;
	
	// Get Handset Folder
	
	NSTreeNode *hHandset;
	
	if([pbTree GetHandsetRoot:&hHandset forFolder:&m_pFolderHandset])
	{
		// Get Current Folder
		m_pFolder = [pbTree GetSelectedItemData];
		
		// Clear Contacts
		if(m_pFolder == m_pFolderHandset)
		{
			[pbList deleteAllItems];
			[self Update];
			[self ResetEditCtrl];
		}
		
		m_pFolderHandset->DeleteAll(INCLUDE_SUBFOLDER);		
		
		if(m_dlgPbRead == nil)
			m_dlgPbRead = [pbReadDlg alloc];
		
		[m_dlgPbRead Init:m_pFolderHandset andParentWnd:self];  
		
		m_vLenLimit.clear();
		
		m_vMemory.clear();
		m_vMemory.push_back(_T("SM"));
		
		[self OnRspCallBack:RESP_PB_GETTOTALNUM andLparam: 0];
	}
}

- (CString)GetMemory:(int)idx
{
	if(idx >= m_vMemory.size())
		return _T("");
	
	return m_vMemory[idx];
}

- (int)GetLimit:(const CString&)strMem
{
	for (size_t i=0; i<m_vMemory.size()-1; i++)
	{
		if (m_vMemory[i] == strMem)
		{
			if (m_vLenLimit.size() <= i)
				return 0;
			
			return m_vLenLimit[i];
		}
	}
	return 0;
}

- (size_t)GetMemorySize
{
	return m_vMemory.size();
}

- (int)GetNameLimit:(const CString&)strMem
{
	return HIWORD([self GetLimit:strMem]);
}

- (int)GetNumberLimit:(const CString&)strMem
{
	return LOWORD([self GetLimit:strMem]);
}

- (void)SetPbkTotal:(const CString&)memtype andTotal:(int)total
{
	if(memtype == _T("ME"))
		m_pbkMeTotal = total;
	else if(memtype == _T("SM"))
		m_pbkSmTotal = total;
}

-(void)EnableAllCtrl:(BOOL)bEnable
{	
	[pbTree.outlineView setEnabled:bEnable];
	[pbList.tableView setEnabled:bEnable];
	
	[pbList.tableView deselectAll:nil];
	
	for (NSInteger i = 0; i < [listMenu numberOfItems]; i++)
	{
		[[listMenu itemAtIndex:i] setEnabled:bEnable];
	}
	
	NSButtonCell *btnCell = [toolbarMatrix cellWithTag:ID_PB_NEW];
	[btnCell setEnabled:bEnable];
	NSImage *image = bEnable? [NSImage imageNamed:pbEnableToolbarImages[ID_PB_NEW]] : [NSImage imageNamed:pbDisableToolbarImages[ID_PB_NEW]];
	[btnCell setImage:image];
	
	btnCell = [toolbarMatrix cellWithTag:ID_PB_FILE];
	[btnCell setEnabled:bEnable];
	image = bEnable? [NSImage imageNamed:pbEnableToolbarImages[ID_PB_FILE]] : [NSImage imageNamed:pbDisableToolbarImages[ID_PB_FILE]];
	[btnCell setImage:image];
	
	
	for(NSInteger i = ID_PB_CUT; i <= ID_PB_DOWNLOAD;  i++)
	{
		btnCell = [toolbarMatrix cellWithTag:i];
		image = bEnable? [NSImage imageNamed:pbEnableToolbarImages[i]] : [NSImage imageNamed:pbDisableToolbarImages[i]];
		[btnCell setImage:image];
	}
	
	[searchField setEnabled:bEnable];
}

- (bool)CheckClipboard:(const PhoneBookItem*)pContact
{
	for(NSInteger i = m_vContact.size()-1; i>=0; i--)
	{
		if(m_vContact[i] == pContact)
		{
			m_vContact.erase(m_vContact.begin()+i);
			return true;
		}
	}
	return false;
}

- (bool)CheckClipboardFolder:(PbFolder *)pFolder
{
	PhoneBookItem *pItem = NULL;
	ItemPos pos = pFolder->GetFirstItemPos();
	
	while((pos != 0) && (!m_vContact.empty()))
	{
		if((pItem = pFolder->GetNextItem(pos)))
		{
			for(NSInteger i=m_vContact.size()-1; i>=0; i--)
			{
				if(m_vContact[i] == pItem)
				{
					m_vContact.clear();	// clipboard is all in one folder
					break;
				}
			}
		}
	}
	pFolder->CloseNextItem(pos);
	
	PbFolder *pSubFldr = NULL;
	pos = pFolder->GetFirstSubfolderPos();
	
	// for every sub-folder...
	while((pos != 0) && (!m_vContact.empty()))
	{
		if ((pSubFldr = pFolder->GetNextSubfolder(pos)))
		{
			[self CheckClipboardFolder:pSubFldr];
		}
	}
	pFolder->CloseNextSubfolder(pos);
	
	return (!m_vContact.empty());
}

- (void)EnterDlg
{
	m_bShowDlg = TRUE;
	[self EnableAllCtrl:FALSE];
	
	SendMessage([TheDelegate getCallhistoryViewController],WM_ENABLEOPERATION, OP_PB, 0);
}

- (void)LeaveDlg
{
	m_bShowDlg = FALSE;
	[self EnableAllCtrl:TRUE];
	
	[self onTBButtonEnabled];
	
	SendMessage([TheDelegate getCallhistoryViewController],WM_ENABLEOPERATION, OP_PB, 1);
}

- (void)ShowEditorDlg:(LPCTSTR)strName Handset:(LPCTSTR)strHandset Home:(LPCTSTR)strHome Company:(LPCTSTR)strCompany Email:(LPCTSTR)strEmail
			   Office:(LPCTSTR)strOffice Fax:(LPCTSTR)strFax Birthday:(LPCTSTR)strBirthday withIdx:(int)nItem andContact:(PhoneBookItem*)pContact
{	
	// Get Handset Folder
	NSTreeNode *hHandset, *hPC;
	PbFolder *pFolderHandset = NULL;
	PbFolder *pPCFolder = NULL;
	
	[pbTree GetHandsetRoot:&hHandset forFolder:&pFolderHandset];
	[pbTree GetPcRoot:&hPC forFolder:&pPCFolder];
	
	if(m_dlgPbEditor == nil)
		m_dlgPbEditor = [pbEditorDlg alloc];
	
	[m_dlgPbEditor Init:strName Handset:strHandset Home:strHome Company:strCompany 
				  Email:strEmail Office:strOffice Fax:strFax Birthday:strBirthday
				withIdx:nItem andFolder:pPCFolder andHandset:pFolderHandset
			 andContact:pContact withAdv:FALSE andParentwnd:self];
	
	[m_dlgPbEditor initWithWindowNibName:@"pbEditorDlg"];
	
	[m_dlgPbEditor showWindow:nil];
	[self EnterDlg];
}

- (void)AddNewContact:(LPCTSTR)strHandset Home:(LPCTSTR)strHome
{
	DCTAppDelegate *delegate = TheDelegate;
	
	[delegate loadPhonebookView:0];
	
	[self ShowEditorDlg:_T("") Handset:strHandset Home:strHome Company:_T("") Email:_T("") 
				 Office:_T("") Fax:_T("") Birthday:_T("") withIdx:-2 andContact:NULL];
}

- (void)NotifyDownloadCH
{
	if(m_bNotify)
	{
		PostMessage([TheDelegate getCallhistoryViewController], WM_CH_STARTDOWNLOAD, 0, m_PHBEnable);
		m_bNotify = FALSE;
	}
}

- (NSWindow*)getWindow
{
	if(m_dlgPbRead)
		return m_dlgPbRead.window;
	else if(m_dlgPbEditor)
		return m_dlgPbEditor.window;
	else if(m_dlgPbCsv)
		return m_dlgPbCsv.window;
	
	return nil;
}

- (void)closeWindow
{
	if(m_dlgPbEditor)
	{
		[self closeEditorWindow];
	}
	else if(m_dlgPbRead)
	{
		[self closeReaderWindow];
	}
	else if(m_dlgPbCsv)
	{
		[self closeCSVWindow];
	}
	
	[self LeaveDlg];
}

@end
