//
//  pbEditorDlg.mm
//  DCT
//
//  Created by mbj on 12-1-16.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "pbEditorDlg.h"
#import "treeNode.h"
#import "cellData.h"
#import "imageAndTextCell.h"
#import "DCTAppDelegate.h"
#import "pbViewControl.h"
#import "PhoneBookItem.h"
#import "nsstrex.h"

@implementation pbEditorDlg

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) 
	{
		m_NSTextName	 = [[NSMutableString alloc] init];
		m_NSTextHandset  = [[NSMutableString alloc] init];
		m_NSTextHome	 = [[NSMutableString alloc] init];
		m_NSTextCompany  = [[NSMutableString alloc] init];
		m_NSTextEmail	 = [[NSMutableString alloc] init];
		m_NSTextOffice	 = [[NSMutableString alloc] init];
		m_NSTextFax		 = [[NSMutableString alloc] init];
		m_NSTextBirthday = [[NSMutableString alloc] init];
		m_formatter = [[phoneNumberFormatter alloc] init];
	}
	
	return self;
}

- (void)dealloc
{	
	[rootNode release];
	[m_NSTextName release];
	[m_NSTextHandset release];
	[m_NSTextHome release];
	[m_NSTextCompany release];
	[m_NSTextEmail release];
	[m_NSTextOffice release];
	[m_NSTextFax release];
	[m_NSTextBirthday release];
	[m_formatter release];
	[super dealloc];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (id)initWithWindowNibName:(NSString *)windowNibName
{
	self = [super initWithWindowNibName:windowNibName];
	
	if(self)
	{
		NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"contactInfoLabel" ofType:@"dict"]];
		
		rootNode = [[self treeNodeFromDictionary:dict] retain];
	}
	
	return self;
}

-(void)awakeFromNib
{	
	m_bClose = FALSE;
	
	[self InitEdit];
	[self InitRadio];
	
	if(m_nMemory == 0)
		[self EnableEdit:FALSE];
	else
		[self EnableEdit:TRUE];
	
	m_range.location = 0;
	m_range.length = 60;
}

#pragma mark--
#pragma mark outlineview delegate, datasource
#pragma mark--

- (NSArray*)childrenForItem:(id)item
{
	if(item == nil)
	{
		return [rootNode childNodes];
	}
	else
	{
		return [item childNodes];
	}
}

- (id)outlineView:(NSOutlineView*)outlineView child:(NSInteger)index ofItem:(id)item
{
	NSArray *children = [self childrenForItem:item];
	
	return [children objectAtIndex:index];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	return NO;
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	NSArray *children = [self childrenForItem:item];
	
	return [children  count];
}

- (id)outlineView:(NSOutlineView*)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
	id object;
	
	treeNode *node = [item representedObject];
	
	object = node.name;
	
	return object;
}

- (void)outlineView:(NSOutlineView*)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	treeNode *node = [item representedObject];
	
	if(node.image == nil)
		node.image = [NSImage imageNamed:node.name]; 
	
	imageAndTextCell *imageCell = (imageAndTextCell*)cell;
	
	[imageCell setImage:node.image];
}

- (CGFloat)outlineView:(NSOutlineView*)outlineView heightOfRowByItem:(id)item
{
/*	NSRect rect = [self.window.contentView convertRect:[labelList bounds] toView:self.window.contentView];
	float height = rect.size.height/8.6;
	
	return height;
 */
	
	NSRect newRect = [m_ctlEditName convertRect:[m_ctlEditName bounds] toView:self.window.contentView];
	
	return newRect.size.height - 1;
}

- (NSTreeNode*)treeNodeFromDictionary:(NSDictionary*)dict
{
	NSString *nodeName = [dict objectForKey:NAME_KEY];
	treeNode *nodeData = [treeNode nodeDataWithName:nodeName];
	
	NSTreeNode *result = [NSTreeNode treeNodeWithRepresentedObject:nodeData];
	
	NSArray *children = [dict objectForKey:CHILDREN_KEY];
	
	for(id item in children)
	{
		NSTreeNode *childTreeNode;
		
		treeNode *childNodeData = [[treeNode alloc] initWithName:item];
		
		childTreeNode = [NSTreeNode treeNodeWithRepresentedObject:childNodeData];
		[childNodeData release];
		
		[[result mutableChildNodes] addObject:childTreeNode];
	}
	
	return result;
}

- (void)InitRadio
{
	NSString* strSM = LocalizeString(@"IDS_PBMEMORY_SIM");
	[[m_ctlMemType cellWithTag:0]setTitle:strSM];
	NSString* strPC = LocalizeString(@"IDS_PBMEMORY_PC");
	[[m_ctlMemType cellWithTag:1]setTitle:strPC];
	
	pbViewControl *pView = [TheDelegate getPbViewController];
	int nSmTotal = [pView GetPbkSmTotal];
	
	BOOL bEnable = TRUE;
	BOOL bConnect = [pView getSimStatus];
	CString str = _T("PC");
	if(bConnect)
		str = _T("SM");
	
	if(m_pContact)
	{
		const CString &sMem = m_pContact->GetMemType();
		if(sMem == _T("PC"))
			bEnable = FALSE;
		
		[self InitRadio:1 andState:(!bEnable)];
		str = sMem;
	}
	else
	{
		if(str == _T("SM") && nSmTotal <= 0)
			str = _T("PC");
	}
	
	[self InitRadio:0 andState:(nSmTotal>0 && bEnable && bConnect)];
	
	if(str == _T("SM"))
	{
		m_nMemory = 0;
		if(!bConnect)
		{
			[m_ctlEditName setEnabled:FALSE];
			[m_ctlEditHandset setEnabled:FALSE];
			[m_ctlButtonOK setEnabled:FALSE];
		}
	}
	else
		m_nMemory = 1;
	
	[m_ctlMemType selectCellWithTag:m_nMemory];	
	
	[self UpdateData:FALSE];
}

- (void)InitEdit
{
	std::vector<NSTextField*> &vEdit = m_vEdit;
	vEdit.push_back(m_ctlEditName);
	vEdit.push_back(m_ctlEditHandset);
	vEdit.push_back(m_ctlEditHome);
	vEdit.push_back(m_ctlEditCompany);
	vEdit.push_back(m_ctlEditEmail);
	vEdit.push_back(m_ctlEditOffice);
	vEdit.push_back(m_ctlEditFax);
	vEdit.push_back(m_ctlEditBirthday);
	
	NSRect newRect = [m_ctlEditName convertRect:[m_ctlEditName bounds] toView:self.window.contentView];	
	NSRect rect = [self.window.contentView convertRect:[labelList bounds] toView:self.window.contentView];
/*	float height = rect.size.height/8.6;
	float unit = height - newRect.size.height;
	newRect.size.height = height;
	newRect.origin.y -= unit;	
	
	[m_ctlEditName setFrame:newRect];
*/	
	for(int i=1; i<vEdit.size(); i++)
	{
		newRect.origin.y -= newRect.size.height+1;
		[vEdit[i] setFrame:newRect];
		
/*		if([vEdit[i] respondsToSelector:@selector(setFormatter:)] == YES)
		 [vEdit[i] setFormatter:m_formatter];
*/
	}
	
}

- (BOOL)step1:(BOOL)bTarget
{
	// Check Phone Number (Handset)
	if(![utility checkPhoneNumber:[NSString CStringToNSString:m_strHandset]])
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
						withMsg:LocalizeString(@"IDS_ERROR_PBNUMBER")
						forType:MB_OK];
		return FALSE;
	}
	
	// Check Phone Number Len
	if(![utility CheckPhoneNumberLen:m_strHandset andFlag:TRUE])
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
						withMsg:LocalizeString(@"IDS_ERROR_PBNUMBER_MAX")
						forType:MB_OK];
		return FALSE;
	}
	
	// Doesn't allow number/name are both empty
	if(m_strHandset.IsEmpty() && m_str.IsEmpty())
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
						withMsg:LocalizeString(@"IDS_ERROR_PBNAME")
						forType:MB_OK];
		return FALSE;
	}
	
	// Check Other Fields
	Advanced_PBItem_Struct m_adv_pb_item;
	if(m_strMemory == _T("PC") )
	{
		if(![utility checkPhoneNumber:[NSString CStringToNSString:m_strHome]])
		{
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_HOMENUMBER")
							forType:MB_OK];
			return FALSE;
		}
		if(![utility CheckPhoneHomeNumber:m_strHome])
		{
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_PBHOME_MAX")
							forType:MB_OK];
			return FALSE;
		}
		else
		{
			m_adv_pb_item.m_home_num = m_strHome;
		}
		
		if(![utility checkPhoneNumber:[NSString CStringToNSString:m_strOffice]])
		{
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_OFFICENUMBER")
							forType:MB_OK];
			return FALSE;
		}
		if(![utility CheckPhoneOfficeNumber:m_strOffice])
		{
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_PBOFFICE_MAX")
							forType:MB_OK];
			return FALSE;
		}
		else
		{
			m_adv_pb_item.m_office_num = m_strOffice;
		}
		
		if(![utility checkPhoneNumber:[NSString CStringToNSString:m_strFax]])
		{
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_FAXNUMBER")
							forType:MB_OK];
			return FALSE;
		}
		if(![utility CheckPhoneFaxNumber:m_strFax])
		{
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_PBFAX_MAX")
							forType:MB_OK];
			return FALSE;
		}
		else
		{
			m_adv_pb_item.m_fax_num = m_strFax;
		}
		
		if(![utility CheckPhoneCompanyName:m_strCompany])
		{
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_PBCOMPANY_MAX")
							forType:MB_OK];
			return FALSE;
		}
		else
		{
			m_adv_pb_item.m_company_name = m_strCompany;
		}
		if(![utility CheckPhoneEmail:m_strEmail])
		{
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_PBEMAIL_MAX")
							forType:MB_OK];
			return FALSE;
		}
		else
		{
			m_adv_pb_item.m_email = m_strEmail;
		}
		if(![utility CheckPhoneBirthday:m_strBirthday andAdvData:m_adv_pb_item])
		{
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
							withMsg:LocalizeString(@"IDS_ERROR_PBBIRTHDAY_MAX")
							forType:MB_OK];
			return FALSE;
		}
	}
	
	// Insert/Update Contact
	if(m_nItem >= 0 && m_pContact)
	{		
		PhoneBookItem *pContact = [self InitContact:m_nEncodeClass];
		if(pContact)
		{
			CString s = m_strMemory;
			pContact->SetMemType(s);
			pContact->SetAdvPBItem(m_adv_pb_item);
			PostMessage(m_pParentWnd, WM_PB_UPDATE, m_nItem, (LPARAM)pContact);
		}
		[self CloseDialog];
		//PostMessage(m_pParentWnd, WM_PB_DESTROYDLG, PB_DLG_EDITOR, 0);
	}
	else if(m_pFolder)
	{
		if(m_bTarget)
		{
			//HandsetFolder
			PbFolder *pFolder = m_pFolderHandset;
			CString s = m_strMemory;
			
			PhoneBookItem *pContact = [self InitContact:m_nEncodeClass];
			if(pContact)
			{				
				pContact->SetMemType(s);
				pContact->SetAdvPBItem(m_adv_pb_item);
				PostMessage(m_pParentWnd, WM_PB_INSERT, (WPARAM)pFolder, (LPARAM)pContact);
			}
			
			[self CloseDialog];
			//PostMessage(m_pParentWnd, WM_PB_DESTROYDLG, PB_DLG_EDITOR, 0);
		}
		else
		{
			// Show Save Dialog to Choose Folder
			//Gang need to do
			/*CPbSaveDlg dlg(m_strMemory, m_pFolder, m_pFolderHandset);
			 if(::DoModal(dlg) == IDOK)*/
			if(1)
			{
				/*PbFolder *pFolder = dlg.GetFolder();*/
				PbFolder *pFolder = m_pFolder;
				CString s = m_strMemory;
				
				PhoneBookItem *pContact = [self InitContact:m_nEncodeClass];
				if(pContact)
				{				
					pContact->SetMemType(s);
					pContact->SetAdvPBItem(m_adv_pb_item);
					PostMessage(m_pParentWnd, WM_PB_INSERT, (WPARAM)pFolder, (LPARAM)pContact);
				}
				
				[self CloseDialog];
				//PostMessage(m_pParentWnd, WM_PB_DESTROYDLG, PB_DLG_EDITOR, 0);
			}
			else
				return FALSE;
		}
	}	
	
	return TRUE;
}

- (void)Prompt
{
	NSString *strError, *strTxt;
	
	if(m_nEncodeClass == 2)
	{
		if(m_bSM)
		{
			strError = LocalizeString(@"IDS_ERROR_PBNAME_MAX_SIM_0x81");
			strTxt = [NSString localizedStringWithFormat:strError, m_nMaxName, (m_nMaxName-1)/2];
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:strTxt forType:MB_OK];
		}
		else
		{
			strError = LocalizeString(@"IDS_ERROR_PBNAME_MAX_ME_0x81");
			strTxt = [NSString localizedStringWithFormat:strError, m_nMaxName, m_nMaxName-3, (m_nMaxName-1)/2];
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:strTxt forType:MB_OK];
			
		}
	}
	else
	{
		if(m_bSM)
		{				
			strError = LocalizeString(@"IDS_ERROR_PBNAME_MAX_SIM");
			strTxt = [NSString localizedStringWithFormat:strError, m_nMaxName, (m_nMaxName-1)/2];
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:strTxt forType:MB_OK];
		}
		else
		{
			strError = LocalizeString(@"IDS_ERROR_PBNAME_MAX_ME");
			strTxt = [NSString localizedStringWithFormat:strError, m_nMaxName, (m_nMaxName-1)/2];
			[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:strTxt forType:MB_OK];
		}
	}
	return;
}

- (short)CheckPhoneName;
{
	m_strMemory = [self GetMemory];
	
	
	m_bSM = (m_nMemory == 0);
	m_bTarget = FALSE;
	
	m_str = m_strName;
	if(m_strMemory == _T("SM") || m_strMemory == _T("ME"))
		m_bTarget = TRUE;
	
	m_pComAgent = [TheDelegate getComAgent];
	
	m_nEncodeClass = [utility CheckEncodeClass:m_str andLen:m_nLen];
	if(!m_bTarget)
	{
		m_nMaxName = 30;
		return 0;//TO_BREAK;
	}
	BOOL bRet;
	
	const Connection *pCon = CComAgent::GetConnection();
	if(!pCon->CheckSupport0x81CharSet())
	{
		if(m_nEncodeClass == 2)
			m_nEncodeClass = 1;
	}
	
	// Get Maximum Length
	bRet = m_pComAgent->GetPBKNumber(self, WM_CHECKPHONENAME, 1, m_strMemory/*_T("SM")*/);	
	if(bRet == ER_OK)
		return TO_CONTINUE;
	else
	{
		if ([self CheckPBNameContinue:FALSE])
			return TO_BREAK;
	}
	
	return TO_PROMPT;
}

- (BOOL)CheckPBNameContinue:(BOOL)bRet
{
	if(!bRet)
		m_nMaxName = 14;
	
	if(m_nEncodeClass == 1) // UCS2
		m_nMax = (m_nMaxName - 1) / 2;
	else if(m_nEncodeClass == 2) // 0x81
		m_nMax = m_nMaxName - 3;
	else
		m_nMax = m_nMaxName;
	
	// Check Length
	if(m_nLen <= m_nMax)
		return TRUE;
	else if(m_nEncodeClass == 0)
	{ // 7-bit
		m_str = m_str.Left(m_nMax);
		return FALSE;
	}
	else
	{
		// Shorten Name		
		m_str = m_str.Left(m_nMax);			
	}
	return FALSE;
}

- (void)UpdateData:(BOOL)flag
{
	m_nMemory = (int)[(NSButtonCell*)[m_ctlMemType selectedCell] tag];
	
	if(flag)
	{
		m_strName	  = [NSString NSStringToCString:[m_ctlEditName stringValue]];
		m_strHandset  = [NSString NSStringToCString:[m_ctlEditHandset stringValue]];
		m_strHome	  = [NSString NSStringToCString:[m_ctlEditHome stringValue]];
		m_strCompany  = [NSString NSStringToCString:[m_ctlEditCompany stringValue]];
		m_strEmail	  = [NSString NSStringToCString:[m_ctlEditEmail stringValue]];
		m_strFax	  = [NSString NSStringToCString:[m_ctlEditFax stringValue]];
		m_strOffice	  = [NSString NSStringToCString:[m_ctlEditOffice stringValue]];
		m_strBirthday = [NSString NSStringToCString:[m_ctlEditBirthday stringValue]];
	}
	else 
	{
		[m_ctlEditName setStringValue:[NSString CStringToNSString:m_strName.GetBuffer(m_strName.GetLength())]];
		[m_ctlEditHandset setStringValue:[NSString CStringToNSString:m_strHandset.GetBuffer(m_strHandset.GetLength())]];
		[m_ctlEditHome setStringValue:[NSString CStringToNSString:m_strHome.GetBuffer(m_strHome.GetLength())]];
		[m_ctlEditCompany setStringValue:[NSString CStringToNSString:m_strCompany.GetBuffer(m_strCompany.GetLength())]];
		[m_ctlEditEmail setStringValue:[NSString CStringToNSString:m_strEmail.GetBuffer(m_strEmail.GetLength())]];
		[m_ctlEditFax setStringValue:[NSString CStringToNSString:m_strFax.GetBuffer(m_strFax.GetLength())]];
		[m_ctlEditOffice setStringValue:[NSString CStringToNSString:m_strOffice.GetBuffer(m_strOffice.GetLength())]];
		[m_ctlEditBirthday setStringValue:[NSString CStringToNSString:m_strBirthday.GetBuffer(m_strBirthday.GetLength())]];
		
	}
	
}

- (BOOL)ChangeMemory:(PhoneBookItem*)pContact
{
	if (pContact == NULL)
		return FALSE;
	
	[self UpdateData:TRUE];
	
	return (pContact->GetMemType() != [self GetMemory]);
}

- (void)NextEditCtrl
{
	
}

- (void)PrevEditCtrl
{
	
}

- (void)EnableEdit:(BOOL)bEnable
{
	std::vector<NSTextField*> &vEdit = m_vEdit;
	for(int i=2;i<vEdit.size();i++)
	{
		if(vEdit[i] != nil)
			[vEdit[i] setEnabled:bEnable];
	}
}

- (CString)GetMemory
{
	switch (m_nMemory)
	{
		case 0:
			return _T("SM");
		case 1:
		default:
			return _T("PC");
	}
}


- (void)InitRadio:(int)idx andState:(BOOL)bEnable
{
	NSButtonCell *btn = (NSButtonCell*)[m_ctlMemType cellWithTag:idx];
	[btn setEnabled:bEnable];
}

- (void)BtnEnable:(BOOL)bEnable
{
	[m_ctlButtonOK setEnabled:bEnable];
	[m_ctlButtonCancel setEnabled:bEnable];
}

- (PhoneBookItem*)InitContact:(int)encodeClass
{
	PhoneBookItem *pContact = [utility NewContact];
	if(pContact)
	{
		if(m_pContact)
			*pContact = *m_pContact;
		
		pContact->SetPhone((LPCTSTR)m_strName, (LPCTSTR)m_strHandset, encodeClass);
	}
	
	return pContact;
}

- (void)Init:(LPCTSTR)strName Handset:(LPCTSTR)strHandset Home:(LPCTSTR)strHome Company:(LPCTSTR)strCompany 
	   Email:(LPCTSTR)strEmail Office:(LPCTSTR)strOffice Fax:(LPCTSTR)strFax Birthday:(LPCTSTR)strBirthday
	 withIdx:(int)nItem andFolder:(PbFolder*)pFolder andHandset:(PbFolder*)pFolderHandset
  andContact:(PhoneBookItem*)pContact withAdv:(bool) bSupportAdvItem andParentwnd:pParent
{
	m_bModify = FALSE;
	m_strName = strName;
	m_strHandset = strHandset;
	m_strHome = strHome;
	m_strCompany = strCompany;
	m_strEmail = strEmail;
	m_strOffice = strOffice;	
	m_strFax = strFax;
	m_strBirthday = strBirthday;
	m_nItem = nItem;
	m_pFolder = pFolder;
	m_pFolderHandset = pFolderHandset;
	m_pContact = pContact;
	m_nMemory = -1;
	m_bSupportAdvItem = bSupportAdvItem;
	m_pParentWnd = pParent;
}


- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	switch(Msg)
	{
		case WM_CHECKPHONENAME:
			[self OnCheckPhoneName:wparam andLparam:lparam];
			break;
	}
}

- (LRESULT) OnCheckPhoneName:(WPARAM)wParam andLparam:(LPARAM)lParam
{
	WATCH_POINTER(lParam);
	CRespArray *pRespArray;
	short step = LOWORD(wParam);	
	int res;
	
	switch(step)
	{
		case 0:
			res = [self CheckPhoneName];
			if (res == TO_CONTINUE)
				break;
			else if (res == TO_PROMPT)
			{
				[self Prompt];
				[self BtnEnable:TRUE];
			}
			else if (res == TO_BREAK)
			{
				if(![self step1:TRUE])
					[self BtnEnable:TRUE];
			}
			else
			{
				if([self CheckPBNameContinue:TRUE])
				{
					if(![self step1:FALSE])
						[self BtnEnable:TRUE];
				}
				else
				{
					[self Prompt];
					[self BtnEnable:TRUE];
				}
			}
			break;
		case 1:
			pRespArray = (CRespArray*)lParam;
			if (HIWORD(wParam) == ER_OK)
			{
				m_nMaxNum = pRespArray->Value()[1];
				m_nMaxName = pRespArray->Value()[2];
				if (![self CheckPBNameContinue:TRUE])
				{
					[self Prompt];
					[self BtnEnable:TRUE];
					break;
				}
			}
			else
			{
				if (![self CheckPBNameContinue:FALSE])
				{
					[self Prompt];
					[self BtnEnable:TRUE];
					break;
				}
			}
			if(![self step1:TRUE])
				[self BtnEnable:TRUE];
			break;
		default:
			break;
	}
	return 0;
}

- (void)OnRadio1	// SIM
{
	if([self ChangeMemory:m_pContact])
		m_bModify = TRUE;
	[self EnableEdit:FALSE];
}

- (void)OnRadio2	// PC
{
	if([self ChangeMemory:m_pContact])
		m_bModify = TRUE;
	[self EnableEdit:TRUE];
}

- (void)CloseDialog
{
	m_bModify = FALSE;
	m_bClose = TRUE;
	
	[[self window] performClose:nil];
	
	PostMessage(m_pParentWnd, WM_PB_DESTROYDLG, PB_DLG_EDITOR, 0);
}

- (IBAction)OnOK:(id)sender
{
	// Update Data
	[self UpdateData:TRUE];
	
	//m_nItem==-2 save from call history
	if(m_strHandset.IsEmpty() && m_strName.IsEmpty())
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_PBNAME") forType:MB_OK];
		return;
	}
	
	if((!m_bModify)&&(m_nItem!=-2))
	{
		[self CloseDialog];
		return;
	}
	
	CString str = m_strName/*, strError*/;	
	
	if(![utility CheckPhoneChar:str])
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_PB_NAME_INVALID_CHAR") forType:MB_OK];
		return;
	}
	
	[self BtnEnable:FALSE];
	SendMessage(self, WM_CHECKPHONENAME, 0, 0);
}

- (IBAction)OnCancel:(id)sender
{
	m_bModify = FALSE;
	m_bClose = TRUE;
	
	[self CloseDialog];
	
}

- (IBAction)OnRadioMatrixChange:(id)sender
{
	switch ([(NSButtonCell*)[sender selectedCell] tag]) 
	{
		case 0:		//	SIM
			[self OnRadio1];
			break;
		case 1:		//  PC
			[self OnRadio2];
			break;
	}
}

- (BOOL)windowShouldClose:(id)sender
{
	if(m_bModify)
	{
		NSInteger result = [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_PB_EDITOR_SAVE") forType:MB_YESNOCANCEL];
		
		switch(result)
		{
			case NSAlertDefaultReturn:
				[self OnOK:nil];
				return NO;
				
			case NSAlertAlternateReturn:
				m_bModify = NO;
				return YES;
				
			case NSAlertOtherReturn:
				return NO;
				
			default:
				break;
		}
	}
	
	return YES;
}

- (void)windowWillClose:(NSNotification *)notification
{
	if(m_bClose)
		return;
	
	PostMessage(m_pParentWnd, WM_PB_DESTROYDLG, PB_DLG_EDITOR, 0);
}

- (void)textDidChange:(NSNotification *)notification
{
	m_bModify = YES;
}

- (void)AdjustStringLength:(NSNotification *)obj
{	
	NSRange range;
	int nSpare;
	
	if([obj object] == m_ctlEditName)
	{
		NSString *strName = [m_ctlEditName stringValue];
		range = [[m_ctlEditName currentEditor] selectedRange];		
		nSpare = (int)([strName length] - m_range.length);
		if (nSpare >= 1)
		{
			NSString *str = [NSString stringWithFormat:@"%@%@", 
							 [strName substringToIndex:range.location-nSpare], 
							 [strName substringFromIndex:range.location]];
			[m_ctlEditName setStringValue:str];
			range.location -= nSpare;
			[[m_ctlEditName currentEditor] setSelectedRange:range];
		}
	}
	else if([obj object] == m_ctlEditHandset)
	{
		NSString *strHandset = [m_ctlEditHandset stringValue];
		range = [[m_ctlEditHandset currentEditor] selectedRange];	
		nSpare = (int)([strHandset length] - m_range.length);
		if (nSpare >= 1)
		{
			NSString *str = [NSString stringWithFormat:@"%@%@", 
							 [strHandset substringToIndex:range.location-nSpare], 
							 [strHandset substringFromIndex:range.location]];
			[m_ctlEditHandset setStringValue:str];
			range.location -= nSpare;
			[[m_ctlEditHandset currentEditor] setSelectedRange:range];
		}
	}
	else if([obj object] == m_ctlEditHome)
	{
		NSString *strHome = [m_ctlEditHome stringValue];
		range = [[m_ctlEditHome currentEditor] selectedRange];	
		nSpare = (int)([strHome length] - m_range.length);
		if (nSpare >= 1)
		{
			NSString *str = [NSString stringWithFormat:@"%@%@", 
							 [strHome substringToIndex:range.location-nSpare], 
							 [strHome substringFromIndex:range.location]];
			[m_ctlEditHome setStringValue:str];
			range.location -= nSpare;
			[[m_ctlEditHome currentEditor] setSelectedRange:range];
		}
		
	}
	else if([obj object] == m_ctlEditCompany)
	{
		NSString *strCompany = [m_ctlEditCompany stringValue];
		range = [[m_ctlEditCompany currentEditor] selectedRange];	
		nSpare = (int)([strCompany length] - m_range.length);
		if (nSpare >= 1)
		{
			NSString *str = [NSString stringWithFormat:@"%@%@", 
							 [strCompany substringToIndex:range.location-nSpare], 
							 [strCompany substringFromIndex:range.location]];
			[m_ctlEditCompany setStringValue:str];
			range.location -= nSpare;
			[[m_ctlEditCompany currentEditor] setSelectedRange:range];
		}
	}
	else if([obj object] == m_ctlEditEmail)
	{
		NSString *strEmail = [m_ctlEditEmail stringValue];
		range = [[m_ctlEditEmail currentEditor] selectedRange];	
		nSpare = (int)([strEmail length] - m_range.length);
		if (nSpare >= 1)
		{
			NSString *str = [NSString stringWithFormat:@"%@%@", 
							 [strEmail substringToIndex:range.location-nSpare], 
							 [strEmail substringFromIndex:range.location]];
			[m_ctlEditEmail setStringValue:str];
			range.location -= nSpare;
			[[m_ctlEditEmail currentEditor] setSelectedRange:range];
		}
	}
	else if([obj object] == m_ctlEditFax)
	{
		NSString *strFax = [m_ctlEditFax stringValue];
		range = [[m_ctlEditFax currentEditor] selectedRange];	
		nSpare = (int)([strFax length] - m_range.length);
		if (nSpare >= 1)
		{
			NSString *str = [NSString stringWithFormat:@"%@%@", 
							 [strFax substringToIndex:range.location-nSpare], 
							 [strFax substringFromIndex:range.location]];
			[m_ctlEditFax setStringValue:str];
			range.location -= nSpare;
			[[m_ctlEditFax currentEditor] setSelectedRange:range];
		}
	}
	else if([obj object] == m_ctlEditOffice)
	{
		NSString *strOffice= [m_ctlEditOffice stringValue];
		range = [[m_ctlEditOffice currentEditor] selectedRange];	
		nSpare = (int)([strOffice length] - m_range.length);
		if (nSpare >= 1)
		{
			NSString *str = [NSString stringWithFormat:@"%@%@", 
							 [strOffice substringToIndex:range.location-nSpare], 
							 [strOffice substringFromIndex:range.location]];
			[m_ctlEditOffice setStringValue:str];
			range.location -= nSpare;
			[[m_ctlEditOffice currentEditor] setSelectedRange:range];
		}
	}
	else if([obj object] == m_ctlEditBirthday)
	{
		NSString *strBirthday = [m_ctlEditBirthday stringValue];
		range = [[m_ctlEditBirthday currentEditor] selectedRange];	
		nSpare = (int)([strBirthday length] - m_range.length);
		if (nSpare >= 1)
		{
			NSString *str = [NSString stringWithFormat:@"%@%@", 
							 [strBirthday substringToIndex:range.location-nSpare], 
							 [strBirthday substringFromIndex:range.location]];
			[m_ctlEditBirthday setStringValue:str];
			range.location -= nSpare;
			[[m_ctlEditBirthday currentEditor] setSelectedRange:range];
		}
	}
}

- (void)controlTextDidChange:(NSNotification *)obj
{
	/*	[self UpdateData:TRUE];
	 
	 if (m_strName.GetLength() || m_strHandset.GetLength() ||
	 m_strHome.GetLength() || m_strCompany.GetLength() ||
	 m_strEmail.GetLength() || m_strFjax.GetLength() ||
	 m_strOffice.GetLength() || m_strBirthday.GetLength())
	 
	 else
	 m_bModify = FALSE; */  
	
	m_bModify = TRUE;
	
	[self AdjustStringLength:obj];
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


@end
