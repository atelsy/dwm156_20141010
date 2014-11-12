//
//  DialupMgr.h
//  DCT
//
//  Created by MBJ on 11-11-22.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "SCFLib.h"
#import "Win2Mac.h"
#import <list>

#define WM_DIALUP_STATUS		6070
#define WM_DIALUP_STATISTICS	2891

typedef struct
{
	CTime	start;
	CTime	end;
	LONG	tx;
	LONG	rx;
}SESSIONRECORD;

@interface ENTRYINFO : NSObject
{
@public
	NSString*	servname;
	NSString*	number;
	NSString*	username;
	NSString*	passwd;
	NSString*	apn;
}
@end

@interface DialupMgr : NSObject {
	NSThread* workerThreadHandler;
	CFRunLoopRef workerRunLoop;
@public
	id			dialupReceiver;
	id			statisticReceiver;
	NSInteger	defaultConnIndex;
	NSInteger	curConnIndex;
	NSString*	curServiceID;
	SCNetworkConnectionRef curConnRef;
	SCNetworkConnectionStatus curConnStatus;
	
	DWORD	m_Tx, m_Rx;
	int		m_spdTx, m_spdRx;
	LONGLONG	m_DayTx, m_DayRx;
	LONGLONG	m_WeekTx, m_WeekRx;
	LONGLONG	m_YearTx, m_YearRx;
	DWORD		m_connDur;
	CTime		m_LastResetTime;
	BOOL		m_bMonitor;
	
	int m_peak;
	std::list<int> m_spdRxList;
	std::list<int> m_spdTxList;
	std::list<SESSIONRECORD> m_SessionRecordList;
	NSMutableArray*	m_EntryInfoList;
}

- (void) SetDefaultConn : (NSInteger)Index;
- (NSInteger) AddEntryWithName: (NSString*)operName;
- (NSInteger)AddEntryWithNameList:(ENTRYINFO*)pInfoList;
- (NSString*) GetServiceID: (NSString*) connName;
- (BOOL) isWorking: (NSInteger)Index;
- (SCNetworkConnectionRef) GetConnectionRef: (NSString*) connName;
- (BOOL) isDialupServiceNameExist: (NSString*)name;
- (NSString*) createConnection: (NSString*)connName
			   withNumber: (NSString*)number
				  withAPN: (NSString*)apn
			 withUsername: (NSString*)username
			 withPassword: (NSString*)password;
- (BOOL) DoJob;
- (BOOL) GetDialupData;
- (void) quit;
- (void) ConnectionMonitor;
// utility functions
- (void) showErrorDialog:(NSString *)fieldName;

@end
