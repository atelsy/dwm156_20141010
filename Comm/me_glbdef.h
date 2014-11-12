/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2011
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef _ME_GLBDEF_H
#define _ME_GLBDEF_H

//#pragma warning(disable:4127)
//#pragma warning(disable:4100)

#define ME_HANDLE		void*
#define URC_COUNT		5
#define RAW_DATA_LEN	1024*8
#define CTL_DATA_LEN	1024*8
#define AT_CMD_LEN		256
#define	EXP_CMD_LEN		16
#define INVALID_PARA	-1

#define ME_PDP_TYPE_LEN	8
#define ME_PDP_ADDR_LEN	64

#define ME_PWD_LEN		EXP_CMD_LEN
#define ME_EXPECT_SIZE  8

#define ME_APN_LEN		ME_PDP_ADDR_LEN
#define ME_DNS_LEN		ME_PDP_ADDR_LEN
#define ME_NAME_LEN		ME_PDP_ADDR_LEN

#define SHORT_TIMEOUT_ELAPSE	3*1000
#define LONG_TIMEOUT_ELAPSE		180*1000


typedef void (*ME_Callback) (void *pData);


typedef enum urc_internal_t
{
	ID_ESIMS = 0,
	ID_CREG,
	ID_ECSQ,
	ID_CGREG,
	ID_CGEV,
	ID_EIND,
	ID_CMTI,
	ID_CMT,
	ID_CDS,
	ID_RING,
	ID_NOCA  = 10,
	ID_CSSI,
	ID_CSSU,
	ID_CUSD,
	ID_CCCM,
	ID_CCWV,
	ID_CCWA,
	ID_STKPCI, 
	ID_PACSP,
	ID_ETWS,
	ID_PSBEARER,
	ID_CBM,
	ID_CIEV, 

	ID_MAX   /*23*/

}URC_ID;

typedef struct tagPREOPERLIST
{
	int index;
	int fmt;
	char oper[40];
	int gsm_act;
	int gsm_compact_act;
	int utran_act;

}PREOPERLIST, *PPREOPERLIST;

typedef struct tagPDPCONTEXT
{
	int	 cid;
	char pdp_type[ME_PDP_TYPE_LEN];
	char apn[ME_APN_LEN];
	char pdp_addr[ME_PDP_ADDR_LEN];
	int	 d_comp;
	int  h_comp;
}PDPCONTEXT, *PPDPCONTEXT;

typedef struct tagSERVICEPRO
{
	int cid;
	int precedence;
	int delay;
	int reliability;
	int peak;
	int mean;
}SERVICEPRO, *PSERVICEPRO;

typedef struct tagPRCOCFG
{
	int cid;
	char name[ME_NAME_LEN];
	char pwd[ME_PWD_LEN];
	char dns1[ME_DNS_LEN];
	char dns2[ME_DNS_LEN];
	int	 auth_type;
}PRCOCFG;

//AT+CGPRCO?
typedef struct tagCGPRCO
{
	int list;
	PRCOCFG cfg[ME_EXPECT_SIZE];	
}CGPRCO, *PCGPRCO;

//AT+EPSB?, AT+ECSQ?, AT+CFUN?, AT+CGATT?, AT+EIND?, AT+CMGS=
//URC: +EIND
typedef struct tagINT_MODEMPARA
{
	int int_value;	
}INT_MODEMPARA, *PINT_MODEMPARA;

//AT+CGSN, AT+CGMI, AT+CGMM, AT+CGMR, AT+CPIN?, AT+CIMI, AT+ICCID?, AT+ESPN?
typedef struct tagSTR_MODEMPARA
{
	char str_value[64];
}STR_MODEMPARA, *PSTR_MODEMPARA;

//AT+EPBSE=?
typedef struct tagBAND_INFO
{
	int gsm_band;
	int umts_band;
}BAND_INFO, *PBAND_INFO;

//AT+CREG?, AT+CGREG? 
//URC: +CREG, +CGREG
typedef struct tagNETWORKREG_INFO
{
	int n;
	int stat;
	char lac[8];
	char ci[8];
	int	 act;
}NETREG_INFO, *PNETREG_INFO;

//AT+EPINC?
typedef struct tagRAMAIN_NUM
{
	int pin;
	int pin2;
	int puk;
	int puk2;
}RAMAIN_NUM, *PRAMAIN_NUM;

typedef struct tagSUB_MSISDN
{
	char alphax[32];
	char number[32];
	int  type;
	int  speed;
	int  service;
	int  itc;

}SUB_MSISDN;

//AT+CNUM
typedef struct tagMSISDN_INFO
{
	int list;
	SUB_MSISDN msisdn[ME_EXPECT_SIZE];
}MSISDN_INFO, *PMSISDN_INFO;


typedef struct tagSUB_FACLOCK
{
	int status;
	int classl;
}SUB_FACLOCK;

//AT+CLCK
typedef struct tagFAC_LOCK
{
	int list;
	SUB_FACLOCK faclock[ME_EXPECT_SIZE];
}FAC_LOCK, *PFAC_LOCK;

typedef struct tagSUB_PDPCONTEXT
{
	int cid;
	int state;	
}SUB_PDPCONTEXT;

//AT+CGACT?
typedef struct tagPDP_CONTEXT
{
	int list;
	SUB_PDPCONTEXT	context[ME_EXPECT_SIZE];
}PDP_CONTEXT, *PPDP_CONTEXT;

//AT+COPS?
typedef struct tagOPER_SEL
{
	int mode;
	int fmt;
	char oper[64];
}OPER_SEL, *POPER_SEL;

typedef struct tagRANGE
{
	int begin;
	int end;
}RANGE, PRANGE;

typedef struct tagSUB_OPERLIST
{
	int	 stat;
	char oper[64];
	char short_oper[64];
	char oper_num[8];
	int	 act;
}SUB_OPERLIST;

//AT+COPS=?
typedef struct tagOPER_LIST
{
	int list;
	SUB_OPERLIST oplist[ME_EXPECT_SIZE];
	RANGE	mode_range;
	RANGE	fmt_range;
}OPER_LIST, *POPER_LIST;

//AT+CSQ
//URC: +ECSQ
typedef struct tagSIGNAL_QUALITY
{
	int rssi;
	int ber;
}SIGNAL_QUALITY, *PSIGNAL_QUALITY;

//AT+CGPADDR	
typedef struct tagPDP_ADDR
{
	int cid;
	char pdp_addr[ME_PDP_ADDR_LEN];
}PDP_ADDR, *PPDP_ADDR;

//AT+MBPC=
typedef struct tagSUB_WWANCONTEXT
{
	int opcode;
	unsigned int entry_id;
	unsigned int context_id;
	unsigned int context_type;
	char access_string[128];
	char name[64];
	char pwd[64];
	int comp;
	int auth;
	char plmn[64];	
}SUB_WWANCONTEXT;

//AT+MBPC=?, AT+MBPC?
typedef struct tagMB_CONTEXT
{
	int list;
	SUB_WWANCONTEXT context[ME_EXPECT_SIZE];
}MB_CONTEXT, *PMB_CONTEXT;

//AT+CNMI=
typedef struct tagSMS_INDICATION
{
	int mode;
	int mt;
	int bm;
	int ds;
	int bfr;
}SMS_INDICATION, *PSMS_INDICATION;

//AT+CSCA=, AT+CSCA?
typedef struct tagSMCS_ADDR
{
	char sca[32];
	int tosca;
}SMCS_ADDR, *PSMCS_ADDR;

//AT+CPMS=
typedef struct tagMSG_MEMSTATE
{
	int used;
	int total;
}MSG_MEMSTATE, *PMSG_MEMSTATE;

//AT+CMGR=
//URC: +CMT, +CDS, +CBM
typedef struct tagMSG_CONTENT
{
	int index; //For AT+CMGL
	int stat;
	char alpha[50];
	int length;
	char pdu[512];
}MSG_CONTENT, *PMSG_CONTENT;

//AT+CMGL=
typedef struct tagLIST_MSG
{
	MSG_CONTENT Msg;
	struct tagLIST_MSG *tail;
	struct tagLIST_MSG *next;
	
}LIST_MSG, *PLIST_MSG;

//AT+CRSM=
typedef struct tagSIM_FILE
{
	int sw1;
	int sw2;
	char resp[16];
}SIM_FILE, *PSIM_FILE;

typedef struct tagATRESULT
{
	int	 err;	  //refer to RESULTTYPE
	int	 err_num; //It will be used, only err is ER_CME_ERROR.
	void *pRet;   //for each AT response structure

}ATRESULT, *PATRESULT;

//URC: +ESIMS
typedef struct tagSIM_STATE
{
	int sim_insert;
	int lock_required;
}SIM_STATE, *PSIM_STATE;

//URC: +CGEV
typedef struct tagNETWORK_STATE
{
	char action[16]; // REJECT, NW REACT, NW DEACT, ME DEACT, NW DETACH, ME DETACH, NW CLASS, ME CLASS
	int	 cid;
	char pdp_type[ME_PDP_TYPE_LEN];
	char apn[ME_APN_LEN];
	char pdp_addr[ME_PDP_ADDR_LEN];
	char classx[8];	
}NETWORK_STATE, *PNETWORK_STATE;

//URC: +CMTI
typedef struct tagSMS_ARRIVED
{
	char mem[8];
	int index;	
}SMS_ARRIVED, *PSMS_ARRIVED;

#endif
