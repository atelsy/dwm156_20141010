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

#include "me_comm.h"

const int GetHeader(char *header, char *src)
{
	char * cp = header;
	
	if(src[0] != '+')
		return 0;
	
	while(*src != ':')
	{
		*cp++ = *src++;
	}
	
	*cp = '\0';
	
	return 1;
}

const int GetSignIdx(char *src, char ch, int seq)
{
	int count = 0, idx = -1;
	
	while(count != seq)
	{
		if(*src == '\0')
		{
			idx = -1;
			break;
		}
		if(*src++ == ch)
			count++;
		
		idx++;		
	}
	
	return idx;
}

const int GetInteger(char *src, int star, int end)
{
	char buf[16];
	char *dst = buf;
	
	star++;

	ASSERT(end != -1);

	while(src[star] == ' ') //skip space
		star++;

	while(src[star] != 0 && star != end)
	{
		*dst++ = src[star++];
	}
	
	*dst = '\0';
	
	return me_atoi(buf);
}

void GetString(char *src, char *dst, int star, int end)
{
	star++;
	
	ASSERT(end != -1);

	while(src[star] == ' ') //skip space
		star++;
	
	while(src[star] != 0 && star != end)
	{
		*dst++ = src[star++];
	}
	
	*dst = '\0';
}

void trim(char ch, char *src, char *dst)
{
	while(*src != '\0')
	{
		if(*src != ch)
			*dst++ = *src++;
		else
			src++;
	}
	
	*dst = '\0';
}

void trimright(char *src)
{
	int len = me_strlen(src);
	while(len > 0)
	{
		if(src[--len] == ' ')
			src[len] = '\0';
		else
			break;
	}
}

const int GetSignCount(char *src, char ch)
{
	int count = 0;
	while(*src != '\0')
	{
		if(*src++ == ch)
			count ++;
	}

	return count;
}

const char GetCrByte(void)
{
	return 0x0d;
} 

const char GetLfByte(void)
{
	return 0x0a;
}

char GetCharFromRawData(void *pData)
{
	PMEFSM pfsm = (PMEFSM)pData;
	unsigned char ch = 0xff;
	
	if (pfsm->nHead == RAW_DATA_LEN)
		pfsm->nHead = 0;
	
	if (pfsm->nHead != pfsm->nTail)
	{
		ch = pfsm->rawdata[pfsm->nHead++];
	}
	
	return ch;
}

BOOLEAN CheckIfGetGtandSpace(char a,  void *pData)
{
	unsigned char ch;
	
	if(a == '>')
	{
		while((ch=GetCharFromRawData(pData)) != 0xFF)
		{
			if( ch == ' ')
				return TRUE;
			else
				return FALSE;
		}		
	}
	else
	{
		return FALSE;
	}

	return CheckIfGetGtandSpace(a, pData);	
}

void FillBufferByState(unsigned char ch, void *pData)
{
	PMEFSM pfsm = (PMEFSM)pData;

	switch(pfsm->state)
	{
	case 0:
		// Get the first tag of CR
		if(ch == GetCrByte())
		{
			pfsm->state = 1;
		}
		break;
		
	case 1:
		// Get the first tag of Lf
		if(ch == GetLfByte())
		{
			pfsm->state = 2;
			pfsm->nWriteIdx = 0;
		}
		else if(pfsm->bReadPdu)
		{
			if(ch != GetCrByte())
			{
				if(pfsm->nWriteIdx<CTL_DATA_LEN)
					pfsm->ctldata[pfsm->nWriteIdx++]=ch;
			}
			else
			{
				if(pfsm->nWriteIdx>1)
				{
					pfsm->state = 3;
				}
				else
				{
					pfsm->state = 1;
					pfsm->nWriteIdx = 0;
				}
			}
		}
		break;
		
	case 2:
		if(pfsm->bWaitPdu)
		{
			if(CheckIfGetGtandSpace(ch, pData))
			{
				pfsm->bWaitPdu = FALSE;				
				pfsm->state = 1;
				pfsm->ctldata[0] = '>';

				if(pfsm->req.atproc != NULL)
					pfsm->req.atproc(pfsm);
			}
		}
		else
		{
			if(ch != GetCrByte())
			{
				if(pfsm->nWriteIdx<CTL_DATA_LEN)
					pfsm->ctldata[pfsm->nWriteIdx++]=ch;
			}
			else
			{
				if(pfsm->nWriteIdx>1)
				{
					// respDataBuf not an empty string
					pfsm->state = 3;
				}
				else
				{
					// respDataBuf an empty string
					// It must get into the condition of
					//	0x0d 0x0a 0x0d 0x0A
					pfsm->state = 1;
					pfsm->nWriteIdx = 0;
				}
			}
		}
		break;
		
	case 3:
		// Get the 2nd Lf
		if(ch == GetLfByte())
		{
			pfsm->ctldata[pfsm->nWriteIdx]='\0';
			
			if(!pfsm->urcproc(pfsm))
			{
				if(pfsm->req.atproc != NULL)
					pfsm->req.atproc(pfsm);
			}

			pfsm->state = 1;
			pfsm->nWriteIdx = 0;
			pfsm->ctldata[0] = 0;
		}
		break;
		
	default:
		break;
	}
}

int GetOneLine(void *pData)
{
	PMEFSM pfsm = (PMEFSM)pData;
	unsigned char ch;

	while((ch=GetCharFromRawData(pfsm)) != 0xff)
	{
		FillBufferByState(ch, pfsm);
	}
	return 0;

}

int DataCallBack(NTSTATUS status, const PUCHAR pTxt, ULONG nLen, void *pData)
{
	PMEFSM	pfsm;
	ULONG tmpHead, tmpTail;
	ULONG overflow, overplus;
	
	if ((nLen == 0) || (pData == 0))
		return 0;
	
	DebugOutA("\r\n[rxd] %s\r\n", pTxt);
	
	pfsm = (PMEFSM)pData;

	if(pfsm->req.event == AT_CANCEL || pfsm->req.event == AT_TIMEOUT)
		return 0;
	
	tmpHead = pfsm->nHead;
	tmpTail = pfsm->nTail;
	
	if (tmpTail >= tmpHead)
		overplus = RAW_DATA_LEN - tmpTail + tmpHead;
	else
		overplus = tmpHead - tmpTail;
	
	if (overplus < nLen)
	{   //shouldn't give a warning? 
		return 0;
	}
	
	//copy data to circular buffer
	if(tmpTail+nLen >= RAW_DATA_LEN)
	{
		overflow = tmpTail + nLen - RAW_DATA_LEN;
		me_memcpy(pfsm->rawdata+tmpTail, (void*)pTxt, (int)(nLen-overflow));
		me_memcpy(pfsm->rawdata, (void*)(pTxt+nLen-overflow), (int)overflow);
		tmpTail = overflow;
	}
	else
	{
		me_memcpy(pfsm->rawdata+tmpTail, pTxt, (int)nLen);
		tmpTail += nLen;
	}

	pfsm->nTail = (int)tmpTail;
	

	GetOneLine(pData);
	return (int)nLen;
}

