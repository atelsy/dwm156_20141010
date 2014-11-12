//
//  utility.m
//  DCT
//
//  Created by Fengping Yu on 11/26/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "utility.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"

#define IS_DTMF_MARK(c) (c == 'p' || c == 'P' || c == 'w' || c== 'W')

static int nModalCounter = 0;

@implementation classToObject

- (id)init
{
	self = [super init];
	if(self)
	{
		// Initialization code here.
//        pMsg = new SMSMessage;
//        pFolder = new SmsFolder;
//	    pbFolder = new PbFolder;
	}
	
	return self;
}

- (void)dealloc
{
//	delete pMsg;
  //  delete pFolder;
	
	[super dealloc];
}

//@synthesize pMsg, pFolder, pbFolder;

@end

@implementation utility

#pragma mark--
#pragma mark construct and destrouct function
#pragma mark--

- (id)init
{
	self = [super init];
	if (self) 
	{
		// Initialization code here.
	}
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

#pragma mark - call history operation

+ (ChFolder*)newChFolder
{
	ChFolder *pFolder = NULL;
	
	try 
	{
		pFolder = new ChFolder;
	} 
	catch (...)
	{
		return NULL;
	}
	
	return pFolder;
}

#pragma mark--
#pragma mark phonebook operation
#pragma mark --

+ (PbFolder*)newPbFolder
{
	PbFolder *pFolder = NULL;
	
	try
	{
		pFolder = new PbFolder;
	}
	catch(...)
	{
		//todo: add the alert message box
		
		return NULL;
	}
	
	return pFolder;
}

+ (PhoneBookItem*) NewContact
{
	PhoneBookItem *pItem = NULL;
	try
	{
		pItem = new PhoneBookItem;
	}
	catch (...)
	{
		[self showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_NEWCONTACT") forType:MB_OK];
		
	}
	
	return pItem;
}

#pragma mark--
#pragma mark sms operation
#pragma mark --

+ (SmsFolder*)newSmsFolder
{
	SmsFolder *pFolder = NULL;
	
	try
	{
		pFolder = new SmsFolder;
	}
	catch(...)
	{
		//todo: add the alert message box
		
		return NULL;
	}
	
	return pFolder;
}

+ (SMSMessage*)newMessage
{
	SMSMessage *pMsg = NULL;
	
	try 
	{
		pMsg = new SMSMessage;
	} 
	catch (...) 
	{
		[self showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_NEWMESSAGE") forType:MB_OK];
	}
	
	return pMsg;
}

+ (unsigned short*)getEmsText:(EMSData *)pEmsData withLength:(int *)len
{
	if((pEmsData != NULL) && (pEmsData->textLength > 0))
	{
		unsigned char *ptemp = NULL;
		try 
		{
			ptemp = new unsigned char[pEmsData->textLength];
		}
		catch (...)
		{
			return nil;
		}
		
		int nlen2 = pEmsData->textLength / 2;
		memcpy(ptemp, pEmsData->textBuffer, pEmsData->textLength);
		convertText(ptemp, nlen2);
		
		unsigned short *pstring = (unsigned short*)ptemp;
		
		unsigned short *spbuf;
		int nlen = nlen2;
		
		try 
		{
			spbuf = new unsigned short[nlen + 1];
		}
		catch (...) 
		{
			return nil;
		}
		
		memcpy(spbuf, pstring, nlen * 2);
		
		spbuf[nlen] = '\0';
		
		*len = nlen;
		
		if(ptemp)
			delete [] ptemp;
		
		return spbuf;
	}
	
	return NULL;
}

+ (NSString*)getEmsText:(EMSData*)pEmsData
{
	int nLen;
	unsigned short *pbuf = [utility getEmsText:pEmsData withLength:&nLen];
	
	if(pbuf)
	{
		NSString *strText = [[[NSString alloc] initWithCharacters:pbuf length:nLen] autorelease];
		delete pbuf;
		
		return strText;
	}
	
	return nil;
}

+ (BOOL)deleteEmsObject:(SMSMessage *)pMsg
{
	EMSData *pEmsData = NULL;
	EMSData *newEmsData = new EMSData;
	EMSTATUS status;
	unsigned short *pbuf;
	int nLen;
	
	pMsg->GetMsg(&pEmsData);
	
	if(InitializeEMSData(newEmsData, pEmsData->dcs) != EMS_OK)
		return NO;
	
	pbuf = [utility getEmsText:pEmsData withLength:&nLen];
	
//	unsigned short *pString;
	unsigned char *pBuf;
	
	if(nLen <= 0)
	{
		status = EMS_OK;
	}
	else
	{
		pBuf = (unsigned char*)pbuf;
		
		convertText(pBuf, nLen);
		
		status = AddString(newEmsData, pBuf, nLen, NULL);
		
		delete pbuf;
	}
	
	if(status != EMS_OK)
	{
		ReleaseEMSData(newEmsData);
		delete newEmsData;
		newEmsData = NULL;
		
		return NO;
	}
		
	
	if(CopyEMSData(pEmsData, newEmsData) != EMS_OK)
	{
		ReleaseEMSData(newEmsData);
		delete newEmsData;
		newEmsData = NULL;
		
		return NO;
	}
	
	if(newEmsData)
	{
		ReleaseEMSData(newEmsData);
		delete newEmsData;
		newEmsData = NULL;
	}
	
	return YES;
}

+ (void)processSMSFolder:(SmsFolder *)pFolder
{
	SMSMessage *pMsg = NULL;
	ItemPos pos = pFolder->GetFirstItemPos();
	
	while(pos != 0)
	{
		pMsg = pFolder->GetNextItem(pos);
		if(![utility deleteEmsObject:pMsg])
		{
			pFolder->CloseNextItem(pos);
			return;
		}
	}
	
	pFolder->CloseNextItem(pos);
	
	return;
}

+ (BOOL)getState:(SMSMessage *)pMsg withFolder:(SmsFolder *)pFolder
{
	if(!pMsg || !pFolder)
		return NO;
	
	folder_state_struct state;
	pFolder->GetFolderState(state);
	
	int result = pMsg->GetStatus() & state.nState;
	
	if(result > 0)
		return YES;
	
	return NO;
}

+ (NSArray*)getMultiAddressOfMsg:(SMSMessage *)pMsg
{
	if(!pMsg)
		return nil;
	
	CString str;
	pMsg->GetAddressNumber(str);
	
	NSString *address = [NSString getStringFromWchar:(LPCTSTR)str];
	
	NSArray *array = [utility getMultiAddress: address];
	
	if([array count] > 0)
		pMsg->SetAddressNumber(CString([[array objectAtIndex:0] UTF8String]));
	
	return array;
}

+ (NSString*)getTime:(SMSMessage *)pMsg
{
	if(pMsg)
	{
		tm time;
		char buf[128];
		
		pMsg->GetTime(time);
		
		strftime(buf, 128, "%Y/%m/%d %X", &time);
		
		NSString *strTime = [[[NSString alloc] initWithUTF8String:buf] autorelease];
		
		return strTime;
	}
	
	return nil;
}

#pragma mark --
#pragma mark text convert function
#pragma mark --

//used for download

void convertText(unsigned char *pBuf, int nLen)
{
	if(!pBuf)
		return;
	
	for(int i = 0; i < nLen * 2; i+=2)
	{
		unsigned char ch = pBuf[i];
		pBuf[i] = pBuf[i+1];
		pBuf[i+1] = ch;
	}
}

//used for write, send (to delete the extra 0x00, 0x00, as in mac, one character is own 4 byte

void reConvertText(unsigned char *pbuf, size_t len)
{
	if(!pbuf)
		return;
	
	int k = 0;
	
	for(int i = 0; i < len * 4 ; i+=4)
	{
		unsigned char ch = pbuf[i];
		pbuf[k++] = pbuf[i+1];
		pbuf[k++] = ch;
	}
	
	pbuf[k] = 0;
}

#pragma mark--
#pragma mark common function
#pragma mark--

+ (NSInteger)showMessageBox:(NSString *)strCaption 
					withMsg:(NSString *)msg 
					forType:(int)type
{
	NSString *defaultButton = nil;
	NSString *alternateButton = nil;
	NSString *otherButton = nil;
	
	switch (type) 
	{
		case MB_OK:
			defaultButton = LocalizeString(@"OK");
			break;
			
		case MB_OKCANCEL:
			defaultButton = LocalizeString(@"OK");
			alternateButton = LocalizeString(@"CANCEL");
			break;
			
		case MB_ABORTRETRYIGNORE:
			defaultButton = LocalizeString(@"ABORT");
			alternateButton = LocalizeString(@"RETRY");
			otherButton = LocalizeString(@"IGNORE");
			break;

		case MB_YESNOCANCEL:
			defaultButton = LocalizeString(@"YES");
			alternateButton = LocalizeString(@"NO");
			otherButton = LocalizeString(@"CANCEL");
			break;
			
		case MB_YESNO:
			defaultButton = LocalizeString(@"YES");
			alternateButton = LocalizeString(@"NO");
			break;
			
		case MB_RETRYCANCEL:
			defaultButton = LocalizeString(@"RETRY");
			alternateButton = LocalizeString(@"CANCEL");
			break;
			
		default:
			break;
	}
	
	[utility setModalCounter:YES];
	
	NSAlert* alert = [NSAlert alertWithMessageText:strCaption 
									 defaultButton:defaultButton 
								   alternateButton:alternateButton 
									   otherButton:otherButton 
						 informativeTextWithFormat:msg];
	
	NSInteger result = [alert runModal];
	
   // int result = (int)NSRunAlertPanel(strCaption, msg, defaultButton, alternateButton, otherButton);
	
	[utility setModalCounter:NO];
	
	return result;
}

+ (int)getModalCounter
{
	return nModalCounter;
}

+ (int)setModalCounter:(signed char)bAdd
{
	if(bAdd)
		nModalCounter++;
	else
	{
		nModalCounter--;
	}
	
	return nModalCounter;
}

+ (NSArray*)getMultiAddress:(NSString *)str
{
	return [str componentsSeparatedByCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@",;"]];
}

#pragma mark--
#pragma PhoneBook method
#pragma mark--

+ (BOOL)checkPhoneNumber:(const NSString *)str
{
	NSCharacterSet *set = [NSCharacterSet characterSetWithCharactersInString:@"0123456789+*#pwPW"];
	
	if([str caseInsensitiveCompare:@"+"] == 0)
		return YES;
	
	NSUInteger pos = [str rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"+"]].location ;
	
	if(pos >= 1 && pos != NSNotFound)
		return NO;
	
	NSUInteger num = [str length];
	
	if(num > 0 && IS_DTMF_MARK([str characterAtIndex:0]))
		return NO;
	
	for(NSUInteger i = 0; i < num; i++)
	{
		if ([set characterIsMember:[str characterAtIndex:i]] == NO)
		{
			return NO;
		}
	}
	
	return YES;
}

+ (int)CheckEncodeClass:(const CString&)str andLen:(int&)nLen
{
	unsigned int code;
	int encodeClass = 0;
	unsigned int base = 0;
	int i = 0;
	
	wchar_t wc[4096];
#ifdef _UNICODE
	_tcscpy(wc, str);
	size_t wLen = _tcslen(wc);
#else
	int wLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, str.GetLength(), wc,  sizeof(wc)/2);
#endif
	unsigned int base1 = 0;
	for(i=0; i < wLen ; i++)
	{
		code = wc[i];
		if(code >= 0x80)
		{
			base1 = Get0x81Base(code);
			if (0 == base1 || (base != 0 && base1 != base))
			{
				encodeClass = PBCHS_UCS2; // UCS2
				break;
			}
			base = base1;
			encodeClass = PBCHS_0x81; // 0x81
		}
	}
	nLen = (int)wLen;
	
	
	if (PBCHS_DEFAULT == encodeClass)
	{	// GSM 7-bit
		const static TCHAR *p = _T("\\|[]{}^~");
		for (i=0; i<wLen; i++)
		{
			if (_tcschr(p, wc[i]) != NULL)
				nLen ++;
		}
	}
	
	return encodeClass;
}

+ (BOOL)CheckPhoneChar:(const CString &)str
{
	const static TCHAR *p = _T("\\`;\"");
	return (str.FindOneOf(p) < 0);
}

+ (BOOL)CheckUnicode:(const CString&)str andLen:(size_t&)nLen2
{
	BOOL b = FALSE;
	nLen2 = str.GetLength();
	int nLen;
	size_t realLen = nLen2;
	
	for(size_t i=0;i<realLen;i++)
	{
		if((unsigned short)str[i]>(unsigned short)127)
			b = TRUE;
		if((unsigned short)str[i] == 0x0D)
		{
			if((i+1 < realLen)&&((unsigned short)str[i] == 0x0A))
			{
				;
			}
			else
			{
				nLen2++;
			}
		}
	}
	nLen = WideCharToMultiByte(CP_ACP, 0, str, realLen, NULL, 0, NULL, NULL);
	return ([utility CheckUnicodeLen:nLen andLen:realLen] | b);
}

+ (BOOL)CheckUnicodeLen:(const size_t)nLen andLen:(const size_t)nWLen
{
	if(nLen != nWLen)	// Unicode - contains 2 bytes
		return TRUE;
	else				// Ansi
		return FALSE;
}

+ (BOOL)CheckPhoneNumberLen:(const CString&)str andFlag:(BOOL)bTarget
{
	size_t nLen, nDefault;
	if (bTarget)
		nDefault = 41;
	else
		nDefault = 31;
	
	BOOL bUnicode = [utility CheckUnicode:str andLen:nLen];
	
	if(bUnicode)
		return FALSE;
	
	if(nLen > nDefault)
		return FALSE;
	else if(nLen == nDefault)
	{
		if(str.GetAt(0) != _T('+'))
			return FALSE;
	}
	
	return TRUE;
}

+ (BOOL)CheckPhoneHomeNumber:(const CString&)str
{
	return [utility CheckPhoneNumberLen:str andFlag:FALSE];
}

+ (BOOL)CheckPhoneCompanyName:(const CString&)str
{
	size_t nLen;
	[utility CheckUnicode:str andLen:nLen];
	
	if(nLen > 20)
		return FALSE;
	
	return TRUE;
}

+ (BOOL)CheckEmailLocal:(const CString&)strEml
{
	/* Reference RFC 2822 */
	const static TCHAR *strSet = _T(".!#$%&\\*+-/=?^_`{|}~");
	
	size_t nLen = strEml.GetLength();
	
	if (nLen <= 0)
		return FALSE;
	
	/* '.' at the begin or end position of local part */
	if (_T('.') == strEml[(size_t)0] || _T('.') == strEml[nLen-1])
		return FALSE;
	
	// check valid characters
	for (size_t i=0; i<nLen; i++)
	{
		if ((strEml[i] >= _T('a') && strEml[i] <= _T('z')) ||
			(strEml[i] >= _T('A') && strEml[i] <= _T('Z')) ||
			(strEml[i] >= _T('0') && strEml[i] <= _T('9')) )
		{
			continue;
		}
		
		if (_tcschr(strSet, strEml[i]) == NULL)
			return FALSE;
	}
	
	return TRUE;
}

+ (BOOL)CheckEmailDomain:(const CString&)strEml
{
	size_t nLen = strEml.GetLength();
	
	if (nLen <= 0)
		return FALSE;
	
	/* '.' at the begin or end position of domain part */
	if (_T('.') == strEml[(size_t)0] || _T('.') == strEml[nLen-1])
		return FALSE;
	
	for (size_t i = 0; i < strEml.GetLength()-1; i++)
	{
		if ((strEml[i] >= _T('a') && strEml[i] <= _T('z')) ||
			(strEml[i] >= _T('A') && strEml[i] <= _T('Z')) ||
			(strEml[i] >= _T('0') && strEml[i] <= _T('9')) ||
			(strEml[i] == _T('-')) || (strEml[i] == _T('.')) )
		{
			continue;
		}
		else
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

+ (BOOL)CheckPhoneEmail:(const CString&)str
{
	size_t nLen = str.GetLength();
	
	if (nLen <= 0)	// empty
		return TRUE;
	if(nLen > 30 || nLen < 3)
		return FALSE;
	
	/* find the '@' position */
	size_t nTmp = str.Find(_T('@'));
	
	/* '@' at first or last or no '@' */
	if (nTmp <= 0 || nTmp >= nLen-1)
		return FALSE;
	
	if (![utility CheckEmailLocal:str.Left(nTmp)])
		return FALSE;
	
	if (![utility CheckEmailDomain:str.Mid(nTmp+1)])
		return FALSE;
	
	return TRUE;
}

+ (BOOL)CheckPhoneOfficeNumber:(const CString&)str
{
	return [utility CheckPhoneNumberLen:str andFlag:FALSE];
}

+ (BOOL)CheckPhoneFaxNumber:(const CString&)str
{
	return [utility CheckPhoneNumberLen:str andFlag:FALSE];
}

+ (BOOL)CheckValidDate:(int)nYear andMonth:(int)nMonth andDay:(int)nDay
{
	int MonthDays[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
	
	if(nYear > 2100 || nMonth < 1 || nMonth > 12)
		return FALSE;

	BOOL bLeapYear = ((nYear & 3) == 0) && ((nYear % 100) != 0 || (nYear % 400) == 0);
	
	int nDaysInMonth = MonthDays[nMonth] - MonthDays[nMonth - 1] + ((bLeapYear && nDay == 29 && nMonth == 2) ? 1 : 0);
	
	if(nDay < 1 || nDay > nDaysInMonth)
		return FALSE;
	
	return TRUE;
}

+ (BOOL)CheckPhoneBirthday:(const CString&)str andAdvData:(Advanced_PBItem_Struct&)adv_pb_item
{
	adv_pb_item.m_birth_year = 0;
	adv_pb_item.m_birth_month = 0;
	adv_pb_item.m_birth_day = 0;
	
	if(str.IsEmpty())
		return TRUE;
	
	int year, month, day;
	if (_stscanf(str, _T("%04d/%02d/%02d"), &year, &month, &day) < 3)
		return FALSE;
	
	// check if before 1900/01/01
	if(year < 1900)
		return FALSE;
	
	// check if valid
	if (![utility CheckValidDate:year andMonth:month andDay:day])
		return FALSE;

	adv_pb_item.m_birth_year = year;
	adv_pb_item.m_birth_month = month;
	adv_pb_item.m_birth_day = day;
	
	return TRUE;
}


#pragma mark--
#pragma help method
#pragma mark--
/*
+ (cellData*)createToolbarCellwithImage:(NSString *)imageName tip:(NSString *)tooltip tag:(int)_tag state:(signed char)bEnable appTag:(appTag)appTag
{
	cellData *data = [[cellData alloc] init];
	data.image = [NSImage imageNamed:imageName];
	data.tooltip = tooltip;
	data.tag = _tag;
	data.bEnable = bEnable;
	data.appTag = appTag;
	
	return data;
}
*/
@end
