//
//  smsSaveController.m
//  DCT
//
//  Created by Fengping Yu on 10/25/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsSaveController.h"
#import "DCTAppDelegate.h"


@implementation smsSaveController

@synthesize treeControl;

#pragma mark--
#pragma mark init, construct, destruct method
#pragma mark--

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) 
	{
		// Initialization code here.
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSelChangedTreeSms:) name:OnSelChangedSaveSmsTree object:nil];
		bclickOK = NO;
	}
	
	return self;
}

- (void)dealloc
{
	// [m_strMemory release];
	
	[super dealloc];
}

- (void)awakeFromNib
{
	[memoryBox addItemWithObjectValue:LocalizeString(@"PC")];
	
	//BOOL isSIMOK = [[(DCTAppDelegate*)TheDelegate getSmsViewController] getSimStatus];
	
	//if( isSIMOK == YES)
	//	[memoryBox addItemWithObjectValue:LocalizeString(@"SIM")];
	
	[memoryBox selectItemAtIndex:0];
	[memoryBox setObjectValue:[memoryBox objectValueOfSelectedItem]];
	
	[memoryBox setDelegate:self];
	
	//if(isSIMOK == YES)
		//[treeControl initWithDictionaryFile:@"folderTreeWithSIM" andFolder:m_pFolder];
	//else ///不显示SIM Inbox Outbox
		[treeControl initWithDictionaryFile:@"folderTreeWithoutSIM" andFolder:m_pFolder];
	
	[saveButton setEnabled:NO];
	
	[treeControl.outlineView expandItem:nil expandChildren:YES];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (void)init:(SmsFolder *)pFolder outbox:(SmsFolder *)pFolderOutbox inbox:(SmsFolder *)pFolderInbox
{
	m_pFolder = pFolder;
	m_pFolderInbox = pFolderInbox;
	m_pFolderOutbox = pFolderOutbox;
	
	//   m_strMemory = [[NSString alloc] init];
	m_pFolderSave = NULL;
}

#pragma mark--
#pragma mark button response
#pragma mark--

- (IBAction)onCancel:(id)sender
{
	[[self window] performClose:nil];
}

- (IBAction)onOK:(id)sender
{
	NSTreeNode *hItem = [treeControl getSelectedItem];
	m_pFolderSave = [treeControl getItemData:hItem];
	
	if(!m_pFolderSave)
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_CHOOSEFOLDER") forType:MB_OK];
		return;
	}
	
	bclickOK = YES;
	
	[[self window] performClose:self];
}

- (void)windowWillClose:(NSNotification *)notification
{
	if([NSApp isHidden])
		[self.window orderOut:self];
	else
	{
		if(bclickOK)
		{
			bclickOK = NO;
			[NSApp stopModal];
		}
		else
		{
			[NSApp abortModal];
		}
	}
}

#pragma mark--
#pragma mark get method
#pragma mark--

- (SmsFolder*)getFolder
{
	return m_pFolderSave;
}

- (NSString*)getMemory
{
	m_strMemory = [memoryBox objectValueOfSelectedItem];
    int m_numSIM1=[(DCTAppDelegate*)TheDelegate smsController]->m_numSIM;
    int m_numME1=[(DCTAppDelegate*)TheDelegate smsController]->m_numME;
    if(m_numSIM1+m_numME1==100) {
        
        [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_SMS_MEMORYFULL") forType:MB_OK];
        
        }
	if([m_strMemory caseInsensitiveCompare:@"SIM"] == NSOrderedSame) {
/************ 20131109 by Zhuwei **********************************************/
       
        NSString * s = @"SM";
        int m_numSIM = [(DCTAppDelegate*)TheDelegate smsController]->m_numSIM;
        BOOL isSIMFull = m_numSIM >= 50;
        SMSMessage *pMsg = [utility newMessage];
        if(isSIMFull && pMsg) {
            pMsg->SetMemType(_T("ME"));
            s = @"ME";
        } else {
            pMsg->SetMemType(_T("SM"));
        }
       
       
        return s;
/******************************************************************************/
	} else
		return m_strMemory;
}

#pragma mark--
#pragma mark delegate method
#pragma mark--

- (void)onSelChangedTreeSms:(NSNotification *)notification
{
	NSDictionary *dict = [notification userInfo];
	
	classToObject *folderObject = [dict objectForKey:ITEM_FOLDER];
	SmsFolder *pFolder = folderObject->pFolder;
	
	[saveButton setEnabled:NO];
	
	if(pFolder == NULL)
		return;
	
	folder_state_struct state;
	pFolder->GetFolderState(state);
	
	if(state.bSave)
	{
		[saveButton setEnabled:YES];
        
		if( (m_pFolderOutbox && (pFolder == m_pFolderOutbox)) || (m_pFolderInbox && (pFolder == m_pFolderInbox)) )
		{
			if([memoryBox indexOfSelectedItem] == 0)
			{
				NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
				NSString * s = [defaults valueForKey:PROFILE_SMSMEM];
				if([s caseInsensitiveCompare:@"SM"] == NSOrderedSame)
				{
					[memoryBox selectItemWithObjectValue:@"SIM"];
				}
				else
				{
					[memoryBox selectItemWithObjectValue:@"ME"];
				}
			}
		} else {
			[memoryBox selectItemWithObjectValue:@"PC"];
		}
	}
}

// combox delegate
- (void)comboBoxSelectionDidChange:(NSNotification *)notification
{
	[memoryBox setObjectValue:[memoryBox objectValueOfSelectedItem]];
	SmsFolder *pFolder = [treeControl getItemData:[treeControl getSelectedItem]];
	
	[saveButton setEnabled:NO]; 
	
	if(pFolder == NULL)
		return;
	
	folder_state_struct state;
	pFolder->GetFolderState(state);
	
	if(state.bSave)
	{
		NSString *str = [memoryBox objectValueOfSelectedItem];
		// m_strMemory = [str retain];
		if(str != nil)
		{
			if((m_pFolderOutbox && (pFolder == m_pFolderOutbox)) || (m_pFolderInbox && (m_pFolderInbox == pFolder)))
			{
				if([str caseInsensitiveCompare:@"PC"] != NSOrderedSame)
					[saveButton setEnabled:YES];
				else
					[saveButton setEnabled:NO];
			}
			else
			{
				if([str caseInsensitiveCompare:@"PC"] == NSOrderedSame)
					[saveButton setEnabled:YES];
				else
					[saveButton setEnabled:NO];
			}
		}
	}
}

@end
