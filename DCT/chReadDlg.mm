//
//  chReadDlg.m
//  DCT
//
//  Created by mtk on 12-6-8.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import "chReadDlg.h"
#import "DCTAppDelegate.h"
#import "callHistoryViewController.h"

@implementation chReadDlg

@synthesize m_bShowDlg;

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) 
	{
		// Initialization code here.
		m_bCancel = NO;
		m_nIdx = 0;
		m_nItem = 1;
		m_vIndex.reserve(3);
		m_vIndex[0] = m_vIndex[1] = m_vIndex[2] = 1;
		
		m_bShowDlg = NO;
	}
	
	return self;
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	
	[self initCtrl];
	
	[self doJob];
}

#pragma mark - init

- (BOOL)initMissCallFolder:(ChFolder *)pMissCallFolder ResvCallFolder:(ChFolder *)pResvCallFolder andDialCallFolder:(ChFolder *)pDialCallFolder withParent:(callHistoryViewController *)pParent
{
	m_pMissCallFolder = pMissCallFolder;
	m_pResvCallFolder = pResvCallFolder;
	m_pDialCallFolder = pDialCallFolder;
	m_pParent = pParent;
	
	m_vMemory.clear();
	m_vMemory.push_back(_T("MC"));
	m_vMemory.push_back(_T("RC"));
	m_vMemory.push_back(_T("LD"));
	
	pComAgent = [self getComAgent];
	if(pComAgent == NULL)
		return NO;
	
	if(CA_OK == pComAgent->GetPBKNumber(m_pParent, WM_RSPCALLBACK, RESP_CH_GETMISSCALLNUM, _T("MC")))
		return YES;
	
	return NO;
}

- (void)initCtrl
{
	[m_ctlProgress setMinValue:0.0f];
	
	[m_ctlProgress setMaxValue:m_vTotal[m_nIdx]];
	
	[m_ctlProgress setDoubleValue:0.0];
}

- (void)setMissCallNumber:(int)nMissTotal withFlag:(BOOL)bFlag
{
	if(bFlag)
		m_vTotal.push_back(nMissTotal);
	else
		m_vMemory.erase(m_vMemory.begin());
}

- (void)setReceiveCallNumber:(int)nRecvTotal withFlag:(BOOL)bFlag
{
	if(bFlag)
		m_vTotal.push_back(nRecvTotal);
	else
		m_vMemory.erase(m_vMemory.begin() + 1);
}

- (void)setDialCallNumber:(int)nDialTotal withFlag:(BOOL)bFlag
{
	if(bFlag)
		m_vTotal.push_back(nDialTotal);
	else
		m_vMemory.erase(m_vMemory.begin() + 2);
}

- (void)getCallNumber:(int)nStep
{
	switch (nStep)
	{
		case RESP_CH_GETRECEIVECALLNUM:
			pComAgent->GetPBKNumber(m_pParent, WM_RSPCALLBACK, RESP_CH_GETRECEIVECALLNUM, _T("RC"));
			break;
			
		case RESP_CH_GETDIALCALLNUM:
			pComAgent->GetPBKNumber(m_pParent, WM_RSPCALLBACK, RESP_CH_GETDIALCALLNUM, _T("LD"));
			break;
			
		default:
			break;
	}
}

#pragma mark - insert folder method

- (BOOL)insertMissCall:(ChFolder *)pFolder withIndex:(int)nIdx
{
	return [self insertCall:pFolder withIndex:nIdx];
}

- (BOOL)insertResvCall:(ChFolder *)pFolder withIndex:(int)nIdx
{
	return [self insertCall:pFolder withIndex:nIdx];
}

- (BOOL)insertDialCall:(ChFolder *)pFolder withIndex:(int)nIdx
{
	return [self insertCall:pFolder withIndex:nIdx];
}

- (BOOL)insertCall:(ChFolder *)pFolder withIndex:(int)nIdx
{
	PhoneBookItem *pMissCall = [utility NewContact];
	
	if(pFolder && pMissCall)
	{
		*pMissCall = m_Call;
		pMissCall->SetIndex(nIdx);
		pFolder->InsertItem(pMissCall);
		
		return YES;
	}
	
	return NO;
}

#pragma mark - download job

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	switch (Msg)
	{
		case WM_STATUS:
			[self onJobStatus:wparam andLparam:lparam];
			break;
			
		case WM_RSPCALLBACK:
			[self onRspCallBack:wparam andLparam:lparam];
			break;
			
		default:
			break;
	}
}

- (void)doJob
{
	if(CA_OK != pComAgent->ReadPBK(self, WM_RSPCALLBACK, RESP_CH_READ, &m_Call, m_vMemory[m_nIdx], m_nItem, (long)&m_Call))
	{
		m_bCancel = YES;
		PostMessage(self, WM_STATUS, m_nIdx, NO);
	}
}

- (BOOL)getNext
{
	if(m_bCancel)
		return NO;
	
	m_nItem++;
	
	if(m_nItem <= m_vTotal[m_nIdx])
	{
		m_vIndex[m_nIdx] = m_nItem;
		return YES;
	}
	
	m_nIdx++;
	
	if(m_nIdx < m_vMemory.size())
	{
		m_nItem = 1;
		[self  initCtrl];
		
		return YES;
	}
	
	return NO;
}

- (LRESULT)onJobStatus:(long)wParam andLparam:(long)lParam
{
	if(lParam)
	{
		if(m_nIdx == 0)
			[self insertMissCall:m_pMissCallFolder withIndex:m_vIndex[m_nIdx]];
		else if(m_nIdx == 1)
			[self insertResvCall:m_pResvCallFolder withIndex:m_vIndex[m_nIdx]];
		else if(m_nIdx == 2)
			[self insertDialCall:m_pDialCallFolder withIndex:m_vIndex[m_nIdx]];
	}
	
	[m_ctlProgress incrementBy:1.0];
	
	if([self getNext])
	{
		[self doJob];
	}
	else
	{
		PostMessage(m_pParent, WM_CH_DESTROYDLG, CH_DLG_READ, 0);
	}
	
	return 0;
}

- (LRESULT)onRspCallBack:(long)wParam andLparam:(long)lParam
{
	WATCH_POINTER(lParam);
	
	CRespFlag *pRespFlag;
	
	switch (LOWORD(wParam))
	{
		case RESP_CH_READ:
			pRespFlag = (CRespFlag*)lParam;
			
			if(pRespFlag->Value())
				PostMessage(self, WM_STATUS, m_nIdx, YES);
			else
				PostMessage(self, WM_STATUS, m_nIdx, NO);
			
			break;
			
		default:
			break;
	}
	
	return 1;
}

#pragma mark - help method

- (CComAgent*)getComAgent
{
	DCTAppDelegate *delegate = TheDelegate;
	
	return [delegate getComAgent];
}

- (IBAction)onCancel:(id)sender
{
	m_bCancel = YES;
}

@end
