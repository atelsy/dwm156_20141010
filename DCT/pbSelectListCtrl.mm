//
//  pbSelectListCtrl.m
//  DCT
//
//  Created by Fengping Yu on 12/13/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "pbSelectListCtrl.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"

@implementation pbSelectData

- (id)init
{
	self = [super init];
	if(self)
	{
		
	}
	
	return self;
}

- (void)dealloc
{
	[strName release];
	[strNumber release];
	
	[super dealloc];
}

@synthesize strName, strNumber;

@end


@implementation pbSelectListCtrl

- (id)init
{
	self = [super init];
	if (self) 
	{
		// Initialization code here.
		dataSource = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (void)dealloc
{
	[dataSource release];
	
	[super dealloc];
}

@synthesize tableView, dataSource;

#pragma mark--
#pragma mark display operation
#pragma mark--

- (BOOL)getData:(int)nItem name:(NSString **)strName number:(NSString **)strNumber
{
	if(nItem >= 0 && nItem < [dataSource count])
	{
		NSDictionary *dict = [dataSource objectAtIndex:nItem];
		
		*strName = [dict objectForKey:NAMECELL];
		*strNumber = [dict objectForKey:PBNUMCELL];
		
		return YES;
	}
	
	return NO;
}

- (void)display:(PbFolder *)pFolder
{
	if(!pFolder)
		return;
	
	[tableView selectAll:nil];
	[self deleteItem];
	
	PhoneBookItem *pItem = NULL;
	ItemPos pos = pFolder->GetFirstItemPos();
	
	while(pos != 0)
	{
		pItem = pFolder->GetNextItem(pos);
		if(pItem)
			[self insertItem:pItem item:-1];
	}
	
	pFolder->CloseNextItem(pos);
	
	[tableView reloadData];
	
	[self selectLastItem];
}

- (void)insertItem:(PhoneBookItem *)pItem item:(int)nItem
{
	if(pItem == NULL)
		return;
	
	CString sName, sNumber;
	pItem->GetPhone(sName, sNumber);
	
	NSString *strName = [NSString CStringToNSString:sName];
	NSString *strNumber = [NSString getStringFromWchar:sNumber];
	
	[self insertItem:strName number:strNumber item:nItem];
}

- (void)insertItem:(NSString *)strName number:(NSString *)strNumber item:(int)nItem
{
	pbSelectData *data = [[[pbSelectData alloc] init] autorelease];
	data.strName = strName;
	data.strNumber = strNumber;
	
	[self addRow:data];
}

- (void)insertItems:(NSArray *)objects   //notes: as the objects is get through [self getselectitems], so it is pbselectdata
{
	for(id object in objects)
	{
		[dataSource addObject:object];
	}
	
	[tableView reloadData];
	
	[self selectLastItem];
}

- (void)addRow:(pbSelectData *)data
{
	NSDictionary *dict = nil;
	dict = [[NSDictionary alloc] initWithObjectsAndKeys:data.strName, NAMECELL, data.strNumber, PBNUMCELL, nil];
	
	[self.dataSource addObject:dict];
	
	[dict release];
}

- (void)selectLastItem
{
	if([self getCount] <= 0)
		return;
	
	[tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:[self getCount] - 1] byExtendingSelection:NO];
	
	[tableView scrollRowToVisible:[self getCount] - 1];
}

#pragma mark --
#pragma mark nstableview datasource and delegate
#pragma mark--

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return [dataSource count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	NSDictionary *dict = [dataSource objectAtIndex:row];
	
	id returnValue = nil;
	
	NSString *identifier = [tableColumn identifier];
	
	if([identifier isEqualToString:NAMECELL])
	{
		returnValue = [dict objectForKey: NAMECELL];
	}
	else if([identifier isEqualToString:PBNUMCELL])
	{
		returnValue = [dict objectForKey:PBNUMCELL];
	}
	
	return returnValue;
}

#pragma mark --
#pragma mark event
#pragma mark--

- (void)keyDown:(NSEvent *)theEvent
{
	NSString *chars = [theEvent characters];
	unichar character = [chars characterAtIndex:0];
	
	if(character == NSDeleteCharacter)
	{
		[self deleteItem];
	}
}

- (void)deleteItem
{
	NSIndexSet *indexSet = [tableView selectedRowIndexes];
	
	[dataSource removeObjectsAtIndexes:indexSet];
	
	[tableView reloadData];
}

#pragma mark--
#pragma mark get set operation
#pragma mark--

- (int)getCount
{
	return (int)[dataSource count];
}

- (int)getSelectCount
{
	return (int)[[tableView selectedRowIndexes] count];
}

- (NSArray*)getSelectItems
{
	NSIndexSet *set = [tableView selectedRowIndexes];
	
	return [dataSource objectsAtIndexes:set];
}

- (void)selectAll
{
	[tableView selectAll:nil];
}

@end
