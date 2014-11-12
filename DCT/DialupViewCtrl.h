//
//  DialupViewCtrl.h
//  DCT
//
//  Created by MBJ on 11-11-29.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface DialupViewCtrl : NSViewController {
	IBOutlet NSTextField*	curConnText;
	IBOutlet NSButton		*dialupBtn;
	
	BOOL		m_bRedial;
    

	//	either dialupviewctrl or dialupsettingsview may not be awaken yet
	//	when opername feedback, so combo selection and dialup button's status 
	//	must be saved firstly.
	NSString*	curOperName;
	long		m_dialupBtnStat;
	BOOL	isDialupAvailable;
	int		m_nAct;
/************ 20131111 by Zhuwei **********************************************/
    BOOL _isGetSoftwareVersion;
/******************************************************************************/
}

- (void) OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
- (void) UpdateCurConnText: (NSString*)selname;
- (void) setDialupStat;
- (IBAction)OnDialupBtnClick:(id)sender;

@end
