//
//  ToolbarCtrl.m
//  DCT
//
//  Created by Fengping Yu on 11-10-14.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "ToolbarCtrl.h"
#import "DCTAppDelegate.h"

#define kCallToolbarItemID			@"Call"
#define kCallHistoryToolbarItemID   @"CallHistory"
#define kPhonebookToolbarItemID		@"Phonebook"
#define kMessageToolbarItemID		@"SMS"
#define kDialupToolbarItemID		@"Dialup"
#define kStatisticsToolbarItemID	@"Statistics"
#define UssdToolbarItemID           @"Ussd"
NSString* normalToolbarImages[] = 
{
	@"Call.png",
	@"callhistory.png",
	@"phonebook.png",
	@"sms.png",
	@"dialup.png",
	@"statistic.png",
    @"Mac_USSD.png"
};

NSString* focusToolbarImages[] = 
{
	@"Call_press.png",
	@"callhistory_press.png",
	@"Phonebook_press.png",
	@"sms_press.png",
	@"dialup_press.png",
	@"statistic_press.png",
    @"Mac_USSD_press.png"
};

@implementation ToolbarCtrl

- (void)awakeFromNib
{
	[toolbar setAllowsUserCustomization:YES];
	
	[toolbar setAutosavesConfiguration:YES]; 
	
	[toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
}

- (void)dealloc
{
	[super dealloc];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

//create toolbar item
- (NSToolbarItem*)toolbarItemWithIdentifies:(NSString*)identifier 
									  lable: (NSString*)label 
								paleteLabel:(NSString*)paletteLabel 
									toolTip:(NSString*)toolTip 
									  image:(NSImage*)image
									 target:(id)target 
									 action:(SEL)action
{
	NSToolbarItem *toolbarItem =[[[NSToolbarItem alloc] initWithItemIdentifier:identifier] autorelease];
	
	[toolbarItem setLabel:label];
	[toolbarItem setPaletteLabel:paletteLabel];
	[toolbarItem setToolTip:toolTip];
	[toolbarItem setImage:image];
	
	[toolbarItem setTarget:target];
	[toolbarItem setAction:action];
	
	return toolbarItem;
}


#pragma mark -
#pragma mark NSToolbarDelegate

//this method is required of NSToolbar delegates, it takes an identifier,and returns the matching NSToolbarItem.
- (NSToolbarItem*)toolbar: (NSToolbar*)toolbar itemForItemIdentifier:(NSString *)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag
{
	NSToolbarItem *toolbarItem = nil;
	
	if([itemIdentifier isEqualToString:kCallToolbarItemID])
	{
		toolbarItem = [self toolbarItemWithIdentifies:kCallToolbarItemID
												lable:@"Call"
										  paleteLabel:@"Call"
											  toolTip:@"Call"
												image:[NSImage imageNamed:@"Call"]
											   target:self
											   action:@selector(loadCallView:)];
		[toolbarItem setTag:CALLVIEW];
	}
	else if([itemIdentifier isEqualToString:kCallHistoryToolbarItemID])
	{
		toolbarItem = [self toolbarItemWithIdentifies:kCallHistoryToolbarItemID 
												lable:@"Call History" 
										  paleteLabel:@"Call History" 
											  toolTip:@"Call History"
												image:[NSImage imageNamed:@"callhistory"]
											   target:self
											   action:@selector(loadCallHistoryView:)];
		[toolbarItem setTag:CALLHISTORYVIEW];
	}
	else if([itemIdentifier isEqualToString:kPhonebookToolbarItemID])
	{
		toolbarItem = [self toolbarItemWithIdentifies:kPhonebookToolbarItemID 
												lable:@"Phonebook" 
										  paleteLabel:@"Phonebook"
											  toolTip:@"Phonebook"
												image:[NSImage imageNamed:@"phonebook"]
											   target:self
											   action:@selector(loadPhonebookView:)];
		[toolbarItem setTag:PHONEBOOKVIEW];
	}
	else if([itemIdentifier isEqualToString:kMessageToolbarItemID])
	{
		toolbarItem = [self toolbarItemWithIdentifies:kMessageToolbarItemID
												lable:@"SMS"
										  paleteLabel:@"SMS"
											  toolTip:@"SMS" 
												image:[NSImage imageNamed:@"sms"]
											   target:self
											   action:@selector(loadSMSView:)];
		[toolbarItem setTag:SMSVIEW];
	}
	else if([itemIdentifier isEqualToString:kDialupToolbarItemID])
	{
		toolbarItem = [self toolbarItemWithIdentifies:kDialupToolbarItemID
												lable:@"Dialup"
										  paleteLabel:@"Dialup"
											  toolTip:@"Dialup"
												image:[NSImage imageNamed:@"dialup"]
											   target:self
											   action:@selector(loadDialupview:)];
		[toolbarItem setTag:DIALUPVIEW];
	}
	else if([itemIdentifier isEqualToString:kStatisticsToolbarItemID])
	{
		toolbarItem = [self toolbarItemWithIdentifies:kStatisticsToolbarItemID
												lable:@"Statistics"
										  paleteLabel:@"Statistics"
											  toolTip:@"Statistics"
												image:[NSImage imageNamed:@"statistic"]
											   target:self
											   action:@selector(loadStatisticsView:)];
		[toolbarItem setTag:STATISTICSVIEW];
	}
    else if([itemIdentifier isEqualToString:UssdToolbarItemID])
	{
		toolbarItem = [self toolbarItemWithIdentifies:UssdToolbarItemID
												lable:@"Ussd"
										  paleteLabel:@"Ussd"
											  toolTip:@"Ussd"
												image:[NSImage imageNamed:@"Mac_USSD"]
											   target:self
											   action:@selector(showUssd:)];
		[toolbarItem setTag:USSD];
	}

	
	return toolbarItem;
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar
{
	NSBundle *mainBundle =[NSBundle mainBundle];
	NSDictionary *dict = [mainBundle infoDictionary];
	moduleIDList = [[[NSMutableArray alloc] init] autorelease];
	
	//check module list from plist file
	NSString *call = [dict objectForKey:@"ModuleCall"];
	NSString *callHistory = [dict objectForKey:@"ModuleCallHistory"];
	NSString *phonebook = [dict objectForKey:@"ModulePhonebook"];
	NSString *message =[dict objectForKey:@"ModuleSMS"];
	NSString *dialup =[dict objectForKey:@"ModuleDialup"];
	NSString *statistics =[dict objectForKey:@"ModuleStatistics"];
	NSString *ussd =[dict objectForKey:@"ModuleUssd"];
	if([call isEqualToString:@"YES"])
	{
		[moduleIDList addObject:kCallToolbarItemID];
	}
	
	if([callHistory isEqualToString:@"YES"])
	{
		[moduleIDList addObject:kCallHistoryToolbarItemID];
	}
	
	if([phonebook isEqualToString:@"YES"])
	{
		[moduleIDList addObject:kPhonebookToolbarItemID];
		//  [moduleIDList addObject:NSToolbarSeparatorItemIdentifier];
	}
	
	if([message isEqualToString:@"YES"])
	{
		[moduleIDList addObject:kMessageToolbarItemID];
		//  [moduleIDList addObject:NSToolbarSeparatorItemIdentifier];
	}
	
	if([dialup isEqualToString:@"YES"])
	{
		[moduleIDList addObject:kDialupToolbarItemID];
		//  [moduleIDList addObject:NSToolbarSeparatorItemIdentifier];
	}
	
	if([statistics isEqualToString:@"YES"])
	{
		[moduleIDList addObject:kStatisticsToolbarItemID];
		//  [moduleIDList addObject:NSToolbarSeparatorItemIdentifier];
	}
	if([ussd isEqualToString:@"YES"])
	{
		[moduleIDList addObject:UssdToolbarItemID];
		//  [moduleIDList addObject:NSToolbarSeparatorItemIdentifier];
	}

	//  [moduleIDList removeObjectAtIndex:[moduleIDList count] - 1];
	
	return [NSArray arrayWithArray:moduleIDList];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects: kCallToolbarItemID,
			kCallHistoryToolbarItemID,
			kPhonebookToolbarItemID,
			kMessageToolbarItemID,
			kDialupToolbarItemID,
			kStatisticsToolbarItemID,
            UssdToolbarItemID,
			NSToolbarSpaceItemIdentifier,
			NSToolbarSeparatorItemIdentifier,
			NSToolbarFlexibleSpaceItemIdentifier,
			nil];
}

#pragma mark -
#pragma mark Actions

- (IBAction)loadCallView:(id)sender
{
	DCTAppDelegate *delegate = TheDelegate;
	
	[delegate loadCallView:sender];
}

- (IBAction)loadCallHistoryView:(id)sender
{
	DCTAppDelegate *delegate = TheDelegate;
	
	[delegate loadCallHistoryView:sender];
}

- (IBAction)loadPhonebookView:(id)sender
{
	DCTAppDelegate *delegate = TheDelegate;
	
	[delegate loadPhonebookView:sender];
	
	[self modifyToolbarItemStatus:PHONEBOOKVIEW];
}

- (IBAction)loadSMSView:(id)sender
{
	DCTAppDelegate *delegate = TheDelegate;
	
	[delegate loadSmsView:sender];
}

- (IBAction)loadDialupview:(id)sender
{
	DCTAppDelegate *delegate = TheDelegate;
	
	[delegate loadDialupView:sender];
}

- (IBAction)loadStatisticsView:(id)sender
{
	DCTAppDelegate *delegate = TheDelegate;
	
	[delegate loadStatisticsView:sender];
}
- (IBAction)showUssd:(id)sender
{
    
	DCTAppDelegate *delegate = TheDelegate;
	
	[delegate showUssd:sender];
    
   

}
- (void)modifyToolbarItemStatus:(NSInteger)selectedTag
{
	NSInteger tag;
	
	for(NSToolbarItem *item in [toolbar items])
	{
		tag = [item tag];
		
		if(tag == selectedTag)
		{
			[item setImage:[NSImage imageNamed:focusToolbarImages[tag]]];
		}
		else
		{
			[item setImage:[NSImage imageNamed:normalToolbarImages[tag]]];
		}
	}
}

@end
