//
//  UssdViewController.m
//  D-Link
//
//  Created by atel_lion on 10/31/13.
//
//

#import "UssdViewController.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"
#include "ComAgent.h"
#include "utility.h"
#include "UssdCodeDialog.h"

@interface UssdViewController ()
@end

@implementation UssdViewController


/************ 20131109 by Zhuwei **********************************************/
@synthesize  btnAdd,btnSend,btnEdit,btnDelete,textCommand,textLog,tableViewList;
#pragma mark init and dealloc methods
/******************************************************************************/

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
        ussdCodedialog = [[UssdCodeDialog alloc]initWithWindowNibName:@"UssdCodeDialog"];
        arrayData = [[NSMutableArray alloc] initWithContentsOfFile:[self getSavePath]];
        if(!arrayData) arrayData = [[NSMutableArray alloc]init];
        
        m_pComAgent = [TheDelegate getComAgent];
        m_pComAgent->RegisterURC(URC_CUSD, self, WM_URCMSG);
        
    }
    
    return self;
}

- (void)awakeFromNib {
    [btnSend setEnabled:NO];
    [btnEdit setEnabled:NO];
    [btnDelete setEnabled:NO];
    [textCommand setEnabled:YES];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textDidChange:) name:NSControlTextDidChangeNotification object:textCommand];
    
}

- (void)dealloc
{
    [ussdCodedialog release];
    [arrayData release];
    [super dealloc];
}

/************ 20131109 by Zhuwei **********************************************/
#pragma mark tableview datasource
/******************************************************************************/

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
  return [arrayData count];

}
/************ 20131109 by Zhuwei **********************************************/
#pragma mark tableview delegate
/******************************************************************************/

- (id)tableView:(NSTableView *)tbview objectValueForTableColumn:(NSTableColumn *)tbcol row:(NSInteger)rowIndex
{
    NSDictionary* dict = [arrayData objectAtIndex:rowIndex];
	NSString *identifier = [tbcol identifier];
	id returnValue = [dict objectForKey:identifier];
	
	return returnValue;
}


- (BOOL)tableView:(NSTableView *)tableView shouldSelectRow:(NSInteger)row {
    NSDictionary* dict = [arrayData objectAtIndex:row];
    self.textCommand.stringValue = [dict objectForKey:@"code"];
    [[NSNotificationCenter defaultCenter] postNotificationName:NSControlTextDidChangeNotification object:textCommand];
    return YES;
}

#pragma mark notification methods
- (void)textDidChange:(NSNotification *)notification {
    if([[textCommand.stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]] isEqualToString:@""]) {
        [btnSend setEnabled:NO];
        [btnEdit setEnabled:NO];
        [btnDelete setEnabled:NO];
    } else {
        [btnSend setEnabled:YES];
        [btnDelete setEnabled:YES];
        [btnEdit setEnabled:YES];
    }
}

/************ 20131109 by Zhuwei **********************************************/
#pragma mark action methods
/******************************************************************************/
- (NSString *)getSavePath {
    NSString* docPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, NO) objectAtIndex:0];
    NSString* saveFolder = [[NSString stringWithFormat:@"%@/D-LINK",docPath] stringByExpandingTildeInPath];
    if(![[NSFileManager defaultManager] fileExistsAtPath:saveFolder]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:saveFolder withIntermediateDirectories:YES attributes:nil error:nil];
    }
    return [NSString stringWithFormat:@"%@/ussdData.plist",saveFolder];;
}

-(IBAction)actionAdd:(id)sender
{
/*
    if(![[textCommand.stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]] isEqualToString:@""]) {
        ussdCodedialog.textUser.stringValue = @"";
         NSLog(@"%@",textCommand.stringValue);
        ussdCodedialog.textCode.stringValue=textCommand.stringValue;
         NSLog(@"%@", ussdCodedialog.textCode.stringValue);
    }
    else
        {
        ussdCodedialog.textUser.stringValue = @"";
        ussdCodedialog.textCode.stringValue = @"";
        }
 
    NSInteger stat = [NSApp runModalForWindow:[ussdCodedialog window]];
    [ussdCodedialog close];
 */
    
    UssdCodeDialog *dlg = [[UssdCodeDialog alloc] retain];

    [dlg Init:@"" withNstring:textCommand.stringValue];
    [dlg initWithWindowNibName:@"UssdCodeDialog"];
    
    NSInteger stat =[NSApp runModalForWindow:[dlg window]];



    if(stat == USSD_STOP_MODAL_STAT_SAVE) {
        //NSString* name = ussdCodedialog.textUser.stringValue;
        //NSString* code = ussdCodedialog.textCode.stringValue;
        NSString* name = [[dlg textUser] stringValue];
        NSString* code = [[dlg textCode] stringValue];
        [arrayData addObject:@{@"name":name,@"code":code}];
        [self.tableViewList reloadData];
        [arrayData writeToFile:[self getSavePath] atomically:YES];
        [self.textCommand setStringValue:code];
    } else {
        [self.textCommand setStringValue:@""];
    }
    
    [dlg close];
    [dlg release];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:NSControlTextDidChangeNotification object:textCommand];
}

-(IBAction)actionSend:(id)sender
{
    const char* command = [self.textCommand.stringValue UTF8String];
    m_pComAgent->UssdControl(self, WM_SETTING_USSD, 1, 1, CString(command));
    //const Connection *conn = CComAgent::GetConnection();
    // m_pComAgent->GetVERNO(<#void *hWnd#>, <#unsigned int uMsg#>)
    
}
-(IBAction)actionEdit:(id)sender
{
    NSInteger selectedIdx = [self.tableViewList selectedRow];
    if(selectedIdx < 0) return;
    
    NSDictionary* dict = [arrayData objectAtIndex:selectedIdx];
    
    UssdCodeDialog *dlg = [[UssdCodeDialog alloc] retain];
    [dlg Init:[dict objectForKey:@"name"] withNstring:[dict objectForKey:@"code"]];
    [dlg initWithWindowNibName:@"UssdCodeDialog"];
    
    NSInteger stat =[NSApp runModalForWindow:[dlg window]];
    
    if(stat == USSD_STOP_MODAL_STAT_SAVE) {
        [arrayData replaceObjectAtIndex:selectedIdx withObject:@{@"name":[[dlg textUser] stringValue],@"code":[[dlg textCode] stringValue]}];
        [arrayData writeToFile:[self getSavePath] atomically:YES];
        self.textCommand.stringValue = [[dlg textCode] stringValue];
        [self.tableViewList reloadData];
    }
    /*
    ussdCodedialog.textUser.stringValue = [dict objectForKey:@"name"];
    ussdCodedialog.textCode.stringValue = [dict objectForKey:@"code"];
    NSInteger stat = [NSApp runModalForWindow:[ussdCodedialog window]];
    [ussdCodedialog close];
    if(stat == USSD_STOP_MODAL_STAT_SAVE) {
        [arrayData replaceObjectAtIndex:selectedIdx withObject:@{@"name":ussdCodedialog.textUser.stringValue,@"code":ussdCodedialog.textCode.stringValue}];
        [arrayData writeToFile:[self getSavePath] atomically:YES];
        self.textCommand.stringValue = ussdCodedialog.textCode.stringValue;
        [self.tableViewList reloadData];
    }
     */
    [dlg close];
    [dlg release];
    [[NSNotificationCenter defaultCenter] postNotificationName:NSControlTextDidChangeNotification object:textCommand];
}
-(IBAction)actionDelete:(id)sender
{
    if([utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_USSD_TRASHDELETE")forType:MB_OKCANCEL]==NSAlertDefaultReturn)
    {
        [arrayData removeObjectAtIndex:[self.tableViewList selectedRow]];
        [self.tableViewList reloadData];
        self.textCommand.stringValue=@"";
        
        [arrayData writeToFile:[self getSavePath] atomically:YES];

        [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION")
                        withMsg:LocalizeString(@"IDS_SMS_DEL_SUCCESS")forType:MB_OK];
            [[NSNotificationCenter defaultCenter] postNotificationName:NSControlTextDidChangeNotification object:textCommand];
    }


}

/************ 20131220by zhangshaohua**********************************************/
#pragma mark 消息循环回调

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
    if(Msg == WM_SETTING_USSD) {
        
    } else if(Msg == WM_URCMSG) {
        WATCH_POINTER(lparam);
        CUrcUssd *p = (CUrcUssd*)lparam;
        int n = p->Value().n;
        NSString* strLog = @"";
        NSString* strShow=@"";
        NSString* strCode=@"";
        switch (n) {
            case 0:
                strCode=self.textCommand.stringValue;
                strCode=[strCode stringByAppendingString:@"\r\n"];
                strLog = @"\r no further user action required\n";
                strShow=[strCode stringByAppendingString:strLog];
                break;
            case 1:
                strCode=self.textCommand.stringValue;
                 strCode=[strCode stringByAppendingString:@"\r\n"];
                if(p->Value().str_urc.IsEmpty())    {
                    
                    strLog = @"\r further user action required\n";
                    
                    }
                    
                else {
                    strLog  =  [NSString CStringToNSString:p->Value().str_urc];
                    strLog  =[strLog stringByAppendingString:@"\n"];
                }
                strShow=[strCode stringByAppendingString:strLog];
                break;
            case 2:
                strCode=self.textCommand.stringValue;
                strCode=[strCode stringByAppendingString:@"\r\n"];
                if(p->Value().str_urc.IsEmpty())    {
                    
                    strLog = @"\r USSD terminated by network\n";
                    
                }
                
                else {
                    strLog  =  [NSString CStringToNSString:p->Value().str_urc];
                    strLog  =[strLog stringByAppendingString:@"\n"];
                }

                strShow=[strCode stringByAppendingString:strLog];
                break;
            case 3:
                strCode=self.textCommand.stringValue;
                 strCode=[strCode stringByAppendingString:@"\r\n"];
                    // strLog = @"\r other local client has responded\n";
                if(p->Value().str_urc.IsEmpty())    {
                    
                    strLog = @"\r other local client has responded\n";
                    
                }
                
                else {
                    strLog  =  [NSString CStringToNSString:p->Value().str_urc];
                    strLog  =[strLog stringByAppendingString:@"\n"];
                }

                strShow=[strCode stringByAppendingString:strLog];
                break;
            case 4:
                strCode=self.textCommand.stringValue;
                 strCode=[strCode stringByAppendingString:@"\r\n"];
                if(p->Value().str_urc.IsEmpty())    {
                    
                    strLog = @"\r Operation not supported\n";                    
                }
                
                else {
                    strLog  =  [NSString CStringToNSString:p->Value().str_urc];
                    strLog  =[strLog stringByAppendingString:@"\n"];
                }

                    // strLog = @"\r operation not supported\n";
                strShow=[strCode stringByAppendingString:strLog];
                break;
            case 5:
                strCode=self.textCommand.stringValue;
                 strCode=[strCode stringByAppendingString:@"\r\n"];
                    //  strLog = @"\r network time out\n";
                if(p->Value().str_urc.IsEmpty())    {
                    
                   strLog = @"\r network time out\n";
                }
                
                else {
                    strLog  =  [NSString CStringToNSString:p->Value().str_urc];
                    strLog  =[strLog stringByAppendingString:@"\n"];
                }

                strShow=[strCode stringByAppendingString:strLog];
                break;
            default:
                break;
        }
        self.textLog.string = [NSString stringWithFormat:@"%@%@",self.textLog.string,strShow];
    }
}
/******************************************************************************/
@end
