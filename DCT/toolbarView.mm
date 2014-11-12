//
//  toolbarView.m
//  DCT
//
//  Created by Fengping Yu on 12/26/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "toolbarView.h"
#import "iconCollectionItem.h"
#import "cellData.h"


NSString* enableToolbarImages[] = 
{
	@"new.png",
	@"reply.png",
	@"forward.png",
	@"cut.png",
	@"copy.png",
	@"paste.png",
	@"delete.png",
	@"download.png"
};

NSString *disableToolbarImages[] = 
{
	@"disable new.png",
	@"disable reply.png",
	@"disable forward.png",
	@"disable cut.png",
	@"disable copy.png",
	@"disable paste.png",
	@"disable delete.png",
	@"disable download.png"
};

NSString *hotToolbarImages[] = 
{
	@"hot new.png",
	@"hot reply.png",
	@"hot forward.png",
	@"hot cut.png",
	@"hot copy.png",
	@"hot paste.png",
	@"hot delete.png",
	@"hot download.png"
};

NSString* pbEnableToolbarImages[] = 
{
	@"new normal.png",
	@"cut normal.png",
	@"copy normal.png",
	@"paste normal.png",
	@"delete normal.png",
	@"download normal.png",
        @"Import_Export_file.png"
};

NSString *pbDisableToolbarImages[] = 
{
	@"new disable.png",
	@"cut disable.png",
	@"copy disable.png",
	@"paste disable.png",
	@"delete disable.png",
	@"download disable.png",
        @"Import_Export_file_disable.png"
};

NSString *pbHotToolbarImages[] = 
{
	@"new hot.png",
	@"cut hot.png",
	@"copy hot.png",
	@"paste hot .png",
	@"delete hot.png",
	@"download hot.png"
};

NSString *chEnableToolbarImages[] = 
{
	@"calls.png",
	@"messages.png",
	@"save.png",
	@"delete normal.png",
	@"download normal.png"
};

NSString *chDisableToolbarImages[] = 
{
	@"disablecall.png",
	@"disablemessage.png",
	@"disable save.png",
	@"delete disable.png",
	@"download disable.png"
};



@implementation toolbarView

NSString *kTrackKey = @"whichTracker";

- (id)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame];
	if (self) {
		// Initialization code here.
		
	}
	
	return self;
}

- (void)dealloc
{
	[trackingAreas release];
	
	[super dealloc];
}

@synthesize trackingAreas;

- (void)drawRect:(NSRect)dirtyRect
{
	// Drawing code here.
	
}

#pragma mark--
#pragma mark tracking operation
#pragma mark--

- (void)setupTrackingAreas
{
	self.trackingAreas = [NSMutableArray array];
	
	NSTrackingAreaOptions trackingOptions = NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways | NSTrackingActiveInActiveApp;
	
	NSView *pView = nil;
	
	
	NSUInteger count = [[self content] count];
	NSInteger tag = 0;
	
	for(NSUInteger index = 0; index < count; index++)
	{
		iconCollectionItem *item = (iconCollectionItem*)[self itemAtIndex:index];
		pView = [item view];
		
		NSDictionary *trackerData = [NSDictionary dictionaryWithObjectsAndKeys:
									 [NSNumber numberWithInteger:tag++], kTrackKey,
									 nil];
		
		NSRect rect = [pView frame];
		rect.origin.x += rect.size.width * index;
		
		
		
		NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:rect options:trackingOptions owner:self userInfo:trackerData];
		
		[self.trackingAreas addObject:trackingArea];
		
		[self addTrackingArea:trackingArea];
		
		[trackingArea release];
	}
}

- (void)removeTrackingAreas
{
	NSTrackingArea *trackingArea;
	
	for(trackingArea in self.trackingAreas)
	{
		[self removeTrackingArea:trackingArea];
	}
}

- (void)viewDidMoveToWindow
{
	[super viewDidMoveToWindow];
	
	return;
	
	if([self window] != nil)
	{
		[self setupTrackingAreas];
	}
	else
	{
		[self removeTrackingAreas];
	}
}

- (int)getTrackIDFromDictionary:(NSDictionary *)dict
{
	id tracker = [dict objectForKey:kTrackKey];
	
	return [tracker intValue];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	[self updateToolbarImage:YES withEvent:theEvent];
	
	m_trackEntered = YES;
	
	[self setNeedsDisplay:YES];
}

- (void)mouseExited:(NSEvent *)theEvent
{
	[self updateToolbarImage:NO withEvent:theEvent];
	
	m_trackEntered = NO;
	
	[self setNeedsDisplay:YES];
}

- (void)updateToolbarImage:(BOOL)bEnter withEvent:(NSEvent*)theEvent
{
	return;
	
	m_trackID = [self getTrackIDFromDictionary:(NSDictionary*)[theEvent userData]];
	
	iconCollectionItem *item = (iconCollectionItem*)[self itemAtIndex:m_trackID];
	
	cellData *data = (cellData*)[item representedObject];
	
	if(data.bEnable == YES)
	{
		if(bEnter == YES)
		{
			if(data.appTag == SMS)
				data.image = [NSImage imageNamed:hotToolbarImages[m_trackID]];
			else if(data.appTag == PHONEBOOK)
				data.image = [NSImage imageNamed:pbHotToolbarImages[m_trackID]];
		}
		else
		{
			if(data.appTag == SMS)
				data.image = [NSImage imageNamed:enableToolbarImages[m_trackID]];
			else
				data.image = [NSImage imageNamed:pbEnableToolbarImages[m_trackID]];
		}
	}
}

@end
