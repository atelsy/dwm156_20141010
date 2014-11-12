//
//  NetworkSettingsController.mm
//  DCT
//
//  Created by MBJ on 11-12-23.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "NetworkSettingsController.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"

@implementation NetworkSettingsController

- (void)awakeFromNib
{
	NSString* str = LocalizeString(@"IDS_TABTITLE_INFORMATION");
	NSTabView* tbv = (NSTabView*)[self view];
	[[tbv tabViewItemAtIndex:0]setLabel:str];
	str = LocalizeString(@"IDS_LABEL_NETWORKTYPE");
	[networkTypeLabel setStringValue:str];
	str = LocalizeString(@"IDS_LABEL_CURROPER");
	[curOperLabel setStringValue:str];
	str = LocalizeString(@"IDS_LABEL_CELLID");
	[curCellIDLabel setStringValue:str];	
	
	str = LocalizeString(@"IDS_TABTITLE_REGISTRATION");
	tbv = (NSTabView*)[self view];
	[[tbv tabViewItemAtIndex:1]setLabel:str];
	str = LocalizeString(@"IDS_LABEL_AUTOSR");
	[[srchregMatrix cellWithTag:1]setTitle:str];
	str = LocalizeString(@"IDS_LABEL_MANUSR");
	[[srchregMatrix cellWithTag:2]setTitle:str];
	str = LocalizeString(@"IDS_BTNTITLE_REFRESH");
	[refreshBtn setTitle:str];
	str = LocalizeString(@"IDS_BTNTITLE_REGISTER");
	[registerBtn setTitle:str];
	
	refreshDlg = [[promptDialog alloc]initWithWindowNibName:@"promptDialog"];

	//init network type combo
    //[networkTypeBox addItemWithTitle:(@"IDS_NETWORK_2G")];
    //[networkTypeBox addItemWithTitle:(@"IDS_NETWORK_3G")];
    //[networkTypeBox addItemWithTitle:(@"IDS_NETWORK_AUTO_NOPREF")];
    //[networkTypeBox addItemWithTitle:(@"IDS_NETWORK_AUTO_PREF3G")];

	  plmnSource = [[NSMutableArray alloc]init];
      [networkTypeBox selectItemAtIndex:2];
     // [networkTypeBox setAutoenablesItems:YES];
     [self SetCtrlWithURC];
    
      m_nOldType = [networkTypeBox indexOfSelectedItem];
	  m_bChangeType = FALSE;
}

- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView
{
	return [plmnSource count];
}

- (id)tableView:(NSTableView *)tbview objectValueForTableColumn:(NSTableColumn *)tbcol row:(NSInteger)rowIndex
{
	NSMutableDictionary* dict = [plmnSource objectAtIndex:rowIndex];
	NSString *identifier = [tbcol identifier];
	id returnValue = [dict objectForKey:identifier];
	
	return returnValue;
}

- (void)tableView:(NSTableView*)tbview setObjectValue:(id)obj forTableColumn:(NSTableColumn*)tbcol row:(int)rowIndex
{
	NSMutableDictionary* dict = [plmnSource objectAtIndex:rowIndex];
	NSString *identifier = [tbcol identifier];
	[dict setValue:obj forKey:identifier];
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	long row = [plmnList selectedRow];
	if (row == -1)
		[registerBtn setEnabled:FALSE];
	else
		[registerBtn setEnabled:TRUE];
}

- (IBAction)OnNetworkTypeSelChange:(id)sender
{
	NSInteger cursel = [networkTypeBox indexOfSelectedItem];
    
	if (cursel < 0)
		return;

	if(cursel != m_nOldType)
	{
		DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
		[self EnableCtrl:FALSE];
		m_bChangeType = TRUE;
		[curOperator setStringValue:@""];
		[dele getComAgent]->SetNetworkType(self, WM_RSPCALLBACK, RESP_SET_NETWORKTP, (int)cursel);
	}
    
}

- (IBAction)OnRadioMatrixChange:(id)sender
{
	switch ([(NSButtonCell*)[sender selectedCell] tag]) 
	{
		case 1:		//	auto
		{
			DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
			if (CA_OK == [dele getComAgent]->SelectOperator(self, WM_RSPCALLBACK, RESP_SET_OPERAUTO, 0))
				[NSApp runModalForWindow:[refreshDlg window]];	
		}	break;
		case 2:		//	manual
		//	[plmnList setEnabled:TRUE];
            
/******************* 20131120 by Zhuwei ***************************************/
        {
            DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
            [dele settingsCtrl]->m_bNetworkAuto = FALSE;
        }
/******************* 20131118 by Zhuwei ***************************************/
            
			[refreshBtn setEnabled:TRUE];
			break;
		default:
			break;
	}
}

- (IBAction)OnBtnRefreshClick:(id)sender
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	if (CA_OK == [dele getComAgent]->GetOperList(self, WM_RSPCALLBACK, RESP_GET_OPLIST))
		[NSApp runModalForWindow:[refreshDlg window]];
}

- (IBAction)OnBtnRegisterClick:(id)sender
{
	long row = [plmnList selectedRow];
	if (row < 0)
		return;
	
	NSMutableDictionary* dict = [plmnSource objectAtIndex:row];
	
	NSString* nsstr = [dict objectForKey:@"operID"];
    CString OperID = [NSString getCStringFromNSString:nsstr];
	nsstr = [dict objectForKey:@"operName"];
    if ([nsstr caseInsensitiveCompare:LocalizeString(@"IDS_CC_UNKNOWN_OPERATOR")]== NSOrderedSame) {
        
        [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_CC_INVALID_REG_NETWORK")forType:MB_OK];
    }
    else
        {
            nsstr = [dict objectForKey:@"operACT"];
            DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
            if (CA_OK == [dele getComAgent]->SelectOperator(self, WM_RSPCALLBACK, RESP_SET_OPERMANUAL, 1, OperID, [nsstr intValue]))
                {
                [self EnableCtrl:FALSE];
                }
        
            
        }
}

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	if (Msg == WM_RSPCALLBACK)
	{
		WATCH_POINTER(lparam);
		
		switch(LOWORD(wparam))
		{	
			case RESP_SET_OPERAUTO:
				[NSApp abortModal];
				[refreshDlg close];
				if (HIWORD(wparam) == ER_OK)
				{
					DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
					[dele settingsCtrl]->m_bNetworkAuto = TRUE;
					[plmnSource removeAllObjects];
					[plmnList reloadData];
					[plmnList setEnabled:FALSE];
					[refreshBtn setEnabled:FALSE];
					[registerBtn setEnabled:FALSE];
				}
				break;
	
			case RESP_SET_OPERMANUAL:
				if (HIWORD(wparam) == ER_OK)
				{
					DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
					[dele settingsCtrl]->m_bNetworkAuto = FALSE;
					NSInteger nItem = [plmnList selectedRow];
					NSMutableDictionary* dict = [plmnSource objectAtIndex:nItem];
					NSString* str = [NSString  stringWithFormat:@"%@(%@)",
							[dict objectForKey:@"operName"],
							[dict objectForKey:@"operType"]];
					
					[curOperator setStringValue:str];
				}
				[self EnableCtrl:TRUE];
				break;
	
			case RESP_GET_OPLIST:
			{
				[NSApp abortModal];
				[refreshDlg close];
				[plmnSource removeAllObjects];
				[plmnList setEnabled:TRUE];

				if (HIWORD(wparam) == ER_OK)
				{
					NSString *str = nil;
					CRespOperList *p = (CRespOperList*)lparam;

					for (int i = 0; i < p->Value().size(); i ++)
					{
						const CString &operID = p->Value()[i].operId;
						int act = p->Value()[i].AcT;
				
						DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
						
                        NSDictionary *subdic=[dele findOperInfor:operID];
                    
                       str=[subdic objectForKey:@"servname"];
						if (str == nil || [str length] == 0)
							str = LocalizeString(@"IDS_CC_UNKNOWN_OPERATOR");

						NSMutableDictionary* dict = [[[NSMutableDictionary alloc]initWithObjectsAndKeys:
													 str, @"operName",
													 [self ParseAct:act], @"operType",
												//	 MAKELONG(wcstod((LPCTSTR)operID, &endptr), act), @"operData",
													  [NSString getStringFromWchar:(LPCTSTR)operID], @"operID",
													  [NSString stringWithFormat:@"%d", act], @"operACT",
													 nil]autorelease];
						[plmnSource addObject:dict];
					}
				}
				[plmnList reloadData];
			}
				break;
	
			case RESP_SET_NETWORKTP:
				[networkTypeBox setEnabled:TRUE];
				m_bChangeType = FALSE;
				if (HIWORD(wparam) != ER_OK)
				{
					if (m_nOldType >= 0)
						[networkTypeBox selectItemAtIndex: m_nOldType];
				}
				else
				{
					m_nOldType = [networkTypeBox indexOfSelectedItem];
                   
                
					DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
					switch (m_nOldType) {
						case 0:			//	2g
							[dele settingsCtrl]->m_nRatMode = 0;
                            							break;
						case 1:			//	3g
							[dele settingsCtrl]->m_nRatMode = 1;
                            							break;
						case 2:			//	auto - no prefer
							[dele settingsCtrl]->m_nRatMode = 2;
                                                       
							break;
						case 3:			//	prefer 3g
							[dele settingsCtrl]->m_nRatMode = 2;
							[dele settingsCtrl]->m_nPreferRat = 2;
							break;
						default:
							break;
					}
                    
				}
	
				[self EnableCtrl:TRUE];
                
				break;
			default:
				break;
		}
	}
}

- (void)EnableCtrl:(BOOL)bEnable
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;

	[networkTypeBox setEnabled:[dele settingsCtrl]->m_bQuerySimlock && bEnable];
	[srchregMatrix setEnabled:[dele settingsCtrl]->m_bQuerySimlock && bEnable];
	NSInteger n = [(NSButtonCell*)[srchregMatrix selectedCell] tag];
	[plmnList setEnabled: ([dele settingsCtrl]->m_bQuerySimlock && bEnable && (n == 2))];
	[refreshBtn setEnabled: ([dele settingsCtrl]->m_bQuerySimlock && bEnable && (n == 2))];
	n = [plmnList selectedRow];
	[registerBtn setEnabled:[dele settingsCtrl]->m_bQuerySimlock && [plmnList isEnabled]&&(n >= 0)];
}
/*************************Clear NSTableView Data ******/
/*-(void)clearData
{

 [plmnSource removeAllObjects];
 [plmnList reloadData];
 //[plmnList release];
}*/
- (void)SetCtrlWithURC
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	
	if ([dele settingsCtrl]->m_bNetworkAuto)
	{
		//[plmnSource removeAllObjects];
        [srchregMatrix selectCellWithTag:1];	//	auto
	}
	else
       
		[srchregMatrix selectCellWithTag:2];	//	manual
	
	NSString* str = [self ParseAct:[dele settingsCtrl]->m_nAct];
	
	NSString *opername = [dele settingsCtrl]->m_strOperName;
	
	NSString* oper;
	
	if ([opername length] > 0)
	{	
		if ([str length])
			oper = [NSString stringWithFormat:@"%@ (%@)", opername, str];
		else
			oper = opername;
	}
	else
	{
		oper = @"";
	}
	
//	if (![dele settingsCtrl]->m_strOperCode.IsEmpty())
		[curOperator setStringValue:oper];
	
	oper = [NSString getStringFromWchar:(LPCTSTR)[dele settingsCtrl]->m_strCellID];
	
	[curCellID setStringValue:oper];
	
	if (m_bChangeType == FALSE)
	{
		if ([dele settingsCtrl]->m_nRatMode == 0 &&
           [dele settingsCtrl]->m_nPreferRat == 0 )		//	2G
		{
			[networkTypeBox selectItemAtIndex:0];
			m_nOldType = 0;
		}
		else if([dele settingsCtrl]->m_nRatMode == 1&&
                [dele settingsCtrl]->m_nPreferRat == 0)	//	3G
		{
			[networkTypeBox selectItemAtIndex:1];
			m_nOldType = 1;
		}
	/*else if([dele settingsCtrl]->m_nRatMode == 2 &&	//	prefer 2G
            	[dele settingsCtrl]->m_nPreferRat == 1)
    {
	        [networkTypeBox selectItemAtIndex:3];
    }*/
		else if([dele settingsCtrl]->m_nRatMode == 2&&	//	prefer 3G
			[dele settingsCtrl]->m_nPreferRat == 2)
		{
			[networkTypeBox selectItemAtIndex:3];
			m_nOldType = 3;
		} //zhang shao hua
		else if([dele settingsCtrl]->m_nRatMode == 2 )//	auto - no prefer
    {
			[networkTypeBox selectItemAtIndex:2];
			m_nOldType = 2;
		}
		
		[self EnableCtrl:TRUE];
	}
}

- (NSString*) ParseAct:(long) act
{
	NSString* sAct;
	switch (act)
	{
		case 0:
			//sAct = @"2G";
            sAct=@"GPRS/EDGE";
			break;
		case 1:
			sAct = @"GPRS/EDGE(Compact)";
			break;
		case 2:
			//sAct = @"3G";
            sAct=@"UMTS/HSPA";
			break;
		case 3:
			sAct = @"GPRS/EDG(EGPRS)";
			break;
		case 4:
			sAct = @"UMTS/HSPA(HSDPA)";
			break;
		case 5:
			sAct = @"UMTS/HSPA(HSUPA)";
			break;
		case 6:
			sAct = @"UMTS/HSPA(HSDPA&HSUPA)";
			break;
		case 7:
			sAct = @"E-UTRAN";
			break;
		default:
			sAct = @"";
			break;
	}
	return sAct;
}

/************ 20131120 by Zhuwei **********************************************/

- (void)clearSource {
    [plmnSource removeAllObjects];
    [plmnList reloadData];
}

/******************************************************************************/

@end
