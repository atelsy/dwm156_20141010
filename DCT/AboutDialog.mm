//
//  AboutDialog.m
//  D-Link
//
//  Created by atel_lion on 11/9/13.
//
//

#import "AboutDialog.h"
#import "DCTAppDelegate.h"
#import "ComAgent.h"
#import "nsstrex.h"

@interface AboutDialog ()

@end

@implementation AboutDialog

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    [[self window] setTitle:@"About D-Link"];
    //[textInfoSoftware setStringValue:@"H/W.:A8\rS/W.:8.0.0IN"];
    DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
    textInfoSoftware.stringValue = dele.softwareVersion;
    
    CComAgent* pComAgent = [dele getComAgent];
    pComAgent->GetVERNO(self, WM_GETVERNO);
}
-(void)windowWillClose:(NSNotification *)notification
{
    [NSApp abortModal];
}

- (void) OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	if(Msg == WM_GETVERNO) {
        WATCH_POINTER(lparam);
        CRespText *pText = (CRespText*)lparam;
        pText->Value();
        NSString *string;
        NSString *strHardware=@"H/W.:A9\rS/W.:";
        NSString *version =  [NSString CStringToNSString:pText->Value()];
        string = [strHardware stringByAppendingString:version];
        [textInfoSoftware setStringValue:string];
    }
	
}
@end
