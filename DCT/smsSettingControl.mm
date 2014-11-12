//
//  smsSettingControl.m
//  DCT
//
//  Created by Fengping Yu on 1/4/12.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "smsSettingControl.h"
#import "utility.h"
#import "DCTAppDelegate.h"

#pragma mark--
#pragma mark init, construct and destruct
#pragma mark--

@implementation smsSettingControl

- (id)init
{
	self = [super init];
	if (self) 
	{
		// Initialization code here.
		//  m_bPlay = NO;
		m_bShowNotification = YES;
		m_bPlaySound = YES;
		
		//m_sound = [NSSound alloc];
		
		//  [m_sound setDelegate:self];
	}
	
	return self;
}

- (void)dealloc
{
	//  [m_sound release];
	
	[super dealloc];
}

//@synthesize m_SoundPath;

- (void)awakeFromNib
{
	// [m_BrowseBtn setImage:[NSImage imageNamed:@"folder browser"]];
	//[m_BrowseBtn setToolTip:LocalizeString(@"IDS_BUTTON_BROWSER")];
	
	//   [m_PlayBtn setImage:[NSImage imageNamed:@"play"]];
	// [m_PlayBtn setToolTip:LocalizeString(@"IDS_BUTTON_PLAY")];
	
	// [m_PlayBtn setEnabled:NO];
	
	// set two check button state
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	NSNumber *number = (NSNumber*)[defaults valueForKey:SHOWNOTIFICATION];
	
	if(number == nil)
	{
		[m_NotifiBtn setState:NSOnState];
	}
	else
	{
		[m_NotifiBtn setState:[number integerValue]];
	}
	
	number = (NSNumber*)[defaults valueForKey:PLAYSOUND];
	
	if(number == nil)
	{
		[m_PlaySoundBtn setState:NSOnState];
	}
	else
	{
		[m_PlaySoundBtn setState:[number integerValue]];
	}
    number = (NSNumber*)[defaults valueForKey:SMSSAVE];
    
	if(number == nil)
        {
		
        [m_SaveMode setState:NSOnState];
        }
	else
        {
		[m_SaveMode setState:[number integerValue]];
        }

	/* NSString *strPath = [defaults valueForKey:SOUNDPATH];
	 
	 if(strPath != nil)
	 {
	 [m_SoundPath setStringValue:strPath];
	 [m_PlayBtn setEnabled:YES];
	 }
	 else
	 {
	 [m_SoundPath setStringValue:@""];
	 }*/
}

#pragma mark--
#pragma mark event response
#pragma mark--

/*- (IBAction)clickBrowseBtn:(id)sender
 {
 NSOpenPanel *panel = [NSOpenPanel openPanel];
 
 [panel setDirectory:NSHomeDirectory()];
 [panel setAllowsMultipleSelection:NO];
 [panel setAllowedFileTypes:[NSArray arrayWithObjects:@"mp3", @"wav", @"aiff", @"next snd", nil]];
 [panel setCanCreateDirectories:NO];
 [panel setCanChooseFiles:YES];
 
 [panel beginSheetModalForWindow:[[self view] window] completionHandler:(^(NSInteger result)
 {
 if(result == NSOKButton)
 {
 NSArray *filePaths = [panel URLs];
 NSURL* url = [filePaths objectAtIndex:0];
 NSString* path = [url path];
 
 [m_SoundPath setStringValue:path];
 
 [m_PlayBtn setEnabled:YES];
 }
 })];
 }
 
 - (IBAction)clickPlayBtn:(id)sender
 {
 if(m_bPlay == NO)
 {
 NSString *path = [m_SoundPath stringValue];
 
 NSURL *url = [NSURL fileURLWithPath:path];
 
 if(url == nil)
 {
 [utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:@"Cannot paly correctly." forType:MB_OK];
 }
 
 NSError *err;
 if([url checkResourceIsReachableAndReturnError:&err] == NO)
 {
 [[NSAlert alertWithError:err] runModal];
 return;
 }
 
 m_bPlay = YES;
 [m_PlayBtn setImage:[NSImage imageNamed:@"stop"]];
 [m_PlayBtn setToolTip:LocalizeString(@"IDS_BUTTON_STOP")];
 
 [m_sound initWithContentsOfURL:url byReference:NO];
 
 [m_sound play];
 }
 else
 {
 m_bPlay = NO;
 [m_PlayBtn setImage:[NSImage imageNamed:@"play"]];
 [m_PlayBtn setToolTip:LocalizeString(@"IDS_BUTTON_PLAY")];
 
 [m_sound stop];
 }
 }
 */
- (void)saveState
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	[defaults setValue:[NSNumber numberWithInteger:[m_NotifiBtn state]] forKey:SHOWNOTIFICATION];
	[defaults setValue:[NSNumber numberWithInteger:[m_PlaySoundBtn state]] forKey:PLAYSOUND];
	[defaults setValue:[NSNumber numberWithInteger:[m_SaveMode state]] forKey:SMSSAVE];
	//  [defaults setValue:[m_SoundPath stringValue] forKey:SOUNDPATH];
}

#pragma mark--
#pragma mark nssound and nswindow delegate
#pragma mark--
/*
 - (void)sound:(NSSound *)sound didFinishPlaying:(BOOL)aBool
 {
 if(aBool)
 {
 m_bPlay = NO;
 [m_PlayBtn setImage:[NSImage imageNamed:@"play"]];
 [m_PlayBtn setToolTip:LocalizeString(@"IDS_BUTTON_PLAY")];
 }
 }
 
 - (void)enablePlayBtn
 {
 if([[m_SoundPath stringValue] length] > 0)
 [m_PlayBtn setEnabled:YES];
 else
 [m_PlayBtn setEnabled:NO];
 }
 */
@end
