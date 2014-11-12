//
//  SmsEditCtrl.m
//  DCT
//
//  Created by Fengping Yu on 10/24/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "SmsEditCtrl.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"

#define MAX_RECIPLIST_LENGTH	601

@implementation SmsEditCtrl

#pragma mark--
#pragma mark construct, destruct and init function
#pragma mark--

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	
	if (self) 
	{
		// Initialization code here.
		defaultPasteboard = [NSPasteboard generalPasteboard];
		[defaultPasteboard declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:nil];
		
		m_saveDlgResult = NSRunAbortedResponse;
		m_lastValidRecipients = [[NSMutableString alloc] init];
	}
	
	return self;
}

@synthesize m_ctlEditMessage, pbNumber, counter, toButton, m_lastValidRecipients;

- (void)awakeFromNib
{
	[m_ctlEditMessage setParent:self];
	
	[pbNumber setStringValue:m_strTo];
	
	m_lastValidRecipients = m_strTo;
}

- (void)dealloc
{
	[toButton release];
	[pbNumber release];
	[counter release];
	
	[m_ctlEditMessage release];
	
	[m_strTo release];
	//   [m_lastValidRecipients release];
	
	[super dealloc];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	[m_ctlEditMessage setDelegate:self];
	[m_ctlEditMessage initDefaultParameters];
	
	if(m_pMsg)
	{
		EMSData *pEmsData;
		m_pMsg->GetMsg(&pEmsData);
		if(m_bReply)
			[m_ctlEditMessage setEmsData:NULL withSelFolder:NO];
		else
			[m_ctlEditMessage setEmsData:pEmsData withSelFolder:NO];
		
		
	//	CString from;
	//	m_pMsg->GetAddressNumber(from);
		
	//	[pbNumber setStringValue:[NSString getStringFromWchar:(LPCTSTR)from]];
		
		NSTextView *pbText = (NSTextView*)[[self window] fieldEditor:YES forObject:pbNumber];
		
		[pbText setSelectedRange:NSMakeRange([[pbText string] length], 0)];
		
		[self updateInfo:YES];
	}
	
	m_bModify = NO;
}

-(void)initwithAddress:(NSString *)strto msgContent:(NSString *)strMsg inItem:(int)item isLast:(BOOL)bLast withSmsfolder:(SmsFolder *)pFolder outboxFolder:(SmsFolder *)pFolderOutbox inboxFolder:(SmsFolder *)pFolderInbox andPhonebookfolder:(PbFolder *)pFolderpb withMsg:(SMSMessage *)pmsg replyOrNot:(BOOL)bReply
{
	if(m_strTo == nil)
		m_strTo = [[NSMutableString alloc] init];
	
	m_bLast = bLast;
	m_nItem = item;
	
	if(strto != nil && [strto length] > 0)
		[m_strTo setString: strto];
	
	m_strMessage = strMsg;
	m_pFolderPb = pFolderpb;
	m_pFolder = pFolder;
	m_pFolderInbox = pFolderInbox;
	m_pFolderOutbox = pFolderOutbox;
	m_pMsg = pmsg;
	m_bReply = bReply;
}

#pragma mark--
#pragma mark other common function
#pragma mark--

- (void)updateInfo:(BOOL)bCheck
{
	BOOL bEms;
	int nNum, nUnit, nMessage;
	
	[m_ctlEditMessage getInfo:nNum unit:nUnit message:nMessage isEms:bEms ischeck:bCheck];
	
	NSString *str = nil;
	
	if(bEms)
		str = [[NSString alloc] initWithFormat:@"%d / %d (Total = %d EMS)", nNum, nUnit, nMessage];
	else
		str = [[NSString alloc]initWithFormat:@"%d / %d (Total = %d SMS)", nNum, nUnit, nMessage];
	
	[counter setStringValue:str];
	
	[str release];
}

- (BOOL)checkRecipients:(NSString *)str
{
	NSArray *vec = [utility getMultiAddress:str];
	
	if([str length] > 0 && [vec count] == 0)
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_RECIPIENTS") forType:MB_OK];
		return NO;
	}
	
	for(int i = 0; i < [vec count]; i++)
	{
		NSString * strNumber = [vec objectAtIndex:i];
		if([strNumber length] > 20 || [utility checkPhoneNumber:strNumber] == NO)
		{
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_RECIPIENTS") forType:MB_OK];
			return NO;
		}
	}
	
	return YES;
}

- (SMSMessage*)initMessage
{
	SMSMessage *pMsg = [utility newMessage];
	if(pMsg)
	{
		if(m_pMsg)
		{
			tm t;
			pMsg->GetTime(t);
			
			*pMsg = *m_pMsg;
			
			eMemStatus nStatus = m_pMsg->GetStatus();
			
			if(nStatus == REC_READ || nStatus == REC_UNREAD)
				pMsg->SetStatus(UNKNOW);
			
			pMsg->SetTime(t);
		}
		else
		{
			pMsg->SetStatus(UNKNOW);
		}
		
		NSString *str = [pbNumber stringValue];
		const char* chars = [str UTF8String];
		
		pMsg->SetAddressNumber(CString(chars));
		
		//emsdata
		EMSData *pEmsData;
		pMsg->GetMsg(&pEmsData);
		
		if(pEmsData)
		{
			ReleaseEMSData(pEmsData);
			delete pEmsData;
		}
		
		try 
		{
			pEmsData = new EMSData;
		} 
		catch (...)
		{
			if(pMsg)
				delete pMsg;
			
			return NULL;
		}
		
		InitializeEMSData(pEmsData, SMSAL_DEFAULT_DCS);
		CopyEMSData(pEmsData, [m_ctlEditMessage getEmsData]);
		pMsg->SetMsg(pEmsData);
	}
	
	return pMsg;
}

#pragma mark--
#pragma mark IBAction operation
#pragma mark--

- (IBAction)onPbSelect:(id)sender
{
	NSInteger result = 0;
	
	if(pbSelectCtrl == nil)
	{
		pbSelectCtrl = [[pbSelectController alloc] retain];
	}
	
	[pbSelectCtrl init:m_pFolderPb];
	
	[pbSelectCtrl initWithWindowNibName:@"contactSelectWindow"];
	
	NSWindow *pbWin = [pbSelectCtrl window];
	
	if([NSApp isHidden])
	{
		[pbWin makeKeyAndOrderFront:self];
	}
	else
	{
		result = [NSApp runModalForWindow:pbWin];
	}
	
	if(result == NSRunStoppedResponse)
	{
		NSString *strNumbers = [pbSelectCtrl getNumbers];
		
		if([strNumbers length] > 0)
		{
			[m_strTo setString:[pbNumber stringValue]];
			
			if([m_strTo length] == 0)
				[m_strTo setString: strNumbers];
			else
			{
				[m_strTo appendString:@";"];
				[m_strTo appendString:strNumbers];
			}
			
			m_bModify = YES;
		}
		
		[pbNumber setStringValue:m_strTo];
	}
	
	[pbSelectCtrl release];
	pbSelectCtrl = nil;
}

- (IBAction)onSmsSave:(id)sender
{
	if ([self checkRecipients:[pbNumber stringValue]] == NO)
	{
		return;
	}
	
	if(!m_pFolder)
		return;
	
	if(m_nItem >= 0)
	{
		SMSMessage *pMsg = [self initMessage];
		if(pMsg)
		{
			NSMutableDictionary *dict = [[NSMutableDictionary alloc] initWithCapacity:2];
			[dict setObject:[NSNumber numberWithInt:m_nItem] forKey:ITEM_INDEX];
			classToObject *msgObject = [[[classToObject alloc] init] autorelease];
			msgObject->pMsg = pMsg;
			[dict setObject: msgObject forKey:ITEM_MESSAGE]; 
			[[NSNotificationCenter defaultCenter] postNotificationName:WM_SMS_UPDATE object:self userInfo:dict];
			
			[dict release];
		}
		
		m_bModify = NO;
		
		[[self window] performClose:nil];
	}
	else
	{
		NSInteger result = 0;
		
		if(saveController == nil)
			saveController = [smsSaveController alloc];
		
		[saveController init:m_pFolder outbox:m_pFolderOutbox inbox:m_pFolderInbox];
		
		[saveController initWithWindowNibName:@"smsSave"];
		
		NSWindow *win = [saveController window];
		
		if([NSApp isHidden])
			[win makeKeyAndOrderFront:self];
		else
		{
			result = [NSApp runModalForWindow:win];
		}
		
		if(result == NSRunAbortedResponse)
		{
			m_saveDlgResult = NSRunAbortedResponse;
		}
		else if(result == NSRunStoppedResponse)
		{
			m_bModify = NO;
			m_saveDlgResult = NSRunStoppedResponse;
			
			SmsFolder *pFolder = [saveController getFolder];
			NSString *memory = [saveController getMemory];
			
			if([memory caseInsensitiveCompare:@"PC"] == NSOrderedSame || [[TheDelegate getSmsViewController] getSimStatus])
			{
				SMSMessage *pMsg = [self initMessage];
				if(pMsg)
				{
					pMsg->SetMemType([NSString getCStringFromNSString:memory]);
					
					NSMutableDictionary *dict = [[[NSMutableDictionary alloc] initWithCapacity:2] autorelease];
					
					classToObject *msgObject = [[[classToObject alloc] init] autorelease];
					msgObject->pMsg = pMsg;
					
					classToObject *folderObject = [[[classToObject alloc] init] autorelease];
					folderObject->pFolder = pFolder;
					
					[dict setObject:msgObject forKey:ITEM_MESSAGE];
					[dict setObject:folderObject forKey:ITEM_FOLDER];
					
					[[NSNotificationCenter defaultCenter] postNotificationName:WM_SMS_INSERT object:self userInfo:dict];
				}
			}
			else
			{
                [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_WRITEMESSAGE") forType:MB_OK];
			}
		}
		
		[saveController release];
		saveController = nil;
		
		if(result == NSRunStoppedResponse)
			[[self window] performClose:self];
	}
}

- (IBAction)onSmsSend:(id)sender
{
	NSString *str = [pbNumber stringValue];
	
	if ([self checkRecipients:str] == NO)
	{
		return;
	}
	
	SMSMessage *pMsg = [self initMessage];
	if(!pMsg)
	{
		[self closeDialog];
		return;
	}
	
	CString strAddress, strLeft;
	pMsg->GetAddressNumber(strAddress);
	bool bHasRecipient = false;
	ssize_t nIdx;
	
	while (!strAddress.IsEmpty())
	{
		nIdx = strAddress.FindOneOf(_T(",;"));
		if(nIdx == -1)
		{
			strLeft = strAddress;
			strAddress.Empty();
		}
		else
		{
			strLeft = strAddress.Left(nIdx);
			strAddress = strAddress.Right(strAddress.GetLength() - nIdx - 1);
		}
		
		if(!strLeft.IsEmpty())
		{
			bHasRecipient = true;
			break;
		}
	}
	
	if(bHasRecipient)
	{
		NSMutableDictionary *dict = [[NSMutableDictionary alloc] initWithCapacity:2];
		[dict setObject:[NSNumber numberWithInt:m_nItem] forKey:ITEM_INDEX];
		
		if(m_nItem <= 0)
			pMsg->SetMemType(_T("PC"));
		
		classToObject *msgObject = [[classToObject alloc] init];
		
		msgObject->pMsg = pMsg;
		
		[dict setObject:msgObject forKey:ITEM_MESSAGE];
		
		if(m_nItem > 0)
		{
			[[NSNotificationCenter defaultCenter] postNotificationName:WM_SMS_SENDOLD object:self userInfo:dict];
		}
		else
		{
			[[NSNotificationCenter defaultCenter] postNotificationName:WM_SMS_SENDNEW object:self userInfo:dict];
		}
		
		m_bModify = NO;
		
		[msgObject release];
		
		[dict release];
		
		[[self window] performClose:nil];
	}
	else
	{
		delete pMsg;
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") 
						withMsg:LocalizeString(@"IDS_ERROR_SENDMESSAGE") 
						forType:MB_OK];
	}
}

#pragma mark -- 
#pragma mark cut, copy and paste
#pragma mark--

- (BOOL)isMsgFull
{
	NSRange range = [m_ctlEditMessage selectedRange];
	EMSData emsData = *[m_ctlEditMessage getEmsData];
	
	if(range.length == 0 && 
	   emsData.Reminder.requiredSegment == 10 && 
	   emsData.Reminder.segRemainingOctet == 0 &&
	   emsData.Reminder.totalRemainingOctet == 0)
		return YES;
	
	return NO;
}

- (id)getFocus
{
	NSResponder *focusView = [[self window] firstResponder];
	
	if([focusView isKindOfClass:[NSTextView class]]) 
	{
		if(focusView == m_ctlEditMessage)
			return m_ctlEditMessage;
		else
		{
			return pbNumber;  //[[[self window] firstResponder] delegate]; modify for remove warning
		}
	}
	
	return nil;
}

- (void)cutString:(NSTextView *)textView
{
	[m_ctlEditMessage setChangeType:CHANGETYPE_CUT];
	
	[textView cut:nil];
}

- (void)copyString:(NSTextView *)textView
{
	[textView copy:nil];
}

- (void)pasteString:(NSTextView *)textView
{
	[textView paste:nil];
}

- (IBAction)onEditCut:(id)sender
{
	id object = [self getFocus];
	
	if(object == pbNumber)
	{
		NSTextView *pbText = (NSTextView*)[[self window] fieldEditor:YES forObject:pbNumber];
		
		[self cutString:pbText];
	}
	else if(object == m_ctlEditMessage)
	{
		[self cutString:m_ctlEditMessage];
	}
}

- (IBAction)onEditCopy:(id)sender
{
	id object = [self getFocus];
	
	if(object == pbNumber)
	{
		NSTextView *textView = (NSTextView*)[[self window] fieldEditor:YES forObject:pbNumber];
		
		[self copyString:textView];
	}
	else if(object == m_ctlEditMessage)
	{
		[self copyString:m_ctlEditMessage];
	}
}

- (IBAction)onEditPaste:(id)sender
{
	id object = [self getFocus];
	
	if(object == pbNumber)
	{
		NSTextView *textView = (NSTextView*)[[self window] fieldEditor:YES forObject:pbNumber];
		[self pasteString:textView];
	}
	else if(object == m_ctlEditMessage)
	{
		if([self isMsgFull] == YES)
		{
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") 
							withMsg:LocalizeString(@"IDS_ERROR_EDITMESSAGE") forType:MB_OK];
			
			return;
		}
		else
		{
			[m_ctlEditMessage setChangeType:CHANGETYPE_REPLACESEL];
			[self pasteString:m_ctlEditMessage];
		}
	}
}

#pragma mark--
#pragma mark delegate method
#pragma mark--

- (void)textDidChange:(NSNotification *)notification
{
	m_bModify = YES;
	
	id object = [notification object];
	
	[m_ctlEditMessage onChange];
	
	if([object isKindOfClass:[NSTextView class]] == YES)
		[self updateInfo:YES];
}

- (void)controlTextDidChange:(NSNotification *)obj
{
	NSString *number = [pbNumber stringValue];
	
	if([number length] > MAX_RECIPLIST_LENGTH)
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_MAX_RECIPIENT_LENGTH") forType:MB_OK];
		
		[pbNumber setStringValue:m_lastValidRecipients];
		return;
	}
	
	NSString *dest = @"";
	NSString *subString;
	
	NSUInteger count = [number length];
	BOOL bInvalid = NO;
	int invalidIndex;
	
	for(NSUInteger i = 0; i < count; i++)
	{
		subString = [number substringWithRange:NSMakeRange(i, 1)];
		if([phoneNumberFormatter checkPhonenumber:subString])
		{
			dest = [dest stringByAppendingString:subString];
		}
		else
		{
			if(bInvalid == NO)
			{
				bInvalid = YES;
				invalidIndex = (int)i;
			}
		}
	}
	
	if([dest length] > 0)
		m_bModify = YES;
	
	[pbNumber setStringValue:dest];
	
	[m_lastValidRecipients setString:dest];
	
	if(bInvalid == YES)
	{
		[[pbNumber currentEditor] moveToBeginningOfLine:nil];
		
		for(int i = 0; i < invalidIndex; i++)
			[[pbNumber currentEditor] moveRight:nil];
	}
}

- (void)setModify:(signed char)bModify
{
	m_bModify = bModify;
}

- (BOOL)windowShouldClose:(id)sender
{
	if(m_bModify)
	{
		NSInteger result = [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_EDITOR_SAVE") forType:MB_YESNOCANCEL];
		
		switch(result)
		{
			case NSAlertDefaultReturn:
				[self onSmsSave:nil];
				if(m_saveDlgResult == NSRunStoppedResponse)
					return YES;
				else 
					return NO;
				
			case NSAlertAlternateReturn:
				m_bModify = NO;
				return YES;
				
			case NSAlertOtherReturn:
				return NO;
				
			default:
				break;
		}
	}
	
	return YES;
}

- (void)windowWillClose:(NSNotification *)notification
{
	[[NSNotificationCenter defaultCenter] postNotificationName:WM_SMS_DESTROYDIALOG object:self userInfo:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:SMS_DLG_EDITOR],ITEM_MSGID, nil]];
}

- (void)closeDialog
{
	if ([NSApp isHidden])
	{
		[[self window] orderOut:self];
	}
	else
	{
		[self autorelease];
	}
}

#pragma mark --
#pragma mark toolbar item enable or disable
#pragma mark--

- (BOOL)enableCutCopy
{
	NSTextView *textView = nil;
	
	if([self getFocus] == pbNumber)
	{
		textView = (NSTextView*)[[self window] fieldEditor:YES forObject:pbNumber];
	}
	else if([self getFocus] == m_ctlEditMessage)
	{
		textView = m_ctlEditMessage;
	}
	
	if(textView)
	{
		if([textView selectedRange].length > 0)
			return YES;
	}
	
	return NO;
}

- (BOOL)enablePaste
{
	NSArray *supportType = [NSArray arrayWithObjects:NSStringPboardType, nil];
	
	NSString *bestType = [defaultPasteboard availableTypeFromArray:supportType];
	
	if([bestType isEqualToString:NSStringPboardType])
		return YES;
	
	return NO;
}

- (BOOL)validateToolbarItem:(NSToolbarItem *)theItem
{
	int tag = (int)[theItem tag];
	
	switch (tag)
	{
		case SENDITEM:
			if([[TheDelegate getSmsViewController] getSimStatus] == YES && [[pbNumber stringValue] length] > 0)
			{
				[theItem setImage:[NSImage imageNamed:@"send"]];
				return YES;
			}
			else
			{
				[theItem setImage:[NSImage imageNamed:@"disable send"]];
				return NO;
			}
			
			
		case SAVEITEM:
			if(m_bModify)
			{
				[theItem setImage:[NSImage imageNamed:@"save"]];
				return YES;
			}
			else
			{
				[theItem setImage:[NSImage imageNamed:@"disable save"]];
				return NO;
			}
			
		case CUTITEM:
			if([self enableCutCopy]  == YES)
			{
				[theItem setImage:[NSImage imageNamed:@"cut"]];
				return YES;
			}
			else
			{
				[theItem setImage:[NSImage imageNamed:@"disable cut"]];
				return NO;
			}
			
		case COPYITEM:
			if([self enableCutCopy]  == YES)
			{
				[theItem setImage:[NSImage imageNamed:@"copy"]];
				return YES;
			}
			else
			{
				[theItem setImage:[NSImage imageNamed:@"disable copy"]];
				return NO;
			}
			
		case PASTEITEM:
			if([self enablePaste] == YES)
			{
				[theItem setImage:[NSImage imageNamed:@"paste"]];
				return YES;
			}
			else
			{
				[theItem setImage:[NSImage imageNamed:@"disable paste"]];
				return NO;
			}
			
		default:
			break;
	}
	
	return NO;
}

// replace the windows default text field editor
- (id)windowWillReturnFieldEditor:(NSWindow *)sender toObject:(id)client
{
	if([client isKindOfClass:[NSTextField class]])
	{
		if(!customFieldEditor)
		{
			customFieldEditor = [[smsTextView alloc] init];
			[customFieldEditor setFieldEditor:YES];
		}
		
		return customFieldEditor;
	}
	
	return nil;
}

//add for debug
- (void)mouseUp:(NSEvent *)theEvent
{
	NSLog(@"\n<<<<<<<<<<mouse up event>>>>>>>>>>>>>>>>\n");
}

@end
