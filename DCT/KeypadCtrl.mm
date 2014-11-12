//
//  KeypadCtrl.m
//  CocoaTry
//
//  Created by MBJ on 12-5-30.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "KeypadCtrl.h"
#import "DCTAppDelegate.h"


@implementation KeypadCtrl


- (id)init
{
	if (self = [super init])
	{
		numSelRange.location = 0;
		numSelRange.length = 0;
		
		oldSelRange.location = 0;
		oldSelRange.length = 0;
		
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(onSelChanged:)
													 name:NSTextViewDidChangeSelectionNotification
												   object:[numEdit currentEditor]];
	}
	
	return self;
}

-(IBAction)keyPressed:(id)sender
{
	if ([sender isKindOfClass:[NSButton class]])
	{
		[self setFocus];
		
		NSButton *btn = (NSButton*)sender;
		
		NSString *btnTxt = [btn title];
		
		NSEvent *event = [NSEvent keyEventWithType:NSKeyDown
										  location:NSZeroPoint
									 modifierFlags:NSFunctionKeyMask
										 timestamp:0
									  windowNumber:0
										   context:nil
										characters:btnTxt
					   charactersIgnoringModifiers:nil
										 isARepeat:NO
										   keyCode:0 ];
		
		
		const void *evtRef = [event eventRef];
		
		[[numEdit currentEditor] keyDown:[NSEvent eventWithEventRef:evtRef]];
		
	//	[theClient onKeyPressed:[btnTxt characterAtIndex:0] 
	//				 withString:[numEdit stringValue]];
	}	
}

-(IBAction)delPressed:(id)sender
{
	unichar delChar = 0x7f;

	NSString *delStr = [NSString stringWithCharacters:&delChar 
											   length:1];
	
	NSEvent *event = [NSEvent keyEventWithType:NSKeyDown
									  location:NSZeroPoint
								 modifierFlags:0
									 timestamp:0
								  windowNumber:0
									   context:nil
									characters:delStr
				   charactersIgnoringModifiers:nil
									 isARepeat:NO
									   keyCode:delChar ];
	
	
	const void *evtRef = [event eventRef];
	
	[[numEdit currentEditor] keyDown:[NSEvent eventWithEventRef:evtRef]];

	[theClient onKeyPressed:KEY_CODE_DEL
				 withString:[numEdit stringValue]];
}

-(IBAction)onSend:(id)sender
{
	[theClient onKeyPressed:KEY_CODE_SEND
				 withString:[numEdit stringValue]];	
}

-(IBAction)onEnd:(id)sender
{
	[theClient onKeyPressed:KEY_CODE_END
				 withString:[numEdit stringValue]];
}

-(IBAction)onFriend:(id)sender
{
	[theClient onKeyPressed:KEY_CODE_FRIEND
				 withString:[numEdit stringValue]];
}

-(void)setText:(NSString*)str
{
	[numEdit setStringValue:str];
	[self selectAll];
}

-(void)setFocus
{
	NSRange range = numSelRange;
	[numEdit selectText:self];
	[[numEdit currentEditor] setSelectedRange:range];
//	[TheDelegate.window makeFirstResponder:numEdit];
}

-(void)enable:(NSUInteger)mask
{
	[btnSend setEnabled:(0!=(mask & KEY_MASK_SEND))];
	[btnFriend setEnabled:(0!=(mask & KEY_MASK_FRIEND))];
	[numEdit setEnabled:(0!=(mask & KEY_MASK_EDIT))];
	[self setFocus];
}

-(void)selectAll
{
	[numEdit selectText:self];
}

-(void)clear
{
	[numEdit setStringValue:@""];
}

- (void)controlTextDidChange:(NSNotification *)obj
{
	if (numSelRange.location > oldSelRange.location)
	{
		NSString *str = [numEdit stringValue];
		
		NSCharacterSet *set = [NSCharacterSet characterSetWithCharactersInString:@"0123456789+*#pwPW"];
		
		NSMutableString *input = [[NSMutableString alloc] init];
		
		NSUInteger bad = 0;
		
		for (NSUInteger i=oldSelRange.location; i<numSelRange.location; ++i)
		{
			if ([set characterIsMember:[str characterAtIndex:i]])
			{
				[input appendFormat:@"%c", [str characterAtIndex:i]];
			}
			else
			{
				++ bad;
			}
		}
		
		if (bad > 0)
		{
			NSString *_str = [NSString stringWithFormat:@"%@%@%@",
							  [str substringToIndex:oldSelRange.location],
							  input,
							  [str substringFromIndex:numSelRange.location]];
			
			NSRange range = NSMakeRange(numSelRange.location-bad, 0);
			
			[numEdit setStringValue:_str];
			
			[[numEdit currentEditor] setSelectedRange:range];
		}
		
		if ([input length] > 0)
		{
			[theClient onKeyPressed:0
						 withString:input];
		}
	}
}
		 
- (void)onSelChanged:(NSNotification*)notification
{
	if ([TheDelegate.window firstResponder]==[numEdit currentEditor])
	{
		oldSelRange = numSelRange;
		numSelRange = [[numEdit currentEditor] selectedRange];
	}
}

-(IBAction)onEnter:(id)sender
{
	[theClient onKeyPressed:KEY_CODE_SEND
				 withString:[numEdit stringValue]];	
}

@end
