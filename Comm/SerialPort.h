/*
 *  SerialPort.h
 *  DemoApp
 *
 *  Created by  mbj on 11-9-27.
 *  Copyright 2011 MediaTek Inc. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <paths.h>
#include <termios.h>
#include <sysexits.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <AvailabilityMacros.h>

/*#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>*/
#include <IOKit/serial/ioss.h>
#include <IOKit/IOBSD.h>

#include "Win2Mac.h"

typedef int (*DataNotify) (int status, const char* pTxt, unsigned long nLen, void *pData);

class CommModule 
{
private:
	int m_handle;
//	struct termios m_ttyAttrs;
	HANDLE m_hThread;
	unsigned int m_nThreadId;
	HANDLE m_hStopEvt;
	void *m_pData;
	DataNotify m_datacb;

public:
	CommModule();
	~CommModule();
	
	int Init_Port(const char *bsdPath);
	void Deinit_Port();
	
	//userflag:  0 means caller is at user,  1 means voice user
	size_t WriteData(const char *buf, const int &size, int userflag);
	void ClosePort();
	bool StartThread();
	bool Initernal_Start();
	void Initernal_Stop();
	void SetCallBack(DataNotify reply, void *pData);

	static DWORD __stdcall WatchProc(LPVOID param);
	DWORD ThreadFunc();
	
};

