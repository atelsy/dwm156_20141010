//
//  DialupMgr.m
//  DCT
//
//  Created by MBJ on 11-11-22.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//
#import "DialupMgr.h"
#import "DCTAppDelegate.h"

#define TIMESPANINSEC		1

static const char * StatusToString(SCNetworkConnectionStatus status)
// Returns a string representation of the network connection status.
{
	const char *		result;
	static const char * kStatusStrings[] = { 
		"kSCNetworkConnectionInvalid",
		"kSCNetworkConnectionDisconnected",
		"kSCNetworkConnectionConnecting",
		"kSCNetworkConnectionConnected",
		"kSCNetworkConnectionDisconnecting"
	};
	
	if (status < kSCNetworkConnectionInvalid || status > kSCNetworkConnectionDisconnecting) 
		result = "unknown";
	else
		result = kStatusStrings[status + 1];
	
	return result;
}

static const char * MinorStatusToString(SCNetworkConnectionPPPStatus minorStatus)
{
	const char *		result;
	static const char * kMinorStatusStrings[] = { 
		"kSCNetworkConnectionPPPDisconnected", 
		"kSCNetworkConnectionPPPInitializing", 
		"kSCNetworkConnectionPPPConnectingLink", 
		"kSCNetworkConnectionPPPDialOnTraffic", 
		"kSCNetworkConnectionPPPNegotiatingLink", 
		"kSCNetworkConnectionPPPAuthenticating", 
		"kSCNetworkConnectionPPPWaitingForCallBack", 
		"kSCNetworkConnectionPPPNegotiatingNetwork", 
		"kSCNetworkConnectionPPPConnected", 
		"kSCNetworkConnectionPPPTerminating", 
		"kSCNetworkConnectionPPPDisconnectingLink", 
		"kSCNetworkConnectionPPPHoldingLinkOff", 
		"kSCNetworkConnectionPPPSuspended", 
		"kSCNetworkConnectionPPPWaitingForRedial"
	};
	
	if (minorStatus < kSCNetworkConnectionPPPDisconnected	||
		minorStatus > kSCNetworkConnectionPPPWaitingForRedial)
		result = "unknown";
	else
		result = kMinorStatusStrings[minorStatus];
	
	return result;
}

void TimerCallBack (CFRunLoopTimerRef timer, void *info)
{
	DialupMgr* pThis = (DialupMgr*)info;
	[pThis ConnectionMonitor];
	[pThis GetDialupData];
	if (pThis->statisticReceiver)
		PostMessage(pThis->statisticReceiver, WM_DIALUP_STATISTICS, 0, 0);
}

void ConnStatusChangeCallBack(SCNetworkConnectionRef	connection,
							  SCNetworkConnectionStatus	status,
							  void*	info)
{
	SCNetworkConnectionPPPStatus	minorStatus;
	time_t						  now;
	struct tm					   nowLocal;
	char							nowLocalStr[30];
	
	assert(connection != NULL);
	assert(info != NULL);
	
	DialupMgr* pThis = (DialupMgr*)info;
	
	// Get a string that represents the current time.
	(void) time(&now);
	(void) localtime_r(&now, &nowLocal);
	(void) strftime(nowLocalStr, sizeof(nowLocalStr), "%X", &nowLocal);
	
	// Due to a bug <rdar://problem/3725976>, it's best to get the major status via 
	// SCNetworkConnectionGetStatus than rely on the value being passed into 
	// the callback.	
	status = SCNetworkConnectionGetStatus(connection);
	
	// Get the minor status from the extended status associated with 
	// the connection.
	minorStatus = GetMinorStatus(connection);
	
	fprintf(stderr, "%s	%s	(%ld)\n", 
			nowLocalStr, 
			StatusToString(status), 
			(long)status);
	
	fprintf(stderr, "%s		%s	(%ld)\n", 
			nowLocalStr, 
			MinorStatusToString(minorStatus), 
			(long) minorStatus);
	
	if (status == kSCNetworkConnectionInvalid ||
		status == kSCNetworkConnectionDisconnected) {
		//	just unschedule, dialupmgr will release this conn ref.
		if (connection != NULL)
		{
			(void) SCNetworkConnectionUnscheduleFromRunLoop(connection,
															CFRunLoopGetCurrent(),
															kCFRunLoopDefaultMode);			
			CFRelease(pThis->curConnRef);
			pThis->curConnRef = NULL;
			pThis->m_bMonitor = TRUE;
		}
	}
	
	NSLog(@"[PPP Dialup]:------------------>set curConnStatus value= %d\n", status);
	pThis->curConnStatus = status;
	if (pThis->dialupReceiver)
		PostMessage(pThis->dialupReceiver, WM_DIALUP_STATUS, status, pThis->curConnIndex);
}

@implementation ENTRYINFO

- (id)init
{
	self = [super init];
	servname = nil;
	number = nil;
	username = nil;
	passwd = nil;
	apn = nil;
	return self;
}

@end

@implementation DialupMgr

- (id)init
{
	self = [super init];
	if (self)
	{
		m_bMonitor = TRUE;
		curServiceID = nil;
		curConnRef = nil;
		NSLog(@"[PPP Dialup]:------------------>Init curConnStatus value= %d\n", kSCNetworkConnectionInvalid);
		curConnStatus = kSCNetworkConnectionInvalid;
		defaultConnIndex = -1;
		curConnIndex = -1;
		
		//	read entry info
		m_EntryInfoList = [[NSMutableArray alloc]init];
		
		NSString *path = [[NSBundle mainBundle] pathForResource:@"DialUpSetting" ofType:@"plist"];
		
		if (path && [path length])
		{
			NSDictionary *dic = [NSDictionary dictionaryWithContentsOfFile:path];
			NSString *key, *str;
			int i = 1;
			
			//	read service info
			while (1) {
				key = [NSString stringWithFormat:@"operator%d", i];
				NSDictionary *subdic = [dic objectForKey: key];
				if (subdic == nil)
					break;
				ENTRYINFO* pInfo = [[ENTRYINFO alloc]init];
				pInfo->servname = [[NSString alloc]initWithString:[subdic objectForKey:@"servname"]];
				pInfo->number = [[NSString alloc]initWithString:[subdic objectForKey:@"number"]];
				pInfo->username = [[NSString alloc]initWithString:[subdic objectForKey:@"username"]];
				pInfo->passwd = [[NSString alloc]initWithString:[subdic objectForKey:@"passwd"]];				
					pInfo->apn = [[NSString alloc]initWithString:[subdic objectForKey:@"apn"]];
					
				[m_EntryInfoList addObject:pInfo]; 
				[pInfo release];
				i++;
				
				NSLog(@"[PPP Dialup]:------------------->name = %@, number = %@\n", pInfo->servname, pInfo->number);
				
			}
			
			NSLog(@"[PPP Dialup]:------------------->Init Entry count%d\n", [m_EntryInfoList count]);
			//	read tx/rx statistics
			str = [dic objectForKey:@"dayTx"];
			m_DayTx = [str longLongValue];
			str = [dic objectForKey:@"dayRx"];
			m_DayRx = [str longLongValue];
			str = [dic objectForKey:@"weekTx"];
			m_WeekTx = [str longLongValue];
			str = [dic objectForKey:@"weekRx"];
			m_WeekRx = [str longLongValue];
			str = [dic objectForKey:@"yearTx"];
			m_YearTx = [str longLongValue];
			str = [dic objectForKey:@"yearRx"];
			m_YearRx = [str longLongValue];
			str = [dic objectForKey:@"lastResetTime"];
			m_LastResetTime = CTime([str integerValue]);
			//	init check
		/*	CTime time = CTime::GetCurrentTime();
			if (time.GetYear() != m_LastResetTime.GetYear())
				m_YearRx = m_YearTx = m_WeekRx = m_WeekTx = m_DayRx = m_DayTx = 0;
			else if (time.GetMonth() != m_LastResetTime.GetMonth())
				m_WeekRx = m_WeekTx = m_DayRx = m_DayTx = 0;
			else if (time.GetDay() != m_LastResetTime.GetDay())
			{
				m_DayRx = m_DayTx = 0;
				if (time.GetDayOfWeek() < m_LastResetTime.GetDayOfWeek() ||
					(time.GetDay() - m_LastResetTime.GetDay() > 7))
					m_WeekRx = m_WeekTx = 0;
			} */
        /********************20131224 By Zhangshaohua ***********************/
            //	init check
         CTime time = CTime::GetCurrentTime();
        if (time.GetYear() != m_LastResetTime.GetYear())
            m_YearRx = m_YearTx = m_WeekRx = m_WeekTx = m_DayRx = m_DayTx = 0;
        else if (time.GetMonth() != m_LastResetTime.GetMonth())
            m_WeekRx = m_WeekTx = m_DayRx = m_DayTx = 0;
        else if (time.GetDay() != m_LastResetTime.GetDay())
			{
            m_DayRx = m_DayTx = 0;
            if (time.GetDayOfWeek() < m_LastResetTime.GetDayOfWeek() ||
                (time.GetDay() - m_LastResetTime.GetDay() > 30))
                m_WeekRx = m_WeekTx = 0;
			}

        
        
        /********************************************************************/
		}
		
		// create dialup worker thread.
		workerThreadHandler = [[NSThread alloc] initWithTarget: self 
													  selector: @selector(workerThread:) 
														object: nil];
		[workerThreadHandler start];
	}
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

- (void) workerThread : (id) param 
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		
	workerRunLoop = CFRunLoopGetCurrent();
	CFRunLoopTimerContext context = {0, self, NULL, NULL, NULL};
	CFRunLoopTimerRef timer = CFRunLoopTimerCreate(kCFAllocatorDefault, 0, 1, 0, 0, TimerCallBack, &context);
	CFRunLoopAddTimer(workerRunLoop, timer, kCFRunLoopCommonModes);

	CFRunLoopRun();
	
	CFRelease(timer);
	
	[pool release];
}

- (void) quit
{
	if (curConnRef)
		CFRelease(curConnRef);
	
	curConnRef = nil;
	curConnStatus = kSCNetworkConnectionInvalid;
	NSLog(@"[PPP Dialup]:------------------>quit curConnStatus value= %d\n", kSCNetworkConnectionInvalid);
	CFRunLoopStop(workerRunLoop);
	while ([workerThreadHandler isExecuting])
		usleep(1);
	
	 //	save connection entry list
    //by zhang shao hua 20131122
  /*  NSArray *doc=NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *docPath=[doc objectAtIndex:0];
    if([[NSFileManager defaultManager]fileExistsAtPath:[docPath stringByAppendingPathComponent:@"DialUpSetting.plist"]]==YES)
    {
    NSString *fileName=[docPath stringByAppendingPathComponent:@"DialUpSetting.plist"];
    NSMutableDictionary *dic = [[[NSMutableDictionary alloc]init]autorelease];

	int i = 1;
	NSString* key;
	//	save service info
	for (ENTRYINFO* p in m_EntryInfoList)
	{
		NSMutableDictionary* subdic = [[[NSMutableDictionary alloc]init]autorelease];
		[subdic setObject:p->servname forKey:@"servname"];
		[subdic setObject:p->number forKey:@"number"];
		[subdic setObject:p->username forKey:@"username"];
		[subdic setObject:p->passwd forKey:@"passwd"];
		[subdic setObject:p->apn forKey:@"apn"];
		
		key = [NSString stringWithFormat:@"operator%d", i];
         NSLog(@"%@",key);
         NSLog(@"%@",p->servname);
		[dic setObject:subdic forKey:key];
		i++;
	}
	//	save statistic records
	key = [NSString stringWithFormat:@"%qu", m_DayTx];
	[dic setObject:key forKey:@"dayTx"];
	key = [NSString stringWithFormat:@"%qu", m_DayRx];
	[dic setObject:key forKey:@"dayRx"];
	key = [NSString stringWithFormat:@"%qu", m_WeekTx];
	[dic setObject:key forKey:@"weekTx"];
	key = [NSString stringWithFormat:@"%qu", m_WeekRx];
	[dic setObject:key forKey:@"weekRx"];
	key = [NSString stringWithFormat:@"%qu", m_YearTx];
	[dic setObject:key forKey:@"yearTx"];
	key = [NSString stringWithFormat:@"%qu", m_YearRx];
	[dic setObject:key forKey:@"yearRx"];
	key = [NSString stringWithFormat:@"%ld", m_LastResetTime.GetTime()];
	[dic setObject:key forKey:@"lastResetTime"];

	//NSString *path = [[NSBundle mainBundle] pathForResource:@"DialUpSetting" ofType:@"plist"];
	[dic writeToFile:fileName atomically:YES];
    [dic release];
	}*/
    
     //	save connection entry list
   NSMutableDictionary *dic = [[[NSMutableDictionary alloc]init]autorelease];
    
	int i = 1;
	NSString* key;
	//	save service info
	for (ENTRYINFO* p in m_EntryInfoList)
	{
		NSMutableDictionary* subdic = [[[NSMutableDictionary alloc]init]autorelease];
		[subdic setObject:p->servname forKey:@"servname"];
		[subdic setObject:p->number forKey:@"number"];
		[subdic setObject:p->username forKey:@"username"];
		[subdic setObject:p->passwd forKey:@"passwd"];
		[subdic setObject:p->apn forKey:@"apn"];
		
		key = [NSString stringWithFormat:@"operator%d", i];
        NSLog(@"%@",key);
        NSLog(@"%@",p->servname);
		[dic setObject:subdic forKey:key];
		i++;
	}
	//	save statistic records
	key = [NSString stringWithFormat:@"%qu", m_DayTx];
	[dic setObject:key forKey:@"dayTx"];
	key = [NSString stringWithFormat:@"%qu", m_DayRx];
	[dic setObject:key forKey:@"dayRx"];
	key = [NSString stringWithFormat:@"%qu", m_WeekTx];
	[dic setObject:key forKey:@"weekTx"];
	key = [NSString stringWithFormat:@"%qu", m_WeekRx];
	[dic setObject:key forKey:@"weekRx"];
	key = [NSString stringWithFormat:@"%qu", m_YearTx];
	[dic setObject:key forKey:@"yearTx"];
	key = [NSString stringWithFormat:@"%qu", m_YearRx];
	[dic setObject:key forKey:@"yearRx"];
	key = [NSString stringWithFormat:@"%ld", m_LastResetTime.GetTime()];
	[dic setObject:key forKey:@"lastResetTime"];
    
	NSString *path = [[NSBundle mainBundle] pathForResource:@"DialUpSetting" ofType:@"plist"];
    [dic writeToFile:path atomically:YES];
    [dic release];
	//	should entrylist release its memebers here?
	//	YES  
   	for (ENTRYINFO* info in m_EntryInfoList)
	{
		if (info->servname)
			[info->servname release];
		if (info->number)
			[info->number release];
		if (info->username)
			[info->username release];
		if (info->passwd)
			[info->passwd release];
		if (info->apn)
			[info->apn release];
		[info release];
	}
}

- (BOOL) DoJob {
	
	NSLog(@"[PPP Dialup]:------------------>To Dojob\n");
	//	make sure worker thread is ready now.
	while (workerRunLoop == nil)
	{
		NSLog(@"[PPP Dialup]:------------------>workerRunLoop == nil\n");
		Sleep(100);
	}
	
	BOOL ret = FALSE;
	NSLog(@"[PPP Dialup]:------------------>curConnStatus = %d\n", curConnStatus);
	switch (curConnStatus)
	{
	//	case kSCNetworkConnectionDisconnected:
	//		ret = (DialupConnect(curConnRef, workerRunLoop) == 0);
	//		break;
		case kSCNetworkConnectionConnected:
		case kSCNetworkConnectionConnecting:
			ret = (DialupDisconnect(curConnRef, workerRunLoop) == 0);
			break;
		case kSCNetworkConnectionDisconnected:
		case kSCNetworkConnectionInvalid:
		{
			NSLog(@"[PPP Dialup]:------------------>curConnRef = 0x%08x\n", curConnRef);
			if (curConnRef)
			{
				SCNetworkConnectionUnscheduleFromRunLoop(curConnRef,
															workerRunLoop,
															kCFRunLoopDefaultMode);
				CFRelease(curConnRef);
				curConnRef = NULL;
			}
			NSLog(@"[PPP Dialup]:------------------>defaultConnIndex = %d\n", defaultConnIndex);
			    if(defaultConnIndex < 0)
                    defaultConnIndex = 0; 
			curConnIndex = defaultConnIndex;
			NSLog(@"[PPP Dialup]:------------------>m_EntryInfoList count = %d\n", [m_EntryInfoList count]);
			if ([m_EntryInfoList count]>0 && curConnIndex >= 0)
			{
				ENTRYINFO* info = (ENTRYINFO*)[m_EntryInfoList objectAtIndex:curConnIndex];
				if (info == nil)
				{
					NSLog(@"[PPP Dialup]:------------------>Entry info = 0x%08x\n", info);
					break;
				}
				m_bMonitor = FALSE;
				NSString* serviceID = [self createConnection: info->servname
									  withNumber: info->number
										 withAPN: info->apn
									withUsername: info->username
									withPassword: info->passwd];
				if (serviceID == nil)
				{
					NSString * errorMessage = [info->servname stringByAppendingString:@" isn't created!"];
					NSRunCriticalAlertPanel(@"Error: Failed to create a new Dialup Service!", @"%@", @"OK", nil, nil, errorMessage);	
					m_bMonitor = TRUE;
					break;
				}
				if (curServiceID)
					[curServiceID release];
				curServiceID = [[NSString alloc]initWithString: serviceID];

				curConnRef = [self GetConnectionRef: curServiceID];
				if (!curConnRef)
				{
					NSLog(@"[PPP Dialup]:------------------>curConnRef 1 = 0x%08x\n", curConnRef);
					curServiceID = nil;
					curConnStatus = kSCNetworkConnectionInvalid;
					m_bMonitor = TRUE;
					break;
				}
				NSLog(@"[PPP Dialup]:------------------>To wait connection status\n");
				//	status may be invalid here, we have to wait for a while.
				while ((curConnStatus = SCNetworkConnectionGetStatus(curConnRef)) == kSCNetworkConnectionInvalid)
					Sleep(1000);
				
				ret = SCNetworkConnectionScheduleWithRunLoop(curConnRef,workerRunLoop, kCFRunLoopDefaultMode);
				if (!ret)
				{
					fprintf(stderr, "add connection ref to runloop error: %d\n", SCError());
					CFRelease(curConnRef);
					curConnRef = nil;
					curServiceID = nil;
					curConnStatus = kSCNetworkConnectionInvalid;
				}
				NSLog(@"[PPP Dialup]:------------------>To call DialupConnect\n");
				ret = (DialupConnect(curConnRef, workerRunLoop) == 0);
			}
		}
			break;
		default:
			ret = false;
			break;
	}
	return ret;
}

- (NSString*) GetServiceID: (NSString*) connName
{
	if (connName == nil)
		return nil;
	
	SCPreferencesRef fPRef = SCPreferencesCreate(nil, CFSTR("findServiceID"), nil);
	NSDictionary* fAllService = (NSDictionary *) GetPPPoMServiceIDFromSet (fPRef);
    
    NSString* fDialupServiceID = [fAllService valueForKey:connName];
	return fDialupServiceID;
}

- (SCNetworkConnectionRef) GetConnectionRef:(NSString *)fDialupServiceID
{
	SCNetworkConnectionContext context;	
	// Set up the context to reference those parameters.
	context.version		 = 0;
	context.info			= (void*)self;
	context.retain		  = NULL;
	context.release		 = NULL;
	context.copyDescription = NULL;
		
	SCNetworkConnectionRef connection = SCNetworkConnectionCreateWithServiceID(NULL,
																			   (CFStringRef)fDialupServiceID, 
																			   ConnStatusChangeCallBack,
																			   &context);	
	return connection;
	}
	
- (void) SetDefaultConn : (NSInteger)connIndex
{
	defaultConnIndex = connIndex;
/*
	NSString* serviceID = nil;
	if ([m_EntryInfoList count] > 0 && curConnIndex >= 0)
{
		ENTRYINFO* info = [m_EntryInfoList objectAtIndex:curConnIndex];
		if (info)
			serviceID = [self GetServiceID: info->servname];
	}

	if (serviceID == nil)	//	invalid connection
{
		if (curConnRef)
		{
			SCNetworkConnectionUnscheduleFromRunLoop(curConnRef,
													 workerRunLoop,
													 kCFRunLoopDefaultMode);
			CFRelease(curConnRef);
		}
		curConnRef = nil;
		curConnStatus = kSCNetworkConnectionInvalid;
	
		if (dialupReceiver)
			PostMessage(dialupReceiver, WM_DIALUP_STATUS, curConnStatus, 0);
	}
	else if ([serviceID compare: curServiceID] != NSOrderedSame)	//	curr connection changed.
	{
		if (curServiceID)
			[curServiceID release];
		curServiceID = [[NSString alloc]initWithString: serviceID];
		
		//	first unschedule and release current connection ref
		if (curConnRef)
		{
			SCNetworkConnectionUnscheduleFromRunLoop(curConnRef,
													 workerRunLoop,
													 kCFRunLoopDefaultMode);
			CFRelease(curConnRef);
		}
	
		//	then create new ref for current connection and schedule it.
		curConnRef = [self GetConnectionRef: curServiceID];
		if (curConnRef)
		{
			curConnStatus = SCNetworkConnectionGetStatus(curConnRef);
			bool ret = SCNetworkConnectionScheduleWithRunLoop(curConnRef,workerRunLoop, kCFRunLoopDefaultMode);
			if (!ret)
			{
				fprintf(stderr, "add connection ref to runloop error: %d\n", SCError());
				CFRelease(curConnRef);
				curConnRef = nil;
				curServiceID = nil;
				curConnStatus = kSCNetworkConnectionInvalid;
			}
		}
		else
		{
			curServiceID = nil;
			curConnStatus = kSCNetworkConnectionInvalid;
		}
		
		if (dialupReceiver)
			PostMessage(dialupReceiver, WM_DIALUP_STATUS, curConnStatus, 0);
	}*/
}

- (NSInteger) AddEntryWithName: (NSString*)operName
{
	if (operName == nil || [operName length] == 0)
		return -1;
		
	NSInteger n;
	//	enum if already has an entry with the same name
	for (ENTRYINFO* p in m_EntryInfoList)
		if ([operName compare:p->servname] == NSOrderedSame)
		{
			n = [m_EntryInfoList indexOfObject:p];
			[self SetDefaultConn:n];
			return n;
		}

	//	add a new member
	ENTRYINFO* pInfo = [[ENTRYINFO alloc]init];
	pInfo->servname = [[NSString alloc]initWithString:operName];
	pInfo->number = [[NSString alloc]initWithString:@""];
	pInfo->username = [[NSString alloc]initWithString:@""];
	pInfo->passwd = [[NSString alloc]initWithString:@""];
	pInfo->apn = [[NSString alloc]initWithString:@""];
	[m_EntryInfoList addObject:pInfo];
	
	//	debug
	NSLog(@"addEntryWithName: %@\n", pInfo->servname);
	n = [m_EntryInfoList indexOfObject:pInfo];
	[pInfo release];
	[self SetDefaultConn:n];
	return n;
}
- (NSInteger)AddEntryWithNameList:(ENTRYINFO*)pInfoList
{
    
    if (pInfoList == nil )
		return -1;
	NSInteger n;
        //	enum if already has an entry with the same name
	for (ENTRYINFO* p in m_EntryInfoList)
		if ([pInfoList->servname compare:p->servname] == NSOrderedSame)
            {
			 n = [m_EntryInfoList indexOfObject:p];
			  [self SetDefaultConn:n];
		      	return n;
            }
    
        //	add a new member
    ENTRYINFO* pInfo = [[ENTRYINFO alloc]init];
	pInfo->servname = [[NSString alloc]initWithString:pInfoList->servname];
	pInfo->number = [[NSString alloc]initWithString:pInfoList->number];
	pInfo->username = [[NSString alloc]initWithString:pInfoList->username];
	pInfo->passwd = [[NSString alloc]initWithString:pInfoList->passwd];
	pInfo->apn = [[NSString alloc]initWithString:pInfoList->apn];
	[m_EntryInfoList addObject:pInfo];
	
        //	debug
	NSLog(@"addEntryWithName: %@\n", pInfo->servname);
	n = [m_EntryInfoList indexOfObject:pInfo];
	[pInfo release];
	[self SetDefaultConn:n];
     return n;
    
	

}
- (BOOL) isWorking: (NSInteger)Index
{
	return (curConnIndex == Index && 
			(curConnStatus == kSCNetworkConnectionConnected ||
			 curConnStatus == kSCNetworkConnectionConnecting||
			 curConnStatus == kSCNetworkConnectionDisconnecting));
}

- (NSString*) createConnection: (NSString*)connName
			   withNumber: (NSString*)number
				  withAPN: (NSString*)apn
			 withUsername: (NSString*)username
			 withPassword: (NSString*)password
{
	NSLog(@"[PPP Dialup]:------------------> To create connection");
	NSString* interfaceName = nil;
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	
	if ([dele getModemPath] && strlen([dele getModemPath]))
		interfaceName = [NSString stringWithUTF8String:strchr([dele getModemPath], '.') + 1];
	
	if(interfaceName == nil){
		fprintf(stderr, "invalid modem interface.\n");
		return nil;
	}	
	
	if(connName == nil){
		fprintf(stderr, "invalid dialup service name.\n");
		return nil;
	}
	
	NSString * bundleID = [[NSBundle mainBundle] bundleIdentifier];
	if(bundleID == nil){
		fprintf(stderr, "invalid bundle id of Dialup Tool.\n");
		return nil;
	}
	
	//	20120308 by foil:
	//	1.	the authorized agent is not necessary.
	//	2.	if a service with the same name already exists, remove it first and
	//		create a new one (how to change params? i dont know). make sure that 
	//		the service's params is our settings.
//	bool ret = (NSString *)DialupServiceCreateAgent((CFStringRef)bundleID, (CFStringRef)interfaceName, (CFStringRef)connName,
//													(CFStringRef)number, (CFStringRef)apn, (CFStringRef)username, (CFStringRef)password);
	AuthorizationRef auth = nil;	
	//Build an AuthorizationRef to pass in to the SystemConfiguration framework
	AuthorizationFlags rootFlags = kAuthorizationFlagDefaults 
	| kAuthorizationFlagExtendRights 
	| kAuthorizationFlagInteractionAllowed 
	| kAuthorizationFlagPreAuthorize;
	
	OSStatus authErr = AuthorizationCreate(nil, kAuthorizationEmptyEnvironment, rootFlags, &auth);
	if(authErr != errAuthorizationSuccess)
	{
		fprintf(stderr, "[PPP Dialup] AuthorizationCreate failed: %s\n", SCErrorString(authErr));
		return nil;
	}
	SCPreferencesRef thisPRef = SCPreferencesCreateWithAuthorization(NULL, (CFStringRef)bundleID, NULL, auth);
	if(thisPRef == nil)
	{
		fprintf(stderr, "[PPP Dialup] SCPreferencesCreateWithAuthorization failed: %s\n", SCErrorString(SCError()));
		return nil;
	}
	
	// lock preference
	SCPreferencesLock(thisPRef, TRUE);
	//	if same named service already exists, remove it
	NSString* serviceID = [self GetServiceID:connName];
	NSLog(@"[PPP Dialup] serviceID = %@, connName = %@\n", serviceID, connName);
	
	if (serviceID != nil)
	{
		SCNetworkServiceRef PPPoMSer = SCNetworkServiceCopy(thisPRef, (CFStringRef)serviceID);
		SCNetworkSetRef SetRef = SCNetworkSetCopyCurrent(thisPRef);
		if(!SCNetworkSetRemoveService(SetRef, PPPoMSer))
			fprintf(stderr, "[PPP Dialup] SCNetworkSetRemoveService failed: %s\n", SCErrorString(SCError()));
		if (!SCNetworkServiceRemove(PPPoMSer))
			fprintf(stderr, "[PPP Dialup] SCNetworkServiceRemove failed: %s\n", SCErrorString(SCError()));
		CFRelease(PPPoMSer);
					
		fprintf(stderr, "[PPP Dialup] Removed service");
	}
	
	// get all Network Interfaces from the machine
	CFArrayRef NIRefAll = SCNetworkInterfaceCopyAll();
   
	// get dedicated service from name
	SCNetworkInterfaceRef ModemIf = SCNetworkInterfaceGetModemInterfaceByName(NIRefAll, (CFStringRef)interfaceName);
	serviceID = (NSString*)DialupServiceCreate(thisPRef, ModemIf, (CFStringRef)connName, (CFStringRef)number,
													(CFStringRef)apn, (CFStringRef)username, (CFStringRef)password);
	
	NSLog(@"[PPP Dialup] new serviceID = %@\n", serviceID);
	CFRelease(NIRefAll);
		
	// unlock preference
	SCPreferencesUnlock(thisPRef);
	CFRelease(thisPRef);
	
	return serviceID;
}

- (BOOL) GetDialupData
{
	//	save curve data
	static int oldtx = 0;
	static int oldrx = 0;
	static CTime lastcount = CTime::GetCurrentTime();
	
	BOOL  bRet = FALSE;
	
	if (curConnStatus == kSCNetworkConnectionConnected)
	{
		//	m_LastResetTime = CTime::GetCurrentTime();
		NSDictionary* dic = (NSDictionary*)SCNetworkConnectionCopyStatistics(curConnRef);
		id tx = [[dic objectForKey:@"PPP"] objectForKey:@"BytesOut"];
		id rx = [[dic objectForKey:@"PPP"] objectForKey:@"BytesIn"];
		m_Tx = [tx intValue];
		m_Rx = [rx intValue];

		//	connect time is what?
	//	dic = (NSDictionary*)SCNetworkConnectionCopyExtendedStatus(curConnRef);
	//	id dur = [[dic objectForKey:@"PPP"] objectForKey:@"ConnectTime"];
	//	m_connDur = [dur intValue];
		m_connDur += TIMESPANINSEC*1000;
	}	
	else if (curConnStatus == kSCNetworkConnectionDisconnected ||
			 curConnStatus == kSCNetworkConnectionInvalid)
	{
		if (m_connDur > 0)	//	last connection finished
		{
			SESSIONRECORD rec;
			rec.end = CTime::GetCurrentTime();
			rec.start = rec.end - CTimeSpan(m_connDur/1000);	//	duration in seconds
			rec.rx = m_Rx;
			rec.tx = m_Tx;
			m_SessionRecordList.push_front(rec);
			m_LastResetTime = rec.end;
		}
		m_connDur = 0;
		m_Rx = m_Tx = 0;
	}
	
	m_spdTx = (m_Tx - oldtx)/TIMESPANINSEC;
	m_spdRx = (m_Rx - oldrx)/TIMESPANINSEC;
	oldtx = m_Tx;
	oldrx = m_Rx;
	
	if (m_spdTx < 0)	m_spdTx = 0;
	if (m_spdRx < 0)	m_spdRx = 0;
	m_spdTxList.push_front(m_spdTx);
	m_spdRxList.push_front(m_spdRx);
	
	CTime time = CTime::GetCurrentTime();
	if (time.GetYear() != lastcount.GetYear())
		m_YearRx = m_YearTx = m_WeekRx = m_WeekTx = m_DayRx = m_DayTx = 0;
	else if (time.GetMonth() != lastcount.GetMonth())
		m_WeekRx = m_WeekTx = m_DayRx = m_DayTx = 0;
	else if (time.GetDay() != lastcount.GetDay())
	{
		m_DayRx = m_DayTx = 0;
		if (time.GetDayOfWeek() < lastcount.GetDayOfWeek() ||
			(time.GetDay() - lastcount.GetDay() > 30))
			m_WeekRx = m_WeekTx = 0;
	}
	lastcount = time;
	
	//	remove session records before 2 month ago
	while (m_SessionRecordList.size())
	{
		CTimeSpan ts = time - m_SessionRecordList.back().end;
		if (ts.GetDays() < 60)
			break;
		m_SessionRecordList.pop_back();
	}
	
	//	add day, month, year and time
	m_DayTx += m_spdTx;
	m_DayRx += m_spdRx;
	m_WeekTx += m_spdTx;
	m_WeekRx += m_spdRx;
	m_YearTx += m_spdTx;
	m_YearRx += m_spdRx;
	
	m_peak = m_spdRx>m_peak?m_spdRx:m_peak;
	m_peak = m_spdTx>m_peak?m_spdTx:m_peak;
	
	return bRet;
}

// utility functions
- (void) showErrorDialog:(NSString *)fieldName {
	
	NSString * preMessage = @"Please give a valid value for ";
	NSString * errorMessage = [preMessage stringByAppendingString:fieldName];
	
	NSRunCriticalAlertPanel(@"Error: Invalid Field Value!", @"%@.", @"OK", nil, nil, errorMessage);
}

- (BOOL) isDialupServiceNameExist : (NSString*)name
{
	SCPreferencesRef fPRef = SCPreferencesCreate(nil, CFSTR("isDialupServiceNameExist"), nil);
	
	NSDictionary* fAllService = (NSDictionary *) GetPPPoMServiceIDFromSet (fPRef);
	BOOL compareResult = FALSE;	
	
	for (id keyService in fAllService) {
		NSString * tmpStr = keyService;		
		compareResult = ( [name compare:tmpStr] == NSOrderedSame );
		
		if(compareResult)
			return true;
	}
	
	return false;
}

- (void) ConnectionMonitor
{
	if (!m_bMonitor) 
		return;
	
	SCNetworkConnectionStatus tmpStatus;
	if(curConnRef != nil)
	{
		ConnStatusChangeCallBack(curConnRef, SCNetworkConnectionGetStatus(curConnRef), (void*)self);
		return;
	}
	

	for(int i=0; i<[m_EntryInfoList count]; i++)
	{
		ENTRYINFO* info = (ENTRYINFO*)[m_EntryInfoList objectAtIndex:i];
		NSString* serviceID = [self GetServiceID:info->servname];

		if(serviceID != nil)
		{
			SCNetworkConnectionRef tmpConnRef = [self GetConnectionRef: serviceID];
			if(tmpConnRef != nil)
			{
				if((tmpStatus = SCNetworkConnectionGetStatus(tmpConnRef)) > kSCNetworkConnectionDisconnected)
				{			
					curConnRef = tmpConnRef;
					curConnIndex = i;
					break;
				}
			}
		}
	}
}

@end
