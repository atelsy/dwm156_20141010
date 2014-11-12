//
//  smsReadDlg.m
//  DCT
//
//  Created by Fengping Yu on 12/17/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsReadDlg.h"
#import "DCTAppDelegate.h"
#import "SMSViewCtrl.h"
#import "nsstrex.h"

@implementation smsReadDlg

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) 
	{
		// Initialization code here.
		//[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onJobStatus:) name:WM_SMSREAD_STATUS object:nil];
	}
	
	return self;
}

- (void)dealloc
{
	[m_vMemory release];
	[m_vCurMemType release];
	
	[super dealloc];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (void)awakeFromNib
{
	[self initCtrl];
	
	m_nItem = m_vBgn[0];
	
	[self doJob];
}

- (BOOL)init:(SmsFolder *)pFolderInbox andOutbox:(SmsFolder *)pFolderOutbox
{
	m_nIdx = 0;
	m_nItem = 0;
	m_nReadItem = 0;
	m_bCancel = NO;
	m_bMissed = NO;
	
	m_pFolderInbox = pFolderInbox;
	m_pFolderOutbox = pFolderOutbox;
	
	m_vMemory = [[NSMutableArray alloc] init];
	m_vCurMemType = [[NSMutableArray alloc] init];
	
	// m_vMemory.clear();
	m_vUsed.clear();
	m_vEnd.clear();
	m_vBgn.clear();
	//m_vCurMemType.clear();
	
	[m_vMemory addObject:@"ME"];
	[m_vMemory addObject:@"SM"];
	
	m_pCom = [TheDelegate getComAgent];
	
	int i;
	CString memory;
	
	SMSViewCtrl *smsView = [TheDelegate getSmsViewController];
	
	for(i = 0; i < [m_vMemory count]; i++)
	{
		memory = [NSString getCStringFromNSString:[m_vMemory objectAtIndex:i]];
		
		if(memory.Compare(_T("SM")) == 0)
		{
			if(CA_OK != m_pCom->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, memory, READ_SM))
				return NO;
		}
		else if(memory.Compare(_T("ME")) == 0)
		{
			if(CA_OK != m_pCom->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, memory, READ_ME))
				return NO;
		}
	}
	
	if(i > 0)
	{
		memory = [NSString getCStringFromNSString:[m_vMemory objectAtIndex:i-1]];
		
		if(CA_OK != m_pCom->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, memory, READ_RET))
			return NO;
	}
	
	return YES;
}

- (void)initCtrl
{
	NSString *string = [m_ctlStatic stringValue];
	
	CString memory = [NSString getCStringFromNSString:[m_vMemory objectAtIndex:m_nIdx]];
	
	if(memory == _T("SM"))
		[string stringByAppendingString:@"SIM ..."];
	else
		[string stringByAppendingString:@"ME ..."];
	
	[m_ctlStatic setStringValue:string];
	
	[m_ctlProgress setMinValue:0];
	
	[m_ctlProgress setMaxValue:m_vUsed[m_nIdx]];
	[m_ctlProgress setDoubleValue:0.0];
	
	[m_ctlButton setEnabled:YES];
}

- (void)doJob
{
	CString memory = [NSString getCStringFromNSString:[m_vMemory objectAtIndex:m_nIdx]];
	
	if(CA_OK != m_pCom->ReadSMS(self, WM_RSPCALLBACK, RESP_SMS_READ, &m_Msg, memory, m_nItem, (long)&m_Msg))
	{
		m_bCancel = YES;
		
		[self postSmsReadMessage:m_nIdx status:NO];
	}
}

- (void)onCancel:(id)sender
{
	m_bCancel = YES;
}

- (void)disableButton
{
	double nProgress = [m_ctlProgress doubleValue];
	double nMax = [m_ctlProgress maxValue];
	
	if(fabs(nMax - nProgress) <= 1.0f)
	{
		[m_ctlButton setEnabled:NO];
	}
}

- (void)setTotalNumber:(signed char)flag 
				memory:(NSString *)memory 
			withVector:(const std::vector<int> &)vec
{
	if(!flag)
	{
		m_nSimTotal = 0;
		return;
	}
	
	m_vBgn.push_back(vec[0]);
	m_vEnd.push_back(vec[1]);
	
	if(vec[2] > 0)
	{
		m_vUsed.push_back(vec[2]);
	}
	else
	{
		m_vUsed.push_back(0);
	}
}

- (void)setRcvMsgFlag:(const CMsgInd &)urc
{
	if(m_bMissed == YES)
		return;
	
	CString memory = [NSString getCStringFromNSString:[m_vMemory objectAtIndex:m_nIdx]];
	
	if(urc.mem == memory && urc.index > m_nItem)
		m_bMissed = NO;
	else if(urc.mem == _T("ME") && memory == _T("SM"))
		m_bMissed = YES;
	else if(urc.mem == _T("SM") && memory == _T("ME"))
		m_bMissed = NO;
	else
		m_bMissed = YES;
}

- (BOOL)getRcvMsgFlag
{
	return m_bMissed;
}

- (void)insertMessage:(SmsFolder *)pFolder
{
	SMSMessage *pMsg = [utility newMessage];
	
	if(pFolder && pMsg)
	{
		*pMsg = m_Msg;
		pFolder->InsertItem(pMsg);
	}
}

- (BOOL)getNext
{
	if(m_bCancel == YES)
		return NO;
	
	m_nItem++;
	
	if((m_nReadItem < m_vUsed[m_nIdx]) && (m_nItem <= m_vEnd[m_nIdx]))
		return YES;
	
	m_nIdx++;
	if(m_nIdx < [m_vMemory count])
	{
		m_nItem = m_vBgn[m_nIdx];
		m_nReadItem = 0;
		
		[self initCtrl];
		
		return YES;
	}
	
	return NO;
}

- (void)onJobStatus:(NSDictionary *)dict
{
	BOOL result = [[dict objectForKey:ITEM_BOOL] boolValue];
	
	if(result == YES)
	{
		switch (m_Msg.GetStatus())
		{
			case REC_READ:
			case REC_UNREAD:
			{
				SMSViewCtrl *smsView = [TheDelegate getSmsViewController];
				m_Msg.SetMsgIndex(m_nItem);
				[smsView updateMessageStatusInex:&m_Msg withItem:m_nItem];
				[self insertMessage:m_pFolderInbox];
			}
				break;
				
			case STO_UNSENT:
			case STO_SENT:
				[self insertMessage:m_pFolderOutbox];
				break;
				
			default:
				[self insertMessage:m_pFolderOutbox];
				break;
		}
		
		[m_ctlProgress incrementBy:1.0];
		
		[self disableButton];
	}
	
	if([self getNext] == YES)
		[self doJob];
	else
	{
		m_pFolderInbox->ProcessSMSItems();
		m_pFolderOutbox->ProcessSMSItems();
		
		[utility processSMSFolder:m_pFolderInbox];
		[utility processSMSFolder:m_pFolderOutbox];
		
		[[NSNotificationCenter defaultCenter] postNotificationName:WM_SMS_DESTROYDIALOG object:self userInfo:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:SMS_DLG_READ],ITEM_MSGID, nil]];
	}
	
}

- (void)windowWillClose:(NSNotification *)notification
{
	NSLog(@"window will close");
	m_bCancel = YES;
}

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	if(Msg == WM_RSPCALLBACK)
	{
		WATCH_POINTER(lparam);
		CRespFlag *pResp = (CRespFlag*)lparam;
		
		switch (LOWORD(wparam))
		{
			case RESP_SMS_READ:
				if(pResp->Value())
				{
					m_nReadItem++;
					
					[self postSmsReadMessage:m_nIdx status:YES];
				}
				else
				{
					[self postSmsReadMessage:m_nIdx status:NO];
				}
				
				break;
				
			default:
				break;
		}
	}
}

- (void)postSmsReadMessage:(int)nIndex status:(signed char)status
{
	NSMutableDictionary *dict = [[[NSMutableDictionary alloc] initWithCapacity:3] autorelease];
	[dict setObject:[NSNumber numberWithInt:m_nIdx] forKey:ITEM_INDEX];
	[dict setObject:[NSNumber numberWithBool:status] forKey:ITEM_BOOL];
	
	[self performSelectorOnMainThread:@selector(onJobStatus:) withObject:dict waitUntilDone:NO];
}
/*
 - (void)updateMessageStatusInex
 {
 DCTAppDelegate *delegate = TheDelegate;
 
 eMemStatus status = m_Msg.GetStatus();
 m_Msg.SetMsgIndex(m_nItem);
 
 std::map<int, BOOL>::iterator iter = delegate.m_indexMap.msgStatusIndexMap.find(m_nItem);
 
 size_t count = delegate.m_indexMap.msgStatusIndexMap.size();
 
 if(iter != delegate.m_indexMap.msgStatusIndexMap.end() && count > 0)
 {
 if((*iter).second == TRUE)
 m_Msg.SetStatus(REC_READ);
 else if((*iter).second == FALSE)
 m_Msg.SetStatus(REC_UNREAD);
 }
 else
 {
 if(status == REC_READ)
 delegate.m_indexMap.msgStatusIndexMap.insert(std::make_pair(m_nItem, TRUE));
 else if(status == REC_UNREAD)
 delegate.m_indexMap.msgStatusIndexMap.insert(std::make_pair(m_nItem, FALSE));
 }
 }
 */
@end
