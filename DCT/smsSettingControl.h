//
//  smsSettingControl.h
//  DCT
//
//  Created by Fengping Yu on 1/4/12.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

#define  SHOWNOTIFICATION   @"showNotificationMsgArriving"
#define  PLAYSOUND  @"playSoundMsgArriving"
#define  SOUNDPATH  @"soundPathMsgArriving"
#define SMSSAVE  @"NewSMSSaveMode"

@interface smsSettingControl : NSViewController<NSSoundDelegate>
{
@private
	IBOutlet NSButton *m_NotifiBtn;
	IBOutlet NSButton *m_PlaySoundBtn;
    IBOutlet NSButton *m_SaveMode;
	// IBOutlet NSTextField *m_SoundPath;
	
	//IBOutlet NSButton *m_BrowseBtn;
	//IBOutlet NSButton *m_PlayBtn;
	
	//   BOOL m_bPlay;
	BOOL m_bShowNotification;
	BOOL m_bPlaySound;
	BOOL m_bSaveMode;

	// NSSound *m_sound;
}

//- (IBAction)clickBrowseBtn:(id)sender;
//- (IBAction)clickPlayBtn:(id)sender;

- (void)saveState;
//- (void)enablePlayBtn;

//@property(retain) NSTextField *m_SoundPath;

@end
