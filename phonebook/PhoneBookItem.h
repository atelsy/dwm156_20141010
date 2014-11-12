/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2007
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

/*******************************************************************************
* Filename:
* ---------
*  PhoneBookItem.h
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   Phonebook Item Class
*
* Author:
* -------
*   Fengping Yu
*
*******************************************************************************/


#ifndef MTK_PHONEBOOKITEM_H
#define MTK_PHONEBOOKITEM_H

#include "BaseItem.h"
#include "GSMMisc.h"
#include "FolderState.h"

typedef enum
{
	PBCHS_DEFAULT = 0,
	PBCHS_UCS2,
	PBCHS_0x81,	
}ePhonebookCHS;


class Advanced_PBItem_Struct
{
public: 
	
	CString m_home_num;
	CString m_company_name;
	CString m_email;
	CString m_office_num;
	CString m_fax_num;
	CString m_title;
	CString m_url;
	CString m_address;
	CString m_note;
	unsigned short m_birth_year;
	unsigned char  m_birth_month;
	unsigned char  m_birth_day;
	
	Advanced_PBItem_Struct(){m_birth_year=m_birth_month=m_birth_day=0;};
	bool HasData()
	{
		return m_home_num.GetLength()||m_company_name.GetLength()||m_email.GetLength()||
			m_office_num.GetLength()||m_fax_num.GetLength()||m_title.GetLength()||
			m_url.GetLength()||m_address.GetLength()||m_note.GetLength()||
			m_birth_year||m_birth_month||m_birth_day;
	};
	void Clear()
	{
		m_home_num=m_company_name=m_email=m_office_num=m_fax_num=m_title="";
		m_url=m_address=m_note="";
		m_birth_year=m_birth_month=m_birth_day=0;
	}
};


class PhoneBookItem : public BaseItem
{
public: 	   
	
	PhoneBookItem();
	
	//virtual functions
	virtual bool send(Connection *pct, int &cms_error) { return false; }
	virtual bool read(Connection *pct, CString memtype, int index, int &cms_error);	
	virtual bool write(Connection *pct, CString memtype, int &cms_error);
	
#ifdef _VCARD_SUPPORT_
	bool read_adv_items(Connection *pct, int &cms_error); //get advanced items	
//	bool Export(FILE *fd);
//	bool Import(CString &str);
#endif

	int GetIndex() const;
	void SetIndex(int index);
	
	void SetPhone(const CString &str_name, const CString &number, BYTE chs);   //0:7-bit, 1:UCS2, 2:0x81	
	void GetPhone(CString &str_name, CString &str_number);	  
	
	void GetTime(tm &t);
	void SetTime(const tm t);
	
	bool EraseItem(Connection *pct, int &cms_error);
	bool ReplaceItem(Connection *pct, int &cms_error, PhoneBookItem &newitem);
	
	const CString &GetMemType(void) const;
	bool SetMemType(const CString str);
	
	void GetAdvPBItem(Advanced_PBItem_Struct &pb){pb = m_adv_pb_item;};
	void SetAdvPBItem(Advanced_PBItem_Struct &pb){m_adv_pb_item = pb;};
	
	CString GetName() const;
	
	void SetSortKey(efolder_sort_key key);
	bool operator<(const PhoneBookItem &rhs);
	
protected:
	
#ifdef _VCARD_SUPPORT_
	bool write_adv_items(Connection *pct, int &cms_error); //write advanced items	
	bool output_vcardfile(const CString &filename);
	bool input_vcard(const CString &filename);
#endif

	bool compose_write_cmd(char *pBuf, const string &name) const;
	BYTE encode_name(Connection *pct, string &result) const;
	
	static efolder_sort_key sort_key;
	
	GSMAddress m_addr;
	CString m_name;   
	GSMTimeStamp m_CHTimeStamp;
	int m_index;
	CString memtype_str;	 //memory type in the mobile
	
	unsigned char m_bCharSet;
	Advanced_PBItem_Struct m_adv_pb_item;		
	
private:
	
	friend ::ostream& operator<<(::ostream &os, const PhoneBookItem &sms);
	friend ::istream& operator>>(::istream &is, PhoneBookItem &sms);		
};

::ostream& operator<<(::ostream &os, const PhoneBookItem &sms);
::istream& operator>>(::istream &is, PhoneBookItem &sms);

::ostream& operator<<(::ostream &os, const Advanced_PBItem_Struct &pbitem);
::istream& operator>>(::istream &is, Advanced_PBItem_Struct &pbitem);

::ostream& operator<<(::ostream &os, const CString &str);
::istream& operator>>(::istream &is, CString &str);

WORD Get0x81Base(TCHAR code);

#endif
