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

#ifndef _ME_COMM_H
#define _ME_COMM_H

#include "me_osal.h"

typedef void (*ME_ATInternalProc) (void *pData);
typedef int (*ME_URCInternalProc) (void *pData);

typedef enum me_fsm_event 
{
	AT_RESPONSE, 
	AT_TIMEOUT,
	AT_WRITEFAIL,
	AT_CANCEL

}FSMEVENT;

typedef enum me_fsm_state
{
	FSM_STATE_START, 
	FSM_STATE_STEP1,
	FSM_STATE_STEP2,
	FSM_STATE_STEP3,
	FSM_STATE_DONE

}FSFSTATE;


typedef struct tagURCTABLE
{
	int		id;
	ME_URCInternalProc	proc;
}URCTABLE;

typedef struct tagMEREQUEST
{
//	char	cmd[AT_CMD_LEN];
	char	expectedCmd[EXP_CMD_LEN];
	int		nFsmState;
	int		event;
	void	*context;   //at result
	HANDLE  hBlockEvt;
	ME_ATInternalProc atproc;	//internal AT response proc
	ME_Callback reply;			//AT response callback
	
}MEREQUEST;

typedef struct tagURCLIST
{
	ME_Callback reply;			//urc callback
	void	*context;

}URCLIST;

typedef struct tagIO_CTL
{
	void *ioproxy; //for irp or com port.
	ME_TX_NOTIFY notify;
}IO_CTL;

typedef struct tagMEFSM
{
	// for parse data
	int state;
	int nHead;
	int nTail;
	int nReadIdx;
	int nWriteIdx;	
	int	nUrcState;
	int nUrcId;
	char rawdata[RAW_DATA_LEN];
	char ctldata[CTL_DATA_LEN];
	char pdu[CTL_DATA_LEN];
	BOOLEAN	bWaitPdu;
	BOOLEAN	bReadPdu;
	BOOLEAN bWriteDone;
	
	// for AT processing
	MEREQUEST req;
	OSALTIMER Timer;
	OSALLOCK  Lock;
	IO_CTL	  ioctl;
	void	*pRspObj;   // for AT response
	void	*pUrcObj;	// Only for CDS and CMT
	
	// for cancel and send AT operation
	OSALLOCK  LockSending;

	// for URC processing
	URCLIST urclist[ID_MAX];
	ME_URCInternalProc urcproc;	//internal urc proc
	
}MEFSM, *PMEFSM;

const int GetHeader(char *header, char *src);
const int GetSignIdx(char *src, char ch, int seq);
const int GetInteger(char *src, int star, int end);
void GetString(char *src, char *dst, int star, int end);
const int GetSignCount(char *src, char ch);
void trimright(char *src);
int DataCallBack(NTSTATUS status, const PUCHAR pTxt, ULONG nLen, void *pData);

#endif