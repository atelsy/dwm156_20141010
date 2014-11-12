//
//  smsJobThreadDlg.m
//  DCT
//
//  Created by Fengping Yu on 12/10/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsJobThreadDlg.h"
#import "DCTAppDelegate.h"
#import "SMSViewCtrl.h"

@implementation smsJobThreadDlg

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) 
	{
		// Initialization code here.
		
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSmsThreadInsert:) name:WM_SMSTHREAD_INSERT object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSmsThreadDeleteMove:) name:WM_SMSTHREAD_DELETEMOVE object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSmsThreadInsertMove:) name:WM_SMSTHREAD_INSERTMOVE object:nil];
	}
	
	return self;
}

- (void)initWithParameter:(SmsJobPara *)para
{
	m_pvMessage = para->pvMessage;
	m_pFolderFrom = para->pFolderFrom;
	m_pFolder = para->pFolder;
	m_Memory = para->sMemory;
	m_bIsInbox = para->bIsInbox;
	m_bIsFromPC = para->bIsFromPC;
	m_strTitle = para->strTitle;
	m_Job = para->job;
	m_nCtlType = para->nCtrlType;
	m_bNotify = NO;
	m_pInsertMsg = NULL;
	m_pInsertNewMsg = NULL;
	m_bFinish = YES;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
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
	
	if(m_Job != -1)
	{
		[self initCtrl];
		[self doJob];
	}
}

- (IBAction)onButtonCancel:(id)sender
{
	m_bCancel = true;
	
	
    [m_ctlButton setEnabled:NO];
}

- (void)initCtrl
{
	[self.window setTitle:m_strTitle];
	
	[m_ctlProgress setMinValue:0];
	[m_ctlProgress setMaxValue:m_pvMessage->size()];
	
	[m_ctlProgress setDoubleValue:0.0];
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

#pragma mark--
#pragma mark job relate operation
#pragma mark--

- (void)doJob
{
	m_pvResultMessage.clear();
	
	switch (m_Job)
	{
		case 0:   // JOB_SMSINSERTMOVE, JOB_SMSDELETEMOVE
			[self start:m_pFolderFrom folder:m_pFolder messages:m_pvMessage isInbox:m_bIsInbox andisFromPC:m_bIsFromPC withMemory:m_Memory];
			break;
			
		case 1:  //JOB_SMSCOPY
			[self start:m_pFolder messages:*m_pvMessage WithMemory:m_Memory isInbox:m_bIsInbox];
			
		default:
			break;
	}
}

- (bool)start:(SmsFolder *)pFolder message:(SMSMessage *)pMsg withMemory:(NSString *)sMemory andIndex:(int)nItem
{
	if(m_bFinish == NO)
		return false;
	
	if(pFolder && pMsg)
	{
		if(nItem < 0)
			m_nType = JOB_SMSINSERT;
		else
			m_nType = JOB_SMSUPDATE;
		
		m_pFolder = pFolder;
		m_pMsg = pMsg;
		m_sMemory = sMemory;
		m_nItem = nItem;
		
		[self func];
		
		return true;
	}
	
	return false;
}

- (bool)start:(SmsFolder *)pFolder messages:(std::vector<SMSMessage *>)vMsg WithMemory:(NSString *)sMemory isInbox:(signed char)bIsInbox
{
	if(m_bFinish == NO)
		return false;
	
	if(pFolder)
	{
		m_nType = JOB_SMSCOPY;
		m_pFolder = pFolder;
		m_vMsg = vMsg;
		m_sMemory = sMemory;
		m_bIsInbox = bIsInbox;
		
		[self func];
		
		return true;
	}
	
	return false;
}

- (bool)start:(SmsFolder *)pFolderFrom folder:(SmsFolder *)pFolder messages:(std::vector<SMSMessage *> *)pvMsg isInbox:(signed char)bIsInbox andisFromPC:(signed char)bIsFromPC withMemory:(NSString *)sMemory
{
	if(m_bFinish == NO)
		return false;
	
	if(pFolderFrom && pFolder && pvMsg)
	{
		if([sMemory length] > 0)
			m_nType = JOB_SMSINSERTMOVE;
		else
			m_nType = JOB_SMSDELETEMOVE;
		
		m_pFolderFrom = pFolderFrom;
		m_pFolder = pFolder;
		m_pvMsg = pvMsg;
		m_vMsg = *pvMsg;
		m_sMemory = sMemory;
		m_bIsInbox = bIsInbox;
		m_bIsFromPC = bIsFromPC;
		
		[self func];
		
		return true;
	}
	
	return false;
}

#pragma mark--
#pragma mark thread function
#pragma mark --

- (void)func
{
	m_bFinish = NO;
	m_bCancel = NO;
	
	switch (m_nType)
	{
		case JOB_SMSINSERT:
			[self smsInsert];
			break;
			
		case JOB_SMSUPDATE:
			[self smsUpdate];
			break;
			
		case JOB_SMSCOPY:
			[self smsCopy];
			break;
			
		case JOB_SMSDELETEMOVE:
			[self smsDeleteMove];
			break;
			
		case JOB_SMSINSERTMOVE:
			[self smsInsertMove];
			break;
			
		default:
			break;
	}
	
	m_bFinish = YES;
}

- (void)sendMessage:(NSString *)msgID folderParam:(SmsFolder *)pFolder andMsgParam:(SMSMessage *)pMsg
{
	NSMutableDictionary *dict = [[[NSMutableDictionary alloc] initWithCapacity:2] autorelease];
	
	classToObject *object = [[[classToObject alloc] init]autorelease];
	object->pFolder = pFolder;
	[dict setObject:object forKey:ITEM_FOLDER];
	
	if(pMsg != NULL)
	{
		classToObject *msgObject = [[[classToObject alloc] init] autorelease];
		msgObject->pMsg = pMsg;
		[dict setObject:msgObject forKey:ITEM_MESSAGE];
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:msgID object:self userInfo:dict];
}

#pragma mark--
#pragma mark update message
#pragma mark--

- (bool)updateMessage:(SMSMessage *)pMsg
{
	if(pMsg)
	{
		if(CA_OK == [self ReplaceSMS:WM_PROCESSSMS step:RESP_SMS_UPDATE message:pMsg extendValue:0])
			return true;
	}
	
	return false;
}

- (void)smsUpdate
{
	m_nID = WM_SMSTHREAD_UPDATE;
	
	m_Msg = (*m_pMsg);
	
	m_vAddress = [utility getMultiAddressOfMsg:m_pMsg];
	
	if([self updateMessage:m_pMsg] == false)
	{
		delete m_pMsg;
		
		NSMutableDictionary *dict = [[[NSMutableDictionary alloc] initWithCapacity:2] autorelease];
		[dict setObject:[NSNumber numberWithInt:-1] forKey:ITEM_INDEX];
		[dict setObject:nil forKey:ITEM_MESSAGE];
		
		[[NSNotificationCenter defaultCenter] postNotificationName:m_nID object:self userInfo:dict];
		
		[self notifyDlg];
	}
}

#pragma mark--
#pragma mark sms save
#pragma mark--

- (void)smsInsert
{
	SMSMessage *pMsg = m_pMsg;
	m_Msg = (*pMsg);
	
	m_vAddress = [[utility getMultiAddressOfMsg:pMsg] retain];
	
	if(![self writeMessage:pMsg withMemory:m_sMemory index:0 notify:true])
		[self notifyDlg];
}

- (bool)writeMessage:(SMSMessage *)pMsg withMemory:(NSString *)memory index:(int)nIndex notify:(bool)bNotify
{
	m_nID = WM_SMSTHREAD_INSERT;
	m_pCurMsg = pMsg;
	m_bNotify = bNotify;
	
	if(pMsg)
	{
		if(CA_OK == [self writeSMS:WM_PROCESSSMS step:RESP_SMS_WRITE message:pMsg memory:memory extendValue:nIndex])
		{
			return true;
		}
		else
		{
			if(bNotify)
			{
				delete pMsg;
				
				[self sendMessage:m_nID folderParam:m_pFolder andMsgParam:NULL];
			}
		}
	}
	else
	{
		if(bNotify)
		{
			delete pMsg;
			
			[self sendMessage:m_nID folderParam:m_pFolder andMsgParam:NULL];
		}
	}
	
	return false;
}

#pragma mark--
#pragma mark delete message
#pragma mark--

- (void)smsDeleteMove
{
	[self deleteNext:0];
}

- (void)deleteNext:(int)nItem
{
	CRespFlag *resp = new CRespFlag(TRUE, nItem);
	[self onSmsDeleteMove:0 andLParam:(LPARAM)resp];
}

- (void)deleteEnd:(CRespFlag *)pResp
{
	[self notifyDlg];
}

- (LRESULT)onSmsDeleteMove:(long)wParam andLParam:(long)lParam
{
	WATCH_POINTER(lParam);
	CRespFlag *p = (CRespFlag*)lParam;
	int i = (int)p->m_lExt;
	BOOL bSuccess = p->Value();
	int step = LOWORD(wParam);
	SmsFolders *pFolders = NULL;
	
	switch (step)
	{
		case 0:
			if(i < m_vMsg.size() && bSuccess)
			{
				m_pInsertMsg = m_vMsg[i];
				if(m_pInsertMsg && [utility getState:m_pInsertMsg withFolder:m_pFolder])
				{
					if(CA_OK != [self eraseSMS:WM_SMSDELETEMOVE step:1 message:m_pInsertMsg extendValue:i])
					{
						[self onSMsThreadDeleteMove:-1 andLParam:NULL];
						[self notifyDlg];
						break;
					}
				}
				else
					[self deleteNext:i + 1];
			}
			else
				[self deleteEnd:NULL];
			
			break;
			
		case 1:
			if(bSuccess)
			{
				if((pFolders = [self newFolders]) != NULL)
				{
					if([self onSMsThreadDeleteMove: i andLParam:(LONG)pFolders] == false)
						delete pFolders;
				}
			}
			else
			{
				[self onSMsThreadDeleteMove:-1 andLParam:NULL];
				[self notifyDlg];
				break;
			}
			
			if(m_bCancel)
			{
				[self notifyDlg];
				break;
			}
			
			[self deleteNext:i + 1];
			
			break;
			
		case 2:
			if(bSuccess)
			{
				if((pFolders = [self newFolders]) != NULL)
				{
					if(![self onSmsThreadInsertMove: i andLParam:(LONG)pFolders])
						delete pFolders;
				}
			}
			else
				[self onSmsThreadInsertMove:-1 andLParam:NULL];
			
			if(m_bCancel)
			{
				[self notifyDlg];
				break;
			}
			
			[self insertMoveNext:i+1];
			
			break;
			
		default:
			break;
	}
	
	return 0;
}

- (LRESULT)onSMsThreadDeleteMove:(long)wParam andLParam:(long)lParam
{
	[m_ctlProgress incrementBy:1.0];
	
	[self disableButton];
	
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	smsTreeCtrl *tree = pView.smsTree;
	
	if(wParam >= 0 && lParam)
	{
		int nItem = (int)wParam;
		SmsFolders *pFolders = (SmsFolders*)lParam;
		SmsFolder *pFolderFrom = pFolders->pFolderFrom;
		SmsFolder *pFolderTo = pFolders->pFolderTo;
		std::vector<SMSMessage*>& vec = *(pFolders->pvMsg);
		SMSMessage *pMsg = vec[nItem];
		
		if([tree ifSelectedFolder:pFolderFrom])
		{
			CString sMem;
			pMsg->GetMemType(sMem);
			
			[[TheDelegate getSmsViewController].smsList updateStatusBar];
		}
		
		if(pMsg)
			pMsg->SetMemType(_T("PC"));
		
		//move item
		BOOL bCheck = NO;
		if(pFolders->pvMsg != &[pView getCopyMessage])
			bCheck = YES;
		
		[self updateMsgIndex:pFolderFrom withMessages:vec andIndex:nItem];
		
		[self moveItem:pFolderFrom toFolder:pFolderTo withMsgs:vec index:nItem isCheck:bCheck];
		
		delete pFolders;
		return 1;
	}
	else
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_MOVEMESSAGE2") forType:MB_OK];
	}
	
	return 0;
}

#pragma mark--
#pragma mark copy message
#pragma mark--

- (void)smsCopy
{
	[self copyNext:0];
}

- (void)copyNext:(int)nItem
{
	[self onSmsCopy:0 status:TRUE andIndex:nItem];
}

- (void)copyEnd:(int)nNum
{
	[self onSmsCopy:2 status:FALSE andIndex:nNum];
}

- (LRESULT)onSmsCopy:(int)step status:(BOOL)bSuccess andIndex:(int)nNum
{
	int i = nNum;
	
	switch (step)
	{
		case 0:
			m_pInsertMsg = m_vMsg[i];
			if(i < m_vMsg.size() && bSuccess)
			{
				if(m_pInsertMsg && [utility getState:m_pInsertMsg withFolder:m_pFolder])
				{
					m_pInsertNewMsg = [utility newMessage];
					if(!m_pInsertNewMsg)
						return 0;
					
					*m_pInsertNewMsg = *m_pInsertMsg;
					if(m_bIsInbox)
						m_pInsertNewMsg->SetStatus(REC_READ);
					else
						m_pInsertNewMsg->SetStatus(STO_UNSENT);
					
					m_Msg = (*m_pInsertNewMsg);
					m_vAddress = [utility getMultiAddressOfMsg:m_pInsertNewMsg];
					
					if([self writeMessage:m_pInsertNewMsg withMemory:m_sMemory index:i notify:true] == false)
						[self copyEnd:nNum];
				}
				else
					[self copyNext:i+1];
				
				if(m_bCancel)
					[self copyEnd:nNum];
			}
			else
				[self copyEnd:nNum];
			
			break;
			
		case 1:
			if(bSuccess && m_bCancel == NO)
			{
				[self copyNext:i+1];
			}
			else
			{
				[self copyEnd:nNum];
			}
			
			break;
			
		case 2:
			[self notifyDlg];
			break;
			
		default:
			break;
	}
	
	return 0;
}

#pragma mark--
#pragma mark insert move(cut)
#pragma mark--

- (void)smsInsertMove
{
	[self insertMoveNext:0];
}

- (void)insertMoveNext:(int)nItem
{
	[self onSmsInsertMove:0 status:TRUE andIndex:nItem];
}

- (void)insertMoveEnd:(int)nNum
{
	[self onSmsInsertMove:2 status:FALSE andIndex:nNum];
}

- (LRESULT)onSmsInsertMove:(int)step status:(BOOL)bSuccess andIndex:(int)nNum
{
	int i = nNum;

	SmsFolders *pFolders = NULL;
	SmsFolder *folder;
	NSTreeNode *hItem;
	SMSViewCtrl *pView = TheDelegate.smsController;
	BOOL bInsertMove = NO;
	
	switch (step)
	{
		case 0:
			if(i < m_vMsg.size() && bSuccess)
			{
				m_pInsertMsg = m_vMsg[i];
				
				// add for message recovery operation
				eDelMsgOrigin eo = m_pInsertMsg->GetMsgDeleteOrigin();
				
				if(eo == SMS_DELETE_FROM_HANDSET_INBOX)
				{
					[pView.smsTree getHandsetInbox:&hItem forFolder:&folder];
					bInsertMove = [utility getState:m_pInsertMsg withFolder:folder];
				}
				else if(eo == SMS_DELETE_FROM_HANDSET_OUTBOX)
				{
					[pView.smsTree getHandsetOutbox:&hItem forFolder:&folder];
					bInsertMove = [utility getState:m_pInsertMsg withFolder:folder];
				}
				else
					bInsertMove = [utility getState:m_pInsertMsg withFolder:m_pFolder];
				
				if(m_pInsertMsg && bInsertMove)
				{
					m_pInsertNewMsg = [utility newMessage];
					if(!m_pInsertNewMsg)
						return 0;
					
					*m_pInsertNewMsg = *m_pInsertMsg;
					
					if(eo != SMS_DELETE_FROM_HANDSET_INBOX && eo != SMS_DELETE_FROM_HANDSET_OUTBOX)
					{
						if(m_bIsInbox)
							m_pInsertNewMsg->SetStatus(REC_READ);
						else
							m_pInsertNewMsg->SetStatus(STO_UNSENT);
					}
					
					m_Msg = (*m_pInsertNewMsg);
					
					m_vAddress = [utility getMultiAddressOfMsg:m_pInsertNewMsg];
					
					if(![self writeMessage:m_pInsertNewMsg withMemory:m_sMemory index:i notify:true])
					{
						[self onSmsThreadInsertMove:-1 andLParam:NULL];
						
						[self insertMoveEnd:nNum];
						break;
					}
				}
				else
					[self insertMoveNext:i + 1];
				
				if(m_bCancel)
					[self insertMoveEnd:nNum];
			}
			else
				[self insertMoveEnd:nNum];
			
			break;
			
		case 1:
			if(bSuccess)
			{
				if(!m_bIsFromPC)
				{
					if(CA_OK != [self eraseSMS:WM_SMSDELETEMOVE step:2 message:m_pInsertMsg extendValue:i])
					{
						[self onSmsThreadInsertMove:-1 andLParam:NULL];
						[self notifyDlg];
						break;
					}
					else
						break;
				}
				else
				{
					if((pFolders = [self newFolders]) != NULL)
					{
						if(![self onSmsThreadInsertMove:i andLParam:(LONG)pFolders])
							delete pFolders;
					}
				}
			}
			else
			{
				[self insertMoveEnd:nNum];
				break;
			}
			
			if(m_bCancel)
			{
				[self insertMoveEnd:nNum];
				break;
			}
			
			[self insertMoveNext:i+1];
			
			break;
			
		case 2:
			[self notifyDlg];
			break;
			
		default:
			break;
	}
	
	return 0;
}

- (void)onSmsThreadInsert:(NSNotification *)notification
{
	[m_ctlProgress incrementBy:1.0];
	
	[self disableButton];
	
	NSDictionary *dict = [notification userInfo];
	
	classToObject *folderObject = [dict objectForKey:ITEM_FOLDER];
	classToObject *msgObject = [dict objectForKey:ITEM_MESSAGE];
	
	if(folderObject != nil && msgObject != nil)
	{
		SMSMessage *pMsg = msgObject->pMsg;
		[self updateMsgIndex:folderObject->pFolder withMessage:pMsg];
		m_pvResultMessage.push_back(pMsg);
		
		return;
	}
	else if(folderObject != nil && msgObject == nil)
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_WRITEMESSAGE") forType:MB_OK];
		return;
	}
}

#pragma mark--
#pragma mark thread function
#pragma mark--

- (LRESULT)onSmsThreadInsertMove:(long)wParam andLParam:(long)lParam
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	
	if(wParam >= 0 && lParam)
	{
		int nItem = (int)wParam;
		SmsFolders *pFolders = (SmsFolders*)lParam;
		SmsFolder *pFolderFrom = pFolders->pFolderFrom;
		
		std::vector<SMSMessage*>& vec = *(pFolders->pvMsg);
		SMSMessage *pMsg = vec[nItem];
		
		BOOL bCheck = NO;
		if(pFolders->pvMsg != &[pView getCopyMessage])
			bCheck = YES;
		
		pFolderFrom->DeleteItem(pMsg);
		
		if(bCheck)
			[pView checkClipboard:pMsg];
		
		vec[nItem] = NULL;
		
		delete pFolders;
		
		return 1;
	}
	else
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_MOVEMESSAGE2") forType:MB_OK];
	}
	
	return 0;
}

- (LRESULT)onProcessSMS:(WPARAM)wParam andLParam:(LPARAM)lParam
{
	WATCH_POINTER(lParam);
	CRespFlag *pResp = (CRespFlag*)lParam;
	NSString *msgID;
	
	switch (LOWORD(wParam))
	{
		case RESP_SMS_WRITE:
			if(m_nType == JOB_SMSINSERT)
				msgID = WM_SMSTHREAD_SAVE;
			else
				msgID = m_nID;
			
			if(pResp->Value())
			{
				if(m_bNotify)
				{
					[self sendMessage:msgID folderParam:m_pFolder andMsgParam:m_pCurMsg];
				}
			}
			else
			{
				if(m_bNotify)
				{
					delete m_pCurMsg;
					[self sendMessage:msgID folderParam:m_pFolder andMsgParam:NULL];
					[self notifyDlg];
					break;
				} 
			}
			
			if(m_nType == JOB_SMSINSERT)
				[self notifyDlg];
			else if(m_nType == JOB_SMSCOPY)
				[self onSmsCopy:1 status:pResp->Value() andIndex:(int)pResp->m_lExt];
			else if(m_nType == JOB_SMSINSERTMOVE)
				[self onSmsInsertMove:1 status:pResp->Value() andIndex:(int)pResp->m_lExt];
			
			break;
			
		case RESP_SMS_UPDATE:
			if(pResp->Value())
			{
				NSMutableDictionary *dict = [[[NSMutableDictionary alloc] initWithCapacity:2] autorelease];
				[dict setObject:[NSNumber numberWithInt:m_nItem] forKey:ITEM_INDEX];
				
				classToObject *msgObject = [[[classToObject alloc] init] autorelease];
				msgObject->pMsg = m_pMsg;
				[dict setObject:msgObject forKey:ITEM_MESSAGE];
				
				[[NSNotificationCenter defaultCenter] postNotificationName:m_nID object:self userInfo:dict];
			}
			else
			{
				delete m_pMsg;
				
				NSMutableDictionary *dict = [[[NSMutableDictionary alloc] initWithCapacity:2] autorelease];
				[dict setObject:[NSNumber numberWithInt:-1] forKey:ITEM_INDEX];
				
				[dict setObject:nil forKey:ITEM_MESSAGE];
				
				[[NSNotificationCenter defaultCenter] postNotificationName:m_nID object:self userInfo:dict];
			}
			
			[self notifyDlg];
			
			break;
			
		default:
			break;
	}
	
	return 0;
}

#pragma mark --
#pragma mark sms write, update and delete
#pragma mark--

- (CAERR)writeSMS:(int)uMsg step:(int)nStep message:(SMSMessage *)pMsg memory:(NSString *)strMem extendValue:(long)lExt
{
	if([self getSimStatus] == NO)
		return CA_UNKNOWN;
	
	return m_pComAgent->WriteSMS(self, uMsg, nStep, pMsg, CString([strMem UTF8String]), lExt);
}

- (CAERR)ReplaceSMS:(int)uMsg step:(int)nStep message:(SMSMessage *)pMsg extendValue:(long)lExt
{
	if([self getSimStatus] == NO)
		return CA_UNKNOWN;
	
	return m_pComAgent->ReplaceSMS(self, uMsg, nStep, pMsg, lExt);
}

- (CAERR)eraseSMS:(int)uMsg step:(int)nStep message:(SMSMessage *)pMsg extendValue:(long)lExt
{
	if([self getSimStatus] == NO)
		return CA_UNKNOWN;
	
	return m_pComAgent->EraseSMS(self, uMsg, nStep, pMsg, lExt);
}

#pragma mark --
#pragma mark messages responde function
#pragma mark--

- (BOOL)getFinish
{
	return m_bFinish;
}

- (BOOL)getSimStatus
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	
	return [pView getSimStatus];
}

- (SMSViewCtrl*)getSMSViewControl
{
	DCTAppDelegate *delegate = TheDelegate;
	
	return delegate.smsController;
}

- (std::vector<SMSMessage*>)getResultMsgVec
{
	return m_pvResultMessage;
}

- (SmsFolders*)newFolders
{
	SmsFolders *pFolders = NULL;
	
	try 
	{
		pFolders = new SmsFolders;
		if(pFolders == NULL)
			return NULL;
		
		pFolders->pFolderFrom = m_pFolderFrom;
		pFolders->pFolderTo = m_pFolder;
		pFolders->pvMsg = &m_vMsg;
	} 
	catch (...)
	{
		return NULL;
	}
	
	return pFolders;
}

- (void)moveItem:(SmsFolder *)pFolderFrom toFolder:(SmsFolder *)pFolderTo withMsgs:(std::vector<SMSMessage *> &)vec index:(int)nIdx isCheck:(signed char)bCheck
{
	SMSViewCtrl *pView = [TheDelegate getSmsViewController];
	
	SMSMessage *pMsg = vec[nIdx];
	if(!pFolderFrom || !pFolderTo || !pMsg)
		return;
	
	pFolderFrom->MoveItemtoFolder(pMsg, pFolderTo);
	
	if(bCheck)
		[pView checkClipboard:pMsg];
	
	m_pvResultMessage.push_back(pMsg);
	
	vec[nIdx] = NULL;
}

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	if(Msg == WM_PROCESSSMS)
	{
		[self onProcessSMS:wparam andLParam:lparam];
	}
	else if(Msg == WM_SMSDELETEMOVE)
	{
		[self onSmsDeleteMove:wparam andLParam:lparam];
	}
    
    m_pComAgent = [TheDelegate getComAgent];
    
    SMSViewCtrl *smsView = [TheDelegate getSmsViewController];
    
    m_pComAgent->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "SM", READ_SM);
    
    m_pComAgent->GetSMSNumber(smsView, WM_RSPCALLBACK, RESP_SMS_GETTOTALNUM, "ME",READ_ME);
}

- (void)notifyDlg
{
	[[NSNotificationCenter defaultCenter] postNotificationName:WM_DESTROY_DIALOG object:self userInfo:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt: m_nCtlType],ITEM_MSGID, nil]]; 
}

#pragma mark - modify message index for dinstinuish

- (void)updateMsgIndex:(SmsFolder *)pFolder withMessages:(std::vector<SMSMessage *> &)vec andIndex:(int)index
{
	SMSMessage *pMsg = vec[index];
	
	SMSViewCtrl *pView = [self getSMSViewControl];
	smsTreeCtrl *tree = pView.smsTree;
	
	if([tree ifHandsetInbox:pFolder] == NO)
		return;
	
	NSMutableDictionary *dict = TheDelegate.m_indexMap;
	
	NSString *key = [NSString stringWithFormat:@"%d", pMsg->GetMsgIndex()];
	
	if([dict objectForKey:key] != nil)
		[dict removeObjectForKey:key];
}

- (void)updateMsgIndex:(SmsFolder *)pFolder withMessage:(SMSMessage *)pMsg
{
	SMSViewCtrl *pView = [self getSMSViewControl];
	smsListCtrl *list = pView.smsList;
	
	return [list updateMsgStatusIndex:pFolder withMessage:pMsg];
}

@end
