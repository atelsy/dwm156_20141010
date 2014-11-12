//
//  DialupSettingsController.mm
//  DCT
//
//  Created by MBJ on 11-12-23.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "DialupSettingsController.h"
#import "DCTAppDelegate.h"

@implementation DialupSettingsController

- (void)awakeFromNib
{
	NSString* str = LocalizeString(@"IDS_TABTITLE_DIALUP");
	NSTabView* tbv = (NSTabView*)[self view];
	[[tbv tabViewItemAtIndex:0]setLabel:str];
	str = LocalizeString(@"IDS_LABEL_SERVICE");
	[serviceLabel setStringValue:str];
	str = LocalizeString(@"IDS_LABEL_SERVICENAME");
	[serviceNameLabel setStringValue:str];
	str = LocalizeString(@"IDS_LABEL_TELEPHONENUM");
	[telNumberLabel setStringValue:str];
	str = LocalizeString(@"IDS_LABEL_ACCOUNTNAME");
	[accountNameLabel setStringValue:str];
	str = LocalizeString(@"IDS_LABEL_PASSWORD");
	[passWordLabel setStringValue:str];
	str = LocalizeString(@"IDS_BTNTITLE_NEW");
	[btnNew setTitle:str];
	str = LocalizeString(@"IDS_BTNTITLE_EDIT");
	[btnEdit setTitle:str];
	str = LocalizeString(@"IDS_BTNTITLE_DELETE");
	[btnDelete setTitle:str];
	str = LocalizeString(@"IDS_BTNTITLE_SAVE");
	[btnSave setTitle:str];
	str = LocalizeString(@"IDS_BTNTITLE_CANCEL");
	[btnCancel setTitle:str];
	
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	m_EntryInfoList = dele.dialupMgr->m_EntryInfoList;
	
	//	init combo box
	if ([m_EntryInfoList count] > 0)
	{
		for (ENTRYINFO* info in m_EntryInfoList)
		{	
			//	debug
		//	NSLog(@"entry list info name: %@\n", info->servname);
			[servicesBox addItemWithObjectValue:info->servname];
		}
		[servicesBox selectItemAtIndex:0];
	}

	[self OnSelServChange:self];
}

- (IBAction)OnBtnNewClick:(id)sender
{
	m_iLastSel = [servicesBox indexOfSelectedItem];
	[self FillCtrls:nil];
	m_iStat = STAT_NEW;
	[self SetCtrlStat];
	[self SetBtnStat];
}

- (IBAction)OnBtnEditClick:(id)sender
{
	m_iStat = STAT_EDIT;
	[self SetCtrlStat];
	[self SetBtnStat];
}

- (IBAction)OnBtnDeleteClick:(id)sender
{
	if([utility showMessageBox: LocalizeString(@"DIALUPSETTING_MSGBOX_CAPTION") 
					   withMsg:LocalizeString(@"IDS_DIALUP_DELETE_CONFIRM") 
					   forType:MB_YESNO] == NSAlertDefaultReturn)
	{
		NSInteger i = [servicesBox indexOfSelectedItem];
		
		DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
		if ([dele.dialupMgr isWorking:i])
		{
			[utility showMessageBox: LocalizeString(@"DIALUPSETTING_MSGBOX_CAPTION") 
							withMsg:LocalizeString(@"IDS_DIALUP_SERVICE_UNCHANGABLE") 
							forType:MB_OK];
			return;
		}
		[servicesBox removeItemAtIndex: i];
		[m_EntryInfoList removeObjectAtIndex: i];
		if ([servicesBox numberOfItems] > 0)
			[servicesBox selectItemAtIndex: 0];
		else
		{
		//	[servicesBox selectItemAtIndex:-1];
			[servicesBox setStringValue:@""];
		}

			[self OnSelServChange:self];
		}
}

- (IBAction)OnBtnSaveClick:(id)sender
{	
	NSLog(@"[ppp profile] current index = %d\n", [servicesBox indexOfSelectedItem]);
	NSString* str = [serviceName stringValue];
	if ([str length] == 0)
	{
		[utility showMessageBox: LocalizeString(@"DIALUPSETTING_MSGBOX_CAPTION") 
						withMsg:LocalizeString(@"IDS_DIALUP_SERVNAME_EMPTY") 
						forType:MB_OK];
		return;
	}
	if (m_iStat == STAT_NEW) 
	{
		for (ENTRYINFO* info in m_EntryInfoList)
		{
			if ([str isEqualToString: info->servname])
			{
				[utility showMessageBox: LocalizeString(@"DIALUPSETTING_MSGBOX_CAPTION") 
								withMsg:LocalizeString(@"IDS_DIALUP_SERVNAME_EXISTED") 
								forType:MB_OK];
				return;
			}
		}
		
		ENTRYINFO *newinf = [[ENTRYINFO alloc]init];
		[self FillEntryParam:newinf];
		[m_EntryInfoList addObject:newinf];
		[newinf release];
		[servicesBox addItemWithObjectValue:newinf->servname];
		[servicesBox selectItemWithObjectValue:newinf->servname];
	}
	else if (m_iStat == STAT_EDIT)
	{
		ENTRYINFO* info = nil;
//		NSInteger cursel = [servicesBox indexOfSelectedItem];
		NSInteger cursel = m_iLastSel;
		NSLog(@"[ppp profile] edit index = %d\n", m_iLastSel);
		for (NSInteger i = 0; i < [m_EntryInfoList count]; i ++)
		{
			info = [m_EntryInfoList objectAtIndex:i];
			if ([str isEqualToString: info->servname] && i != cursel)
			{
				[utility showMessageBox: LocalizeString(@"DIALUPSETTING_MSGBOX_CAPTION") 
								withMsg:LocalizeString(@"IDS_DIALUP_SERVNAME_EXISTED") 
								forType:MB_OK];
				return;
			}
		}
		DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
		if ([dele.dialupMgr isWorking:cursel])
		{
			[utility showMessageBox: LocalizeString(@"DIALUPSETTING_MSGBOX_CAPTION") 
							withMsg:LocalizeString(@"IDS_DIALUP_SERVICE_UNCHANGABLE") 
							forType:MB_OK];
			return;
		}
		[self FillEntryParam:[m_EntryInfoList objectAtIndex:cursel]];
		[servicesBox removeItemAtIndex:cursel];
		[servicesBox insertItemWithObjectValue:str atIndex:cursel];
		[servicesBox selectItemAtIndex:cursel];
	}
	m_iStat = STAT_SELECTED;
	[self SetCtrlStat];
	[self SetBtnStat];
}

- (IBAction)OnBtnCancelClick:(id)sender
{
	if (m_iStat == STAT_EDIT) {
		m_iStat = STAT_SELECTED;
	}
	else if (m_iStat == STAT_NEW)
	{
		if (m_iLastSel == -1)
			m_iStat = STAT_NOITEM;
		else
		{
			m_iStat = STAT_SELECTED;
			[servicesBox selectItemAtIndex:m_iLastSel];
		}
	}
	[self OnSelServChange:self];
}

- (IBAction)OnSelServChange:(id)sender
{
	NSLog(@"[ppp profile] select sel = %d\n", [servicesBox indexOfSelectedItem]);
	NSInteger cursel = [servicesBox indexOfSelectedItem];
	if (cursel >= 0)
	{
		m_iStat = STAT_SELECTED;
		[self FillCtrls:[m_EntryInfoList objectAtIndex:cursel]];
	}
	else
	{
		m_iStat = STAT_NOITEM;
		[self FillCtrls:nil];
	}
	[self SetCtrlStat];
	[self SetBtnStat];
	
	m_iLastSel = cursel;
}

- (void)SetCtrlStat
{
	[servicesBox setEnabled:(m_iStat == STAT_NOITEM || m_iStat == STAT_SELECTED)];
	[serviceName setEnabled:(m_iStat == STAT_NEW || m_iStat == STAT_EDIT)];
	[telNumber setEnabled:(m_iStat == STAT_NEW || m_iStat == STAT_EDIT)];
	[accountName setEnabled:(m_iStat == STAT_NEW || m_iStat == STAT_EDIT)];
	[passWord setEnabled:(m_iStat == STAT_NEW || m_iStat == STAT_EDIT)];
	[APN setEnabled:(m_iStat == STAT_NEW || m_iStat == STAT_EDIT)];
}

- (void)SetBtnStat
{
	[btnNew setEnabled:(m_iStat == STAT_NOITEM || m_iStat == STAT_SELECTED)];
	[btnEdit setEnabled:(m_iStat == STAT_SELECTED)];
	[btnDelete setEnabled:(m_iStat == STAT_SELECTED)];
	[btnSave setEnabled:(m_iStat == STAT_NEW || m_iStat == STAT_EDIT)];
	[btnCancel setEnabled:(m_iStat == STAT_NEW || m_iStat == STAT_EDIT)];
}

- (void)FillCtrls:(ENTRYINFO*)info
{
	[serviceName setStringValue:info?info->servname:@""];
	[telNumber setStringValue:info?info->number:@""];
    [accountName setStringValue:info?info->username:@""];
	[passWord setStringValue:info?info->passwd:@""];
	[APN setStringValue:info?info->apn:@""];
    

	//	if noitem, clear combo content
	if (m_iStat == STAT_NOITEM)
		[servicesBox setStringValue:@""];
}

- (void)FillEntryParam:(ENTRYINFO *)info
{
	if (info->servname)
		[info->servname release];
	info->servname = [[NSString alloc]initWithString:[serviceName stringValue]];
	if (info->number)
		[info->number release];
	info->number = [[NSString alloc]initWithString:[telNumber stringValue]];
	if (info->username)
		[info->username release];
	info->username = [[NSString alloc]initWithString:[accountName stringValue]];
	if (info->passwd)
		[info->passwd release];
	info->passwd = [[NSString alloc]initWithString:[passWord stringValue]];
	if (info->apn)
		[info->apn release];
	info->apn = [[NSString alloc]initWithString:[APN stringValue]];
}

- (void)SaveChanges
{
	if (m_iStat == STAT_NEW || m_iStat == STAT_EDIT)
	{
		if([utility showMessageBox: LocalizeString(@"DIALUPSETTING_MSGBOX_CAPTION") 
						withMsg:LocalizeString(@"IDS_DIALUP_SAVE_CONFIRM") 
						forType:MB_YESNO] == NSAlertDefaultReturn)
			[self OnBtnSaveClick:self];
		else
			[self OnBtnCancelClick:self];
		}
	
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	[dele.dialupMgr SetDefaultConn:[servicesBox indexOfSelectedItem]];
}

- (NSInteger)UpdateServList:(NSString*)selname
{
	[servicesBox removeAllItems];
	NSInteger sel = -1;

	for (NSInteger i = 0; i < [m_EntryInfoList count]; i ++)
	{
		ENTRYINFO* info = [m_EntryInfoList objectAtIndex:i];
		[servicesBox addItemWithObjectValue:info->servname];
		//	1 selname must not be nil
		//	2 choose the first equal one.
		if ((sel == -1) && selname && [info->servname isEqualToString:selname])
			sel = i;
	}
	
	//	dont select now, for user may editing or creating.
    // [servicesBox selectItemAtIndex:sel];
	// m_iStat = STAT_SELECTED;
	// [self OnSelServChange:self];
	
	return sel;
}

@end
