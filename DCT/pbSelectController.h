//
//  contactSelectController.h
//  DCT
//
//  Created by Fengping Yu on 10/25/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "pbSelectListCtrl.h"
#import "pbSelectTreeCtrl.h"

@interface pbSelectController : NSWindowController<NSWindowDelegate>
{
@private
	IBOutlet NSSearchField *contactSearch;
	IBOutlet pbSelectListCtrl *contactList;
	IBOutlet pbSelectListCtrl *recipientList;
	IBOutlet pbSelectTreeCtrl *folderTree;
	IBOutlet NSButton *addButton;
	IBOutlet NSButton *deleteButton;
	IBOutlet NSButton *okButton;
	IBOutlet NSButton *cancelButton;
	
	NSUInteger searchCategory;
	
	PbFolder *m_pFolder;
	NSMutableString *m_strNumbers;
	
	NSMutableArray *globalData;
	BOOL bclickOK;
}

- (void)init:(PbFolder*)pbFolder;
- (NSString*)getNumbers;

- (IBAction)onAdd:(id)sender;
- (IBAction)onDelete:(id)sender;
- (IBAction)onOK:(id)sender;
- (IBAction)onCancel:(id)sender;
- (IBAction)onSearch:(id)sender;
- (IBAction)onPbSelectRecipients:(id)sender;
- (IBAction)onPbSelectRecipientsall:(id)sender;
- (IBAction)onPbSelectDelete:(id)sender;
- (IBAction)onPbSelectDeleteall:(id)sender;

- (IBAction)setSearchCategoryName:(id)sender;
- (IBAction)setSearchCategoryNumber:(id)sender;

- (void)onSelChangedTreePb:(NSNotification*)notification;

@property(readwrite, retain)NSMutableArray *globalData;
@property(nonatomic, retain)IBOutlet pbSelectListCtrl *contactList;
@property(nonatomic, retain)IBOutlet pbSelectListCtrl *recipientList;
@property(nonatomic, retain)IBOutlet pbSelectTreeCtrl *folderTree;

@end
