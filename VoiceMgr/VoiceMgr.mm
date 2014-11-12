//
//  VoiceMgr.m
//  VoiceMgr
//
//  Created by mtk on 12-6-6.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//
#include "VoiceMgr.h"

#define _ENABLE_LOG_

#define MAXDATAPACK		60
#define LOCKPLAY()		/*[m_lock lock]	*/WaitForSingleObject(m_hPlayMutex, INFINITE)
#define UNLOCKPLAY()	/*[m_lock unlock]	*/			ReleaseMutex(m_hPlayMutex)

UInt32 gIsRunning = 0;

#pragma mark - play 

void checkError(OSStatus error, const char* operation)
{
	if(error == noErr)
		return;
	
	char errorString[20];
	
	*(UInt32*)(errorString + 1) = CFSwapInt32HostToBig(error);
	
	if(isprint(errorString[1]) && isprint(errorString[2]) && isprint(errorString[3]) && isprint(errorString[4]))
	{
		errorString[0] = errorString[5] = '\'';
		errorString[6] = '\0';
	}
	else
	{
		sprintf(errorString, "%d", (int)error);
		
		fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
	}
}

int SpeechReceiver(int status, const char* pTxt, unsigned long nLen, void* pData)
{
	VoiceMgr* pThis = (VoiceMgr*)pData;
	if (pThis)
		[pThis PushSoundData: pTxt
					  Length: nLen];
	return 0;
}

void OutputCallback(void *inUserData,
					AudioQueueRef inAQ,
					AudioQueueBufferRef inBuffer)
{
	VoiceMgr* pThis = (VoiceMgr*)inUserData;
	if (pThis)
		[pThis outputQueue: inAQ
				withBuffer: inBuffer];
	AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);	
}

#pragma mark - record

void InputCallback(void *									inUserData,
					AudioQueueRef							inAQ,
					AudioQueueBufferRef						inBuffer,
					const AudioTimeStamp *					inStartTime,
					UInt32									inNumberPacketDescriptions,
					const AudioStreamPacketDescription *	inPacketDescs)
{
	VoiceMgr* pThis = (VoiceMgr*)inUserData;
	
//#ifdef DEBUG
	if(inNumberPacketDescriptions > 0)
	{
		OSStatus status = AudioFileWritePackets(pThis->m_RecordFile, false, inBuffer->mAudioDataByteSize, inPacketDescs, pThis->m_RecordPacket, &inNumberPacketDescriptions, inBuffer->mAudioData);
		
		if(status == noErr)
			pThis->m_RecordPacket += inNumberPacketDescriptions;
	}
//#endif
	
	if (pThis)
	{
	   [pThis inputQueue:inAQ withBuffer:inBuffer];
	}
		
	checkError(AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL), "AudioqueueenqueueBuffer failed.");
}

void MyAudioQueuePropertyListenerProc (void *  inUserData,
									  AudioQueueRef   inAQ,
									  AudioQueuePropertyID	inID)
{
	UInt32 size = sizeof(gIsRunning);
	OSStatus err = AudioQueueGetProperty (inAQ, kAudioQueueProperty_IsRunning, &gIsRunning, &size);
	if (err) 
		gIsRunning = 0;
}

@implementation VoiceMgr

- (id)init
{
	self = [super init];
	
	if (self)
	{
		m_BufferSize = 0;
		m_pRecvBuff = NULL;
		m_RecvBuffCursor = 0;
		m_hPlayMutex = NULL;
		m_CaptureQueue = m_PlayQueue = nil;
	//	/*m_CaptureThreadHandler =*/ m_PlayThreadHandler = nil;
		m_RecordPacket = m_PlayPacket = 0;
		
		NSDictionary *dict = [[NSBundle mainBundle] infoDictionary];
		NSString *info = [dict objectForKey:@"SaveAudioFile"];
		
		if([info isEqualToString:@"YES"])
			m_bSaveWav = YES;
		else
			m_bSaveWav = NO;

		m_begin = 0;

		m_lock = [[NSLock alloc] init];
	}
	return self;
}

#pragma mark - dealloc method

- (void) dealloc
{
	[self Free];
	[m_lock release];
	
	[super dealloc];
}

- (void) Free
{
	[self FreeCapture];
	[self FreePlay];
}

- (void) FreeCapture
{
	[self StopCapture];
	
	NSLog(@"begsin dispose capture queue.\n");
	
	if (m_CaptureQueue)
	{
		AudioQueueDispose(m_CaptureQueue, TRUE);
		m_CaptureQueue = nil;
	}
	/*
	if (m_CaptureThreadHandler)
	{
		while ([m_CaptureThreadHandler isExecuting])
		{
			NSLog(@"loop in capture thread \n");
			usleep(1);
		}
		
		NSLog(@"finish loop capture.\n");
		
		m_CaptureThreadHandler = nil;
	}
	*/
	NSLog(@"free capture sucessful.\n");
}

- (void) FreePlay
{
	[self StopPlay];
	
	NSLog(@"dispose queue.\n");
	
	if (m_PlayQueue)
	{
		AudioQueueDispose(m_PlayQueue, TRUE);
		m_PlayQueue = nil;
	}
	
	NSLog(@"finish dispose queue.\n");
	
/*	if (m_PlayThreadHandler)
	{
		while ([m_PlayThreadHandler isExecuting])
		{
			NSLog(@"loop\n");
			usleep(1);
		}
		
		m_PlayThreadHandler = nil;
	}
*/	
	NSLog(@"close playthread handler.\n");
	
	DATAQUE::iterator dp;
	if (m_hPlayMutex)
	{
		LOCKPLAY();
		if (!m_RecvQueue.empty())
		{
			for (dp = m_RecvQueue.begin(); dp != m_RecvQueue.end(); dp ++)
				delete[] (*dp);
			m_RecvQueue.clear();
		}
		UNLOCKPLAY();
		
		NSLog(@"clear queue.\n");
		
		CloseHandle(m_hPlayMutex);
		m_hPlayMutex = NULL;
	}
}

#pragma mark - set init state method

- (BOOL) Prepare
{
	[self SetWaveFormat];
	
	m_BufferSize = [self Interval2Bytes: 0.08];
	if (m_pRecvBuff)
		delete []m_pRecvBuff;
	m_pRecvBuff = new char[m_BufferSize];
	
	NSDictionary *info = [[NSBundle mainBundle] infoDictionary];
	
	if([[info objectForKey:@"kVoiceRecord"] caseInsensitiveCompare:@"yes"] == NSOrderedSame)
	{
		if (![self InitCapture])
			DebugOut(_T("[VoiceMgr]: init capture device fail.\n"));
	}
	
	if([[info objectForKey:@"kVoicePlay"] caseInsensitiveCompare:@"yes"] == NSOrderedSame)
	{
		if (![self InitPlay])
			return FALSE;
	}
	
	return TRUE;
}

- (void) SetWaveFormat
{
	memset(&m_Format, 0, sizeof(m_Format));
	
	m_Format.mSampleRate = 16000.;
	m_Format.mChannelsPerFrame = 1;
	// default to PCM, 16 bit int
	m_Format.mFormatID = kAudioFormatLinearPCM;
	m_Format.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
	m_Format.mBitsPerChannel = 16;
	m_Format.mBytesPerPacket = m_Format.mBytesPerFrame =
	(m_Format.mBitsPerChannel / 8) * m_Format.mChannelsPerFrame;
	m_Format.mFramesPerPacket = 1;
}

- (int) Interval2Bytes: (Float64)interval
{
	int frames = (int)ceil(interval * m_Format.mSampleRate);
	return frames * m_Format.mBytesPerFrame;
}

- (BOOL) AttachPort: (char*)path
{
	DebugOut(_T("[VoiceMgr]: AttachPort: %s.\n"), path);
	return m_Conn.Init_Voice(path, SpeechReceiver, (char*)self);
}

- (BOOL) ReleasePort
{
	DebugOut(_T("[VoiceMgr]: ReleasePort.\n"));
	return m_Conn.DeInit_Voice();
}

#pragma mark - capture objective method

- (BOOL) InitCapture
{
	[self FreeCapture];
	/*m_CaptureThreadHandler = [[NSThread alloc] initWithTarget: self 
													 selector: @selector(CaptureThread:) 
													   object: nil];
	[m_CaptureThreadHandler start];
	
	while (!m_CaptureQueue)
		usleep(1);
	*/
	
	//NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	OSStatus status = AudioQueueNewInput(&m_Format,
										 InputCallback,
										 self,
										 CFRunLoopGetCurrent(), 
										 kCFRunLoopCommonModes,
										 0,
										 &m_CaptureQueue);
	m_RecordPacket = 0;
	
	if(status != noErr)
	{
		NSLog(@"create capture queue error.\n");
		return FALSE;
	}
	
	if(m_bSaveWav)
	{
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		
		NSString *recordFile = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"record.wav"];
		CFURLRef url =  CFURLCreateWithFileSystemPath(kCFAllocatorDefault, 
													  (CFStringRef)recordFile,
													  kCFURLPOSIXPathStyle, 
													  NO);   
		
		AudioFileCreateWithURL(url, kAudioFileCAFType, &m_Format, kAudioFileFlags_EraseFile, &m_RecordFile);
		//[self copyEncoderCookie:m_CaptureQueue toFile:m_RecordFile];
		CFRelease(url);
	}
	
	
	for (int i = 0; i < BUFNUM; ++i)
	{
		AudioQueueBufferRef buffer;
		checkError(AudioQueueAllocateBuffer(m_CaptureQueue, 
											(UInt32)m_BufferSize,
											&buffer), "AudioQueueAllocateBuffer failed.");
		
		checkError(AudioQueueEnqueueBuffer(m_CaptureQueue, 
										   buffer, 
										   0,
										   NULL), "AudioQueueEnqueueBuffer failed.");
	}
	
	return TRUE;
}

/*
- (void) CaptureThread : (id) param 
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	OSStatus status = AudioQueueNewInput(&m_Format,
					   InputCallback,
					   self,
					   CFRunLoopGetCurrent(), 
					   kCFRunLoopCommonModes,
					   0,
					   &m_CaptureQueue);
	m_RecordPacket = 0;
	
	if(status != noErr)
	{
		NSLog(@"create capture queue error.\n");
	}
	

	if(m_bSaveWav)
	{
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		
		NSString *recordFile = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"record.wav"];
		CFURLRef url =  CFURLCreateWithFileSystemPath(kCFAllocatorDefault, 
													  (CFStringRef)recordFile,
													  kCFURLPOSIXPathStyle, 
													  NO);   
		
		AudioFileCreateWithURL(url, kAudioFileCAFType, &m_Format, kAudioFileFlags_EraseFile, &m_RecordFile);
		//[self copyEncoderCookie:m_CaptureQueue toFile:m_RecordFile];
		CFRelease(url);
	}
	
	CFRunLoopRun();
	DebugOut(_T("capture thread exit.\n"));
	[pool release];
}
*/

- (void)inputQueue:(AudioQueueRef)inAQ withBuffer:(AudioQueueBufferRef)inBuffer
{
	if(inAQ != m_CaptureQueue)
		return;
	
	m_Conn.SpeechSender((const char*)inBuffer->mAudioData, inBuffer->mAudioDataByteSize);
}

- (BOOL) StartCapture
{
	NSLog(@"\nStartCapture() called\n");
	
	if (!m_CaptureQueue)
		return FALSE;
	
	checkError(AudioQueueStart(m_CaptureQueue, NULL), "AudioQueueStart failed.");
		
	NSLog(@"\nStartCapture() success\n");
	
	return TRUE;
}

- (BOOL) StopCapture
{
	NSLog(@"Before stop capture.\n");
	
	if (!m_CaptureQueue)
	{
		NSLog(@"capture queue = nil.\n");
		
		return FALSE;
	}
	
	NSDictionary *info = [[NSBundle mainBundle] infoDictionary];
	bool stopImmediately;
	
	if([[info objectForKey:@"kStopImmediately"] caseInsensitiveCompare:@"yes"] == NSOrderedSame)
		stopImmediately = true;
	else
		stopImmediately = false;
	
 	checkError(AudioQueueStop(m_CaptureQueue, stopImmediately), "AudioQueueStop failed.");
	
	NSLog(@"stop queue sucessful.\n");
	
    if(m_bSaveWav)
		AudioFileClose(m_RecordFile);
	
	NSLog(@"finish stop capture.\n");
	
	return TRUE;
}

#pragma mark - play object method

- (BOOL) InitPlay
{
	[self FreePlay];
	m_hPlayMutex = CreateMutex(NULL, FALSE, NULL);
//	m_PlayThreadHandler = [[NSThread alloc] initWithTarget: self 
//													 selector: @selector(PlayThread:) 
//													   object: nil];
//	[m_PlayThreadHandler start];
	
//	while (!m_PlayQueue)
//		usleep(1);
	
	AudioQueueNewOutput(&m_Format,
						OutputCallback,
						self, 
						CFRunLoopGetCurrent(),
						kCFRunLoopCommonModes,
						0,
						&m_PlayQueue);
	
	if(m_bSaveWav)
	{
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		
		NSString *recordFile = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"play.wav"];
		CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
													 (CFStringRef)recordFile,
													 kCFURLPOSIXPathStyle,
													 NO);
		AudioFileCreateWithURL(url, kAudioFileWAVEType, &m_Format, kAudioFileFlags_EraseFile, &m_PlayFile);
		CFRelease(url);
	}
	
	
	for (int i = 0; i < BUFNUM; ++i) 
	{
		//AudioQueueBufferRef buffer;
		AudioQueueAllocateBuffer(m_PlayQueue, (UInt32)m_BufferSize, &m_PlayBuffer[i]);
		m_PlayBuffer[i]->mAudioDataByteSize = (UInt32)m_BufferSize;
		memset(m_PlayBuffer[i]->mAudioData, 0, m_BufferSize);
		AudioQueueEnqueueBuffer(m_PlayQueue, m_PlayBuffer[i], 0, NULL);
	}
	
	AudioQueueSetParameter(m_PlayQueue, kAudioQueueParam_Volume, 1);
	
	/*AudioQueueAddPropertyListener(m_PlayQueue,
								  kAudioQueueProperty_CurrentDevice, 
								  MyAudioQueuePropertyListenerProc, 
								  NULL);
	*/
	return TRUE;
}
/*
- (void) PlayThread : (id) param 
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	AudioQueueNewOutput(&m_Format,
						OutputCallback,
						self, 
						CFRunLoopGetCurrent(),
						kCFRunLoopCommonModes,
						0,
						&m_PlayQueue);
	
	if(m_bSaveWav)
	{
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		
		NSString *recordFile = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"play.wav"];
		CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
													 (CFStringRef)recordFile,
													 kCFURLPOSIXPathStyle,
													 NO);
		AudioFileCreateWithURL(url, kAudioFileWAVEType, &m_Format, kAudioFileFlags_EraseFile, &m_PlayFile);
		CFRelease(url);
	}

 
	CFRunLoopRun();

	[pool release];
}
*/
- (BOOL) StartPlay
{
	NSLog(@"\nStartPlay() called\n");
	
	if (!m_PlayQueue)
	{
		NSLog(@"\nStartPlay(): m_PlayQueue==NULL\n");
		return FALSE;
	}
	
	AudioQueueStart(m_PlayQueue, NULL);
	
	NSLog(@"\nStartPlay() success\n");
	
	return TRUE;
}

- (BOOL) StopPlay
{
	NSLog(@"begin stop play.\n");
	
	if (!m_PlayQueue)
	{
		NSLog(@"play queue = nil\n");
		return FALSE;
	}
	
	NSLog(@"begin stop queue.\n");
	
	NSDictionary *info = [[NSBundle mainBundle] infoDictionary];
	bool stopImmediately;
	
	if([[info objectForKey:@"kStopImmediately"] caseInsensitiveCompare:@"yes"] == NSOrderedSame)
		stopImmediately = true;
	else
		stopImmediately = false;

	
	AudioQueueStop(m_PlayQueue, stopImmediately);
	
	NSLog(@"Audioqueuestop sucessful.\n");
	
	LOCKPLAY();
	if (!m_RecvQueue.empty())
	{
		DATAQUE::iterator dp;
		
		for (dp = m_RecvQueue.begin(); dp != m_RecvQueue.end(); dp ++)
			delete[] (*dp);
		m_RecvQueue.clear();
        m_RecvBuffCursor = 0;
	}
	UNLOCKPLAY();

	NSLog(@"finish empty queue buffer.\n");
	
    if(m_bSaveWav)
		AudioFileClose(m_PlayFile);
	
	NSLog(@"finish stop play.\n");
 
	return TRUE;
}

- (BOOL) PushSoundData: (const char *)pData
				Length: (unsigned long) nLen
{
	if (!pData || !nLen)
		return FALSE;
	
	if(m_bSaveWav)
	{
		UInt32 numBytes = (UInt32)nLen;
		
		OSStatus status = AudioFileWriteBytes(m_PlayFile, false, m_begin, &numBytes, (const void*)pData);
		if(status == noErr)
			m_begin += nLen;
	}
	
	unsigned long nRead = 0;
	
	while (nRead < nLen)
	{
		unsigned long nLeft = m_BufferSize - m_RecvBuffCursor;
		if (nLeft <= nLen-nRead)
		{
			memcpy(m_pRecvBuff+m_RecvBuffCursor, pData+nRead, nLeft);
			
			char* ptr = new char[m_BufferSize];
			memcpy(ptr, m_pRecvBuff, m_BufferSize);
			
			LOCKPLAY();
			//NSLog(@"receive serial port data.\n");
			m_RecvQueue.push_back(ptr);
			UNLOCKPLAY();
			
			nRead += nLeft;
			m_RecvBuffCursor = 0;
		}
		else
		{
			unsigned long toWrite = nLen-nRead;
		//	DebugOut(_T("RecvBuffCursor: %d, to write: %d\n"), m_RecvBuffCursor, toWrite);
			memcpy(m_pRecvBuff+m_RecvBuffCursor, pData+nRead, toWrite);
			nRead += toWrite;
			m_RecvBuffCursor += toWrite;
		}
	}
	//	prevent full
	LOCKPLAY();
	if (m_RecvQueue.size() > MAXDATAPACK)
	{
		DATAQUE::iterator dp;
		
		for (dp = m_RecvQueue.begin(); dp != m_RecvQueue.end(); dp ++)
			delete[] (*dp);
		m_RecvQueue.clear();
		
		NSLog(@"the queue is full, remove the data.\n");
	}
	UNLOCKPLAY();
	
	return TRUE;
	
}

- (void) outputQueue: (AudioQueueRef)inAQ
		  withBuffer: (AudioQueueBufferRef)inBuffer;
{
	if (inAQ != m_PlayQueue)
		return;

	//	got the head packet from our queue
	char* dp = NULL;
	
	LOCKPLAY();
	if (!m_RecvQueue.empty())
	{
		//DebugOut(_T("enter output queue.\n"));
		
		dp = m_RecvQueue.front();
		m_RecvQueue.pop_front();
	}
	UNLOCKPLAY();
	
	//	copy to inBuffer
	if (dp)
	{
		memcpy(inBuffer->mAudioData, dp, m_BufferSize);
        
        //add by CS
        delete dp;
        dp=NULL;
	}  
	else
	{
		memset(inBuffer->mAudioData, 0, m_BufferSize);
	}
}

@end
