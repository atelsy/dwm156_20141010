//
//  pbCSVDlg.h
//  DCT
//
//  Created by mbj on 12-8-6.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "utility.h"

#define BUFSIZE			1024
#define PBIMPORTCOUNT	3000
//Outlook contact field index
#define FIRSTNAME_INDEX		2   //Column B
#define LASTNAME_INDEX		4	//Column D
#define COMPANY_INDEX		6	//Column F
#define FAX_INDEX			31	//Column AE
#define OFFICE_INDEX		32  //Column AF
#define HOME_INDEX			38	//Column AL
#define MOBILE_INDEX		41	//Column AO
#define EMAIL_CH_INDEX		52	//Column AZ
#define BIRTHDAY_CH_INDEX	75	//Column BW
#define EMAIL_ENG_INDEX		58	//Column BF
#define BIRTHDAY_ENG_INDEX	53	//Column BA

enum PBIMPORT
{
	PBIMPORT_OK = 0,
	PBIMPORT_FULL,
	PBIMPORT_ERR
};


@interface pbCSVDlg : NSWindowController {

	IBOutlet NSButton *m_ctlButtonOK;
	IBOutlet NSButton *m_ctlButtonCancel;
	IBOutlet NSMatrix *m_ctlMatrix;
	
	id	m_pParent;
	PbFolder *m_pHansetFolder;
	PbFolder *m_pPcFolder;
	
	bool m_bModelDlg;
}

- (IBAction)OnOK:(id)sender;
- (IBAction)OnCancel:(id)sender;

- (void)MultibyteToSinglebyte:(CString &)str To:(unsigned char*)buf;
- (BOOL)Init:(PbFolder*)pHandSetFolder withPcFolder:(PbFolder*)pPcFolder andParent:(id)pParent;
- (PBIMPORT)ImportPhonebook:(NSString*)strFile;
- (bool)ExportPhonebook:(NSString*)strFile;
- (bool)WriteContact:(PbFolder*)pFolder Handle:(FILE*)fd;
- (void)awakeFromNib;
- (NSString*)OpenFile;
- (NSString*)SaveFile;
- (bool)IsModelDlg;
@end
