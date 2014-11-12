//
//  UssdViewController.h
//  D-Link
//
//  Created by atel_lion on 10/31/13.
//
//

#import <Cocoa/Cocoa.h>
#import "UssdCodeDialog.h"
#import "Connection.h"
#include "ComAgent.h"


@interface UssdViewController : NSViewController <NSTableViewDataSource,NSTableViewDelegate,NSTextFieldDelegate>
{
@private
    CComAgent*              m_pComAgent;
    NSMutableArray*			arrayData;
    UssdCodeDialog*         ussdCodedialog;
    
/************ 20131109 by Zhuwei **********************************************/
    NSButton*    btnAdd;
    NSButton*    btnSend;
    NSButton*    btnEdit;
    NSButton*    btnDelete;
    NSTextField* textCommand;
    NSTextView*  textLog;
    NSTableView* tableViewList;
/******************************************************************************/
}

@property (assign, nonatomic) IBOutlet NSButton*    btnAdd;
@property (assign, nonatomic) IBOutlet NSButton*    btnSend;
@property (assign, nonatomic) IBOutlet NSButton*    btnEdit;
@property (assign, nonatomic) IBOutlet NSButton*    btnDelete;
@property (assign, nonatomic) IBOutlet NSTextField* textCommand;
@property (assign, nonatomic) IBOutlet NSTextView*  textLog;
@property (assign, nonatomic) IBOutlet NSTableView* tableViewList;

-(IBAction)actionAdd:(id)sender;
-(IBAction)actionSend:(id)sender;
-(IBAction)actionEdit:(id)sender;
-(IBAction)actionDelete:(id)sender;

@end
