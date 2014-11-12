//
//  emsEdit.m
//  DCT
//
//  Created by Fengping Yu on 12/1/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "emsEdit.h"
#include "Win2Mac.h"
#import "utility.h"
#import "DCTAppDelegate.h"
#import "string.h"

@implementation emsEdit

#pragma mark --
#pragma mark construct and destructor
#pragma mark--

- (id)init
{
	self = [super init];
	if (self) 
	{
		// Initialization code here.
		[self initDefaultParameters];
	}
	
	return self;
}

- (void)initDefaultParameters
{
	m_bUnicode = NO;
	m_bSelFolder = NO;
	m_nPos = 0;
	m_changeType = CHANGETYPE_REPLACESEL;
	m_bFirst = YES;
	InitializeEMSData(&m_EmsData, SMSAL_DEFAULT_DCS);
}

- (void)dealloc
{
	ReleaseEMSData(&m_EmsData);
	
	[super dealloc];
}

#pragma mark--
#pragma mark helper method
#pragma mark--

- (void)setChangeType:(ChangeType)type
{
	m_changeType = type;
}

- (void)setParent:(id)_parent
{
	m_parent = _parent;
}

#pragma mark--
#pragma mark get function
#pragma mark--

- (void)getInfo:(int &)nNum unit:(int &)nUit message:(int &)nMessage isEms:(signed char &)bEms ischeck:(signed char)bCheck
{
	int nLen = (int)[[[self textStorage] string] length];
	if(nLen > m_nLen)
		m_nLen = nLen;
	
	if(bCheck)
	{
		[self CheckUnicodeAll:nil];
	}
	
	nUit = m_EmsData.Reminder.segUsableOctet;
	nMessage = m_EmsData.Reminder.requiredSegment;
	int nTotoalUsedNum = nUit * nMessage - m_EmsData.Reminder.segRemainingOctet;
	int nTotalReminNum = nUit * nMessage - nTotoalUsedNum;
	
	nNum = nTotoalUsedNum;
	nUit = nTotalReminNum;
	
	if(m_EmsData.listHead == NULL)
		bEms = NO;
	else
		bEms = YES;
}

- (EMSData*)getEmsData
{
	return &m_EmsData;
}

- (long)getTextLength
{
	return [[[self textStorage] string] length];
}

- (void)debugedit
{
	BOOL ret;
	
	ReleaseEMSData(&m_EmsData);
	InitializeEMSData(&m_EmsData, SMSAL_DEFAULT_DCS);
	
	m_bUnicode = NO;
	
	long nStart = 0, nEnd = 0;
	
	[self getSel:&nStart andEnd:&nEnd];
	
	ret = [self addTextFrom: 0 to: nEnd];
	
	if(ret)
		[self setSel:nEnd andEnd:nEnd];
}

#pragma mark--
#pragma mark edit change function
#pragma mark --

- (void)onChange
{
	[self debugedit];
	return;
	
	switch(m_changeType)
	{
		case CHANGETYPE_CUT:
		case CHANGETYPE_PASTE:
		case CHANGETYPE_REPLACESEL:
		case CHANGETYPE_CHINESE:
		{
			
			//[self hideSelection];
			
			//   ReleaseEMSData(&m_EmsData);
			//   InitializeEMSData(&m_EmsData, SMSAL_DEFAULT_DCS);
			
			
			// [self checkSelection];
			
			//get curosr position
			long nStart = 0, nEnd = 0;
			
			[self getSel:&nStart andEnd:&nEnd];
			
			[self setSel:m_nPos andEnd:nEnd];
			
			BOOL ret;
			if(m_bSelFolder)
				m_EmsData.id = 0;
			
			ret = [self addTextFrom: m_nPos to: nEnd];
			
			if(ret)
				[self setSel:nStart andEnd:nEnd];
			else
				[self setSel:m_nPos andEnd:m_nPos];
			
			m_nLen = [self getTextLength];
			
			// [self showSelection];
			break;
		}
			
		default:
			break;
	}
	
	m_changeType = CHANGETYPE_UNDO;
}

- (BOOL)addTextFrom:(long)nStart to:(long)nEnd
{
	[self hideSelection];
	
	int nLen;
	CString str;
	
	for(int i = (int)nStart; i < nEnd; i++)
	{
		nLen = [self getSelectionLen:i to:i+1 withString:str];
		if(nLen == 0)
			nLen = [self getSelectionLen:i to:i + 2 withString:str];
		
		if(nLen == 0)
			continue;
		
		if([self addText:str] == EMS_OK)
		{
			i += nLen;
			m_nPos = i;
		}
		else
		{
			[self setSel:i andEnd:nEnd];
			[self clear];
			
			return NO;
		}
		
		--i;
	}
	
	[self showSelection];
	
	return YES;
}

- (EMSTATUS)addText:(CString)str
{
	if(str.IsEmpty())
		return EMS_OK;
	
	EMSTATUS status;
	
	if(EMS_OK == (status = [self CheckUnicodeAll:nil]))
	{
		unsigned short *pString;
		unsigned char *pBuf;
		CString tmpstr;
		
		//   NSLog(@"str length is = %d.\n", str.GetLength());
		
		for(int i = 0; i < str.GetLength(); i++)
		{
			if(str.GetAt(i) == 0x0D)// && (i + 1 < str.GetLength() && str.GetAt(i + 1) != 0x0A))
			{
				return EMS_OK;
				tmpstr += str.GetAt(i);
				tmpstr += TCHAR(0x0A);
			}
			else if(str.GetAt(i) == 0x0A && (i == 0 || (i > 1 && str.GetAt(i - 1) != 0x0D)))
			{
				tmpstr += TCHAR(0x0D);
				tmpstr += str.GetAt(i);
			}
			else
			{
				tmpstr += str.GetAt(i);
			}
		}
		
		pString = (unsigned short*)(LPCTSTR)tmpstr;
		pBuf = (unsigned char*)pString;
		
		reConvertText(pBuf, tmpstr.GetLength());
		
		status = AddString(&m_EmsData, pBuf, tmpstr.GetLength(), NULL);
	}
	
	if(status != EMS_OK)
	{
		switch (status)
		{
			case EMS_NO_SPACE:
				[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_EDITMESSAGE") forType:MB_OK];
				break;
				
			case EMS_INVALID_DCS:
				[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_EMS_INVALIDDCS") forType:MB_OK];
				break;
				
			case EMS_NO_MEMORY:
				[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_EMS_NOMEMORY") forType:MB_OK];
				break;
				
			default:
				[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_EMS_UNSPECIFIED") forType:MB_OK];
				break;
		}
	}
	
	return status;
}

- (void)setEmsData:(EMSData *)pEmsData withSelFolder:(BOOL)bSelFolder
{
	// hide selection
	m_bSelFolder = bSelFolder;
	
	if(!pEmsData || bSelFolder)
	{
		[self setString:@""];
		return;
	}
	
	ReleaseEMSData(&m_EmsData);
	InitializeEMSData(&m_EmsData, SMSAL_DEFAULT_DCS);
	CopyEMSData(&m_EmsData, pEmsData);
	
	m_changeType = CHANGETYPE_UNKNOWN;
	
	NSString *text = [utility getEmsText:&m_EmsData];
	
	if(text != nil)
		[self setString:text];
	
	[self hideSelection];
	
	[self setSelectedRange:NSMakeRange(0, 0)];
	
	ResetCurrentPosition(&m_EmsData);
	m_nPos = 0;
	
	[self showSelection];
}

#pragma mark--
#pragma mark selection operation
#pragma mark --

- (void)hideSelection
{
	return;
	
	/*  m_selRange = [self selectedRange];
	 
	 [self setSelectedRange:NSMakeRange(0, 0)];*/
}

- (void)showSelection
{
	return;
	
	//  [self setSelectedRange:m_selRange];
}

- (void)getSelection
{
	if(![self getValidSel:&m_nStart andEnd:&m_nEnd])
		m_nEnd = -1;
	else
	{
		m_strSel = [self getSelText];
	}
	
	if(m_nStart == 0)
		NSLog(@"\nThe start value is = %ld.\n", m_nStart);
	if(m_nEnd == 2)
		NSLog(@"\nThe end value is == %ld.\n", m_nEnd);
}

- (long)checkSelection
{
	int nLen;
	int nSelLen = -1;
	
	if(m_nEnd > m_nStart)
	{
		[self movePosition:m_nStart toEnd:m_nEnd withString:m_strSel];
		
		[self checkUnicode:m_strSel withLength:nLen];
		CancelCurrentPosition(&m_EmsData, nLen);
		
		nSelLen = (int)(m_nEnd - m_nStart);
		
		m_nLen -= nSelLen;
		
		[self resetSelection];
	}
	
	m_bFirst = YES;
	
	return nSelLen;
}

- (BOOL)checkValidSel:(long*)nStart andEnd:(long*)nEnd
{
	long nLen = [self getTextLength];
	
	if(*nEnd > nLen)
	{
		*nEnd = min(*nEnd, nLen);
		
		if(*nStart >= nLen)
			return NO;
	}
	
	return YES;
}

- (int)getSelectionLen:(int)nStart to:(int)nEnd withString:(CString &)str
{
	[self setSel:nStart andEnd:nEnd];
	long tmpStart, tmpEnd;
	
	[self getSel:&tmpStart andEnd:&tmpEnd];
	
	NSString *selText = [self getSelText];
	const char* characters = [selText cStringUsingEncoding:NSUTF16LittleEndianStringEncoding];
	
	str = CString((LPCTSTR)characters);
	
	return (int)(tmpEnd - tmpStart);
}

- (NSString*)getSelText
{
	NSString *str = [[self textStorage] string];
	
	NSRange range = [self selectedRange];
	
	if(range.location == NSNotFound)
		return @"";
	
	return [str substringWithRange:range];
}

- (void)getSel:(long*)nStart andEnd:(long*)nEnd
{
	NSRange range = [self selectedRange];
	
	// NSLog(@"The selected range length is %ld, and the location is %ld\n", range.length,  range.location);
	
	if(range.length == 0)
	{
		*nStart = *nEnd = range.location;
		/*NSUInteger len = [[self string] length];
		 if(len > 0)
		 *nStart = *nEnd = m_nPos + 1;
		 else
		 *nStart = *nEnd = 0;*/
	}
	else
	{
		*nStart = range.location;
		
		*nEnd = range.location + range.length;
	}
}

- (BOOL)getValidSel:(long*)nStart andEnd:(long*)nEnd
{
	[self getSel:nStart andEnd:nEnd];
	
	return [self checkValidSel:nStart andEnd:nEnd];
}

- (void)setSel:(long)nStart andEnd:(long)nEnd
{
	NSRange range = NSMakeRange(nStart, nEnd - nStart);
	
	[self setSelectedRange:range];
}

- (void)clear
{
	[self delete:nil];
}

- (void)resetSelection
{
	m_nStart = m_nEnd = -1;
	m_strSel = @"";
}

#pragma mark --
#pragma mark unicode operation
#pragma mark --

- (BOOL)isUnicode
{
	return m_bUnicode;
}

- (EMSTATUS)CheckUnicodeAll:(NSString *)str
{
	NSString *strings;
	EMSTATUS status = EMS_OK;
	
	if(str != nil)
		strings = str;
	else
		strings = [[self textStorage] string];
	
	if([strings canBeConvertedToEncoding:NSASCIIStringEncoding] == NO)  // unicode
	{
		if(m_bUnicode == NO)
		{
			m_bUnicode = YES;
			status = EMSSetDCS(&m_EmsData, SMSAL_UCS2_DCS);
			if(status != EMS_OK)
			{
				m_bUnicode = NO;
				return status;
			}
		}
	}
	else
	{
		if(m_bUnicode == YES)
		{
			m_bUnicode = NO;
			status = EMSSetDCS(&m_EmsData, SMSAL_DEFAULT_DCS);
		}
	}
	
	return status;
}

- (BOOL)checkUnicode:(int)nLen withWLength:(int)nWLen
{
	if(nLen != nWLen)
		return YES;
	else
		return NO;
}

- (BOOL)checkUnicode:(NSString *)str withLength:(int &)nLen2
{
	BOOL b = NO;
	
	nLen2 = (int)[str length];
	
	int nLen;
	int realLen = nLen2;
	
	for(int i = 0; i < realLen; i++)
	{
		unichar character = [str characterAtIndex:i];
		if(character > 127)
			b = YES;
	}
	
	nLen = (int)[str lengthOfBytesUsingEncoding: NSASCIIStringEncoding];
	
	return ([self checkUnicode:nLen withWLength:realLen] | b);
}

#pragma mark --
#pragma mark key and mouse event
#pragma mark --

- (void)keyDown:(NSEvent *)theEvent
{
	NSString *chars = [theEvent characters];
	
	unichar character = [chars characterAtIndex:0];
	
	NSUInteger flags = [theEvent modifierFlags];
	
	if(character == NSDeleteCharacter)
	{
		m_changeType = CHANGETYPE_BACK;
		
		[self onKeyDownDelete: theEvent];
		
		//  [super keyDown:theEvent];
	}
	else if(flags & NSCommandKeyMask && ([chars isEqualToString:@"a"] || [chars isEqualToString:@"A"]))
	{
		[self selectAll:nil];
	}
	else if(flags & NSCommandKeyMask && (character == 'x' || character == 'X'))
	{
		if ([m_parent isKindOfClass:[SmsEditCtrl class]]) 
		{
			[self commonKeyDown:theEvent];
			
			[(SmsEditCtrl*)m_parent cutString:self];
			
			return;
		}
	}
	else if(flags & NSCommandKeyMask && (character == 'c' || character == 'C'))
	{
		if([m_parent isKindOfClass:[SmsEditCtrl class]])
		{
			[self commonKeyDown:theEvent];
			[(SmsEditCtrl*)m_parent copyString:self];
		}
		
		return;
	}
	else if(flags & NSCommandKeyMask && (character == 'v' || character == 'V'))
	{
		if([m_parent isKindOfClass:[SmsEditCtrl class]])
		{
			[self commonKeyDown:theEvent];
			[(SmsEditCtrl*)m_parent pasteString:self];
			return;
		}
	}
	else
	{
		[self commonKeyDown:theEvent];
	}
}

- (void)commonKeyDown:(NSEvent *)theEvent
{
	[super keyDown:theEvent];
	return;
	
	NSString *chars = [theEvent characters];
	
	unichar character = [chars characterAtIndex:0];
	
	long nStart = 0, nEnd = 0;
	
	m_changeType = CHANGETYPE_REPLACESEL;
	BOOL bGet = YES;
	
	if(character > 128)
	{
		bGet = m_bFirst;
		if(m_bFirst)
			m_bFirst = NO;
	}
	
	if(bGet)
	{
		NSLog(@"the input character is %d.\n", character);
		[self getSelection];
		
		[self checkSelection];
	}
	
	[super keyDown:theEvent];
	
	if(character > 128 && [self getValidSel:&nStart andEnd: &nEnd])
	{
		if(m_nEnd > m_nStart && nEnd == nStart)
		{
			[self hideSelection];
			
			long s, e, d;
			NSString *str1, *str2;
			str1 = [self getSelText];
			
			d = m_nEnd - m_nStart - 2;
			[self setSel:m_nEnd - d andEnd:m_nOriLen - d];
			str2 = [self getSelText];
			[self setSel:s andEnd:e];
			
			[str1 stringByAppendingString:str2];
			[self CheckUnicodeAll:str1];
			
			[self showSelection];
			
			[self checkSelection];
		}
	}
	
}

- (void)mouseDown:(NSEvent *)theEvent
{
	[super mouseDown:theEvent];
	
	if([theEvent type] == NSLeftMouseDown)
	{
		long nStart = 0, nEnd = 0;
		[self getSel:&nStart andEnd:&nEnd];
		
		if(nStart == nEnd)
		{
			[self movePosition:nStart toEnd:nEnd withString:nil];
		}
		else
		{
			int nLen = (int)[[[self textStorage] string] length];
			if(nLen <= 0)
				return;
			
			if(nEnd - nStart > nLen)
				nEnd = nLen;
			
			[self movePosition:nEnd toEnd:nEnd withString:nil];
		}
	}
}

- (void)mouseUp:(NSEvent *)theEvent
{
	[super mouseUp:theEvent];
	
	if ([theEvent type] == NSLeftMouseUp)
	{
		long nStart = 0, nEnd = 0;
		[self getSel:&nStart andEnd:&nEnd];
		
		if(nStart == nEnd)
		{
			[self movePosition:nStart toEnd:nEnd withString:nil];
		}
	}
}

- (void)onKeyDownDelete:(NSEvent*)theEvent
{
	long nStart, nEnd;
	int nLen;
	int nStep = 1;
	
	if(![self getValidSel:&nStart andEnd:&nEnd])
	{
		return;
	}
	
	if(nEnd > nStart)
	{
		NSString *str = [self getSelText];
		
		if([str length] == 0)
			return;
		
		[self movePosition:nStart toEnd:nEnd withString:nil];
		[self checkUnicode:str withLength:nLen];
		CancelCurrentPosition(&m_EmsData, nLen);
	}
	else if(nStart == nEnd)
	{
		nStep = [self getCancelStep];
		
		if(nStep > 0)
			CancelCurrentPosition(&m_EmsData, nStep);
	}
	
	[super keyDown:theEvent];
	
	[self resetSelection];
	
	[self CheckUnicodeAll:nil];
	
	[self updatePostion];
}

- (NSMenu*)menuForEvent:(NSEvent *)event
{
	return nil;
}

#pragma mark--
#pragma mark position move function
#pragma mark--

- (void)movePosition:(long)begin toEnd:(long)end withDirection:(MOVEDirection)direction andString:(NSString*)pStr
{
	int nLen;
	NSString *str;
	
	if(pStr != nil)
		str = pStr;
	else
	{
		[self setSelectedRange:NSMakeRange(begin, end - begin)];
		str = [self getSelText];
	}
	
	[self checkUnicode:str withLength:nLen];
	
	if(direction == BACKFORWARD)
		BackwardCurrentPosition(&m_EmsData, nLen);
	else if(direction == FORWARD)
		ForwardCurrentPosition(&m_EmsData, nLen);
}

- (void)movePosition:(long)begin toEnd:(long)end withString:(NSString *)str
{
	long offSet = end - m_nPos;
	
	//back up selection
	NSRange originSelRange = [self selectedRange];
	
	if(offSet < 0)
		[self movePosition:end toEnd:m_nPos withDirection:BACKFORWARD andString:str];
	else if(offSet > 0)
		[self movePosition:m_nPos toEnd:end withDirection:FORWARD andString:str];
	
	m_nPos = begin;
	
	[self setSelectedRange:originSelRange];
}

- (int)getCancelStep
{
	long nStart, nEnd;
	[self getSel:&nStart andEnd:&nEnd];
	
	// nStart =  [self getLayoutLineCount];
	
	//  nEnd = [self getParaCount];
	
	if(nEnd >= 1)
	{
		[self setSel:nEnd - 1 andEnd:nEnd];
		NSString * str = [self getSelText];   
		
		[self setSel:nStart andEnd:nEnd];
		
		if([str characterAtIndex:0] == 0x0D || [str characterAtIndex:0] == 0x0A)
			return 2;
	}
	
	return 1;
}

- (void)updatePostion
{
	NSRange range = [self selectedRange];
	
	if(range.length == 0)
		m_nPos = range.location;   // delete  + 1 as when debug delete '\n' in the line end
	
	m_nLen = [self getTextLength];
}

#pragma mark--
#pragma mark line count and index information
#pragma mark--

//counting hard line breaks

- (int)getParaCount
{
	NSString *string = [self string];
	unsigned int numberOfLines, index, stringLength = (int)[string length];
	
	for(index = 0, numberOfLines = 0; index < stringLength; numberOfLines++)
		index = (unsigned int)NSMaxRange([string lineRangeForRange:NSMakeRange(index, 0)]);
	
	return numberOfLines;
}

// counting lines of wrapper text

- (int)getLayoutLineCount
{
	NSLayoutManager *layout = [self layoutManager];
	unsigned numberOfLines = 0, index, numberOfGlyphs = (unsigned)[layout numberOfGlyphs];
	NSRange lineRange;
	
	for(numberOfLines = 0, index = 0; index < numberOfGlyphs; numberOfLines++)
	{
		(void)[layout lineFragmentRectForGlyphAtIndex:index effectiveRange:&lineRange];
		
		index = (unsigned)NSMaxRange(lineRange);
	}
	
	return numberOfLines;
}

@end
