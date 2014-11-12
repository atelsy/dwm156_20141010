//
//  emsEdit.h
//  DCT
//
//  Created by Fengping Yu on 12/1/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "utility.h"
#include "ems.h"
#import "smsTextView.h"

enum MOVEDirection 
{
	BACKFORWARD = 0,
	FORWARD = 1
};

enum ChangeType
{
	CHANGETYPE_UNKNOWN  = 0,
	CHANGETYPE_REPLACESEL,
	CHANGETYPE_DELETE,
	CHANGETYPE_BACK,
	CHANGETYPE_MOVE,
	CHANGETYPE_CUT,
	CHANGETYPE_COPY,
	CHANGETYPE_PASTE,
	CHANGETYPE_UNDO,
	CHANGETYPE_SETCHARFORMAT,
	CHANGETYPE_CHINESE,
};

@interface emsEdit : smsTextView 
{
@private
	EMSData m_EmsData;
	long m_nLen;
	long m_nPos;
	long m_nStart;
	long m_nEnd;
	long m_nOriLen;
	BOOL m_bUnicode;
	BOOL m_bSelFolder;
	
	ChangeType m_changeType;
	
	NSString *m_strSel;
	
	id m_parent;
	
	BOOL m_bFirst;
}

- (void)initDefaultParameters;

- (void)getInfo:(int&)nNum unit:(int&)nUit message:(int&)nMessage isEms:(BOOL&)bEms ischeck:(BOOL)bCheck;
- (void)setEmsData:(EMSData*)pEmsData withSelFolder:(BOOL)bSelFolder;
- (EMSData*)getEmsData;
- (long)getTextLength;

- (EMSTATUS)CheckUnicodeAll:(NSString*)str;
- (BOOL)isUnicode;
- (BOOL)checkUnicode:(NSString*)str withLength:(int&)nLen2;
- (BOOL)checkUnicode: (int)nLen withWLength:(int)nWLen;

- (void)hideSelection;
- (void)showSelection;
- (long)checkSelection;
- (BOOL)checkValidSel:(long*)nStart andEnd:(long*)nEnd;
- (NSString*)getSelText;
- (void)getSel:(long*)nStart andEnd:(long*)nEnd;
- (BOOL)getValidSel:(long*)nStart andEnd:(long*)nEnd;
- (void)setSel:(long)nStart andEnd:(long)nEnd;
- (int)getSelectionLen:(int)nStart to:(int)nEnd withString:(CString&)str;
- (void)clear;
- (void)resetSelection;
- (void)getSelection;

- (void)onKeyDownDelete:(NSEvent*)theEvent;
- (int)getCancelStep;
- (void)movePosition:(long)begin toEnd:(long)end withString:(NSString*)str;
- (void)movePosition:(long)begin toEnd:(long)end withDirection:(MOVEDirection)direction andString:(NSString*)pStr;
- (void)updatePostion;

- (void)onChange;
- (BOOL)addTextFrom:(long)nStart to:(long)nEnd;
- (EMSTATUS)addText:(CString)str;

- (int)getParaCount;
- (int)getLayoutLineCount;

// helper method
- (void)setChangeType:(ChangeType) type;
- (void)setParent:(id)_parent;

- (void)commonKeyDown:(NSEvent*)theEvent;

// for debug
- (void)debugedit;

@end
