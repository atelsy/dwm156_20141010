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
*  Folder.h
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   Folder.h
*
* Author:
* -------
*   Fengping Yu
*
*==============================================================================
*		   HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Revision:$
* $Modtime:$
* $Log:$
* 
* 08 29 2012 fengping.yu
* [STP100004315]  check in code
* .
 *
 * 01 10 2011 gang.wei
 * [STP100004317]  Data Card Tool source code check in
 * .
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*==============================================================================
*******************************************************************************/



#ifndef MTK_FOLDER_H
#define MTK_FOLDER_H

#pragma warning(disable:4786)
#include <string>
#include <list>
#include <iterator>
#include <map>
#include "Win2Mac.h"
#include "Connection.h"
#include "FolderState.h"

#define MFOLDER_PREFIX	"MFOL"
#define MFOLDER_PREFIX_LEN	4
#define MFOLDER_VER_MAIN	1
#define MFOLDER_VER_SUB		0

//using namespace std;

template <class Type> class MFolder;

template <class Type> istream& operator>> (istream &os, MFolder<Type> &mf);
template <class Type> ostream& operator<< (ostream &os, const MFolder<Type> &mf);

typedef long ItemPos;
typedef long SubfolderPos;

template <class Type>
class MFolder
{
public:

	MFolder();
	~MFolder();
	MFolder & operator=(MFolder<Type> &rhs);

	void GetFullPath(CString &str_name) const;
	void SetFullPath(CString &str_name);
	void GetFolderName(CString &str_name) const;
	void SetFolderName(const CString &str_name);
	int  GetItemSize() const;
	bool InsertItem(Type *t);
	bool InsertFirstPos(Type *t);
	ItemPos GetFirstItemPos();
	Type* GetNextItem(ItemPos& pos);
	void CloseNextItem(ItemPos& pos);
	bool DeleteItem(Type *t);
	void DeleteAll(bool issubfolder);
	
	bool SendAll(Connection *pcn, bool issubfoler, int &cms_error);
	bool WriteAll(Connection *pcn, const CString &memtype, bool issubfolder, int &cms_error);
	
	//For SMS
//	bool ReadAllSMS(Connection *pcn, const CString &memtype, int &cms_error);
	BOOL ProcessSMSItems();
	void Cancel();	

	int  GetSubfolderSize() const;
	bool InsertSubfolder(MFolder *pf);
	bool DeleteSubfolder(MFolder *pf);
	int CountContacts(MFolder *pf);
	MFolder *GetParentFolder() const;
	SubfolderPos GetFirstSubfolderPos();
	MFolder* GetNextSubfolder(SubfolderPos &pos);
	void CloseNextSubfolder(SubfolderPos& pos) const;
	bool MoveItemtoFolder(Type *t, MFolder *pf);
	bool MoveSubfoldertoFolder(MFolder *pf, MFolder *targetfolder);
	void SetFolderState(const folder_state_struct &state);
	void GetFolderState(folder_state_struct &state);
	void Sort(efolder_sort_key order, bool ascending);

	bool ExportFolder(FILE *fd);

protected:
	CString nFullPath;
	CString foldername;
	folder_state_struct folder_state;

	HANDLE hRWMutex;
	HANDLE hCancelEvent;

public:
	std::list<Type *> item_list;
	std::list<MFolder *> subfolder_list;
	MFolder *parentfolder;

	friend ostream& operator<< <Type>(ostream &os, const MFolder<Type> &mf);
	friend istream& operator>> <Type>(istream &is, MFolder<Type> &mf);
};

template <class Type>
int MFolder<Type>::CountContacts(MFolder *pf)
{
	int nCount = pf->GetItemSize();
	MFolder *pSubFldr = NULL;
	SubfolderPos pos = pf->GetFirstSubfolderPos();
	
	while(pos)
	{
		if(pSubFldr == pf->GetNextSubfolder(pos))
		{
			nCount += CountContacts(pSubFldr);
		}
	}
	pf->CloseNextSubfolder(pos);
	
	return nCount;
}

template <class Type>
MFolder<Type>::MFolder()
{
	parentfolder = NULL;
	hRWMutex = CreateMutex(NULL, FALSE, NULL);
}

template <class Type>
MFolder<Type>::~MFolder()
{
	parentfolder = NULL;
	DeleteAll(true);
	CloseHandle(hRWMutex);
}

template <class Type>
MFolder<Type> & MFolder<Type>::operator=(MFolder<Type> &rhs)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	DeleteAll(true);
	
	foldername = rhs.foldername;
	folder_state = rhs.folder_state;
	parentfolder = NULL;

	ItemPos ipos = rhs.GetFirstItemPos();
	while(ipos!=0)
	{
		Type *ori = rhs.GetNextItem(ipos);
		Type *t = new Type;
		*t = *ori;
		InsertItem(t);
	}
	rhs.CloseNextItem(ipos);

//	int x=rhs.GetSubfolderSize();
	
	SubfolderPos fpos = rhs.GetFirstSubfolderPos();
	while(fpos!=0)
	{
		MFolder *fori = rhs.GetNextSubfolder(fpos);
		MFolder *f = new MFolder<Type>;
		*f = *fori;
		InsertSubfolder(f);
	}
	rhs.CloseNextSubfolder(fpos);
	
	ReleaseMutex(hRWMutex);
	return *this;
}

template <class Type>
void MFolder<Type>::Sort(efolder_sort_key order, bool ascending)
{
	WaitForSingleObject(hRWMutex, INFINITE);

	if(item_list.size() > 1)
	{
		std::list<Type *> temp_list;
		temp_list = item_list;  //assign
		item_list.clear();
		
		typename list<Type *>::iterator sec;
		typename std::list<Type *>::iterator iter = temp_list.begin();
		(*iter)->SetSortKey(order);
		while(iter != temp_list.end())  //get one from temp_list
		{
			sec = item_list.begin();
			while(sec != item_list.end())  //compare and insert to item_list
			{
				if(**iter < **sec)
				{
					item_list.insert(sec, *iter);
					break;
				}
				sec++;
			}
			if(sec == item_list.end())	//not insert yet, push_back.
				item_list.push_back(*iter);
			iter++;
		}
		if(!ascending)
			item_list.reverse();
	}
	ReleaseMutex(hRWMutex);
}

template <class Type>
void MFolder<Type>::GetFullPath(CString &str_name) const
{
	WaitForSingleObject(hRWMutex, INFINITE);
	str_name = nFullPath;
	ReleaseMutex(hRWMutex);
}

template <class Type>
void MFolder<Type>::SetFullPath(CString &str_name)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	nFullPath=str_name;
	ReleaseMutex(hRWMutex);   
}

template <class Type>
void MFolder<Type>::GetFolderName(CString &str_name) const
{
	WaitForSingleObject(hRWMutex, INFINITE); 
	str_name.Empty();
	str_name = foldername;
	ReleaseMutex(hRWMutex);   
}

template <class Type>
void MFolder<Type>::SetFolderName(const CString &str_name)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	foldername=str_name;
	ReleaseMutex(hRWMutex);   
}

template <class Type>
int MFolder<Type>::GetItemSize() const
{
	WaitForSingleObject(hRWMutex, INFINITE);
	int size=(int)item_list.size();
	ReleaseMutex(hRWMutex);
	return size;
}

template <class Type>
bool MFolder<Type>::InsertItem(Type *t)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	bool result=false;
	if(t!=NULL)
	{
		item_list.push_back(t);
		result=true;
	}
	ReleaseMutex(hRWMutex);   
	return result;
}

template <class Type>
bool MFolder<Type>::InsertFirstPos(Type *t)
{
	WaitForSingleObject(hRWMutex,INFINITE);
	bool result = false;
	if(t != NULL)
	{
		item_list.push_front(t);
		result = true;
	}
	ReleaseMutex(hRWMutex);
	return result;
}


template <class Type>
ItemPos MFolder<Type>::GetFirstItemPos()
{
	WaitForSingleObject(hRWMutex, INFINITE);
	typename std::list<Type *>::iterator *ret = NULL;
	if(item_list.size()>0)
	{
		ret = new (typename std::list<Type *>::iterator);
		*ret = item_list.begin();
	}
	ReleaseMutex(hRWMutex);   
	return (long)ret ;   
}

template <class Type>
Type* MFolder<Type>::GetNextItem(ItemPos &pos)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	Type *t=NULL;
	typename std::list<Type *>::iterator iter=*((typename std::list<Type *>::iterator *)pos);
	if(pos != 0)
	{
		t = *iter;
		iter++;
		if(iter == item_list.end())
		{
			delete (typename std::list<Type *>::iterator *)pos;
			pos = NULL;
		}
		else
			*((typename std::list<Type *>::iterator *)pos)=iter;
	}
	ReleaseMutex(hRWMutex);   
	return t;
}

template <class Type>
void MFolder<Type>::CloseNextItem(ItemPos &pos)
{
	if(pos!=0)
		delete (typename std::list<Type *>::iterator *)pos;
}

template <class Type>
bool MFolder<Type>::DeleteItem(Type *t)
{ 
	WaitForSingleObject(hRWMutex, INFINITE);
	typename std::list<Type *>::iterator iter = item_list.begin();
	bool ret=false;
	while(iter!=item_list.end())
	{
		if(t == *iter)
			break;
		iter++;
	}
	if(iter!=item_list.end())
	{
		delete (*iter);
		item_list.erase(iter);
		ret = true;
	}
	ReleaseMutex(hRWMutex);   
	return ret;
}

template <class Type>
void MFolder<Type>::DeleteAll(bool issubfolder)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	typename std::list<Type *>::iterator iter = item_list.begin();
	while(iter!=item_list.end())
	{
	//	delete (*iter);
		iter++;
	}
	item_list.clear();
	if(issubfolder)
	{
		typename std::list<MFolder *>::iterator foiter = subfolder_list.begin();
		while(foiter != subfolder_list.end())
		{
			delete (*foiter);
			foiter++;
		}
		subfolder_list.clear();
	}
	ReleaseMutex(hRWMutex);
}

template <class Type>
void MFolder<Type>::Cancel()
{
	SetEvent(hCancelEvent);
}

template <class Type>
bool MFolder<Type>::SendAll(Connection *pcn, bool issubfoler, int &cms_error)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	bool ret = false;
	hCancelEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	typename std::list<Type *>::iterator iter = item_list.begin();
	while(iter!=item_list.end())
	{
		if(WaitForSingleObject(hCancelEvent, 0)==WAIT_OBJECT_0)
			break;
		if(!(*iter)->send(pcn, cms_error))
			break;
		iter++;
	}
	if(iter==item_list.end())
	{   
		if(issubfoler)
		{
			typename std::list<MFolder *>::iterator foiter = subfolder_list.begin();
			while(foiter != subfolder_list.end())
			{
				if(!(*foiter)->SendAll(pcn, issubfoler, cms_error))
					break;
				foiter++;
			}   
			if(foiter == subfolder_list.end())
				ret = true;
		}
	}
	CloseHandle(hCancelEvent);   
	ReleaseMutex(hRWMutex);
	return ret;
}

template <class Type>
bool MFolder<Type>::WriteAll(Connection *pcn, const CString &memtype, bool issubfoler, int &cms_error)
{ 
	WaitForSingleObject(hRWMutex, INFINITE);
	bool ret = false;
	hCancelEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	typename std::list<Type *>::iterator iter = item_list.begin();
	while(iter!=item_list.end())
	{
		if(WaitForSingleObject(hCancelEvent, 0)==WAIT_OBJECT_0)
			break;
		if(!(*iter)->write(pcn, memtype, cms_error))
			break;
		iter++;
	}
	if(iter==item_list.end())
	{   
		if(issubfoler)
		{
			typename std::list<MFolder *>::iterator foiter = subfolder_list.begin();
			while(foiter != subfolder_list.end())
			{
				if(!(*foiter)->WriteAll(pcn, memtype, issubfoler, cms_error))
					break;
				foiter++;
			}   
			if(foiter == subfolder_list.end())
				ret = true;
		}
	}
	CloseHandle(hCancelEvent);
	ReleaseMutex(hRWMutex);
	return ret;
}

/*
template <class Type>
bool MFolder<Type>::ReadAllSMS(Connection *pcn, const CString &memtype, int &cms_error)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	bool ret = false;
	hCancelEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	
	
	int total, used, count=0;
	int start = 1;	
	
	if(memtype == "ME")  //ME index start after SM
	{
		pcn->GetTotalSMSNumber("SM", total, used);
		start = total+1;		
	}
	pcn->GetTotalSMSNumber(memtype, total, used);   
	
	if(used>0)
	{
		for(int index=start; index<total+start; index++)
		{				
			if(WaitForSingleObject(hCancelEvent, 0)==WAIT_OBJECT_0)
				break;		
			Type *t = new Type;
			if(t->read(pcn, memtype, index, cms_error))
			{				   
				item_list.push_back(t);
				count++;
				if(count == used)
				{
					ret=true;
					break;
				}
			}
		}
	}	
	else
		ret=true;
	
	ProcessSMSItems();  //post-process after reading
	CloseHandle(hCancelEvent);
	ReleaseMutex(hRWMutex);
	return ret;
}
//*/

template <class Type>
int MFolder<Type>::GetSubfolderSize() const
{
	WaitForSingleObject(hRWMutex, INFINITE);
	int size=(int)subfolder_list.size();
	ReleaseMutex(hRWMutex);   
	return size;
}

template <class Type>
bool MFolder<Type>::InsertSubfolder(MFolder *pf)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	bool ret = false;
	if(pf!=NULL)
	{
		pf->parentfolder = this;
		subfolder_list.push_back(pf);
		ret = true;
	}
	ReleaseMutex(hRWMutex);
	return ret;
}

template <class Type>
bool MFolder<Type>::DeleteSubfolder(MFolder *pf)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	bool ret = false; 
	typename std::list<MFolder *>::iterator iter = subfolder_list.begin();
	while(iter!=subfolder_list.end())
	{
		if(pf == *iter)
			break;
		iter++;
	}
	if(iter!=subfolder_list.end())
	{
		delete (*iter);
		subfolder_list.erase(iter);
		ret = true;
	}
	ReleaseMutex(hRWMutex);   
	return ret;
}

template <class Type>
MFolder<Type>* MFolder<Type>::GetParentFolder() const
{ 
	return parentfolder;
}

template <class Type>
SubfolderPos MFolder<Type>::GetFirstSubfolderPos()
{
	WaitForSingleObject(hRWMutex, INFINITE);
	typename std::list<MFolder *>::iterator *ret = NULL;
	if(subfolder_list.size() > 0)
	{
		ret = new (typename std::list<MFolder *>::iterator);
		*ret = subfolder_list.begin();
	}
	ReleaseMutex(hRWMutex);   
	return (long)ret;
}

template <class Type>
MFolder<Type>* MFolder<Type>::GetNextSubfolder(SubfolderPos& pos)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	MFolder *f=NULL;
	typename std::list<MFolder *>::iterator iter=*((typename std::list<MFolder *>::iterator *)pos);
	if(pos != 0)
	{
		f = *iter;
		iter++;
		if(iter == subfolder_list.end())
		{
			delete (typename std::list<MFolder *>::iterator *)pos;  
			pos = NULL;
		}
		else
			*((typename std::list<MFolder *>::iterator *)pos)=iter;
	}
	ReleaseMutex(hRWMutex);   
	return f;
}

template <class Type>
void MFolder<Type>::CloseNextSubfolder(SubfolderPos& pos) const
{
	if(pos!=0)
		delete (typename std::list<MFolder *>::iterator *)pos;  
}

template <class Type>
bool MFolder<Type>::MoveItemtoFolder(Type *t, MFolder *pf)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	typename std::list<Type *>::iterator iter = item_list.begin();
	bool ret=false;
	while(iter!=item_list.end())
	{
		if(t == *iter)
			break;
		iter++;
	}
	if(iter!=item_list.end())
	{
		if(pf!=NULL)
		{
			pf->InsertItem(*iter);
			item_list.erase(iter);
			ret = true;
		}
	}
	ReleaseMutex(hRWMutex);   
	return ret;   
}

template <class Type>
bool MFolder<Type>::MoveSubfoldertoFolder(MFolder *pf, MFolder *targetfolder)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	bool ret = false; 
	typename std::list<MFolder *>::iterator iter = subfolder_list.begin();
	while(iter!=subfolder_list.end())
	{
		if(pf == *iter)
			break;
		iter++;
	}
	if(iter!=subfolder_list.end())
	{
		if(targetfolder!=NULL)
		{
			targetfolder->InsertSubfolder(*iter);
			subfolder_list.erase(iter);
			ret = true;
		}
	}
	ReleaseMutex(hRWMutex);   
	return ret;
}

template <class Type>
void MFolder<Type>::SetFolderState(const folder_state_struct &state)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	folder_state=state;
	ReleaseMutex(hRWMutex);   
}

template <class Type>
void MFolder<Type>::GetFolderState(folder_state_struct &state)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	state = folder_state;
	ReleaseMutex(hRWMutex);   
}

template <class Type>
BOOL MFolder<Type>::ProcessSMSItems()
{
	WaitForSingleObject(hRWMutex, INFINITE);   
	
	//long 4 byte: ID0-IDS-Total-Count
	std::map<long, Type *> processmap;  //a map to collect the concatenated msg
	long conbuf=0;
	typename std::list<Type *>::iterator iter, tmp_iter;
	iter = item_list.begin();
	while(iter!=item_list.end())
	{
		tmp_iter = iter;
		iter++;
		if((*tmp_iter)->GetConcatenateIDbuf(conbuf))
		{
			// long message
		 /*   char *pcs = (char*)&conbuf;
			if(pcs[2] > item_list.size())
			{
				ReleaseMutex(hRWMutex);
				return false;
			}
		 */   
			typename std::pair<typename std::map<long, Type *>::iterator, bool> ret;
			ret = processmap.insert(make_pair(conbuf, *tmp_iter));
			if(ret.second == true)
				item_list.erase(tmp_iter);
			else
			{
				//the concatenate msg has the same msg id!!
			}
		}
	}

	char *udh;
	long tmp=0;
	char *ptmp=(char *)&tmp;
	//now process the map
	while(processmap.size()>0)
	{
		typename std::map<long, Type *>::iterator proe=processmap.begin();
		udh = (char *) &(proe->first); //cast from &long to char*

		unsigned char msgid = udh[0];
		unsigned char en_msgid = udh[1];
		unsigned char totalmsg = udh[2];

		Type *t = NULL;
		for(unsigned char i=1; i<=totalmsg; i++)  //now get every msg of this id
		{
			ptmp[0] = msgid; //ID
			ptmp[1] = en_msgid; //enhanced ID
			ptmp[2] = totalmsg; //total
			ptmp[3] = i;  //count
			proe = processmap.find(tmp);
			if(proe!=processmap.end())
			{
				if(t==NULL)
					t = proe->second;
				else
				{
					t->ConcatenateMessage(proe->second);
					delete proe->second;
				}
				processmap.erase(proe);
			}
			else  
			{
				//some msg not exist
			}
		}
		InsertItem(t);
	}

	//now unpack all the sms message
	iter = item_list.begin();
	while(iter!=item_list.end())
	{ 
		(*iter)->UnPackEMSData();
		iter++;
	}
	
	ReleaseMutex(hRWMutex);
	
	return true;
}

template <class Type>
bool MFolder<Type>::ExportFolder(FILE *fd)
{
	WaitForSingleObject(hRWMutex, INFINITE);
	bool ret = false;
	
	typename std::list<Type *>::iterator iter = item_list.begin();
	while(iter!=item_list.end())
	{   
		if(!(*iter)->Export(fd))
			break;
		iter++;
	}
	if(iter==item_list.end())
	{
		typename std::list<MFolder *>::iterator foiter = subfolder_list.begin();
		while(foiter != subfolder_list.end())
		{
			if(!(*foiter)->ExportFolder(fd))
				break;
			foiter++;
		}
		if(foiter == subfolder_list.end())
			ret = true;
	}
	ReleaseMutex(hRWMutex);
	return ret;
}

template <class Type>
ostream& operator<<(ostream &os, const MFolder<Type> &mf)
{
	if( !os ) return os;

	os.write(MFOLDER_PREFIX, MFOLDER_PREFIX_LEN);
	int ver = MFOLDER_VER_MAIN;
	os.write((char *)&ver, sizeof(ver));
	ver = MFOLDER_VER_SUB;
	os.write((char *)&ver, sizeof(ver));
	
	//foldername
	os << mf.foldername;

	//folder_state
	os << mf.folder_state;

	//item_list
	int nbrofentry = (int)mf.item_list.size();
	os.write((char*)&nbrofentry, sizeof(nbrofentry));
	typename std::list<Type *>::const_iterator iter=mf.item_list.begin();
	while(iter!=mf.item_list.end())
	{
		os << **iter;
		iter++;
	}
	
	//subfolder_list
	nbrofentry = (int)mf.subfolder_list.size();
	os.write((char*)&nbrofentry, sizeof(nbrofentry));
	typename std::list<MFolder<Type> *>::const_iterator fo_iter = mf.subfolder_list.begin();
	while(fo_iter!=mf.subfolder_list.end())
	{
		os << **fo_iter;
		fo_iter++;
	}
	return os;
}

template <class Type>
istream& operator>>(istream &is, MFolder<Type> &mf)
{
	if( !is ) return is;
	
	char		file_prefix[MFOLDER_PREFIX_LEN+1];
	int			ver_main;
	int			ver_sub;

	is.read(file_prefix, MFOLDER_PREFIX_LEN);
	file_prefix[MFOLDER_PREFIX_LEN]='\0';
	is.read((char *)&(ver_main), sizeof(ver_main));
	is.read((char *)&(ver_sub), sizeof(ver_sub));
	if(strcmp(file_prefix, MFOLDER_PREFIX) != 0)	  
	{	
		// setstate is not a memeber of istream under _UNICODE
		//is.setstate( ios::badbit|ios::failbit );
		return is;		
	}
	
	int nbrofentry;

	//foldername
	is >> mf.foldername;	

	//folder_state
	is >> mf.folder_state;

	//item_list
	Type *t;
	mf.item_list.clear();
	is.read((char*)&nbrofentry, sizeof(nbrofentry));	
	for(; nbrofentry>0; nbrofentry--)
	{
		t = new Type;
		is >> *t;
		mf.InsertItem(t);
	}
	
	//subfolder_list
	MFolder<Type> *pf;
	mf.subfolder_list.clear();
	is.read((char*)&nbrofentry, sizeof(nbrofentry));
	for(; nbrofentry>0; nbrofentry--)
	{
		pf = new MFolder<Type>;
		is >> *pf;
		mf.InsertSubfolder(pf);
	}
	return is;
}

#endif
