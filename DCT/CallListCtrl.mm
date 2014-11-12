//
//  CallListCtrl.m
//  CocoaTry
//
//  Created by MBJ on 12-5-30.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "Win2Mac.h"
#import "ComAgent.h"
#import "CallListCtrl.h"
#import "CallEngine.h"
#import "ImageTextCell.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"

#define CELL_TAG_ALPHA	@"CallTag"
#define CELL_TAG_TICK	@"CallTick"

#define _MakeItLong(x)	x

@interface CallListCtrl(Private)

- (BOOL)functionTest:(NSInteger)code
		   withIndex:(short)idx;

- (void)updateButtons;
- (void)updateMenu;

- (short)selectedCallIdx;
- (short)clickedCallIdx;
- (short)callIdxFromRow:(NSInteger)row;

- (NSInteger)rowFromCallIdx:(short)idx;

- (const CCall*)findCall:(NSInteger)idx;

- (NSImage*)getIcon:(const CCall*)call
		   withMpty:(UINT)mpty;

- (void)selectCall;

@end


@implementation CallListCtrl

- (id)init
{
	if (self = [super init])
	{
		dataSource = nil;
		hotCallIdx = 0;
	}

	return self;
}

#pragma mark -- 
#pragma mark tableview delegate
#pragma mark--

- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView
{
	return (dataSource==NULL) ? 0 : dataSource->Size();
}

- (_MakeItLong(id))tableView:(NSTableView *)tableView 
   objectValueForTableColumn:(NSTableColumn *)tableColumn 
						 row:(NSInteger)row
{
	const CCall *pCall = [self findCall:row];
	
	if (pCall == NULL)
	{
		return nil;
	}
	
	NSString *identifier = [tableColumn identifier];
	
	if([identifier isEqualToString:CELL_TAG_ALPHA])
	{
		NSCell *cell = [tableColumn dataCell];
		ImageTextCell *imageCell = (ImageTextCell*)cell;

		UINT uMpty = dataSource->GetMptyCount();
		CString &alpha = (CString&)(pCall->GetAlpha());

		NSImage *image = [self getIcon:pCall
							  withMpty:uMpty];

		NSString *strAlpha = [NSString CStringToNSString:alpha];
		
		[imageCell setIcon:image];
		[imageCell setPrimaryTextKey:strAlpha];
		
		return strAlpha;
	}
	else
	{
		const CTimeSpan &dura = pCall->GetDuration();
		
		NSString *str = [NSString localizedStringWithFormat:@"%02ld:%02d:%02d", 
						 dura.GetTotalHours(),
						 dura.GetMinutes(),
						 dura.GetSeconds()];
		
		return str;
	}

}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	[self updateButtons];
}

#pragma mark --
#pragma mark menu delegate
#pragma mark--
/*
- (BOOL)menu:(NSMenu *)menu updateItem:(NSMenuItem *)item
	 atIndex:(NSInteger)index shouldCancel:(BOOL)shouldCancel
{
	[item setEnabled:[self functionTest:[item tag]
							  withIndex:[self clickedCallIdx]]];
	return YES;
}
*/

- (void)menuWillOpen:(NSMenu *)menu
{
	[self updateMenu];
}

#pragma mark--
#pragma mark common function
#pragma mark--

- (void)updateCalls:(const CCallList*)callList
		 withEngine:(CallEngine*)en9ine
{
	if (callList != NULL)
	{
        short hotIdx = hotCallIdx;
		dataSource = callList;
		[tableView reloadData];
        hotCallIdx = hotIdx;
		[self selectCall];
	}
	
	if (en9ine != nil)
	{
		engine = en9ine;
		[self updateCtrl];
	}
}

- (void)updateCtrl
{
	if (engine != nil)
	{
		[self updateButtons];
		[self updateMenu];
	}
}

// reload list without updating buttons
- (void)updateTick
{
	if (dataSource != nil)
	{
		[tableView reloadData];
	}
}

- (IBAction)onButtonClick:(id)sender
{
	ASSERT([sender isKindOfClass:[NSButton class]]);

	[((callControl*)target) onCallCtrl:[((NSButton*)sender) tag]
								 index:[self selectedCallIdx]];
}

- (IBAction)onContextMenu:(id)sender
{
	ASSERT([sender isKindOfClass:[NSMenuItem class]]);
	
	[((callControl*)target) onCallCtrl:[((NSMenuItem*)sender) tag]
								 index:[self clickedCallIdx]];
}

@end


@implementation CallListCtrl(Private)

- (BOOL)functionTest:(NSInteger)code
		   withIndex:(short)idx
{
	if (engine == nil)
	{
		return NO;
	}
	
	switch (code)
	{
		case CC_BTN_SWITCH:
			return ([engine SwitchTest] == CA_OK);
		case CC_BTN_SWAP:
			return NO/*([engine SwapTest] == CA_OK)*/;
		case CC_BTN_ANSWER:
			return ([engine AnswerTest] == CA_OK);
		case CC_BTN_JOIN:
			return ([engine JoinTest] == CA_OK);
		case CC_BTN_EXTRACT:
			return ([engine ExtractTest:idx] == CA_OK);
		case CC_BTN_REPLACE:
			return ([engine ReplaceTest] == CA_OK);
		case CC_BTN_REJECT:
			return ([engine RejectTest] == CA_OK);
		case CC_BTN_TERMINATE:
			return ([engine TerminateTest:idx] == CA_OK);
		case CC_BTN_CUT_ACTIVE:
			return ([engine ReleaseActiveTest] == CA_OK);
		case CC_BTN_CUT_HELD:
			return ([engine ReleaseHeldTest] == CA_OK);
		default:
			break;
	}
	
	ASSERT(0);
	return NO;
}

- (void)updateButtons
{
	NSButton *btnAry[] =
	{
		btnSwitch,		btnSwap,
		btnAnswer,		btnJoin,
		btnExtract,		btnReplace,
		btnReject,		btnTerminate,
		btnCutActive,	btnCutHeld
	};
	
	short idx = [self selectedCallIdx];
	
	for (int i=0; i<CC_BTN_MAX; ++i)
	{
		[btnAry[i] setEnabled:[self functionTest:i 
									   withIndex:idx]];
	}
	
	hotCallIdx = idx;
}

- (void)updateMenu
{
	short idx = [self clickedCallIdx];
	
	for (int i=0; i<CC_BTN_MAX; ++i)
	{
		[[menu itemWithTag:i] setEnabled:[self functionTest:i 
												  withIndex:idx]];
	}
}

- (short)selectedCallIdx
{
	return [self callIdxFromRow:[tableView selectedRow]];
}

- (short)clickedCallIdx
{
	return [self callIdxFromRow:[tableView clickedRow]];
}

- (short)callIdxFromRow:(NSInteger)row
{
	if (row >= 0)
	{
		const CCall *p = [self findCall:row];
		return (NULL == p) ? 0 : p->idx;
	}
	else
	{
		return 0;
	}	
}

- (const CCall*)findCall:(NSInteger)row
{
	if (dataSource != NULL)
	{
		const CCall *p = NULL;
		
		CALLPOS pos = dataSource->GetFirstPos();
		
		while ((p=dataSource->GetNext(pos)) != NULL)
		{
			if ((row--) == 0)
			{
				return p;
			}
		}
	}
	
	return NULL;
}

- (NSInteger)rowFromCallIdx:(short)idx
{
	if (idx > 0)
	{
		NSInteger i = 0;
		
		const CCall *p = NULL;
		
		CALLPOS pos = dataSource->GetFirstPos();
		
		while ((p=dataSource->GetNext(pos)) != NULL)
		{
			if (idx == p->idx)
			{
				return i;
			}
            ++ i;
		}
	}

	return -1;
}

- (NSImage*)getIcon:(const CCall*)call
		   withMpty:(UINT)mpty
{
	NSString *iconName = nil;
	
	switch (call->status)
	{
		case CCall::INCOMING:
			iconName = @"incoming.png";
			break;
		case CCall::WAITING:
			iconName = @"waiting.png";
			break;
		case CCall::DIALING:
		case CCall::ALERTING:
			if (call->IsEmergency())
				iconName = @"sos.png";
			else
				iconName = @"outgoing.png";
			break;
		case CCall::ACTIVE:
			if (mpty > 1 && call->IsMpty())
				iconName = @"active_conf.png";
			else
				iconName = @"active.png";
			break;
		case CCall::HELD:
			if (mpty > 1 && call->IsMpty())
				iconName = @"held_conf.png";
			else
				iconName = @"held.png";
			break;
		default:
			return nil;
	}
	
	return [NSImage imageNamed:iconName];
}

- (void)selectCall
{
	if (dataSource != NULL &&
		dataSource->Size() > 0)
	{
		NSInteger i = [self rowFromCallIdx:hotCallIdx];
		
		if (i < 0)
		{
			hotCallIdx = [self callIdxFromRow:0];
			i = 0;
		}
		
		NSIndexSet *idxSet = [NSIndexSet indexSetWithIndex:i];
		
		[tableView selectRowIndexes:idxSet
			   byExtendingSelection:NO];
		
	}
}

@end

