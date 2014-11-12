//
//  smsSaveController.h
//  DCT
//
//  Created by Fengping Yu on 10/25/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "smsSaveTreeControl.h"
#import "utility.h"

@interface smsSaveController : NSWindowController<NSComboBoxDelegate, NSWindowDelegate> 
{
@private
	IBOutlet NSButton *saveButton;
	IBOutlet NSButton *cancelButton;
	IBOutlet NSComboBox *memoryBox;
	
	IBOutlet smsSaveTreeControl *treeControl;
	
	SmsFolder *m_pFolder;
	SmsFolder *m_pFolderInbox;
	SmsFolder *m_pFolderOutbox;
	SmsFolder *m_pFolderSave;
	
	NSString *m_strMemory;
	BOOL bclickOK;
}

@property (nonatomic, retain) IBOutlet smsSaveTreeControl* treeControl;

- (IBAction)onOK:(id)sender;
- (IBAction)onCancel:(id)sender;
- (void)init:(SmsFolder*)pFolder outbox:(SmsFolder*)pFolderOutbox inbox:(SmsFolder*)pFolderInbox;

- (SmsFolder*)getFolder;
- (NSString*)getMemory;

- (void)onSelChangedTreeSms:(NSNotification*)notification;

@end
