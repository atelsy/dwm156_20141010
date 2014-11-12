//
//  chJobThreadDlg.m
//  DCT
//
//  Created by mtk on 12-6-9.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import "chJobThreadDlg.h"
#import "callHistoryViewController.h"
#import "DCTAppDelegate.h"
#import "chTreeControl.h"
#import "chListCtrl.h"

@implementation chJobThreadDlg

@synthesize m_strTitle;

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) 
	{
		// Initialization code here.
		m_bFinish = YES;
	}
	
	return self;
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	pComAgent = [TheDelegate getComAgent];
	/*
	 if(pComAgent != NULL)
	 pComAgent->RegisterURC(URC_SYS, self, WM_URCCALLBACK);
	 else
	 return;
	 */
	if(m_Job == -1)
		return;
	
	[self initCtrl];
	
	[self doJob];
}

#pragma mark - init method

- (void)initWithTitle:(NSString*)strTitle items:(std::vector<PhoneBookItem *> *)pvCall jobType:(int)job fromFolder:(ChFolder *)pFolderFrom withNumber:(int)ncount andParent:(chListCtrl *)parent
{
	m_pvCall = pvCall;
	m_pFolderDelete = pFolderFrom;
	m_Count = ncount;
	m_strTitle = strTitle;
	m_Job = job;
	m_pParent = parent;
	
	if(m_Job == 0)
		m_pvMissCall = pvCall;
	
	if(m_Job == 1)
		m_pvResvCall = pvCall;
	
	if(m_Job == 2)
		m_pvDialCall = pvCall;
	
	m_bShowDlg = NO;
	m_bFinish = 1;
}

- (void)initCtrl
{
	[[self window] setTitle:m_strTitle];
	
	if(m_Job == 0)
		[m_ctlProgress setMaxValue:m_pvMissCall->size()];
	else if(m_Job == 1)
		[m_ctlProgress setMaxValue:m_pvResvCall->size()];
	else if(m_Job == 2)
		[m_ctlProgress setMaxValue:m_pvDialCall->size()];
	
	[m_ctlProgress setMinValue:0.0f];
	
	[m_ctlProgress setDoubleValue:0.0];
}

#pragma mark - job method

- (void)doJob
{
	switch (m_Job)
	{
		case 0:
			m_pvResultCall.clear();
			[self start:m_pFolderDelete withItem:m_pvMissCall andCount:m_nCount];
			break;
			
		case 1:
			m_pvResultCall.clear();
			[self start:m_pFolderDelete withItem:m_pvResvCall andCount:m_nCount];
			break;
			
		case 2:
			m_pvResultCall.clear();
			[self start:m_pFolderDelete withItem:m_pvDialCall andCount:m_nCount];
			break;
			
		default:
			break;
	}
}

- (BOOL)start:(ChFolder *)pFolderDelete withItem:(std::vector<PhoneBookItem *> *)pvCall andCount:(int)nCount
{
	if(!m_bFinish)
		return NO;
	
	if(pvCall && pFolderDelete)
	{
		m_pvJobCall = pvCall;
		m_vCall = *pvCall;
		m_nCount = nCount;
		m_pJobFolderDelete = pFolderDelete;
		
		[self func];
		
		return YES;
	}
	
	return NO;
}

- (DWORD)func
{
	m_bFinish = NO;
	m_bCancel = NO;
	
	switch (m_Job)
	{
		case 0:
			[self chDeleteMissCall];
			break;
			
		case 1:
			[self chDeleteResvCall];
			break;
			
		case 2:
			[self chDeleteDialCall];
			break;
			
		default:
			break;
	}
	
	m_bFinish = YES;
	
	return 0;
}

#pragma mark - delete method

- (void)chDeleteMissCall
{
	[self chDeleteCall];
}

- (void)chDeleteResvCall
{
	[self chDeleteCall];
}

- (void)chDeleteDialCall
{
	[self chDeleteCall];
}

- (void)chDeleteCall
{
	[self chDeleteNext:0];
}

- (void)chDeleteNext:(int)nNum
{
	CRespFlag *resp = new CRespFlag(YES, nNum);
	SendMessage(self, WM_CHDELETE, 0, (LPARAM)resp);
}

- (void)deleteEnd:(CRespFlag *)pResp
{
	[self notifyDlg];
}

- (void)notifyDlg
{
	PostMessage(m_pParent, WM_CH_DESTROYCALLDLG, 0, 0);
}

- (CAERR)erasePBK:(void *)hWnd msg:(unsigned int)uMsg step:(short)nStep andItem:(PhoneBookItem *)pPbk exten:(long)lExt
{
	callHistoryViewController *pView = [m_pParent getParent];
	
	if([pView m_CHBEnable] == NO)
		return CA_UNKNOWN;
	
	return  pComAgent->ErasePBK(self, uMsg, nStep, pPbk, lExt);
}

#pragma mark - notification method

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	switch (Msg)
	{
		case WM_CHTHREAD_DELETE:
			[self onChThreadDelete:wparam andLparam:lparam];
			break;
			
		case WM_CHDELETE:
			[self onCHDelete:wparam andLparam:lparam];
			break;
			/*
			 case WM_URCCALLBACK:
			 [self onURCCallBack:wparam andLparam:lparam];
			 break;
			 */
		default:
			break;
	}
}

- (LRESULT)onCHDelete:(long)wParam andLparam:(long)lParam
{
	WATCH_POINTER(lParam);
	callHistoryViewController *pView = [m_pParent getParent];
	ChFolder *pFolder = NULL;
	CRespFlag *p = (CRespFlag*)lParam;
	int i = (int)p->m_lExt;
	
	BOOL bSuccess = p->Value();
	int step = LOWORD(wParam);
	PhoneBookItem *pCall = NULL;
	PhoneBookItem *pCallTemp;
	CHFoldres *pFolders;
	int nIdx;
	
	if(m_Job == 0)
		pFolder = [pView getMissCallFolder];
	else if(m_Job == 1)
		pFolder = [pView getResvCallFolder];
	else if(m_Job == 2)
		pFolder = [pView getDialCallFolder];
	
	switch (step)
	{
		case 0:
			if(i < m_vCall.size() && bSuccess)
			{
				pCall = m_vCall[i];
				if(pCall)
				{
					if(CA_OK != [self erasePBK:self msg:WM_CHDELETE step:1 andItem:pCall exten:i])
					{
						SendMessage(self, WM_CHTHREAD_DELETE, -1, NULL);
						
						[self notifyDlg];
						
						break;
					}
					else
					{
						nIdx = pCall->GetIndex();
						ItemPos pos = pFolder->GetFirstItemPos();
						
						while (pos)
						{
							if((pCallTemp = pFolder->GetNextItem(pos)))
							{
								int nIdxtemp = pCallTemp->GetIndex();
								if(nIdxtemp < nIdx)
									continue;
								else if(nIdxtemp > nIdx)
									pCallTemp->SetIndex(nIdxtemp - 1);
							}
						}
						
						pFolder->CloseNextItem(pos);
					}
				}
				
				if(m_bCancel)
				{
					[self notifyDlg];
					break;
				}
			}
			else
				[self deleteEnd:NULL];
			break;
			
		case 1:
			if(bSuccess)
			{
				if((pFolders = [self newFolders]))
				{
					if(SendMessage(self, WM_CHTHREAD_DELETE, i, (LPARAM)pFolders) == NO)
					{
						delete pFolders;
					}
				}
			}
			else
			{
				SendMessage(self, WM_CHTHREAD_DELETE, -1, NULL);
				[self notifyDlg];
				break;
			}
			
			if(m_bCancel)
			{
				[self notifyDlg];
				break;
			}
			
			[self chDeleteNext:i + 1];
			break;
			
		case 2:
			break;
			
		default:
			break;
	}
	
	return 0;
}
/*
 - (LRESULT)onURCCallBack:(long)wParam andLparam:(long)lParam
 {
 WATCH_POINTER(lParam);
 return 0;
 }
 */
- (LRESULT)onChThreadDelete:(long)wParam andLparam:(long)lParam
{
	[m_ctlProgress incrementBy:1.0];
	
	callHistoryViewController *pView = [m_pParent getParent];
	chTreeControl *tree = [pView chTree];
	
	long nItem = wParam;
	
	if(nItem >= 0 && lParam)
	{
		CHFoldres *pFolders = (CHFoldres*)lParam;
		ChFolder *pFolderDelete = pFolders->pFolderDelete;
		std::vector<PhoneBookItem*>& vec = *(pFolders->pvCall);
		
		if([tree ifMissCallFolder:pFolderDelete])
			[pView chList].m_numMC--;
		
		if([tree ifResvCallFolder:pFolderDelete])
			[pView chList].m_numRC--;
		
		if([tree ifDialCallFolder:pFolderDelete])
			[pView chList].m_numLD--;
		
		[self moveItem:pFolderDelete withItem:vec andIndex:(int)nItem];
		
		[pView resetSearch];
		
		delete pFolders;
		
		return 1;
	}
	else
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_DELETECALL") forType:MB_OK];
		
		return 0;
	}
	
	return 0;
}

- (void)moveItem:(ChFolder *)pFolderDelete withItem:(std::vector<PhoneBookItem *> &)vec andIndex:(int)nIdx
{
	PhoneBookItem *pCall = vec[nIdx];
	if(!pFolderDelete || !pCall)
		return;
	
	m_pvResultCall.push_back(pCall);
	pFolderDelete->DeleteItem(pCall);
	
	vec[nIdx] = NULL;
}

#pragma mark - IBActon method

- (IBAction)onCancel:(id)sender
{
	m_bCancel = YES;
}

- (std::vector<PhoneBookItem*>)getResultCall
{
	return m_pvResultCall;
}

- (CHFoldres*)newFolders
{
	CHFoldres *pFolders = NULL;
	
	try 
	{
		pFolders = new CHFoldres;
		pFolders->pFolderDelete = m_pJobFolderDelete;
		pFolders->pvCall = m_pvJobCall;
	} 
	catch (...)
	{
		return NULL;
	}
	
	return pFolders;
}

@end
