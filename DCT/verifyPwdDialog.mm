//
//  verifyPwdDialog.mm
//  DCT
//
//  Created by MBJ on 12-1-7.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "verifyPwdDialog.h"
#import "DCTAppDelegate.h"

#define CC_PINVALID	15

BOOL IsDec(NSString* str)
{
	if (!str)	return FALSE;
	const char* p = [str UTF8String];
	while (*p) {
		if (*p < '0' || *p > '9')
			return FALSE;
		p++;
	}
	return TRUE;
}

@implementation verifyPwdDialog

- (void)setJob:(int)job
{
	m_nJob = job;
}

- (void)awakeFromNib
{
	NSString *str = LocalizeString(@"IDS_VERIFYPWD_WND_CAPTION");
	[[self window]setTitle:str];
	
	[self showJobCtrls];
}

- (void)showJobCtrls
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	BOOL ret = FALSE;
	[pwd1 setStringValue:@""];
	[pwd2 setStringValue:@""];
	[pwd3 setStringValue:@""];
	[title1 setStringValue:@""];
	[title2 setStringValue:@""];
	[title3 setStringValue:@""];
	
	switch(m_nJob)
	{
		case SEC_LOCKSIM:
		case SEC_UNLOCKSIM:
		case SEC_VERIFYPIN:
			[title2 setHidden:YES];
			[title3 setHidden:YES];
			[pwd2 setHidden:YES];
			[pwd3 setHidden:YES];
			[title1 setStringValue:LocalizeString(@"IDS_INPUT_OLDPIN")];
			[pwd1 setStringValue:@""];
			ret = [dele getComAgent]->QueryPIN(self, WM_QUERY_PIN, RESP_QUERY_REMAINTRY, PWD_PIN);			
			break;
		case SEC_CHANGEPIN:
			[title1 setHidden:NO];
			[title2 setHidden:NO];
			[title3 setHidden:NO];
			[pwd1 setHidden:NO];
			[pwd2 setHidden:NO];
			[pwd3 setHidden:NO];
			[title1 setStringValue:LocalizeString(@"IDS_INPUT_OLDPIN")];
			[title2 setStringValue:LocalizeString(@"IDS_INPUT_NEWPIN")];
			[title3 setStringValue:LocalizeString(@"IDS_INPUT_CONFIRM")];
			ret = [dele getComAgent]->QueryPIN(self, WM_QUERY_PIN, RESP_QUERY_REMAINTRY, PWD_PIN);
			break;
		case SEC_CHANGEPIN2:
			[title1 setHidden:NO];
			[title2 setHidden:NO];
			[title3 setHidden:NO];
			[pwd1 setHidden:NO];
			[pwd2 setHidden:NO];
			[pwd3 setHidden:NO];
			[title1 setStringValue:LocalizeString(@"IDS_INPUT_OLDPIN2")];
			[title2 setStringValue:LocalizeString(@"IDS_INPUT_NEWPIN2")];
			[title3 setStringValue:LocalizeString(@"IDS_INPUT_CONFIRM")];
			ret = [dele getComAgent]->QueryPIN(self, WM_QUERY_PIN, RESP_QUERY_REMAINTRY, PWD_PIN2);
			break;
		case SEC_VERIFYPUK:
			[title1 setHidden:NO];
			[title2 setHidden:NO];
			[title3 setHidden:NO];
			[pwd1 setHidden:NO];
			[pwd2 setHidden:NO];
			[pwd3 setHidden:NO];
			[title1 setStringValue:LocalizeString(@"IDS_INPUT_PUK")];
			[title2 setStringValue:LocalizeString(@"IDS_INPUT_NEWPIN")];
			[title3 setStringValue:LocalizeString(@"IDS_INPUT_CONFIRM")];
			ret = [dele getComAgent]->QueryPIN(self, WM_QUERY_PIN, RESP_QUERY_REMAINTRY, PWD_PUK);
			break;
		case SEC_VERIFYPUK2:
			[title1 setHidden:NO];
			[title2 setHidden:NO];
			[title3 setHidden:NO];
			[pwd1 setHidden:NO];
			[pwd2 setHidden:NO];
			[pwd3 setHidden:NO];
			[title1 setStringValue:LocalizeString(@"IDS_INPUT_PUK2")];
			[title2 setStringValue:LocalizeString(@"IDS_INPUT_NEWPIN2")];
			[title3 setStringValue:LocalizeString(@"IDS_INPUT_CONFIRM")];
			ret = [dele getComAgent]->QueryPIN(self, WM_QUERY_PIN, RESP_QUERY_REMAINTRY, PWD_PUK2);
			break;
		default:
			break;
	}
		
	if (ret != CA_OK)
	{
		[retryTimes setStringValue:LocalizeString(@"IDS_ERROR_CHANGEPASSWORD")];
		[btnCancel setEnabled:TRUE];
	}
}

- (IBAction)OnOKClick:(id)sender
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	
	if (![self CheckPassword])
		return;
	
	[btnCancel setEnabled:FALSE];
	[btnOK setEnabled:FALSE];
	CAERR ret = CA_OK;

	if (m_nJob == SEC_LOCKSIM)
	{
		//	seems no need to check
		NSString* str = [pwd1 stringValue];
		ret = [dele getComAgent]->SetSIMLock(self, WM_RSPCALLBACK, RESP_SET_CLCK, TRUE, CString([str UTF8String]));
	}
	else if (m_nJob == SEC_UNLOCKSIM)
	{
		NSString* str = [pwd1 stringValue];
		ret = [dele getComAgent]->SetSIMLock(self, WM_RSPCALLBACK, RESP_SET_CLCK, FALSE, CString([str UTF8String]));
	}
	else if (m_nJob == SEC_VERIFYPIN)
	{
		NSString* str = [pwd1 stringValue];
		ret = [dele getComAgent]->PinValidate(self, WM_RSPCALLBACK, CC_PINVALID, CString([str UTF8String]), _T(""));
	}
	else if (m_nJob == SEC_CHANGEPIN)
	{
		NSString* str1 = [pwd1 stringValue];
		NSString* str2 = [pwd2 stringValue];
		ret = [dele getComAgent]->ChangePassword(self, WM_RSPCALLBACK, RESP_SET_PIN, FL_SC, CString([str1 UTF8String]), CString([str2 UTF8String]));
	}
	else if (m_nJob == SEC_CHANGEPIN2)
	{
		NSString* str1 = [pwd1 stringValue];
		NSString* str2 = [pwd2 stringValue];
		ret = [dele getComAgent]->ChangePassword(self, WM_RSPCALLBACK, RESP_SET_PIN2, FL_P2, CString([str1 UTF8String]), CString([str2 UTF8String]));
	}
	else if (m_nJob == SEC_VERIFYPUK)
	{
		NSString* str1 = [pwd1 stringValue];
		NSString* str2 = [pwd2 stringValue];
		ret = [dele getComAgent]->PinValidate(self, WM_RSPCALLBACK, RESP_SET_PUK, CString([str1 UTF8String]), CString([str2 UTF8String]));			
	}
	else if (m_nJob == SEC_VERIFYPUK2)
	{
		NSString* str1 = [pwd1 stringValue];
		NSString* str2 = [pwd2 stringValue];
		ret = [dele getComAgent]->PinValidate(self, WM_RSPCALLBACK, RESP_SET_PUK2, CString([str1 UTF8String]), CString([str2 UTF8String]));			
	}
	
	if (ret != CA_OK)
	{
		[retryTimes setStringValue:LocalizeString(@"IDS_ERROR_CHANGEPASSWORD")];
		[btnCancel setEnabled:TRUE];
	}
}

- (void) OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	if (Msg == WM_RSPCALLBACK)
	{
		WATCH_POINTER(lparam);
		CRespPinState *pResp = (CRespPinState *)lparam;		
		int type = pResp->Value().alpha;
		int remain_try  = pResp->Value().beta;
		
		if (HIWORD(wparam) == ER_OK)
		{
			if (LOWORD(wparam) == RESP_SET_CLCK)
				[dele settingsCtrl]->m_bSimLock = ![dele settingsCtrl]->m_bSimLock;

			[retryTimes setStringValue:LocalizeString(@"IDS_PROMPT_SUCCESS")];
			[btnCancel setEnabled:TRUE];
			Sleep(1000);
			[NSApp abortModal];
			return;
		}

		//	error
		WORD dwErr = HIWORD(wparam);
		if (dwErr == ER_PUKREQ)
		{
			m_nJob = SEC_VERIFYPUK;
			[self showJobCtrls];
			return;
		}
		if ((dwErr != ER_WRONGPWD) && (dwErr != ER_PUK2REQ) && (dwErr != ER_UNKNOWN))
		{
			[btnCancel setEnabled:TRUE];
			if(remain_try == 0)
			{
				[retryTimes setStringValue:LocalizeString(@"IDS_ERROR_CHANGEPASSWORD")];
				Sleep(1000);
				[NSApp abortModal];
			}
			return;		
		}
		
		switch(LOWORD(wparam))
		{
			case RESP_SET_CLCK:
			/*	if (type == PWD_PIN)
					m_nJob = SEC_LOCKSIM;
				else*/ if (type == PWD_PUK)
					m_nJob = SEC_VERIFYPUK;		
				break;
			case RESP_SET_PIN:
				if (type == PWD_PIN)
					m_nJob = SEC_CHANGEPIN;
				else if (type == PWD_PUK)
					m_nJob = SEC_VERIFYPUK;
				break;
			case RESP_SET_PIN2:
				if (type == PWD_PIN2)
					m_nJob = SEC_CHANGEPIN2;
				else if (type == PWD_PUK2)
					m_nJob = SEC_VERIFYPUK2;
				break;	
			case RESP_SET_PUK:
				m_nJob = SEC_VERIFYPUK;
				break;
			case RESP_SET_PUK2:
				m_nJob = SEC_VERIFYPUK2;
				break;
			case CC_PINVALID:
				if (type == PWD_PIN)
					m_nJob = SEC_VERIFYPIN;
				else if (type == PWD_PUK)
					m_nJob = SEC_VERIFYPUK;
				break;
			default:
				break;
		}
		[self showJobCtrls];
		[retryTimes setStringValue:LocalizeString(@"IDS_TIPS_PWDWRONG")];
		Sleep(1000);
		NSString* pmt = LocalizeString(@"IDS_PROMPT_REMAIN");
		[retryTimes setStringValue:[NSString stringWithFormat:pmt, remain_try]];

		[btnOK setEnabled:FALSE];
		[btnCancel setEnabled:TRUE];
	}
	else if (Msg == WM_QUERY_PIN)
	{
		WATCH_POINTER(lparam);
		CRespPinState *pRes = (CRespPinState*)lparam;
		if (HIWORD(wparam) != ER_OK) 
		{
			[btnCancel setEnabled:TRUE];
			return;
		}
		
		int remain_try = pRes->Value().beta;
		CString strPrompt;
		
		if (remain_try ==0)/*&& m_status == SEC_PIN2)*/
		{
			if(m_nJob == SEC_CHANGEPIN2)
			{
				m_nJob = SEC_VERIFYPUK2;
				[self showJobCtrls];
				return;
			}
			else if(m_nJob == SEC_VERIFYPUK2)
			{
				[retryTimes setStringValue:LocalizeString(@"IDS_ERROR_CHANGEPASSWORD")];
				[btnCancel setEnabled:TRUE];
				Sleep(1000);
				[NSApp abortModal];
				return;
			}	
		}
		NSString* pmt = LocalizeString(@"IDS_PROMPT_REMAIN");
		[retryTimes setStringValue:[NSString stringWithFormat:pmt, remain_try]];

		[pwd1 becomeFirstResponder];
		[btnCancel setEnabled:TRUE];
		[btnOK setEnabled:TRUE];
	}
}
		 
- (IBAction)OnCancelClick:(id)sender
{
	[NSApp stopModalWithCode:0];
}

- (BOOL)CheckPassword
{
	NSString *str1 = [pwd1 stringValue];
	NSString *str2 = [pwd2 stringValue];
	NSString *str3 = [pwd3 stringValue];
	if (m_nJob == SEC_LOCKSIM || m_nJob == SEC_UNLOCKSIM)
	{
		if ([str1 length] < 4 || [str1 length] > 8 || !IsDec(str1)) {
			[retryTimes setStringValue:LocalizeString(@"IDS_TIPS_PWDWRONG")];
			[pwd1 setStringValue:@""];
			[pwd1 becomeFirstResponder];
			return FALSE;
}
	}
	else if (m_nJob == SEC_CHANGEPIN || m_nJob == SEC_CHANGEPIN2 ||
			 m_nJob == SEC_VERIFYPUK || m_nJob == SEC_VERIFYPUK2)
{
		if ([str1 length] < 4 || [str2 length] < 4 || [str3 length] < 4 ||
			[str1 length] > 8 || [str2 length] > 8 || [str3 length] > 8 ||
			!IsDec(str1) || !IsDec(str2) || !IsDec(str3))
	{
			[retryTimes setStringValue:LocalizeString(@"IDS_TIPS_PWDWRONG")];
			[pwd1 setStringValue:@""];
			[pwd2 setStringValue:@""];
			[pwd3 setStringValue:@""];
			[pwd1 becomeFirstResponder];
			return FALSE;
		}
		if ([str2 compare:str3] != NSOrderedSame)
		{
			[retryTimes setStringValue:LocalizeString(@"IDS_PSWMATCH_ERR")];
			[pwd2 setStringValue:@""];
			[pwd3 setStringValue:@""];
			[pwd2 becomeFirstResponder];
			return FALSE;
		}
	}
	return TRUE;
}

@end
