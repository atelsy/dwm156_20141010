//
//  ussdCodeDialog.m
//  D-Link
//
//  Created by atel_lion on 11/6/13.
//
//

#import "ussdCodeDialog.h"

@interface UssdCodeDialog ()

@end

@implementation  UssdCodeDialog


@synthesize textUser,textCode,btnSave,btnCancel;

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)Init:(NSString*)Text withNstring:(NSString*)Code
{
    strText = [NSString stringWithString:Text];
    strCode = [NSString stringWithString:Code];

}
-(void)awakeFromNib
{
    [textUser setStringValue:strText];
    [textCode setStringValue:strCode];
}
- (void)windowDidLoad
{
    [btnSave setEnabled:NO];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textDidChange:) name:NSControlTextDidChangeNotification object:textCode];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textDidChange:) name:NSControlTextDidChangeNotification object:textUser];
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

#pragma mark notification methods
- (void)textDidChange:(NSNotification *)notification {
    if([[textCode.stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]] isEqualToString:@""]||[[textUser.stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]] isEqualToString:@""] ) {
        [btnSave setEnabled:NO];
        
        
    } else {
        [btnSave setEnabled:YES];
        
    }
}

-(IBAction)saveBtn:(id)sender
{
    [NSApp stopModalWithCode:USSD_STOP_MODAL_STAT_SAVE];
}

-(IBAction)cancelBtn:(id)sender
{
    [NSApp stopModalWithCode:USSD_STOP_MODAL_STAT_CANCEL];
}

-(void)windowWillClose:(NSNotification *)notification {
    [self cancelBtn:self.btnCancel];
}
@end
