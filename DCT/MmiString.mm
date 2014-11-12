//////////////////////////////////////////////////////////////////////

#import "Win2Mac.h"
#import "callControl.h"
#import "MmiString.h"
#import "utility.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"

#define IS_DTMF_P(c)	(_T('p')==c || _T('P')==c)
#define IS_DTMF_W(c)	(_T('w')==c || _T('W')==c)

#define TIP_OWNER_DTMF	'd'

@implementation callControl(MMIString)

- (void)checkOnDtmf
{
	if (idxForDtmf <= 0)
		return;
	
	if (strForDtmf.IsEmpty() && 
		[engine DtmfTest:idxForDtmf])
	{
		// hot line turns to active
		if ([self setupDtmf])
		{
			[self beginDtmf];
		}
	}
	else if (!strForDtmf.IsEmpty() &&
			 ![engine DtmfTest:idxForDtmf])
	{
		// hot line is not hot any more
		[self closeDtmf];
	}
}

// to get the whole DTMF string without prefix, for display only
- (TCHAR)getDtmf:(CString&)strDtmf
{
	if (idxForDtmf <= 0 || strForDtmf.IsEmpty())
		return _T('\0');
	
	if (![engine DtmfTest:idxForDtmf])
		return _T('\0');
	
	// to remove prefix 'p'
	if (IS_DTMF_P(strForDtmf[(_Size_T)0]))
	{
		strDtmf = strForDtmf.Mid(1);
		return _T('p');
	}
	
	// to remove prefix 'w'
	for (int i=0; i<strForDtmf.GetLength(); i++)
	{
		if (!IS_DTMF_W(strForDtmf[(_Size_T)i]))
		{
			strDtmf = strForDtmf.Mid(i);
			return _T('w');
		}
	}
	
	return _T('\0');
}

// to get a part of DTMF string, to send
- (TCHAR)popDtmf:(CString&)strDtmf
{
	TCHAR chFix = _T('\0');
	
	if (idxForDtmf <= 0 || strForDtmf.IsEmpty())
		return chFix;
	
	if (![engine DtmfTest:idxForDtmf])
		return chFix;
	
	_Size_T i = 0;
	
	if (IS_DTMF_P(strForDtmf[(_Size_T)0]))
	{
		chFix = _T('p');
		i = 1;
	}
	else
	{
		while (i < strForDtmf.GetLength())
		{
			// to skip consecutive 'w'
			if (!IS_DTMF_W(strForDtmf[i]))
			{
				break;
			}
			i ++;
		}
	}
	
	while (i < strForDtmf.GetLength())
	{
		if (IS_DTMF_P(strForDtmf[i]) ||
			IS_DTMF_W(strForDtmf[i]) )
		{
			break;
		}
		strDtmf += strForDtmf[i];
		i ++;
	}
	
	strForDtmf = strForDtmf.Mid(i);
	
	// by now, strForDtmf should be empty or start with 'w'
	
	if (strForDtmf.IsEmpty())
		idxForDtmf = -1;
	else if (![self isValidDtmf:&strForDtmf])
		[self clearDtmf];
	
	if (strDtmf.IsEmpty())
		return _T('\0');
	else if (IS_DTMF_W(chFix))
		return _T('w');
	else
		return _T('p');
}


- (void)clearDtmf
{
	strForDtmf.Empty();
	idxForDtmf = -1;
	[self stopDtmfTimer];
}

- (void)closeDtmf
{
	[self dismissTips:TIP_OWNER_DTMF];
	[self clearDtmf];
}

- (BOOL)isValidDtmf:(const CString*)str
{
	if (str == NULL)
		return NO;
	if (str->FindOneOf(_T("0123456789*#")) < 0)
		return NO;
	else
		return YES;
}

- (BOOL)setupDtmf
{
	if (![engine DtmfTest:idxForDtmf])
	{
		return NO;
	}
	
	if (idxForDtmf > 0 && strForDtmf.IsEmpty())
	{
		const CCall *p = [engine callList]->Get(idxForDtmf);
		
		if ([self isValidDtmf:p->GetDtmf()])
		{
			strForDtmf = *(p->GetDtmf());
			return YES;
		}
		
		[self clearDtmf];
	}
	
	return NO;
}

- (void)beginDtmf
{
	CString strMsg, strDtmf;
	
	[self dismissDtmfWin];
	
	switch ([self getDtmf:strDtmf])
	{
		case _T('p'):
		case _T('P'):
			[self showDtmfWin:_T('p') dtmfString:strDtmf];			
			[self startDtmfTimer];
			break;
			
		case _T('w'):
		case _T('W'):
			[self showDtmfWin:_T('w') dtmfString:strDtmf];
			break;
			
		default:
			[self clearDtmf];
			break;
	}
}

- (void)sendDtmf
{
	CString strDtmf;

	if ([self popDtmf:strDtmf] != _T('\0'))
	{
		// send the current part
		if (CA_OK != [engine SendDTMF:strDtmf])
		{
			[self clearDtmf];
		}
	}
	else
	{
		// try the next part
		[self beginDtmf];
	}
}

- (void)showDtmfWin:(TCHAR)type
		 dtmfString:(CString&)str
{
//	ASSERT(dlgForDtmf == nil);
	
	NSString *strDtmf = [NSString CStringToNSString:str];	
	NSString *strFmt = nil;
	NSString *strMsg = nil;
	if (IS_DTMF_W(type))
	{
		strFmt = LoadString(@"IDS_CC_DTMFCONFIRM");
		strMsg = [NSString stringWithFormat:strFmt, strDtmf];
		
		[self showConfirm:strMsg
				withOwner:TIP_OWNER_DTMF
			  andSelector:@selector(onDtmfConfirm:)];
	}
	else
	{
		strFmt = LoadString(@"IDS_CC_SENDINGDTMF");
		strMsg = [NSString stringWithFormat:strFmt, strDtmf];

		[self showNotify:strMsg
			   withOwner:TIP_OWNER_DTMF];
	}
}

- (void)dismissDtmfWin
{
	[self dismissTips];
//	dlgForDtmf = nil;
}

- (void)onDtmfConfirm:(NSNumber*)code
{
	if ([code boolValue])
		[self sendDtmf];
	else
		[self clearDtmf];
}

- (void)onDtmfTimer:(NSTimer*)timer
{
	// get DTMF string & send
//	HideWaitWin();
	dtmfTimer = nil;
	[self sendDtmf];
}

- (void)startDtmfTimer
{
	ASSERT(dtmfTimer == nil);
	
	dtmfTimer = [self setTimer:3.0
					  selector:@selector(onDtmfTimer:)
					   repeats:NO];
}

- (void)stopDtmfTimer
{
	[dtmfTimer invalidate];
	dtmfTimer = nil;
}


- (BOOL)crackDialString:(NSString*)mmiStr
				forDial:(CString&)strForDial
				forDtmf:(CString&)strFroDtmf
{
	if ([mmiStr length] == 0)
		return NO;
	
	if (![utility checkPhoneNumber:mmiStr])
		return NO;
	
	NSCharacterSet *set = 
		[NSCharacterSet characterSetWithCharactersInString:@"pwPW"];
	
	NSUInteger pos = [mmiStr rangeOfCharacterFromSet:set].location;
	
	if(pos != NSNotFound)
	{
		ASSERT(pos > 1);	// ensured by 'checkPhoneNumber'
		
		strForDial = 
			[NSString NSStringToCString:[mmiStr substringToIndex:pos]];
		
		strForDtmf = 
			[NSString NSStringToCString:[mmiStr substringFromIndex:pos]];
	}
	else
	{
		strForDial = [NSString NSStringToCString:mmiStr];
		strForDtmf.Empty();
	}
	
	return YES;
}

@end