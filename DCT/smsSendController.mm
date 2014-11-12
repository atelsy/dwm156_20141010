//
//  smsSendController.m
//  DCT
//
//  Created by Fengping Yu on 11/8/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsSendController.h"
#import "DCTAppDelegate.h"
#import "SMSViewCtrl.h"
#import "nsstrex.h"

#define WM_SMSSEND_STATUS   @"onMsgSendStatus"

@implementation sendStatus

- (id)init
{
	self = [super init];
	
	if(self)
	{
		// Initialize code here
	}
	
	return self;
}

- (void)dealloc
{
	[phoneNumber release];
	[status release];
	[super dealloc];
}

@synthesize phoneNumber, status;

@end

@implementation smsSendController

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) 
	{
		// Initialization code here.
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSendJobStatus:) name:WM_SMSSEND_STATUS object:nil];
		
		status = [[NSMutableArray alloc] init];
		
		m_pComAgent = [TheDelegate getComAgent];
	}
	
	return self;
}

- (void)awakeFromNib
{
	[self initCtrl];
	
	if([self doJob] == NO)
		[self postNotification:NO];
}

- (void)dealloc
{
	[self setStatus:nil];
	
	[super dealloc];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (IBAction)onCancel:(id)sender
{
	if(m_bFinish)
	{
		if([NSApp isHidden])
			[self.window orderOut:sender];
		else
			[NSApp stopModal];
		
		[[NSNotificationCenter defaultCenter] postNotificationName:WM_SMS_DESTROYDIALOG object:self userInfo:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:SMS_DLG_SEND],ITEM_MSGID, nil]];
	}
	else
		m_bCancel = YES;
}

#pragma mark--
#pragma mark init operation
#pragma mark--

- (BOOL)initWithMsg:(std::vector<SMSMessage *> *)pvMsg andOutboxFolder:(SmsFolder *)pFolderOutbox SentFoler:(SmsFolder *)pFolderSentbox
{
	size_t nIdx;
	CString str, strLeft;
	
	m_nIdx = 0;
	m_npIdx = -1;
	m_bFinish = NO;
	m_bCancel = NO;
	m_pvMsg = pvMsg;
	m_pFolderOutbox = pFolderOutbox;
	m_pFolderSentbox = pFolderSentbox;
	
	SMSMessage msg;
	vector<SMSMessage*>& vec = *m_pvMsg;
	int nSize = (int)vec.size();
	m_vMsg.clear();
	
	for(int i = 0; i < nSize; i++)
	{
		vec[i]->GetAddressNumber(str);
		
		while (!str.IsEmpty())
		{
			nIdx = str.FindOneOf(_T(",;"));
			if(nIdx == -1)
			{
				strLeft = str;
				str.Empty();
			}
			else
			{
				strLeft = str.Left(nIdx);
				str = str.Right(str.GetLength() - nIdx - 1);
			}
			
			if(!strLeft.IsEmpty())
			{
				msg = *vec[i];
				msg.SetAddressNumber((LPCTSTR)strLeft);
				m_vMsg.push_back(msg);
				m_vIdx.push_back(i);
			}
		}
	}
	
	if(m_vMsg.size() > 0)
		return YES;
	
	return NO;
}

- (void)initCtrl
{
	int nSize = (int)m_vMsg.size();
	
	if(nSize == 1)
		[cancelButton setEnabled:NO];
	
	[self initStatic:1];
	
	[progress setMinValue:0];
	[progress setMaxValue:nSize];
	
	[progress setDoubleValue:0];
	
	[cancelButton setTitle:LocalizeString(@"IDS_APP_CANCEL")];
	
	CString s;
	
	NSString *result = LocalizeString(@"IDS_APP_PENDING");
	NSMutableArray *array = [[NSMutableArray alloc] init];
	
	for(int i = 0; i < nSize; i++)
	{
		m_vMsg[i].GetAddressNumber(s);
		
		sendStatus *itemStatus = [[sendStatus alloc] init];
		
		itemStatus.phoneNumber = [NSString getStringFromWchar:(LPCTSTR)s];
		itemStatus.status = result;
		
		[array addObject:itemStatus];
		
		[itemStatus release];
	}
	
	[self setStatus:array];
	
	[array release];
}

- (void)initListStatus:(int)index withStatus:(signed char)bSuccess
{
	NSMutableArray *array = status;
	
	NSString *str;
	
	if(bSuccess == YES)
		str = LocalizeString(@"IDS_APP_OK");
	else
		str = LocalizeString(@"IDS_APP_FAIL");
	
	if(index >= [array count])
	{
		NSLog(@"Error\n");
		return;
	}
	
	sendStatus *item = [array objectAtIndex:index];
	
	item.status = str;
}

- (void)initStatic:(int)num
{
	NSString * str = [[[NSString alloc] initWithFormat:@"%d/%ld", num, m_vMsg.size()] autorelease];
	
	[ctlStatic setStringValue:str];
}

#pragma mark --
#pragma mark notification message
#pragma mark--

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	if(Msg == WM_RSPCALLBACK)
	{
		WATCH_POINTER(lparam);
		CRespFlag *pResp = (CRespFlag*)lparam;
		
		switch (LOWORD(wparam))
		{
			case RESP_SMS_SEND:
			{
				NSMutableDictionary *dict = [[NSMutableDictionary alloc] initWithCapacity:2];
				[dict setObject:[NSNumber numberWithInt:m_nIdx] forKey:ITEM_INDEX];
				
				if (pResp->Value())
				{
					[dict setObject:[NSNumber numberWithBool:YES] forKey:ITEM_BOOL];
				}
				else
				{
					[dict setObject:[NSNumber numberWithBool:NO] forKey:ITEM_BOOL];
				}
				
				[[NSNotificationCenter defaultCenter] postNotificationName:WM_SMSSEND_STATUS object:self userInfo:dict];
				
				[dict release];
				
				break;
			}
				
			default:
				break;
		}
	}
}

- (void)windowWillClose:(NSNotification *)notification
{
	[self onCancel:nil];
}

#pragma mark--
#pragma mark do job
#pragma mark--

- (void)postNotification:(BOOL)result
{
	NSMutableDictionary *dict = [[NSMutableDictionary alloc] initWithCapacity:2];
	[dict setObject:[NSNumber numberWithInt:m_nIdx] forKey:ITEM_INDEX];
	[dict setObject:[NSNumber numberWithBool:result] forKey:ITEM_BOOL];
	[[NSNotificationCenter defaultCenter] postNotificationName:WM_SMSSEND_STATUS object:self userInfo:dict];
	
	[dict release];
}

- (BOOL)doJob
{
	if(CA_OK != m_pComAgent->SendSMS(self, WM_RSPCALLBACK, RESP_SMS_SEND, &m_vMsg[m_nIdx]))
		return NO;
	
	return YES;
}

- (BOOL)getNext
{
	if(m_bCancel == YES)
		return NO;
	
	m_nIdx++;
	if(m_nIdx < m_vMsg.size())
		return YES;
	
	return NO;
}

- (void)onSendJobStatus:(NSNotification *)notification
{
	NSDictionary *dict = [notification userInfo];
	id object = [notification object];
	
	if([object isKindOfClass:[self class]] == NO)
		return;
	
	BOOL result = [[dict objectForKey:ITEM_BOOL] boolValue];
	vector<SMSMessage*>& vec = *m_pvMsg;
	
	if(result == YES)
	{
		// move to pc sent box
		if(m_npIdx != m_vIdx[m_nIdx])
		{
			m_npIdx = m_vIdx[m_nIdx];
			vec[m_npIdx]->SetMsgSendStatus(SMS_SEND_SUCESSFUL);
			
		}
		
		tm time;
		m_vMsg[m_nIdx].GetTime(time);
		vec[m_npIdx]->SetTime(time);
		
		[self initListStatus:m_nIdx withStatus:YES];
	}
	else
	{
		if(m_npIdx != m_vIdx[m_nIdx])
		{
			m_npIdx = m_vIdx[m_nIdx];
			vec[m_npIdx]->SetMsgSendStatus(SMS_SEND_FAILED);
			
		}
		
		[self initListStatus:m_nIdx withStatus:NO];
	}
	
	if([[self getSmsViewControl] isPCFolderFull] == NO)
		m_pFolderOutbox->MoveItemtoFolder(vec[m_npIdx], m_pFolderSentbox);
	
	
	[self initStatic:m_nIdx + 1];
	
	[progress incrementBy:1.0];
	
	if([self getNext] == YES)
	{
		if([self doJob] == NO)
		{
			[self postNotification:NO];
		}
	}
	else
	{
		m_bFinish = YES;
		[cancelButton setEnabled:YES];
		[cancelButton setTitle:LocalizeString(@"IDS_APP_OK")];
        [[self window ]setTitle:LocalizeString(@"ID_SMS_Send_Successful")];
        
    
	}
}

- (void)setStatus:(NSMutableArray *)_status
{
	if(status == _status)
		return;
	
	[_status retain];
	[status release];
	
	status = _status;
}

- (SMSViewCtrl*)getSmsViewControl
{
	DCTAppDelegate *delegate = TheDelegate;
	
	return delegate.smsController;
}

@end
