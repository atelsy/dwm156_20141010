//
//  contactSelectController.m
//  DCT
//
//  Created by Fengping Yu on 10/25/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "pbSelectController.h"
#import "imageAndTextCell.h"

#define NAMECRETICA 4
#define NUMBERCRETICA   5

enum contextMenuTag
{
	TO_RECIPIENTS = 0,
	ALL_TORECIPIENTS,
	DELETE,
	DELETE_ALL,
};

@implementation pbSelectController

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) 
	{
		// Initialization code here.
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onSelChangedTreePb:) name:PBSELECT_CHANGE object:nil];
		bclickOK = NO;
	}
	
	return self;
}

- (void)awakeFromNib
{
	//construct the search field category menu item
	NSMenu *menu = [[[NSMenu alloc] initWithTitle:@"Contact Search"] autorelease];
	
	NSMenuItem *item = nil;
	
	item = [[[NSMenuItem alloc] initWithTitle:@"Name" action:@selector(setSearchCategoryName:) keyEquivalent:@""] autorelease];
	[item setTarget: self];
	[item setTag:NAMECRETICA];
	[item setState:NSOnState];
	[menu insertItem:item atIndex:0];
	
	item = [[[NSMenuItem alloc] initWithTitle:@"Phone Number" action:@selector(setSearchCategoryNumber:) keyEquivalent:@""] autorelease];
	[item setTarget:self];
	[item setTag:NUMBERCRETICA];
	[menu insertItem:item atIndex:1];
	
	id searchCell = [contactSearch cell];
	[searchCell setSearchMenuTemplate:menu];
	
	searchCategory = NAMECRETICA;
	
	[folderTree initWithDictionaryFile:@"contactTree" andFolder:m_pFolder];
	
	[folderTree.outlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
	
	[contactList display:[folderTree getSelectItemData]];
	
	globalData = [[NSMutableArray alloc] initWithArray:contactList.dataSource copyItems:YES];
	
	[contactList.tableView setDoubleAction:@selector(onPbSelectRecipients:)];
}

- (void)dealloc
{
	[m_strNumbers release];
	[globalData release];
	
	[contactList release];
	[recipientList release];
	[folderTree release];
	
	[super dealloc];
}

@synthesize globalData, recipientList, folderTree, contactList;

- (void)init:(PbFolder*)pbFolder
{
	m_pFolder = pbFolder;
	m_strNumbers = [[NSMutableString alloc] init];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

#pragma mark --
#pragma mark iboutlet action
#pragma makr--

- (IBAction)onPbSelectDelete:(id)sender
{
	[recipientList deleteItem];
}

- (IBAction)onPbSelectDeleteall:(id)sender
{
	[recipientList selectAll];
	
	[recipientList deleteItem];
}

- (IBAction)onPbSelectRecipients:(id)sender
{
	NSArray *object = [contactList getSelectItems];
	
	[recipientList insertItems:object];
}

- (IBAction)onPbSelectRecipientsall:(id)sender
{
	[contactList selectAll];
	
	[self onPbSelectRecipients:nil];
	
	[contactList.tableView deselectAll:nil];
}

#pragma mark--
#pragma mark search operation
#pragma mark--

- (IBAction)setSearchCategoryName:(NSMenuItem*)item
{
	searchCategory = (int)[item tag];
	
	NSMenu *menu = [item menu];
	[[menu itemAtIndex:1] setState:NSOffState];
	[item setState:NSOnState];
	
	[self onSearch:nil];
}

- (IBAction)setSearchCategoryNumber:(NSMenuItem*)item
{
	searchCategory = (int)[item tag];
	
	NSMenu *menu = [item menu];
	[[menu itemAtIndex:0] setState:NSOffState];
	[item setState:NSOnState];
	
	[self onSearch:nil];
}

- (IBAction)onSearch:(id)sender
{
	NSPredicate *predicate = nil;
	NSString *searchString = [contactSearch stringValue];
	
	if([globalData count] >= [contactList.dataSource count] && [globalData count] != 0)
	{
		[contactList.dataSource removeAllObjects];
		[contactList.dataSource addObjectsFromArray:globalData];
	}
	
	if((searchString != nil) && (![searchString isEqualToString:@""]))
	{
		if(searchCategory == NAMECRETICA)
		{
			predicate = [NSPredicate predicateWithFormat:@"NameCell contains %@", searchString];
		}
		else
		{
			predicate = [NSPredicate predicateWithFormat:@"PhoneNumberCell contains %@", searchString];
		}
		
		[globalData removeAllObjects];
		[globalData addObjectsFromArray:contactList.dataSource];
		
		[contactList.dataSource filterUsingPredicate:predicate];
	}
	
	[contactList.tableView reloadData];
}

#pragma mark--
#pragma mark IBAction message
#pragma mark--

- (IBAction)onAdd:(id)sender
{
	[self onPbSelectRecipients:nil];
}

- (IBAction)onDelete:(id)sender
{
	[recipientList deleteItem];
}

- (IBAction)onOK:(id)sender
{
	int number = [recipientList getCount];
	NSString *strName, *strNumber;
	
	for(int i = 0; i < number; i++)
	{
		if([recipientList getData:i name:&strName number:&strNumber])
		{
			if([strNumber length] == 0)
				continue;
			
			if([m_strNumbers length] == 0)
				[m_strNumbers setString:strNumber];
			else
			{
				[m_strNumbers appendString:@";"];
				[m_strNumbers appendString:strNumber];
			}
		}
	}
	
	bclickOK = YES;
	
	[[self window] performClose:nil];
}

- (IBAction)onCancel:(id)sender
{
	[[self window] performClose:nil];
}

- (void)windowWillClose:(NSNotification *)notification
{
	if([NSApp isHidden])
	{
		[self.window orderOut:self];
	}
	else
	{
		if(bclickOK)
		{
			bclickOK = NO;
			[NSApp stopModal];
		}
		else
		{
			[NSApp abortModal];
		}
	}
}

#pragma mark--
#pragma mark others
#pragma mark--

- (NSString*)getNumbers
{
	return m_strNumbers;
}

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	if([menuItem tag] == TO_RECIPIENTS )
	{
		if([contactList getSelectCount] > 0)
			return YES;
		else
			return NO;
	}
	else if([menuItem tag] == ALL_TORECIPIENTS)
	{
		if([contactList getCount] > 0)
			return YES;
		else
			return NO;
	}
	else if([menuItem tag] == DELETE)
	{
		if([recipientList getSelectCount] > 0)
			return YES;
		else
			return NO;
	}
	else if([menuItem tag] == DELETE_ALL)
	{
		if([recipientList getCount] > 0)
			return YES;
		else
			return NO;
	}
	else if([menuItem tag] == NAMECRETICA || [menuItem tag] == NUMBERCRETICA)
	{
		if([contactList getCount] > 0 || [globalData count] > 0)
			return YES;
		else
			return NO;
	}
	
	return YES;
}

#pragma mark--
#pragma mark delegate method
#pragma mark--

- (void)onSelChangedTreePb:(NSNotification *)notification
{
	NSDictionary *dict = [notification userInfo];
	
	classToObject *object = [dict objectForKey:ITEM_FOLDER];
	
	PbFolder *pFolder = object->pbFolder;
	
	if(pFolder)
	{
		[contactList display:pFolder];
		
		[globalData removeAllObjects];
		[globalData addObjectsFromArray:contactList.dataSource];
	}
}

@end
