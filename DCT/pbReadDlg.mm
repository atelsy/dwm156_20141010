//
//  pbReadDlg.mm
//  DCT
//
//  Created by mbj on 12-1-16.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "pbReadDlg.h"
#import "DCTAppDelegate.h"
#import "pbViewControl.h"
#import "Win2Mac.h"

@implementation pbReadDlg

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
	[self InitCtrl];
	
	[pbViewControl SetDownloadState:FALSE];
	[pbViewControl ClearAllIdx];
	
	[self DoJob];
}

- (IBAction)OnCancel:(id)sender
{
	m_bCancel = YES;
}

- (BOOL)GetNext
{
	if(m_bCancel)
	{
		//CPBFormView::m_mIndex.clear();
		[pbViewControl ClearAllIdx];
		return FALSE;
	}
	
	// Add m_nItem
	m_nItem++;
	// 	m_nReadItem++;
	if(m_nItem <= HIWORD(m_vTotal[m_nIdx]) && m_nReadItem < LOWORD(m_vTotal[m_nIdx]))
		return TRUE;
	
	for (int i=m_nItem; i<=HIWORD(m_vTotal[m_nIdx]); i++)
	{
		//CPBFormView::m_mIndex.Insert(GetMemory(m_nIdx), i);
		[pbViewControl InsertFreeIdx:[self GetMemory:m_nIdx] withIdx:i];
	}
	
	// Add m_nIdx
	m_nIdx++;
	if(m_nIdx < [self GetMemorySize])
	{
		m_nItem = 1;
		m_nReadItem = 0;
		[self InitCtrl];
		return TRUE;
	}
	
	//CPBFormView::m_bDownloadComplete = TRUE;
	[pbViewControl SetDownloadState:TRUE];
	return FALSE;
}

- (BOOL)InsertContact:(PbFolder*)pFolder
{
	PhoneBookItem *pContact = [utility NewContact];
	if(pFolder && pContact)
	{
		*pContact = m_Contact;
		pFolder->InsertItem(pContact);
	}
	
	return FALSE;
}

- (void)DoJob
{
	if ( CA_OK != m_pComAgent->ReadPBK(self, WM_RSPCALLBACK, RESP_PB_READ, &m_Contact, 
									   [self GetMemory:m_nIdx], m_nItem, (long)&m_Contact))
	{
		m_bCancel = TRUE;
		PostMessage(self, WM_STATUS, m_nIdx, FALSE);
	}
}

- (void)InitCtrl
{
	NSString *string = [m_ctlStatic stringValue];
	
	CString memory = [self GetMemory:m_nIdx];
	
	if(memory == _T("SM"))
		[string stringByAppendingString:@"SIM ..."];
	else
		[string stringByAppendingString:@"ME ..."];
	
	[m_ctlStatic setStringValue:string];
	
	[m_ctlProgress setMinValue:0];
	
	int nMax = LOWORD(m_vTotal[m_nIdx]);
	[m_ctlProgress setMaxValue:nMax];
	[m_ctlProgress setDoubleValue:0.0];
}

- (BOOL)Init:(PbFolder*)pFolder andParentWnd:(id)pParent
{
	m_nIdx = 0;
	m_nItem =1;
	m_nReadItem =0;
	m_bCancel = FALSE;
	m_pFolder = pFolder;	
	m_bShowDlg = FALSE;
	m_pParentWnd = pParent;
	
	m_pComAgent = [TheDelegate getComAgent];
	
	return TRUE;
}


- (void)SetTotalNumber:(BOOL)flag andTotal:(int)nTotal
{
	if (flag)
		m_vTotal.push_back(nTotal);	
}

- (CString)GetMemory:(int)idx
{
	pbViewControl *p = (pbViewControl*)m_pParentWnd;
	return [p GetMemory:idx];
}

- (size_t)GetMemorySize
{
	pbViewControl *p = (pbViewControl*)m_pParentWnd;
	return [p GetMemorySize];
}


- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	switch(Msg)
	{
			//	case WM_URCCALLBACK:
			//		[self OnURCCallBack:wparam andLparam:lparam];
			//		break;
		case WM_RSPCALLBACK:
			[self OnRspCallBack:wparam andLparam:lparam];
			break;
		case WM_STATUS:
			[self OnJobStatus:wparam andLparam:lparam];
			break;
	}
}

- (LRESULT)OnJobStatus:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	if(lParam)
	{
		[self InsertContact:m_pFolder];
		
		m_nReadItem++;
		
		// Update Progress
		// m_ctlProgress.StepIt();
		[m_ctlProgress incrementBy:1.0];
		[self DisableButton];
	}
	else	// Insert PbIndex
	{
		if(m_nItem <= HIWORD(m_vTotal[m_nIdx]))
			//CPBFormView::m_mIndex.Insert(GetMemory(m_nIdx), m_nItem);
			[pbViewControl InsertFreeIdx:[self GetMemory:m_nIdx] withIdx:m_nItem];
	}
	
	
	// Get Next Message
	if([self GetNext])
		[self DoJob];
	else		
	{
		//DestroyWindow();
		PostMessage(m_pParentWnd, WM_PB_DESTROYDLG, PB_DLG_READ, 0);	
		
	}
	
	return 0;
}

- (LRESULT)OnRspCallBack:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	WATCH_POINTER(lParam);
	CRespFlag *pRespFlag;
	switch(LOWORD(wParam))
	{
		case RESP_PB_READ:
			pRespFlag = (CRespFlag *)lParam;
			if (pRespFlag->Value())
				PostMessage(self, WM_STATUS, m_nIdx, TRUE);
			else
				PostMessage(self, WM_STATUS, m_nIdx, FALSE);
			break;
		default:
			break;	
	}
	return TRUE;
}
#if 0
- (LRESULT)OnURCCallBack:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	WATCH_POINTER(lParam);
	//CUrcInt *pUrc = (CUrcInt*)lParam;
	switch(wParam)
	{
		case URC_SYS:
			/*
			 if (pUrc->GetValue() == SYS_ATREADY)
			 EnableCtrl(TRUE);
			 else if (pUrc->GetValue() == SYS_PUKREQ)
			 EnableCtrl(FALSE);
			 */
		 	break;
		default:
			break;
	}
	return TRUE;
}
#endif

- (BOOL)windowShouldClose:(id)sender
{
	m_bCancel = TRUE;
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
