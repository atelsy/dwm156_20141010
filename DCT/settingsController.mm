//
//  settingsWindow.mm
//  DCT
//
//  Created by MBJ on 11-12-21.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "settingsController.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"
	
@implementation settingsController

@synthesize secDialog, dialupSettingsController;

- (id)initWithWindowNibName:(NSString *)nibNameOrNil
{
	self = [super initWithWindowNibName:nibNameOrNil];
	if (self)
	{
		m_bQueryNet = m_bQuerySimlock = FALSE;
		secDialog = nil;
		m_nAct = -1;
		m_bNetworkAuto = TRUE;
		m_nRatMode = -1;
		m_nPreferRat = -1;
		
		m_bSimLock = FALSE;
		
		m_strOperName = nil;
		
		itemSource = [[NSMutableArray alloc] init];
		NSMutableDictionary *dict = nil;
		
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 @"General", @"settingClasses",
				 nil]autorelease];
		[itemSource addObject:dict];
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 @"Dialup", @"settingClasses",
				 nil]autorelease];
		[itemSource addObject:dict];
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 @"Network", @"settingClasses",
				 nil]autorelease];
		[itemSource addObject:dict];
				dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:@"SMS", @"settingClasses", nil] autorelease];
				[itemSource addObject:dict];
		
		DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
		[dele getComAgent]->RegisterURC(URC_SYS, self, WM_URCCALLBACK);
		[dele getComAgent]->RegisterURC(URC_CGREG, self, WM_URCCALLBACK);
	//	[dele getComAgent]->RegisterURC(URC_PACSP, self, WM_URCCALLBACK);
	}
	return self;
}

- (void)awakeFromNib
{
	NSString* str = LocalizeString(@"IDS_SETTING_WND_CAPTION");
	[[self window]setTitle:str];
	str = LocalizeString(@"CANCEL");
	[btnCancel setTitle:str];
	str = LocalizeString(@"OK");
	[btnOK setTitle:str];
	
	curController = generalSettingsController;
	[viewBox setContentView:[curController view]];
}

- (void)dealloc
{	
	[customFieldEditor release];
	[m_strOperName release];
	[super dealloc];
}

- (void) OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	if (Msg == WM_URCCALLBACK)
	{
		[self onUrcCallback:wparam lParam:lparam];
	}
	else if (Msg == WM_RSPCALLBACK)
	{
		[self onRespCallback:wparam lParam:lparam];
	}
}

- (void)onUrcCallback:(long)wparam lParam:(long)lparam
{
	WATCH_POINTER(lparam);
	
	CUrcSys *pUrc = (CUrcSys*)lparam;
	
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	
	switch (wparam)
	{
		case URC_SYS:
			[dele setStatusInd:pUrc->Value().alpha];
			if (pUrc->Value().alpha >= SYS_NETREADY)
			{
				[dele getComAgent]->QueryOperator(self, WM_RSPCALLBACK, RESP_GET_CUROP, 0);
				[dele getComAgent]->QueryPSStatus(self, WM_RSPCALLBACK, RESP_GET_CURNETTP);
					
				if (networkSettingsController)
					[networkSettingsController EnableCtrl:FALSE];
			}
			else if (pUrc->Value().alpha >= SYS_PINREADY)
			{
				if (!m_bQuerySimlock)
				{
					[dele getComAgent]->GetSIMLock(self, WM_RSPCALLBACK, RESP_GET_CLCK);
					m_bQuerySimlock = TRUE;
				}
				if (networkSettingsController)
					[networkSettingsController EnableCtrl:TRUE];
			}
			else if (pUrc->Value().alpha == SYS_PINREQ)
			{
				[self showPwdDialog:SEC_VERIFYPIN];
				
				if (generalSettingsController)
					[generalSettingsController SetCtrlWithURC];
			}
			else if (pUrc->Value().alpha == SYS_PUKREQ)
			{
				[self showPwdDialog:SEC_VERIFYPUK];
				
				if (generalSettingsController)
					[generalSettingsController SetCtrlWithURC];
			}
			else if (pUrc->Value().alpha == SYS_PIN2REQ)
			{
				[self showPwdDialog:SEC_VERIFYPIN2];

				if (generalSettingsController)
					[generalSettingsController SetCtrlWithURC];
			}
			else if (pUrc->Value().alpha == SYS_PUK2REQ)
			{
				[self showPwdDialog:SEC_VERIFYPUK2];

				if (generalSettingsController)
					[generalSettingsController SetCtrlWithURC];
			}
			else
			{
				m_bQueryNet = m_bQuerySimlock = FALSE;
				m_nAct = -1;
				m_nRatMode = -1;
				m_nPreferRat = -1;
				m_strCellID = _T("");
				
				[m_strOperName release];
				m_strOperName = nil;
				
				if (networkSettingsController)
					[networkSettingsController SetCtrlWithURC];
				if (generalSettingsController)
					[generalSettingsController EnableCtrl:FALSE];
				
				//	if has popup dialog close it
				//	seems not needed
			}
			break;
		case URC_CGREG:
		{	
		//	20110817 by foil - use cussdcode class.. embarrassed.
		//	m_nAct	= HIWORD(pUrc->GetValue());	
			CUrcUssd* pUrc = (CUrcUssd*)lparam;
			m_nAct = pUrc->Value().dcs;
			m_strCellID = pUrc->Value().str_urc;
			if (networkSettingsController)
				[networkSettingsController SetCtrlWithURC];
		}
			break;
/*		case URC_PACSP:
		{
			int val = ((CUrcInt*)lparam)->Value();
			if (val == 0)
			{
			//	::SendMessage(m_RefreshOperDlg, WM_CLOSE, 0, 0);
			//	EnableCtrl(FALSE);
			}
			//	here should test.
//			else
//				EnableCtrl(TRUE);
		}
			break;*/
		default:
			break;
	}	
}

- (void)onRespCallback:(long)wparam lParam:(long)lparam
{
    DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	DialupMgr* mgr = dele.dialupMgr;
	WATCH_POINTER(lparam);	
	switch(LOWORD(wparam))
	{
		case RESP_GET_CUROP:
			if (HIWORD(wparam) == ER_OK)
			{
				CRespOperInfo *p = (CRespOperInfo*)lparam;
            [m_strOperName release];
            NSDictionary *subdic=[[TheDelegate findOperInfor:p->Value().operId] retain];
            ENTRYINFO* pInfo = [[ENTRYINFO alloc]init];
            [dialupSettingsController FillEntryParam:pInfo];
            if (subdic == nil)
                break;
            
            pInfo->servname = [[NSString alloc]initWithString:[subdic objectForKey:@"servname"]];
            NSLog(@"%@",pInfo->servname);
            pInfo->number = [[NSString alloc]initWithString:[subdic objectForKey:@"number"]];
            NSLog(@"%@",pInfo->number);
            pInfo->username = [[NSString alloc]initWithString:[subdic objectForKey:@"username"]];
            pInfo->passwd = [[NSString alloc]initWithString:[subdic objectForKey:@"password"]];
            pInfo->apn =  [[NSString alloc]initWithString:[subdic objectForKey:@"apn"]];
            NSLog(@"%@",pInfo->apn );
             [dialupSettingsController FillCtrls:pInfo];
             m_strOperName = pInfo->servname;
            if(m_strOperName==nil) {
                
               m_strOperName=[NSString CStringToNSString:p->Value().operId];
            }
            [TheDelegate setStatusText:m_strOperName];
            [dialupSettingsController UpdateServList:pInfo->servname];
            [ pInfo release];
        
        
				if (networkSettingsController)
					[networkSettingsController SetCtrlWithURC];
			}
			break;
		case RESP_GET_CURNETTP:
		{
			CRespArray *p = (CRespArray*)lparam;
			if (HIWORD(wparam) == ER_OK && p->Value().size() >= 3)
			{
//				int CurRat = p->Value()[0];
//				int GprsMode = p->Value()[1];
				m_nRatMode = p->Value()[2];
				m_nPreferRat = p->Value()[3];
				//	for test
				NSLog(@"ratmode:%d, prefer:%d\n", m_nRatMode, m_nPreferRat);
			}
		}
			break;
		case RESP_GET_CLCK:
		{
			CRespFlag *p = (CRespFlag*)lparam;
			m_bSimLock = p->Value();
			if (generalSettingsController)
				[generalSettingsController SetCtrlWithURC];
		}
			break;
		default:
			break;
	}
	if (networkSettingsController)
		[networkSettingsController SetCtrlWithURC];	
}

- (BOOL)showPwdDialog:(int)pinType
{
	if (secDialog)
		return NO;
	
	secDialog = [[verifyPwdDialog alloc]initWithWindowNibName:@"verifyPwdDialog"];
	
	[secDialog setJob:pinType];
	
	[NSApp runModalForWindow:[secDialog window]];
	
	[secDialog close];
	[secDialog release];
	
	secDialog = nil;
	
	return YES;
}

- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView
{
	return [itemSource count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	NSMutableDictionary* dict = [itemSource objectAtIndex:row];
	NSString *identifier = [tableColumn identifier];
	id returnValue = [dict objectForKey:identifier];
	
	return returnValue;
}

- (void)tableView:(NSTableView*)tbview setObjectValue:(id)obj forTableColumn:(NSTableColumn*)tbcol row:(int)rowIndex
{
	NSMutableDictionary* dict = [itemSource objectAtIndex:rowIndex];
	NSString *identifier = [tbcol identifier];
	[dict setValue:obj forKey:identifier];
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	const
	long row = [itemTable selectedRow];
	
	if (row != 1 && curController == dialupSettingsController)
		[dialupSettingsController SaveChanges];
	
	if (row == 0 && curController != generalSettingsController)
	{
		curController = generalSettingsController;
	}
	else if (row == 1 && curController != dialupSettingsController)
	{
		curController = dialupSettingsController;
	}
	else if (row == 2 && curController != networkSettingsController)
	{
		curController = networkSettingsController;
	}
		else if(row == 3 && curController != smsSettingController)
		{
			  curController = smsSettingController;
		}

	[viewBox setContentView:[curController view]];
}

- (IBAction)OnBtnOKClick:(id)sender
{
    	if (curController == dialupSettingsController)
		[dialupSettingsController SaveChanges];
	    [smsSettingController saveState];
       // [networkSettingsController clearSource];
	[NSApp stopModal];
}

- (void)windowWillClose:(NSNotification *)notification
{
	[self OnBtnOKClick:nil];
}

- (IBAction)OnBtnCancelClick:(id)sender
{
	[NSApp stopModal];
}

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
/*
- (void)controlTextDidChange:(NSNotification *)obj
{
	NSDictionary *dict = [obj userInfo];
	
	id object = [dict objectForKey:@"NSFieldEditor"];
	
	NSTextView *view = (NSTextView*)[[self window] fieldEditor:YES forObject:smsSettingController.m_SoundPath];
	
	if(object == view)  //it is sms setting sound path editor
	{
		[smsSettingController enablePlayBtn];
	}
}*/

@end
