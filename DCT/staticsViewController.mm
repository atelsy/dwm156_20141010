//
//  staticsViewController.m
//  DCT
//
//  Created by Fengping Yu on 11/30/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//
#import "staticsViewController.h"
#import "DCTAppDelegate.h"

#define PIXELSPERSECOND		2		// 2 pixels per second
#define BYTESGROWSCALE		5000	// grow up unit is 5k
#define SCALENUM			10		// 10 rows

@implementation statisticDiagramView

- (id)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame];
	if (self) {
		m_BytesPerScale = BYTESGROWSCALE;
		m_ViewRect.size.height = frame.size.height;
		m_ViewRect.size.width = frame.size.width;
	}
	return self;
}

- (void)drawRect: (NSRect)ViewRect
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	DialupMgr* mgr = dele.dialupMgr;
	
	CGContextRef myContext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	CGContextBeginPath(myContext);
	
	CGContextSelectFont(myContext, "Helvetica", 12, kCGEncodingMacRoman);
	CGContextSetTextDrawingMode(myContext, kCGTextFill);
	CGAffineTransform xform = CGAffineTransformMake(1, 0, 0, 1, 0, 0);
	CGContextSetTextMatrix(myContext, xform);
	
	CRect diagRect = CRect(m_ViewRect.origin.x+30, m_ViewRect.origin.y + m_ViewRect.size.height-30, 
						   m_ViewRect.origin.x + m_ViewRect.size.width-20, 
						   m_ViewRect.origin.y+30);
	
	//	calculate scale
	if (mgr->m_peak > m_BytesPerScale*SCALENUM)
	{
		int growscalenum = (mgr->m_peak/SCALENUM)/BYTESGROWSCALE + 1;
		m_BytesPerScale = growscalenum*BYTESGROWSCALE;
	}
		
	while (mgr->m_spdTxList.size() * PIXELSPERSECOND > (diagRect.right-diagRect.left))
	{
		mgr->m_spdTxList.pop_back();
		mgr->m_spdRxList.pop_back();
	}
	
//	draw background
	CGContextSetRGBFillColor(myContext, 0, 0, 0, 1);
	CGContextFillRect(myContext, NSRectToCGRect(m_ViewRect));
	
	CGContextSetRGBFillColor(myContext, 0, 1, 1, 1);
	CGContextSetRGBStrokeColor(myContext, 0, 1, 1, 1);
	CGContextSetLineWidth(myContext, 1);
	
	//	draw comments
	char tag[64];
	strcpy(tag, "Speed unit: KB/s");
	CGContextShowTextAtPoint(myContext, diagRect.left+10, diagRect.bottom-15, tag, strlen(tag));
	strcpy(tag, "Download:");
	CGContextShowTextAtPoint(myContext, diagRect.left+130, diagRect.bottom-15, tag, strlen(tag));
	strcpy(tag, "Upload:");
	CGContextShowTextAtPoint(myContext, diagRect.left+250, diagRect.bottom-15, tag, strlen(tag));

	CGContextSetRGBFillColor(myContext, 1, 1, 0, 1);	//	rx: yellow
	CGContextFillRect(myContext, CGRectMake(diagRect.left+205, diagRect.bottom-15, 10, 7));
	CGContextSetRGBFillColor(myContext, 0, 1, 0, 1);	//	tx: green
	CGContextFillRect(myContext, CGRectMake(diagRect.left+305, diagRect.bottom-15, 10, 7));

	//	draw grids
	CGContextSetRGBFillColor(myContext, 0, 1, 1, 1);
	double smallgridheight = (double)(diagRect.top-diagRect.bottom)/SCALENUM;
	
	for(int i = 0; i <= SCALENUM; i++)
	{
		float y = diagRect.bottom + smallgridheight * i;
		sprintf(tag, "%d", i*m_BytesPerScale/1000);
		
		CGContextShowTextAtPoint(myContext, diagRect.left-25, y-10, tag, strlen(tag));
		CGContextMoveToPoint(myContext, diagRect.right, y);
		CGContextAddLineToPoint(myContext, diagRect.left, y);
	}
	CGContextStrokePath(myContext);
	
	//	draw data curve
	if (!mgr->m_spdTxList.empty())
	{
		CGContextSetRGBStrokeColor(myContext, 0, 1, 0, 1);	//	tx: green
		std::list<int>::iterator txiter = mgr->m_spdTxList.begin();
		CGContextMoveToPoint(myContext, diagRect.right, 
								diagRect.bottom + (*txiter)*(diagRect.top-diagRect.bottom)/(SCALENUM*m_BytesPerScale));
		int n = 0;
		for (; txiter != mgr->m_spdTxList.end(); txiter ++)
		{
			int spdtx = diagRect.bottom + (*txiter)*(diagRect.top-diagRect.bottom)/(SCALENUM*m_BytesPerScale);
			CGContextAddLineToPoint(myContext, diagRect.right - n*PIXELSPERSECOND, spdtx);
			n ++;
		}
		CGContextStrokePath(myContext);
	}

	if (!mgr->m_spdRxList.empty())
	{
		CGContextSetRGBStrokeColor(myContext, 1, 1, 0, 1);	//	rx: yellow
		std::list<int>::iterator rxiter = mgr->m_spdRxList.begin();
		CGContextMoveToPoint(myContext, diagRect.right, 
								diagRect.bottom + (*rxiter)*(diagRect.top-diagRect.bottom)/(SCALENUM*m_BytesPerScale));
		int n = 0;
		for (; rxiter != mgr->m_spdRxList.end(); rxiter ++)
		{
			int spdrx = diagRect.bottom + (*rxiter)*(diagRect.top-diagRect.bottom)/(SCALENUM*m_BytesPerScale);
			CGContextAddLineToPoint(myContext, diagRect.right - n*PIXELSPERSECOND, spdrx);
			n ++;
		}
		CGContextStrokePath(myContext);
	}
	
	//	draw border - cover tx&rx's green and yellow line of 0 value..
	CGContextSetRGBStrokeColor(myContext, 0, 1, 1, 1);
	CGContextStrokeRectWithWidth(myContext, CGRectMake(diagRect.left, diagRect.bottom, 
										   diagRect.right - diagRect.left, 
										   diagRect.top - diagRect.bottom), 2);
}

@end


@implementation staticsViewController

@synthesize diagramView, recordDataView;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
	if (self)
	{
		DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
		DialupMgr* mgr = dele.dialupMgr;
		mgr->statisticReceiver = self;
		
		[recordDataView setDelegate:self];
		[recordDataView setDataSource:self];

		dataSource = [[NSMutableArray alloc] init];
		NSMutableDictionary *dict = nil;
		
		NSString* dld = LocalizeString(@"IDS_TABLETITLE_DOWNLOADED");
		NSString* upld = LocalizeString(@"IDS_TABLETITLE_UPLOADED");
		NSString* dt = LocalizeString(@"IDS_TABLETITLE_DATA");
		NSString* spd = LocalizeString(@"IDS_TABLETITLE_SPEED");
		
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 dld, @"statisticName", 
				 @"", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 dt, @"statisticName", 
				 @"B", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 spd, @"statisticName", 
				 @"B/s", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];
		
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 upld, @"statisticName", 
				 @"", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];		
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 dt, @"statisticName", 
				 @"B", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 spd, @"statisticName", 
				 @"B/s", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];

		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 @"", @"statisticName", 
				 @"", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];		
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 LocalizeString(@"IDS_TABLETITLE_DURATION"), @"statisticName", 
				 @"", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 @"", @"statisticName", 
				 @"", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];
		
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 LocalizeString(@"IDS_TABLETITLE_DAILY"), @"statisticName", 
				 @"", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];		
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 dld, @"statisticName", 
				 @"B", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 upld, @"statisticName", 
				 @"B", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];
		
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 LocalizeString(@"IDS_TABLETITLE_WEEKLY"), @"statisticName", 
				 @"", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];		
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 dld, @"statisticName", 
				 @"B", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 upld, @"statisticName", 
				 @"B", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];
		
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 LocalizeString(@"IDS_TABLETITLE_YEARLY"), @"statisticName", 
				 @"", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];		
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 dld, @"statisticName", 
				 @"B", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];
		dict = [[[NSMutableDictionary alloc] initWithObjectsAndKeys:
				 upld, @"statisticName", 
				 @"B", @"statisticValue",
				 nil]autorelease];
		[dataSource addObject:dict];

		[recordDataView reloadData];
	}
	
	return self;
}

- (void) OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	if (Msg == WM_DIALUP_STATISTICS)
	{
		DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
		DialupMgr* mgr = dele.dialupMgr;
		
		NSMutableDictionary* dict = [dataSource objectAtIndex:1];
		NSString* str = [self FormatData: mgr->m_Rx ifSpeed: FALSE];
		[dict setValue:str forKey:@"statisticValue"];
		
		dict = [dataSource objectAtIndex:2];
		str = [self FormatData: mgr->m_spdRx ifSpeed: TRUE];
		[dict setValue:str forKey:@"statisticValue"];
		
		dict = [dataSource objectAtIndex:4];
		str = [self FormatData: mgr->m_Tx ifSpeed: FALSE];
		[dict setValue:str forKey:@"statisticValue"];
		
		dict = [dataSource objectAtIndex:5];
		str = [self FormatData: mgr->m_spdTx ifSpeed: TRUE];
		[dict setValue:str forKey:@"statisticValue"];

		dict = [dataSource objectAtIndex:7];
		str = [self FormatDuration: mgr->m_connDur];
		[dict setValue:str forKey:@"statisticValue"];
		
		dict = [dataSource objectAtIndex:10];
		str = [self FormatData: mgr->m_DayRx ifSpeed: FALSE];
		[dict setValue:str forKey:@"statisticValue"];

		dict = [dataSource objectAtIndex:11];
		str = [self FormatData: mgr->m_DayTx ifSpeed: FALSE];
		[dict setValue:str forKey:@"statisticValue"];
	
		dict = [dataSource objectAtIndex:13];
		str = [self FormatData: mgr->m_WeekRx ifSpeed: FALSE];
		[dict setValue:str forKey:@"statisticValue"];
		
		dict = [dataSource objectAtIndex:14];
		str = [self FormatData: mgr->m_WeekTx ifSpeed: FALSE];
		[dict setValue:str forKey:@"statisticValue"];
		
		dict = [dataSource objectAtIndex:16];
		str = [self FormatData: mgr->m_YearRx ifSpeed: FALSE];
		[dict setValue:str forKey:@"statisticValue"];
		
		dict = [dataSource objectAtIndex:17];
		str = [self FormatData: mgr->m_YearTx ifSpeed: FALSE];
		[dict setValue:str forKey:@"statisticValue"];
		
		[recordDataView reloadData];
		[diagramView display];
	}
}

- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView
{
	return [dataSource count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	NSMutableDictionary* dict = [dataSource objectAtIndex:row];
	NSString *identifier = [tableColumn identifier];
	id returnValue = [dict objectForKey:identifier];
	
	return returnValue;
}

- (void)tableView:(NSTableView*)tbview setObjectValue:(id)obj forTableColumn:(NSTableColumn*)tbcol row:(int)rowIndex
{
	NSMutableDictionary* dict = [dataSource objectAtIndex:rowIndex];
	NSString *identifier = [tbcol identifier];
	[dict setValue:obj forKey:identifier];
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
}

- (NSString*) FormatData: (uint64)data 
				 ifSpeed: (BOOL)isSpeed
{
	NSString* str;
	if (data > 1000000000)
		str = [NSString stringWithFormat: @"%.2fGB", (float)(data/10000000)/100];
	else if (data > 1000000)
		str = [NSString stringWithFormat: @"%.2fMB", (float)(data/10000)/100];
	else if (data > 1000)
		str = [NSString stringWithFormat: @"%.2fKB", (float)(data/10)/100];
	else
		str = [NSString stringWithFormat: @"%lldB", data];
	if (isSpeed)
		str = [str stringByAppendingString: @"/s"];
	
	return str;
}

- (NSString*) FormatDuration: (DWORD)dur
{
	NSString* tm;
	int	nSec = dur/1000;   
	nSec %= 60;
	int nMin = dur/60000;   
	nMin %= 60;
	int nHour = dur/(60000*60);   
	nHour %= 24;
	tm = [NSString stringWithFormat:@"%d:%02d:%02d", nHour, nMin, nSec];
	
	return tm;

/*	tm.Format(_T("%02d:%02d:%02d %02d/%02d/%04d"), 
		  mgr->m_LastResetTime.GetHour(),
		  mgr->m_LastResetTime.GetMinute(),
		  mgr->m_LastResetTime.GetSecond(),
		  mgr->m_LastResetTime.GetMonth(),
		  mgr->m_LastResetTime.GetDay(),
		  mgr->m_LastResetTime.GetYear());
*/
}

@end
