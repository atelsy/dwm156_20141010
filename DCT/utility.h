//
//  utility.h
//  DCT
//
//  Created by Fengping Yu on 11/26/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "SMSMessage.h"
#import "PhoneBookItem.h"
#include "Connection.h"
#include "Folder.h"
#import "cellData.h"

//define message notification key
#define ITEM_INDEX	  @"Item index"
#define ITEM_MESSAGE	@"Item message"
#define ITEM_BOOL	   @"Item bool"
#define ITEM_FOLDER	 @"Item folder"
#define ITEM_MSGID	  @"Item messageID"
#define ITEM_NUMBER	 @"Item number"
#define ITEM_MEMORY	 @"Item memory"
#define ITEM_CONTACT	@"Item contact"

//define message type

#define WM_USER			 0x0400
#define WM_STATUS		   WM_USER + 100
#define WM_URCCALLBACK	  WM_USER + 101
#define WM_RSPCALLBACK	  WM_USER + 102
#define WM_SETTING_COMPLETE   WM_USER + 103
#define WM_DIAL_FUNC		  WM_USER + 104
#define WM_SMS_DESTROYDLG	 WM_USER + 105
#define WM_INIT_RESULT		WM_USER + 106
#define WM_SMS_INSERTMOVE	 WM_USER + 107
#define WM_INITCONTACT		  WM_USER + 108
#define WM_PB_REFRESH		  WM_USER + 109
#define WM_PB_DESTROYDLG	  WM_USER + 110
#define WM_CHECKPHONENAME	 WM_USER + 111
#define WM_PROCESSCONTACT	  WM_USER + 112
#define WM_PBINSERTMOVE		  WM_USER + 113
#define WM_PBDELETEMOVE		  WM_USER + 114
#define WM_PBCOPY			  WM_USER + 115
#define WM_DIALUP_DESTROYDLG   WM_USER +116
#define WM_SMSINSERTMOVE	  WM_USER + 117
#define WM_SMSDELETEMOVE	  WM_USER + 118
#define WM_SMSCOPY			  WM_USER + 119
#define WM_PROCESSSMS		 WM_USER + 120
#define WM_CH_DESTROYDLG	  WM_USER + 121
#define WM_CHDELETE		   WM_USER + 122
#define WM_CH_DESTROYCALLDLG  WM_USER + 123	
#define WM_INSERTSMSBYADDR	WM_USER + 125
#define WM_CH_STARTDOWNLOAD   WM_USER + 126
#define WM_DISPLAY_MISSCALL   WM_USER + 127
#define WM_REFRESH_LIST	   WM_USER + 128
#define WM_PB_AFTERREDOWNLOAD WM_USER + 129
#define WM_CLEARDATA		  WM_USER + 130
#define WM_ENTERDLG		   WM_USER + 131
#define WM_LEAVEDLG		   WM_USER + 132
#define WM_ENTER_DOWNLOADSMS  WM_USER + 133
#define WM_LEAVE_DOWNLOADSMS  WM_USER + 134
#define WM_DOWNLOADPROMPT	  WM_USER + 135
#define WM_QUERY_PIN		  WM_USER + 136
#define WM_BUTTON_MOUSEMOVE   WM_USER + 137
#define WM_SS_DESTROYDLG	  WM_USER + 138
#define WM_CHANGEPIN2		 WM_USER + 139
#define WM_TRAY_NOTIFICATION  WM_USER + 140
#define WM_ASYNCREQ		   WM_USER + 141
#define WM_AUTODIALUP		 WM_USER + 142	
#define WM_DISDIALUP		  WM_USER + 143	
#define WM_ENABLEOPERATION	WM_USER + 144
#define WM_UPDATE_SMSINFO	 WM_USER + 145
#define WM_DEFAULT_CLOSE	  WM_USER + 146
#define WM_MONITOR			WM_USER + 147
#define WM_DIALUP_CLOSE	   WM_USER + 148
#define WM_URCMSG			 WM_USER + 149
#define WM_CH_INSERT		  WM_USER + 150
#define WM_POWEROFFDONE		     WM_USER + 151

/************ 20131109 by Zhuwei **********************************************/

#define WM_SETTING_USSD     WM_USER + 152

/******************************************************************************/
/**********************20131112 by zhang shao hua*********************/
#define  WM_GETVERNO      WM_USER+153
/*********************************************************************/

#define APPHANDLE		   (AfxGetMainWnd()->GetSafeHwnd())
#define INCLUDE_SUBFOLDER	true
#define NO_NOTIFY			false
#define TO_PC				TRUE
#define TO_HANDSET			FALSE
#define CHECK_SAVE			TRUE
#define TBIMAGE_CX			24
#define TB_HEIGHT			40
#define DLG_TB_HEIGHT		25
#define USER_IMAGE			8
#define IDI_DEFAULT		 1
#define IDI_EXIT			0
#define WM_INITCOMBOLIST		WM_USER + 73
#define WM_FILE_SHOW_MESSAGE	WM_USER + 82

#define CCWA_SETTING   7
#define CCWA_QUERY	 8
#define COST_CAOC	  9
#define COST_CACM	  10
#define COST_CAMM	  11
#define COST_CPUC	  12
#define SS_DEACTIVATE_CCFC 13
#define SS_QUERY_CCFC	  14
#define SS_CCWA_ENABLE	 15
#define SS_CCWA_DISABLE	16
#define SS_COST_CAOC	   17
#define SS_COST_CACM	   18
#define SS_COST_CAMM	   19
#define SS_COST_CPUC	   20 
#define SS_CCWA_STATUS	   21

// define message box flags
#define MB_OK			   0x00
#define MB_OKCANCEL		 0X01
#define MB_ABORTRETRYIGNORE 0x02
#define MB_YESNOCANCEL	  0x03
#define MB_YESNO			0x04
#define MB_RETRYCANCEL	  0x05

//define split view split range
#define kMinVerticalRightSplit  450.f
#define kMinVerticalLeftSplit   215.f
#define kMinHorizontalDownSplit 180.f
#define kMinHorizontalUpSplit   100.f;

//define registry profile
#define PROFILE_SMSMEM	  @"Memory"

typedef MFolder<SMSMessage> SmsFolder;
typedef MFolder<PhoneBookItem> PbFolder;
typedef MFolder<PhoneBookItem> ChFolder;

#define MSGBOX_CAPTION	_T("Data Card Tool")

#define LocalizeString(key)		\
	[[NSBundle mainBundle] localizedStringForKey:(key) value:@"" table:@"InfoPlist"]

#define LoadString	LocalizeString

enum DIRECTION 
{
	HStoHS = 0,
	HStoPC = 1,
	PCtoHS  = 2,
	PCtoPC = 3
};

enum resp_id
{
	RESP_GET_CLCK = 0,
	RESP_SET_CLCK,
	RESP_GET_CLIR,
	RESP_SET_CLIR,
	RESP_SET_PIN,
	RESP_SET_PIN2,
	RESP_SET_PUK,
	RESP_SET_PUK2,
	RESP_GET_OPERATOR,
	RESP_SET_MODEMAPN,
	RESP_SMS_READ,
	RESP_SMS_SEND,
	RESP_SMS_WRITE,
	RESP_SMS_DELETE,
	RESP_SMS_UPDATE,
	RESP_SMS_GETMEMTYPE,
	RESP_SMS_SETMEMTYPE,
	RESP_SMS_GETTOTALNUM,
	RESP_GET_SMSMEM,
	RESP_SET_SMSMEM,
	RESP_SMS_CONFIG,
	RESP_PB_READ,
	RESP_PB_GETTOTALNUM,
	RESP_PB_INSERTCONTACT,
	RESP_PB_REPLACECONTACT,
	RESP_CH_READ,
	RESP_CH_GETMISSCALLNUM,
	RESP_CH_GETRECEIVECALLNUM,
	RESP_CH_GETDIALCALLNUM,
	RESP_QUERY_REMAINTRY,
	RESP_GET_CALLWAITING,
	RESP_SET_CALLWAITING,
	RESP_GET_CUROP,
	RESP_GET_CURNETTP,
	RESP_SET_NETWORKTP,
    RESP_GET_NETWORKTP,
	RESP_GET_OPLIST,
	RESP_SET_OPERAUTO,
	RESP_SET_OPERMANUAL,
	RESP_SMS_IMSI
};

enum pb_dlg
{
	PB_DLG_READ = 0,
	PB_DLG_EDITOR,
	PB_DLG_CSV
};

enum sms_dlg
{
	SMS_DLG_EDITOR  = 0,
	SMS_DLG_SEND,
	SMS_DLG_READ,
	SMS_DLG_READER,
	SMS_DLG_SAVE
};

enum ch_dlg
{
	CH_DLG_READ = 0
};

enum init_value
{
	INIT_SUCCESS = 0,
	INIT_FAILED	
};

enum DOWNLOAD_TYPE
{
	DOWNLOAD = 0,
	REDOWNLOAD	
};

enum OPERATION_TYPE{
	OP_SMS = 0,
	OP_PB
};

enum SS_FACILITY
{
	SS_CCFC = 0,
	SS_CLCK
};

enum SS_MODE
{
	SS_DEACTIVATE =0,
	SS_ACTIVATE,
	SS_QUERY,	
	SS_CCWA_ACTIVATE,
	SS_CCWA_DEACTIVATE,
	SS_CCWA_QUERY,
	SS_CAOC_QUERY,
	SS_CACM_QUERY,
	SS_CAMM_QUERY,
	SS_CPUC_QUERY,
	SS_CACM_RESET,
	SS_CAMM_RESET,
	SS_CPUC_RESET
};

enum ss_dlg
{
	CCFC_DLG_SETTING = 0,
	CCFC_DLG_PROMPT,
	CLCK_DLG_SETTING,
	CLCK_DLG_CHANGEPWD,
	CCWA_DLG_SETTING,
	COST_DLG_RESET,
	COST_DLG_PROMPT
};

typedef enum BAUDRATE
{
	BAUD2400	=   2400,
	BAUD4800	=   4800,
	BAUD9600	=   9600,
	BAUD14400   =   14400,
	BAUD19200   =   19200,
	BAUD56000   =   56000,
	BAUD115200  =   115200,
	BAUD230400  =   230400,
	BAUD460800  =   460800,
	BAUD921600  =   921600
}BAUDRATE;

@interface classToObject : NSObject
{
@public
	SMSMessage *pMsg;
	PhoneBookItem *pContact;
	SmsFolder *pFolder;
	PbFolder *pbFolder;
	void	 *pbMem;
	int		  nItem;
}

//@property(readwrite) SMSMessage *pMsg;
//@property(readwrite) SmsFolder *pFolder;
//@property(readwrite) PbFolder *pbFolder;

@end

@interface utility : NSObject 
{
@private

}

+ (ChFolder*)newChFolder;
+ (PbFolder*)newPbFolder;
+ (PhoneBookItem*) NewContact;
+ (SmsFolder*)newSmsFolder;
+ (SMSMessage*)newMessage;
+ (unsigned short*)getEmsText:(EMSData*)pEmsData withLength:(int*)len;
+ (NSString*)getEmsText:(EMSData*)pEmsData;
+ (BOOL)getState:(SMSMessage*)pMsg withFolder:(SmsFolder*)pFolder;
+ (BOOL)deleteEmsObject:(SMSMessage*)pMsg;
+ (void)processSMSFolder:(SmsFolder *)pFolder;
+ (NSString*)getTime:(SMSMessage *)pMsg;

+ (NSInteger)showMessageBox:(NSString*)strCaption
					withMsg:(NSString*)msg
					forType:(int)type;

+ (int)getModalCounter;
+ (int)setModalCounter:(BOOL)bAdd;

+ (NSArray*)getMultiAddress:(NSString*)str;
+ (NSArray*)getMultiAddressOfMsg:(SMSMessage*)pMsg;
+ (BOOL)checkPhoneNumber:(const NSString*)str;
//+ (cellData*)createToolbarCellwithImage:(NSString*)imageName tip:(NSString*)tooltip tag:(int)_tag state:(BOOL)bEnable appTag:(appTag)appTag;

+ (BOOL)CheckValidDate:(int)nYear andMonth:(int)nMonth andDay:(int)nDay;
+ (BOOL)CheckPhoneChar:(const CString &)str;
+ (BOOL)CheckUnicode:(const CString&)str andLen:(size_t&)nLen2;
+ (BOOL)CheckUnicodeLen:(const size_t)nLen andLen:(const size_t)nWLen;
+ (int)CheckEncodeClass:(const CString&)str andLen:(int&)nLen;
+ (BOOL)CheckPhoneNumberLen:(const CString&)str andFlag:(BOOL)bTarget;
+ (BOOL)CheckPhoneHomeNumber:(const CString&)str;
+ (BOOL)CheckPhoneCompanyName:(const CString&)str;
+ (BOOL)CheckEmailLocal:(const CString&)strEml;
+ (BOOL)CheckEmailDomain:(const CString&)strEml;
+ (BOOL)CheckPhoneEmail:(const CString&)str;
+ (BOOL)CheckPhoneOfficeNumber:(const CString&)str;
+ (BOOL)CheckPhoneFaxNumber:(const CString&)str;
+ (BOOL)CheckPhoneBirthday:(const CString&)str andAdvData:(Advanced_PBItem_Struct&)adv_pb_item;

void reConvertText(unsigned char *pbuf, size_t len);
void convertText(unsigned char *pBuf, int nLen);

@end
