//
//  UssdCodeDialog.h
//  D-Link
//
//  Created by atel_lion on 11/6/13.
//
//

#import <Cocoa/Cocoa.h>

#define USSD_STOP_MODAL_STAT_SAVE       1
#define USSD_STOP_MODAL_STAT_CANCEL     2

@interface UssdCodeDialog : NSWindowController<NSWindowDelegate,NSTextFieldDelegate> {
    NSTextField*    textUser;
    NSTextField*    textCode;
    NSButton*       btnSave;
    NSButton*       btnCancel;
    NSString*       strText;
    NSString*       strCode;
}

@property (assign, nonatomic) IBOutlet NSTextField* textUser;
@property (assign, nonatomic) IBOutlet NSTextField* textCode;
@property (assign, nonatomic) IBOutlet NSButton*    btnSave;
@property (assign, nonatomic) IBOutlet NSButton*    btnCancel;

-(void)Init:(NSString*)Text withNstring:(NSString*)Code;
-(IBAction)saveBtn:(id)sender;
-(IBAction)cancelBtn:(id)sender;
@end
