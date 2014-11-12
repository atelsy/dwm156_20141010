//
//  pbJobThreadDlg.mm
//  DCT
//
//  Created by mbj on 12-1-16.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "pbJobThreadDlg.h"
#import "DCTAppDelegate.h"
#import "pbViewControl.h"
#import "pbListCtrl.h"
#import "Win2Mac.h"
#import "nsstrex.h"

@implementation pbJobThreadDlg

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (void)awakeFromNib
{
	m_pComAgent = [TheDelegate getComAgent];
	
	if(m_Job == -1)
		return;
	
	[self InitCtrl];
	
	[self DoJob];
}

- (void)initWithParameter:(PbJobPara *)para
{
	m_pvContact = para->pvContact;
	m_pFolderFrom = para->pFolderFrom;
	m_pFolder = para->pFolder;
	m_Memory = para->s;
	m_Count = para->nCount;
	m_strTitle = [NSString getCStringFromNSString:para->strTitle];
	m_Job = para->job;
	m_pParentWnd = para->hParent;
	m_nCtlType = para->nCtrlType;
	m_bShowDlg = FALSE;
	m_bFinish = 1;
}

- (std::vector<PhoneBookItem*>)GetResultContactVec;
{
	return m_pvResultContact;
}

- (void)InitCtrl
{	
	[m_ctlProgress setMinValue:0];
	[m_ctlProgress setMaxValue:m_pvContact->size()];
	[m_ctlProgress setDoubleValue:0.0];
}

- (void)DoJob
{
	switch(m_Job)
	{
		case 0: // JOB_PBINSERTMOVE, JOB_PBDELETEMOVE
			m_pvResultContact.clear();
			[self Start:self withFrom:m_pFolderFrom andFolder:m_pFolder andVector:m_pvContact andCount:m_Count andMem:m_Memory];
			break;
		case 1: // JOB_PBCOPY
			m_pvResultContact.clear();
			[self Start:self withFolder:m_pFolder withVector:*m_pvContact andCount:m_Count];
			break;
	}
	
}

- (void)MoveItem:(PbFolder*)pFolderFrom
	  withFolder:(PbFolder*)pFolder
	   andVector:(std::vector<PhoneBookItem*>&)vec
		andIndex:(int)nIdx
		andCheck:(BOOL)bCheck
{	
	pbViewControl *pView = [TheDelegate getPbViewController];
	//  pbTreeCtrl *tree = pView.pbTree;
	
	PhoneBookItem *pContact = vec[nIdx];
	if(!pFolderFrom || !pFolder || !pContact) return;
	
	// Move Item
	pFolderFrom->MoveItemtoFolder(pContact, pFolder);
	
	// Check Clipboard
	if(bCheck)
		[pView CheckClipboard:pContact];
	
	m_pvResultContact.push_back(pContact);
	
	// Clear Vector
	vec[nIdx] = NULL;
}

- (void)InsertPbIndex:(std::vector<PhoneBookItem*>)vec withIndex:(int)nIndex
{
	while(nIndex < vec.size())
	{		
		[pbViewControl InsertFreeIdx:vec[nIndex]->GetMemType() withIdx:vec[nIndex]->GetIndex()];
		nIndex++;
	}
}

- (void)PbInsertMove
{
	[self InsertMoveNext:0];
}

- (void)PbDeleteMove
{
	[self DeleteNext:0];
}

- (void)PbUpdate
{
	m_nID = WM_PBTHREAD_UPDATE;
	
	// Compare Memory
	
	CString sOld, sNew;
	PbMemory *pMemory = NULL;
	
	if(m_pContact->GetMemType() == m_pContactNew->GetMemType())
		m_bSame = true;
	else 
		m_bSame = false;
	
	// Save Old Contact
	m_Contact = *m_pContact;
	
	if(![self UpdateContact:m_pContact withNew:m_pContactNew])
	{
		if(self != nil)
		{
			if(m_bSame == true)
				PostMessage(m_pParentWnd, m_nID, -1, NULL);
			else if((pMemory = [self NewMemory:m_pContactNew]))
				PostMessage(m_pParentWnd, m_nID, -1, (LPARAM)pMemory);
		}
		delete m_pContactNew;
		
		[self NotifyDlg];
	}
}

- (BOOL)WriteContact:(PhoneBookItem*)pContact
			 withMem:(CString)s
			andIndex:(int)nIndex
		   andNotify:(bool)bNotify
{
	m_nID = WM_PBTHREAD_INSERT;
	m_strMem = s;
	m_bNotify = bNotify;
	
	if(pContact)
	{
		// Write Message
		if (CA_OK == [self WritePBK:self withMsg:WM_PROCESSCONTACT andStep:RESP_PB_INSERTCONTACT andContact:pContact andMem:s andExt:nIndex])
			return true;
		else
		{
			if((self!=nil) && bNotify)
				SendMessage(self, m_nID, NULL, (LPARAM)pContact);
		}
	}
	else
	{
		if((self!=nil) && bNotify)
			SendMessage(self, m_nID, NULL, (LPARAM)pContact);
	}
	
	return FALSE;
}

- (BOOL)UpdateContact:(PhoneBookItem*)pContact withNew:(PhoneBookItem*)pContactNew
{
	if(pContact && pContactNew)
	{
		if (CA_OK == [self ReplacePBK:self withMsg:WM_PROCESSCONTACT andStep:RESP_PB_REPLACECONTACT andContact:pContact andNew:*pContactNew andExt:0])
			return true;
	}
	
	return false;
}

- (void)PbInsert
{
	if(![self WriteContact:m_pContact withMem:m_sMemory andIndex:1 andNotify:true])
		[self NotifyDlg];
}

- (void)PbCopy
{
	[self CopyNext:0];
}

- (PbFolders*)NewFolders
{
	PbFolders *pFolders = NULL;
	try
	{
		pFolders = new PbFolders;
		pFolders->pFolderFrom = m_pJobFolderFrom;
		pFolders->pFolderTo = m_pJobFolder;
		pFolders->pvContact = m_pvJobContact;
	}
	catch (...)
	{
		return NULL;
	}
	
	return pFolders;
}

- (PbMemory*)NewMemory:(PhoneBookItem *)pContact
{
	PbMemory *pMemory = NULL;
	try
	{
		if(pContact)
		{
			pMemory = new PbMemory;
			pMemory->nIdx = pContact->GetIndex();
			pMemory->sMemory = pContact->GetMemType();
		}
	}
	catch (...)
	{
		return NULL;
	}
	
	return pMemory;
}


- (BOOL)Start:(id)hWnd withFolder:(PbFolder*)pFolder andContact:(PhoneBookItem*)pContact andMem:(CString)sMemory //Insert
{
	if(!m_bFinish) return FALSE;
	
	if(pFolder && pContact)
	{
		m_nType = JOB_PBINSERT;
		m_pJobFolder = pFolder;
		m_pContact = pContact;
		m_sMemory = sMemory;
		
		[self Func];
		return TRUE;
	}
	
	return FALSE;
}

- (BOOL)Start:(id)hWnd withFolder:(PbFolder*)pFolder withIndex:(int)nItem andContact:(PhoneBookItem*)pContact andNew:(PhoneBookItem*)pContactNew //Update
{
	if(!m_bFinish) return FALSE;
	
	if(pFolder && pContact && pContactNew)
	{
		m_nType = JOB_PBUPDATE;
		m_pJobFolder = pFolder;
		m_nItem = nItem;
		m_pContact = pContact;
		m_pContactNew = pContactNew;
		
		[self Func];
		return TRUE;
	}
	
	return FALSE;
}

- (BOOL)Start:(id)hWnd withFolder:(PbFolder*)pFolder withVector:(std::vector<PhoneBookItem*>)vContact andCount:(int)nCount	// Copy
{
	if(!m_bFinish) return FALSE;
	
	if(pFolder)
	{
		m_nType = JOB_PBCOPY;
		m_pJobFolder = pFolder;
		m_vContact = vContact;
		m_nCount = nCount;
		
		[self Func];
		return TRUE;
	}
	
	return FALSE;
}

- (BOOL)Start:(id)hWnd withFrom:(PbFolder*)pFolderFrom andFolder:(PbFolder*)pFolder andVector:(std::vector<PhoneBookItem*>*)pvContact andCount:(int)nCount andMem:(CString)sMemory	// Move
{
	if(!m_bFinish) return FALSE;
	
	if(pFolderFrom && pFolder && pvContact)
	{
		if(!sMemory.IsEmpty())
			m_nType = JOB_PBINSERTMOVE;
		else
			m_nType = JOB_PBDELETEMOVE;
		
		m_pJobFolderFrom = pFolderFrom;
		m_pJobFolder = pFolder;
		m_pvJobContact = pvContact;
		m_vContact = *pvContact;
		m_nCount = nCount;
		m_sMemory = sMemory;
		
		[self Func];
		return TRUE;
	}
	
	return FALSE;
}

- (BOOL)GetFinish
{
	return m_bFinish;
}

- (void)DoCancel
{
	m_bCancel = true;
}

- (void)NotifyDlg
{
	//	[[self window] performClose:nil];
	PostMessage(m_pParentWnd, WM_PB_DESTROYDLG, m_nCtlType, 0);
}

- (DWORD)Func
{
	m_bFinish = false;
	m_bCancel = false;
	
	switch (m_nType)
	{
		case JOB_PBINSERT:
			[self PbInsert];
			break;
			
		case JOB_PBUPDATE:
			[self PbUpdate ];
			break;
			
		case JOB_PBCOPY:
			[self PbCopy];
			break;
			
		case JOB_PBDELETEMOVE:
			[self PbDeleteMove ];
			break;
			
		case JOB_PBINSERTMOVE:
			[self PbInsertMove ];
			break;
			
		default:
			break;
	}
	
	m_bFinish = true;
	
	return 0;
}

- (void)CopyEnd:(int)nNum
{	
	[self OnPbCopy:2 Success:FALSE andNum:nNum];
}

- (void)CopyNext:(int)nNum
{
	[self OnPbCopy:0 Success:TRUE andNum:nNum];
}

- (void)InsertMoveEnd:(int)nNum
{	
	[self OnPbInsertMove:2 Success:FALSE andNum:nNum];
}

- (void)InsertMoveNext:(int)nNum
{	
	[self OnPbInsertMove:0 Success:TRUE andNum:nNum];	
}

- (void)DeleteEnd
{
	[self NotifyDlg];
}

- (void)DeleteNext:(int)nNum
{
	CRespFlag *resp = new CRespFlag(TRUE, nNum);
	SendMessage(self, WM_PBDELETEMOVE, 0, (LPARAM)resp);
}

- (BOOL)GetSIMStatus
{
	pbViewControl *pView = [TheDelegate getPbViewController];
	return [pView getSimStatus];
}

- (CAERR)WritePBK:(id)hWnd withMsg:(UINT)uMsg andStep:(short)step 
	   andContact:(PhoneBookItem*)pPbk andMem:(const CString&) strMem andExt:(LONG)lExt
{
	if(![self GetSIMStatus])
		return CA_UNKNOWN;
	
	return m_pComAgent->WritePBK(hWnd, uMsg, step, pPbk, strMem, lExt);
}

- (CAERR)ErasePBK:(id)hWnd withMsg:(UINT)uMsg andStep:(short)step 
	   andContact:(PhoneBookItem*)pPbk andExt:(LONG)lExt
{
	if(![self GetSIMStatus])
		return CA_UNKNOWN;
	
	return m_pComAgent->ErasePBK(hWnd, uMsg, step, pPbk, lExt);
}

- (CAERR)ReplacePBK:(id)hWnd withMsg:(UINT)uMsg andStep:(short)step 
		 andContact:(PhoneBookItem*)pPbk andNew:(const PhoneBookItem&)newItem andExt:(LONG)lExt
{
	if(![self GetSIMStatus])
		return CA_UNKNOWN;
	
	return m_pComAgent->ReplacePBK(hWnd, uMsg, step, pPbk, newItem, lExt);
}


- (LRESULT)OnPbThreadInsert:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	// Update Progress
	[m_ctlProgress incrementBy:1.0];
	
	[self DisableButton];
	
	if(wParam && lParam)
	{
		PhoneBookItem *pContact = (PhoneBookItem*)lParam;
		m_pvResultContact.push_back(pContact);
		return 1;
	}
	else
	{
		if(lParam)
		{
			PhoneBookItem *pContact = (PhoneBookItem*)lParam;
			[pbViewControl InsertFreeIdx:pContact->GetMemType() withIdx:pContact->GetIndex()];
			delete pContact;
		}
		
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_WRITECONTACT") forType:MB_OK];
		[pbViewControl SetDownloadState:FALSE];
	}
	
	return 0;
}

- (LRESULT)OnPbThreadDeleteMove:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	// Update Progress
	[m_ctlProgress incrementBy:1.0];
	
	[self DisableButton];
	
	pbViewControl *pView = [TheDelegate getPbViewController];
	pbTreeCtrl *tree = pView.pbTree;
	
	int nItem = (int)wParam;
	if(nItem >= 0 && lParam)
	{
		PbFolders *pFolders = (PbFolders*)lParam;
		PbFolder *pFolderFrom = pFolders->pFolderFrom;
		PbFolder *pFolderTo = pFolders->pFolderTo;
		std::vector<PhoneBookItem*> &vec = *(pFolders->pvContact);
		PhoneBookItem *pContact = vec[nItem];
		
		// update current folder's number of contacts
		// if current folder is pFolderFrom
		if([tree IfSelectedFolder:pFolderFrom])
		{
			// Memory
			const CString &sMem = pContact->GetMemType();
			/*	if(sMem == _T("SM"))
			 pView->pbList.m_numSIM--;
			 else if(sMem == _T("ME"))
			 pView->pbList.m_numME--;*/
			[pView->pbList ModifyContactCount:sMem];
		}
		
		// Set Memory
		// Insert PbIndex
		[pbViewControl InsertFreeIdx:pContact->GetMemType() withIdx:pContact->GetIndex()];
		// Change Memory
		pContact->SetMemType(_T("PC"));
		
		// Move Item
		BOOL bCheck = FALSE;
		if(pFolders->pvContact != &pView->m_vContact)
			bCheck = TRUE;
		
		[self MoveItem:pFolderFrom withFolder:pFolderTo andVector:vec andIndex:nItem andCheck:bCheck];
		// Clear Memory
		delete pFolders;
		return 1;
	}
	else
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_MOVECONTACT2") forType:MB_OK];
	}
	
	return 0;
}

- (LRESULT)OnPbThreadInsertMove:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	// Update Progress
	[m_ctlProgress incrementBy:1.0];
	
	[self DisableButton];
	
	pbViewControl *pView = [TheDelegate getPbViewController];
	
	int nItem = (int)wParam;
	if(nItem >= 0 && lParam)
	{
		PbFolders *pFolders = (PbFolders*)lParam;
		PbFolder *pFolderFrom = pFolders->pFolderFrom;
		PbFolder *pFolderTo = pFolders->pFolderTo;
		std::vector<PhoneBookItem*> &vec = *(pFolders->pvContact);
		
		// Move Item
		BOOL bCheck = FALSE;
		if(pFolders->pvContact != &pView->m_vContact)
			bCheck = TRUE;
		
		[self MoveItem:pFolderFrom withFolder:pFolderTo andVector:vec andIndex:nItem andCheck:bCheck];
		
		// Clear Memory
		delete pFolders;
		return 1;
	}
	else
	{
		// Move to Handset
		if(lParam)
		{
			PhoneBookItem *pContact = (PhoneBookItem*)lParam;
			
			// Insert PbIndex
			[pbViewControl InsertFreeIdx:pContact->GetMemType() withIdx:pContact->GetIndex()];
			// Change Memory
			pContact->SetMemType(_T("PC"));
			
			[pbViewControl SetDownloadState:FALSE];
		}
		
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_MOVECONTACT2") forType:MB_OK];
	}
	return 0;
}

- (LRESULT)OnProcessContact:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	WATCH_POINTER(lParam);
	
	CRespFlag *pResp = (CRespFlag *)lParam;
	
	PbMemory *pMemory = NULL;
	id hWnd;
	switch(LOWORD(wParam))
	{
		case RESP_PB_INSERTCONTACT:
			if (m_nType == JOB_PBINSERT)
				hWnd = m_pParentWnd;
			else 
				hWnd = self;
			if (m_nType != JOB_PBINSERTMOVE)
			{
				if (pResp->Value())
				{
					m_pContact->SetMemType(m_strMem);
					if((hWnd!=nil) && m_bNotify)	
						SendMessage(hWnd, m_nID, (WPARAM)m_pJobFolder, (LPARAM)m_pContact);
				}
				else
				{
					if((hWnd!=nil) && m_bNotify)
						SendMessage(hWnd, m_nID, NULL, (LPARAM)m_pContact);
					
					if (m_nType == JOB_PBCOPY)
						[self OnPbCopy:2 Success:pResp->Value() andNum:(int)pResp->m_lExt];
					else
						[self NotifyDlg];
					break;
				}
			}				
			if (m_nType == JOB_PBINSERT)
				[self NotifyDlg];
			else if (m_nType == JOB_PBCOPY)
				[self OnPbCopy:1 Success:pResp->Value() andNum:(int)pResp->m_lExt];
			else if (m_nType == JOB_PBINSERTMOVE)	
				[self OnPbInsertMove:1 Success:pResp->Value() andNum:(int)pResp->m_lExt];
			break;
		case RESP_PB_REPLACECONTACT:
			if (pResp->Value())
			{
				if(self)
				{
					if(m_bSame)
						PostMessage(m_pParentWnd, m_nID, m_nItem, NULL);
					else if((pMemory = [self NewMemory:&m_Contact]))
						PostMessage(m_pParentWnd, m_nID, m_nItem, (LPARAM)pMemory);
				}
			}
			else
			{
				if(self)
				{
					if(m_bSame)
						PostMessage(m_pParentWnd, m_nID, -1, NULL);
					else if((pMemory = [self NewMemory:m_pContactNew]))
						PostMessage(m_pParentWnd, m_nID, -1, (LPARAM)pMemory);
				}
			}
			delete m_pContactNew;
			[self NotifyDlg];
			break;
		default:
			break;
	}
	return 0;
}

- (LRESULT)OnPbInsertMove:(int)step Success:(BOOL)bSuccess andNum:(int)nNum;
{	
	PbFolders *pFolders;
	int i = nNum;
	
	switch(step)
	{
		case 0:
			if (i<m_vContact.size() && i<m_nCount)
			{
				m_pContact = m_vContact[i];		
				if(m_pContact)
				{
					// Get Memory
					if(![self WriteContact:m_pContact withMem:m_pContact->GetMemType() andIndex:i andNotify:true])
					{
						if(self)
							SendMessage(self, WM_PBTHREAD_INSERTMOVE, -1, (LPARAM)m_pContact);
						[self InsertMoveEnd:nNum];
						break;
					}
				}
				else
				{
					//TRACE(_T("case 0 Move next num = %d\n"), i+1);
					[self InsertMoveNext:i+1];
				}
				if (m_bCancel)
				{
					[self InsertPbIndex:m_vContact withIndex:i+1];
					[self InsertMoveEnd:nNum];
					break;
				}
			}
			else
				[self InsertMoveEnd:nNum];
			break;
		case 1:
			if (bSuccess)
			{
				if((self != NULL) && (pFolders = [self NewFolders]))
				{
					if(!SendMessage(self, WM_PBTHREAD_INSERTMOVE, i, (LPARAM)pFolders))
						delete pFolders;
				}
			}
			else
			{
				if(self)
					SendMessage(self, WM_PBTHREAD_INSERTMOVE, -1, (LPARAM)m_pContact);
				[self InsertMoveEnd:nNum];
				break;
			}
			if(m_bCancel)
			{
				[self InsertPbIndex:m_vContact withIndex:i+1];
				[self InsertMoveEnd:nNum];
				break;
			}
			[self InsertMoveNext:i+1];
			break;
		case 2:
			i++;
			for(;(i<m_vContact.size() && i<m_nCount);i++)
			{
				[self InsertPbIndex:m_vContact withIndex:i];
				if(m_vContact[i])
					m_vContact[i]->SetMemType(_T("PC"));
			}
			
			[self NotifyDlg];
			break;
	}
	return 0; 
}

- (LRESULT)OnPbDeleteMove:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	WATCH_POINTER(lParam);
	CRespFlag *p = (CRespFlag*)lParam;
	int i = (int)p->m_lExt;
	BOOL bSuccess = p->Value();
	int step = LOWORD(wParam);
	PhoneBookItem *pContact;
	PbFolders *pFolders;
	
	switch(step)
	{
		case 0:
			if (i<m_vContact.size() && bSuccess)
			{
				pContact = m_vContact[i];
				if(pContact)
				{
					if (CA_OK != [self ErasePBK:self withMsg:WM_PBDELETEMOVE andStep:1 andContact:pContact andExt:i])						
					{
						if(self)
							SendMessage(self, WM_PBTHREAD_DELETEMOVE, -1, NULL);
						[self NotifyDlg];
						break;
					}
					
				}
				else
					[self DeleteNext:i+1];
			}
			else
				[self DeleteEnd];
			break;
		case 1:
			if (bSuccess)
			{
				if((self != NULL) && (pFolders = [self NewFolders]))
				{
					if(!SendMessage(self, WM_PBTHREAD_DELETEMOVE, i, (LPARAM)pFolders))
						delete pFolders;
				}
			}
			else if(self)
			{
				SendMessage(self, WM_PBTHREAD_DELETEMOVE, -1, NULL);
				[self NotifyDlg];
				break;
			}
			
			if(m_bCancel)
			{
				[self NotifyDlg];
				break;
			}
			[self DeleteNext:i+1];
			break;
		case 2:
			break;
	}
	return 0;
}

- (LRESULT)OnPbCopy:(int)step Success:(BOOL)bSuccess andNum:(int)nNum
{
	int i = nNum;
	
	switch(step)
	{
		case 0:
			if (i<m_vContact.size() && i<m_nCount)
			{
				m_pContact = m_vContact[i];
				if(m_pContact)
				{
					// Get Memory
					m_sMemory = m_pContact->GetMemType();
					
					// Write Message
					if(![self WriteContact:m_pContact withMem:m_sMemory andIndex:i andNotify:true])
					{
						[self CopyEnd:nNum];
						break;
					}
				}
				if(m_bCancel)
				{
					[self InsertPbIndex:m_vContact withIndex:i+1];
					[self CopyEnd:nNum];
				}
			}
			else
				[self CopyEnd:nNum];
			break;
		case 1:
			if (bSuccess && !m_bCancel)
			{
				[self CopyNext:i+1];
				//TRACE(_T("copy the next contact\n"));
			}
			else
			{
				if (m_bCancel)
					[self InsertPbIndex:m_vContact withIndex:i+1];
				[self CopyEnd:nNum];
			}
			break;
		case 2:
			i++;
			for(;(i<m_vContact.size() && i<m_nCount);i++)
			{
				if(m_vContact[i])
					delete m_vContact[i];
			}
			[self NotifyDlg];
			break;
		default:
			break;
	}
	return 0;
}

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	switch(Msg)
	{
		case WM_PBTHREAD_INSERT:
			[self OnPbThreadInsert:wparam andLparam:lparam];
			break;
		case WM_PBTHREAD_DELETEMOVE:
			[self OnPbThreadDeleteMove:wparam andLparam:lparam];
			break;
		case WM_PBTHREAD_INSERTMOVE:
			[self OnPbThreadInsertMove:wparam andLparam:lparam];
			break;
		case WM_PROCESSCONTACT:
			[self OnProcessContact:wparam andLparam:lparam];
			break;
		case WM_PBDELETEMOVE:
			[self OnPbDeleteMove:wparam andLparam:lparam];
			break;
	}
	
}

-(IBAction)OnCancel:(id)sender
{
	[self DoCancel];
}

- (BOOL)windowShouldClose:(id)sender
{
	[self DoCancel];
	return NO;
}

- (void)DisableButton
{
	double nProgress = [m_ctlProgress doubleValue];
	double nMax = [m_ctlProgress maxValue];
	
	if(fabs(nMax - nProgress) <= 1.0f)
	{
		[m_ctlButton setEnabled:NO];
	}
}

@end
