//
//  verifyPwdDialog.h
//  DCT
//
//  Created by MBJ on 12-1-7.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>

enum SEC_JOBS {
	SEC_LOCKSIM = 0,
	SEC_UNLOCKSIM,
	SEC_VERIFYPIN,
	SEC_VERIFYPIN2,
	SEC_CHANGEPIN,
	SEC_VERIFYPUK,
	SEC_CHANGEPIN2,
	SEC_VERIFYPUK2
};

BOOL IsDec(NSString* str);

@interface verifyPwdDialog : NSWindowController {
	IBOutlet NSTextField*	title1;
	IBOutlet NSTextField*	title2;
	IBOutlet NSTextField*	title3;
	IBOutlet NSTextField*	retryTimes;
	IBOutlet NSSecureTextField*	pwd1;
	IBOutlet NSSecureTextField*	pwd2;
	IBOutlet NSSecureTextField*	pwd3;
	IBOutlet NSButton*	btnOK;
	IBOutlet NSButton*	btnCancel;
	
	int		m_nJob;
}

- (IBAction)OnOKClick:(id)sender;
- (IBAction)OnCancelClick:(id)sender;
- (void)setJob:(int)job;
- (void)showJobCtrls;
- (BOOL)CheckPassword;

@end
