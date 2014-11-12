//
//  staticsViewController.h
//  DCT
//
//  Created by Fengping Yu on 11/30/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//
#import <Cocoa/Cocoa.h>
#import <list>

@interface statisticDiagramView : NSView {
	int		m_BytesPerScale;
	NSRect	m_ViewRect;
}
@end

@interface staticsViewController : NSViewController<NSOutlineViewDelegate, NSOutlineViewDataSource>
{
@private
	IBOutlet statisticDiagramView*	diagramView;
	IBOutlet NSOutlineView*		recordDataView;
	NSMutableArray *dataSource;
	
	IBOutlet NSTextField*		uploadData;
	IBOutlet NSTextField*		uploadSpeed;
	IBOutlet NSTextField*		downloadData;
	IBOutlet NSTextField*		downloadSpeed;
	IBOutlet NSTextField*		duration;
}
@property (retain)IBOutlet statisticDiagramView*	diagramView;
@property (retain)IBOutlet NSTableView*		recordDataView;

- (NSString*) FormatData: (uint64)data 
				 ifSpeed: (BOOL)isSpeed;
- (NSString*) FormatDuration: (uint)dur;

@end
