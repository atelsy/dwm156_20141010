//
//  VoiceMgr.h
//  VoiceMgr
//
//  Created by mtk on 12-6-6.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#ifndef _VOICEMGR_H_
#define _VOICEMGR_H_
//#import <AudioToolbox/AudioToolbox.h>
#import <AudioToolbox/AudioQueue.h>
#import <AudioToolbox/AudioFile.h>
#import <AudioToolbox/AudioFormat.h>
#include "Connection.h"
#include <list>

#define BUFNUM			5

typedef std::list<char*>	DATAQUE;

void checkError(OSStatus error, const char* operation);
int SpeechReceiver(int status, const char* pTxt, unsigned long nLen, void* pData);
void OutputCallback(void *inUserData,
					AudioQueueRef inAQ,
					AudioQueueBufferRef inBuffer);

void InputCallback(void *inUserData,
				   AudioQueueRef inAQ,
				   AudioQueueBufferRef inBuffer,
				   const AudioTimeStamp *inStartTime,
				   UInt32 inNumberPacketDescriptions,
				   const AudioStreamPacketDescription *inPacketDescs);
void MyAudioQueuePropertyListenerProc (void *			  inUserData,
									   AudioQueueRef		   inAQ,
									   AudioQueuePropertyID	inID);


@interface VoiceMgr : NSObject
{
	BOOL			m_bSaveWav;
	unsigned long	m_BufferSize;
	AudioStreamBasicDescription m_Format;
	AudioQueueRef	m_CaptureQueue;
	AudioQueueRef	m_PlayQueue;
//	NSThread*		m_CaptureThreadHandler;
//	NSThread*		m_PlayThreadHandler;
	SInt64			m_PlayPacket;
	DATAQUE			m_RecvQueue;
	HANDLE			m_hPlayMutex;
	char*			m_pRecvBuff;
	unsigned long	m_RecvBuffCursor;
	NSLock			*m_lock;
	Connection		m_Conn;
	AudioQueueBufferRef m_PlayBuffer[BUFNUM];

@public
	SInt64			m_RecordPacket;	

	AudioFileID		m_PlayFile;
	AudioFileID		m_RecordFile;
	SInt64			m_begin;
};
	
- (BOOL) Prepare;
- (BOOL) InitCapture;
- (BOOL) InitPlay;
- (void) Free;
- (void) FreeCapture;
- (void) FreePlay;
- (void) SetWaveFormat;
- (int)  Interval2Bytes: (Float64)interval;
- (BOOL) StartCapture;
- (BOOL) StopCapture;
- (BOOL) StartPlay;
- (BOOL) StopPlay;
//- (void) CaptureThread: (id)param;
//- (void) PlayThread: (id)param;
- (BOOL) PushSoundData: (const char *)pData
				Length: (unsigned long) nLen;
- (void) outputQueue: (AudioQueueRef)inAQ
		  withBuffer: (AudioQueueBufferRef)inBuffer;
- (void)inputQueue:(AudioQueueRef)inAQ
		withBuffer:(AudioQueueBufferRef)inBuffer;
- (BOOL) AttachPort: (char*)path;
- (BOOL) ReleasePort;

@end

#endif