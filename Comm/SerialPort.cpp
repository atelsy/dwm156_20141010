/*
 *  SerialPort.c
 *  DemoApp
 *
 *  Created by  mbj on 11-9-27.
 *  Copyright 2011 MediaTek Inc. All rights reserved.
 *
 */

#include "SerialPort.h"
#include "Win2Mac.h"

CommModule::CommModule()
{
	m_handle = -1;
	m_nThreadId = 0;
	m_hThread = NULL;
	m_hStopEvt = NULL;
	m_datacb = NULL;
	m_pData = NULL;
}

CommModule::~CommModule()
{
	
}

void CommModule::SetCallBack(DataNotify reply, void *pData)
{
	m_datacb = reply;
	m_pData = pData;
   // DebugOutA("\r\nm_datacb = 0x%08x, m_pData = 0x%08x\r\n", reply, pData);
}

int CommModule::Init_Port(const char *bsdPath)
{
	int				handshake;
	struct termios	options;
//	speed_t speed = 115200; // Set 14400 baud
//	unsigned long mics = 1;

	// Open the serial port read/write, with no controlling terminal, and don't wait for a connection.
	// The O_NONBLOCK flag also causes subsequent I/O on the device to be non-blocking.
	// See open(2) ("man 2 open") for details.
	
	m_handle = open(bsdPath, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (m_handle == -1)
	{
		DebugOutA("\r\n[error] opening serial port %s - %s(%d).\n",
			   bsdPath, strerror(errno), errno);
		goto error;
	}
	
	DebugOutA("\r\ndescriptor is %d, bsd path-> %s\r\n",m_handle, bsdPath);

	// Note that open() follows POSIX semantics: multiple open() calls to the same file will succeed
	// unless the TIOCEXCL ioctl is issued. This will prevent additional opens except by root-owned
	// processes.
	// See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
	
	if (ioctl(m_handle, TIOCEXCL) == -1)
	{
		DebugOutA("\r\n[error] setting TIOCEXCL on %s - %s(%d).\r\n",
			   bsdPath, strerror(errno), errno);
		goto error;
	}
	
	// Now that the device is open, clear the O_NONBLOCK flag so subsequent I/O will block.
	// See fcntl(2) ("man 2 fcntl") for details.
  /*  
	if (fcntl(m_handle, F_SETFL, 0) == -1)
	{
		DebugOutA("\r\n[error] clearing O_NONBLOCK %s - %s(%d).\r\n",
			   bsdPath, strerror(errno), errno);
		goto error;
	}
 */   
	// Get the current options and save them so we can restore the default settings later.
	if (tcgetattr(m_handle, &options) == -1)
	{
		DebugOutA("\r\n[error] getting tty attributes %s - %s(%d).\r\n",
			   bsdPath, strerror(errno), errno);
		goto error;
	}
	
	// The serial port attributes such as timeouts and baud rate are set by modifying the termios
	// structure and then calling tcsetattr() to cause the changes to take effect. Note that the
	// changes will not become effective without the tcsetattr() call.
	// See tcsetattr(4) ("man 4 tcsetattr") for details.
	
//	options = m_ttyAttrs;
	
	// Print the current input and output baud rates.
	// See tcsetattr(4) ("man 4 tcsetattr") for details.
	
	DebugOutA("\r\nCurrent input baud rate is %d\r\n", (int) cfgetispeed(&options));
	DebugOutA("\r\nCurrent output baud rate is %d\r\n", (int) cfgetospeed(&options));
	
	// Set raw input (non-canonical) mode, with reads blocking until either a single character 
	// has been received or a one second timeout expires.
	// See tcsetattr(4) ("man 4 tcsetattr") and termios(4) ("man 4 termios") for details.
	
	cfmakeraw(&options);
	options.c_cc[VMIN] = 1;
	options.c_cc[VTIME] = 0;// 10;
	
	// The baud rate, word length, and handshake options can be set as follows:
	
	cfsetspeed(&options, B115200);		// Set 19200 baud	
	options.c_cflag |= (CS7 	   | 	// Use 7 bit words
						PARENB	   | 	// Parity enable (even parity if PARODD not also set)
						CCTS_OFLOW | 	// CTS flow control of output
						CRTS_IFLOW);	// RTS flow control of input
/*	
#if defined(MAC_OS_X_VERSION_10_4) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4)
	// Starting with Tiger, the IOSSIOSPEED ioctl can be used to set arbitrary baud rates
	// other than those specified by POSIX. The driver for the underlying serial hardware
	// ultimately determines which baud rates can be used. This ioctl sets both the input
	// and output speed. 
	
	if (ioctl(m_handle, IOSSIOSPEED, &speed) == -1)
	{
		DebugOutA("\r\n[error] calling ioctl(..., IOSSIOSPEED, ...) %s - %s(%d).\r\n",
			   bsdPath, strerror(errno), errno);
	}
#endif
  */  
	// Print the new input and output baud rates. Note that the IOSSIOSPEED ioctl interacts with the serial driver 
	// directly bypassing the termios struct. This means that the following two calls will not be able to read
	// the current baud rate if the IOSSIOSPEED ioctl was used but will instead return the speed set by the last call
	// to cfsetspeed.
	
	DebugOutA("Input baud rate changed to %d\n", (int) cfgetispeed(&options));
	DebugOutA("Output baud rate changed to %d\n", (int) cfgetospeed(&options));
	
	// Cause the new options to take effect immediately.
	if (tcsetattr(m_handle, TCSANOW, &options) == -1)
	{
		DebugOutA("[error] setting tty attributes %s - %s(%d).\r\n",
			   bsdPath, strerror(errno), errno);
		goto error;
	}
	
	// To set the modem handshake lines, use the following ioctls.
	// See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
	
	if (ioctl(m_handle, TIOCSDTR) == -1) // Assert Data Terminal Ready (DTR)
	{
		DebugOutA("[error] asserting DTR %s - %s(%d).\r\n",
			   bsdPath, strerror(errno), errno);
	}
	
	if (ioctl(m_handle, TIOCCDTR) == -1) // Clear Data Terminal Ready (DTR)
	{
		DebugOutA("[error] clearing DTR %s - %s(%d).\r\n",
			   bsdPath, strerror(errno), errno);
	}
	
	handshake = TIOCM_DTR | TIOCM_RTS | TIOCM_CTS | TIOCM_DSR;
	if (ioctl(m_handle, TIOCMSET, &handshake) == -1)
		// Set the modem lines depending on the bits set in handshake
	{
		DebugOutA("\r\[error] setting handshake lines %s - %s(%d).\r\n",
			   bsdPath, strerror(errno), errno);
	}
	
	// To read the state of the modem lines, use the following ioctl.
	// See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
	
	if (ioctl(m_handle, TIOCMGET, &handshake) == -1)
		// Store the state of the modem lines in handshake
	{
		DebugOutA("\r\n[error] getting handshake lines %s - %s(%d).\r\n",
			   bsdPath, strerror(errno), errno);
	}
	
	DebugOutA("\r\nHandshake lines currently set to %d\r\n", handshake);
/*	
#if defined(MAC_OS_X_VERSION_10_3) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3)

	
	// Set the receive latency in microseconds. Serial drivers use this value to determine how often to
	// dequeue characters received by the hardware. Most applications don't need to set this value: if an
	// app reads lines of characters, the app can't do anything until the line termination character has been
	// received anyway. The most common applications which are sensitive to read latency are MIDI and IrDA
	// applications.
	
	if (ioctl(m_handle, IOSSDATALAT, &mics) == -1)
	{
		// set latency to 1 microsecond
		DebugOutA("\r\n[error] setting read latency %s - %s(%d).\r\n",
			   bsdPath, strerror(errno), errno);
		goto error;
	}
#endif
*/	
	if(Initernal_Start() == false)
		goto error;
	
	// Success
	return m_handle;
	
	// Failure path
error:
	if (m_handle != -1)
	{
		close(m_handle);
		m_handle = -1;
	}
	
	return -1;
}

void CommModule::Deinit_Port()
{
	Initernal_Stop();
	ClosePort();
}

void CommModule::ClosePort()
{
	// Block until all written output has been sent from the device.
	// Note that this call is simply passed on to the serial device driver. 
	// See tcsendbreak(3) ("man 3 tcsendbreak") for details.
 /* if (tcdrain(m_handle) == -1)
	{
		DebugOutA("\r\n[error] waiting for drain - %s(%d).\r\n",
			   strerror(errno), errno);
	}*/
    
//	usleep(100);
	
	// Traditionally it is good practice to reset a serial port back to
	// the state in which you found it. This is why the original termios struct
	// was saved.
/*	if (tcsetattr(m_handle, TCSANOW, &m_ttyAttrs) == -1)
	{
		DebugOutA("\r\n[error] resetting tty attributes - %s(%d).\r\n",
			   strerror(errno), errno);
	}
*/
	close(m_handle);
	m_handle = -1;
}

size_t CommModule::WriteData(const char *buf, const int &size, int userflag)
{
	int nRetryCount = 0;
	size_t nSentBytes = 0;
	size_t wbytes = 0;
	size_t nPerSend = 0;
	
	if(userflag==1) //for test voice
	{
		//DebugOut(_T("aa WriteData() begin.\n"));
	}
	
	while(nSentBytes < size)
	{
		wbytes = 0;
		nPerSend = size - nSentBytes;
		wbytes = write(m_handle, buf+nSentBytes, nPerSend);
		
		if(userflag==1) //for test voice
		{
			//DebugOut(_T("aa m_handle=%d nSentBytes=%d wbytes=%d size=%d.\n"), m_handle, nSentBytes, wbytes, size);
		}
		
		if(wbytes == -1)
		{
			DebugOutA("\r\nError writting to device - %s(%d).\r\n", strerror(errno), errno);
		}
		else if(wbytes < nPerSend) 
		{
			nSentBytes += wbytes;
		}
		else 
		{
			nSentBytes += wbytes;
			nRetryCount =0;
			continue;
		}
		
		if(8 <= nRetryCount)
		{
			DebugOutA("\r\n[Error] write failed\r\n");
			
			if( userflag==1 && (nSentBytes < size) )
			{
				DebugOutA("\r\n[Error] data lost\r\n");
			}
			
			return false;
		}
		
		nRetryCount ++;
		//Sleep(100);
	}
	
	if(userflag==0)
	{
		DebugOutA("\r\n[txd] %s\r\n", buf);
	}
	
	return true;
}

bool CommModule::Initernal_Start()
{
	m_hStopEvt = CreateEvent(NULL, true, false, NULL);
	
	if(!StartThread())
	{
		CloseHandle(m_hStopEvt);
		m_hStopEvt = NULL;
		return false;
	}
	return true;
}

void CommModule::Initernal_Stop()
{
	SetEvent(m_hStopEvt);
	
	if(m_hThread != NULL)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	
	CloseHandle(m_hStopEvt);
	m_hStopEvt = NULL;
}

bool CommModule::StartThread()
{
	m_hThread = _beginthreadex(NULL, 0, WatchProc, this, 0, &m_nThreadId);

	if(m_hThread != NULL)
		return true;
	
	return false;
}

DWORD CommModule::WatchProc(LPVOID param)
{
	CommModule *pto = (CommModule*)param;
	if(pto != NULL)
		return pto->ThreadFunc();
	
	return false;
}

DWORD CommModule::ThreadFunc()
{
	ssize_t numBytes = 0;
	char buf[512];

	while(1)
	{
		if(WaitForSingleObject(m_hStopEvt, 0) == WAIT_OBJECT_0)
			break;
	//	DebugOutA("begin to read\r\n");
		numBytes = read(m_handle, buf, 512-1);
	 //   DebugOutA("read %d bytes data\r\n", numBytes);
		if (numBytes <= 0)
		{
			if(errno == EAGAIN)
			{
				usleep(100);
				continue;
			}
			DebugOutA("\r\n[error]reading com port - %s(%d).\n", strerror(errno), errno);
			return 0;
		}
		else if (numBytes > 0)
		{
#ifdef _ENABLE_LOG_
			buf[numBytes] = 0;
		//	DebugOutA("\r\n[rxd] %s\r\n", buf);
#endif	// _ENABLE_LOG_
		
			if(m_datacb)
				m_datacb(true, (const char*)buf, numBytes, m_pData);
		}
	}
	
	return 1;
}