//
//  DialupViewCtrl.m
//  DCT
//
//  Created by MBJ on 11-11-29.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "DCTAppDelegate.h"
#import "DialupViewCtrl.h"
#import "nsstrex.h"

#include "ComAgent.h"
#include "utility.h"

@implementation DialupViewCtrl

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
	if (self)
	{
		m_nAct = -1;
		m_bRedial = FALSE;
		curOperName = [[NSString alloc]initWithString:@""];
		isDialupAvailable = FALSE;
		m_dialupBtnStat = kSCNetworkConnectionDisconnected;

		DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
		dele.dialupMgr->dialupReceiver = self;
		CComAgent* pComAgent = [dele getComAgent];
		pComAgent->RegisterURC(URC_SYS, self, WM_URCCALLBACK);
		pComAgent->RegisterURC(URC_CGREG, self, WM_URCCALLBACK);
		pComAgent->RegisterURC(URC_PSBEARER, self, WM_URCCALLBACK);
/************ 20131111 by Zhuwei **********************************************/
        _isGetSoftwareVersion = NO;
/******************************************************************************/
        
	}
	return self;
}

- (void)awakeFromNib
{			
	//	disable combo & btn
	[curConnText setEnabled:isDialupAvailable];
	[dialupBtn setEnabled:isDialupAvailable];
	
	[curConnText setStringValue:curOperName];
    [self setDialupStat];
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	[dele OnDialStatChange];
}

- (void)dealloc
{
	[super dealloc];
	if (curOperName)
		[curOperName release];
}

- (IBAction)OnDialupBtnClick:(id)sender
{	
	NSLog(@"[PPP Dialup]: Click connect button\n");
	DCTAppDelegate* dele = (DCTAppDelegate*)[NSApp delegate];
	DialupMgr* mgr = dele.dialupMgr;
	NSLog(@"[PPP Dialup]: mgr = 0x%08x\n", mgr);
	[mgr DoJob];
}

- (void) OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	DialupMgr* mgr = dele.dialupMgr;
	
    
    
	if (Msg == WM_DIALUP_STATUS)
	{
		NSString* opname = nil;
		m_dialupBtnStat = wparam;

		if ([mgr->m_EntryInfoList count] > 0 && lparam >= 0)
		{
			ENTRYINFO* info = (ENTRYINFO*)[mgr->m_EntryInfoList objectAtIndex:lparam];
			if (info)
				opname = info->servname;
		}

		[self UpdateCurConnText:opname];
		[self setDialupStat];
		[dele OnDialStatChange];
	}
	else if (Msg == WM_URCCALLBACK)
	{
		WATCH_POINTER(lparam);
        switch (wparam) {
			case URC_SYS:
				if (((CUrcSys*)lparam)->Value().alpha >= SYS_NETREADY) {
                    
				//	if (mgr->curConnStatus == kSCNetworkConnectionInvalid) {
						//	driver should be already. so autocreateconnection directly.
						[dele getComAgent]->QuerySimOwner(self, WM_RSPCALLBACK, RESP_GET_OPERATOR);
				//	}
				}
				else {
                    
/************ 20131111 by Zhuwei **********************************************/
                    if(!_isGetSoftwareVersion) {
                        DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
                        CComAgent* pComAgent = [dele getComAgent];
                        pComAgent->GetVERNO(self, WM_GETVERNO);
                    }
/******************************************************************************/
                    
					[dele OnDialStatChange];
					//	disable combo & btn
					isDialupAvailable = FALSE;
					[curConnText setEnabled:isDialupAvailable];
					[dialupBtn setEnabled:isDialupAvailable];
					 m_dialupBtnStat = kSCNetworkConnectionDisconnected;
					[self setDialupStat];
					//	debug
					NSLog(@"network not ready..\n");
				}
                
				break;
			case URC_CGREG:
			{
				CUrcUssd *pUrc = (CUrcUssd*)lparam;
				int stat = pUrc->Value().n;
//                m_nAct = pUrc->Value().dcs;
//               [dele OnActChange:m_nAct];
//				
				if (stat !=1 && stat != 5) {
					//	suppose that the connection status should change automatically
					;
				}
				else {
					m_bRedial = TRUE;
				}
			}
			break;
			case URC_PSBEARER:
			{
//                CUrcBearer *pUrc = (CUrcBearer*)lparam;
//				m_nAct = pUrc->Value().beta;
//				[dele OnActChange:m_nAct];
//				NSLog(@"[Dialup type] id = %d\n", m_nAct);
			}
				break;
			default:
				break;
		}
	}
	else if (Msg == WM_RSPCALLBACK)
	{
		WATCH_POINTER(lparam);
		
		switch (wparam) {
            case RESP_SET_NETWORKTP:
//                m_nAct = ((CUrcInt*)lparam)->Value();
//                [dele OnActChange:m_nAct];
                break;
			case RESP_GET_OPERATOR:
				if (HIWORD(wparam) != ER_OK) {
					//	get cimi fail..disable combo & btn
					isDialupAvailable = FALSE;
					[curConnText setEnabled:isDialupAvailable];
					[dialupBtn setEnabled:isDialupAvailable];
					//	debug
					NSLog(@"get cimi fail\n");
				}
				else {
                    // NSString *plmn = [dele findOperName:(((CRespText*)lparam)->Value())];
                  NSDictionary  *subdic=[dele findOperInfor:(((CRespText*)lparam)->Value())];
                    ENTRYINFO* pInfo = [[ENTRYINFO alloc]init];
                   
                    if (subdic == nil)
                        break;
                  pInfo->servname = [[NSString alloc]initWithString:[subdic objectForKey:@"servname"]];
                   NSLog(@"DialupViewCtrl %@",pInfo->servname);
                    pInfo->number = [[NSString alloc]initWithString:[subdic objectForKey:@"number"]];
                    NSLog(@"DialupViewCtrl %@",pInfo->number);

                    pInfo->username = [[NSString alloc]initWithString:[subdic objectForKey:@"username"]];
                    pInfo->passwd = [[NSString alloc]initWithString:[subdic objectForKey:@"password"]];
                pInfo->apn = [[NSString alloc]initWithString:[subdic objectForKey:@"apn"]];
                    
                 NSLog(@"DialupViewCtrl %@",pInfo->apn);
									
                   // [mgr AddEntryWithName:pInfo->servname];
                     [mgr AddEntryWithNameList:pInfo];
					//	update service combo in setting view
					if (dele.settingsCtrl && dele.settingsCtrl.dialupSettingsController) {
                      [dele.settingsCtrl.dialupSettingsController UpdateServList:pInfo->servname];
                    }  
					
           
                    
					//	try auto-dialup
					NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
					NSNumber *number = (NSNumber*)[defaults valueForKey:AUTODIAL];
					if([number intValue] == NSOnState)
						[mgr DoJob];
					
					//	enable combo & btn
					isDialupAvailable = TRUE;
					[curConnText setEnabled:isDialupAvailable];
					[dialupBtn setEnabled:isDialupAvailable];
				}
				break;
			case RESP_SET_MODEMAPN:
				break;
			default:
				break;
		}
	} else if(Msg == WM_GETVERNO) {
        WATCH_POINTER(lparam);
        CRespText *pText = (CRespText*)lparam;
        DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
        NSString *version =  [NSString CStringToNSString:pText->Value()];
        dele.softwareVersion = version;
        _isGetSoftwareVersion = YES;
    }
}

- (void)setDialupStat
{		
        //DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
   // NSButtonCell*  connCell;
	switch (m_dialupBtnStat) {
	case kSCNetworkConnectionConnected:
        // [dialupBtn setEnabled:YES ];
		[dialupBtn setTitle:LocalizeString(@"IDS_BTNTITLE_DISCONNECT")];
        //[connCell setImage:[NSImage imageNamed:@"ToConnectInternet.png"]];
		[curConnText setStringValue:curOperName];
		NSLog(@"[Dialup type] Connected\n");
//		[dele OnActChange:m_nAct];
		break;
	case kSCNetworkConnectionDisconnected:
        //  [dialupBtn setEnabled:YES];
		[dialupBtn setTitle:LocalizeString(@"IDS_BTNTITLE_CONNECT")];
       // [connCell setImage:[NSImage imageNamed:@"ToDisconnectedInternet.png"]];
		[curConnText setStringValue:@""];
		NSLog(@"[Dialup type] Disconnected\n");
//		[dele OnActChange:m_nAct];
		break;
	case kSCNetworkConnectionConnecting:
        //[dialupBtn setEnabled:NO];
		[dialupBtn setTitle:LocalizeString(@"IDS_BTNTITLE_CONNECTING")];
		[curConnText setStringValue:curOperName];
		break;
	case kSCNetworkConnectionDisconnecting:
        //[dialupBtn setEnabled:NO];
		[dialupBtn setTitle:LocalizeString(@"IDS_BTNTITLE_DISCONNECTING")];
		[curConnText setStringValue:curOperName];
		break;
	default:
        
		[dialupBtn setTitle:LocalizeString(@"IDS_BTNTITLE_CONNECT")];
		[curConnText setStringValue:@""];
		break;
	}
}

- (void)UpdateCurConnText:(NSString*)selname
{
	if (curOperName)
		[curOperName release];
	if (selname)
		curOperName = [[NSString alloc]initWithString:selname];
	else
		curOperName = [[NSString alloc]initWithString:@""];
}

@end
