//
//  pbCSVDlg.m
//  DCT
//
//  Created by mbj on 12-8-6.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "pbCSVDlg.h"
#import "Conversions.h"
 #include <iconv.h>

@implementation pbCSVDlg

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	
	return self;
}

- (void)dealloc
{	
	[super dealloc];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	m_bModelDlg = false;

	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (IBAction)OnOK:(id)sender
{	
	NSString *strFile = nil;
	bool bProcess = false;
	int idx = (int)[(NSButtonCell*)[m_ctlMatrix selectedCell] tag]; 
	switch(idx)
	{
		case 0:
		{
			strFile = [self OpenFile];
			if(strFile != nil)
			{
				bProcess = true;
				[self ImportPhonebook:strFile];
			}
			m_bModelDlg = false;
		}
			break;
		case 1:
		{
			strFile = [self SaveFile];
			if(strFile != nil)
			{
				bProcess = true;
				[self ExportPhonebook:strFile];
			}
			m_bModelDlg = false;
		}
			break;
			
	}
	if(bProcess)
		PostMessage(m_pParent, WM_PB_DESTROYDLG, PB_DLG_CSV, 0);
}

-(void)awakeFromNib
{
	[m_ctlMatrix selectCellWithTag:0];
}

- (IBAction)OnCancel:(id)sender
{
	PostMessage(m_pParent, WM_PB_DESTROYDLG, PB_DLG_CSV, 0);
}

- (BOOL)Init:(PbFolder*)pHandSetFolder withPcFolder:(PbFolder*)pPcFolder andParent:(id)pParent
{
	m_pParent = pParent;
	m_pHansetFolder = pHandSetFolder;
	m_pPcFolder = pPcFolder;
	
	[m_ctlMatrix selectCellWithTag:1];
	return YES;
}

- (NSString*)OpenFile
{
	NSString *strFile = nil;
	
	NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	[oPanel setCanChooseDirectories:NO]; 
	[oPanel setAllowsMultipleSelection:NO];
	[oPanel setAllowedFileTypes:[NSArray arrayWithObject:@"csv"]];
	[oPanel setCanChooseFiles:YES];
	m_bModelDlg = true;
	if ([oPanel runModal] == NSOKButton) 
	{  
		strFile = [[[oPanel URLs] objectAtIndex:0] path];
	}

	return strFile;
}

- (NSString*)SaveFile
{
	NSString *strFile = nil;
	
	NSSavePanel *sPanel = [NSSavePanel savePanel];
	[sPanel setAllowedFileTypes:[NSArray arrayWithObject:@"csv"]];
	m_bModelDlg = true;
	if ([sPanel runModal] == NSOKButton) 
	{  
		strFile = [[sPanel URL] path];
	}
	
	return strFile;
}

- (PBIMPORT)ImportPhonebook:(NSString*)strFile
{	
	const char* filename = NULL;
	if (strFile)
		filename = [strFile UTF8String]; 

	std::string strPath;
	kal_uint8 dst[BUFSIZE];
	kal_uint32 endpos;
	
	int quo=0, comma=0;
	bool bTitle=true;
	char c;
	std::string tstr, substr;
	
	FILE *fd = fopen(filename, "rb");
	if (fd == NULL)
		return PBIMPORT_ERR;
	
	CString fname, lname, company, fax, office, home, mobile, email, birthday;
	unsigned short ch;	
	int i;

	Advanced_PBItem_Struct adv_pb_item;
	while(!feof(fd))
	{
		fread(&c, 1, 1, fd);
		tstr += c;
		if(c == '\"')
			quo++;
		else if (c == ',' && quo%2 == 0 && bTitle == false)
		{
			comma++;
			switch(comma)
			{
				case FIRSTNAME_INDEX:
					memset(dst, 0, BUFSIZE);
					substr = tstr.substr(1, tstr.length()-3);					
					if(substr.size() >0)
					{
						gb2312_to_ucs2(dst, BUFSIZE, (unsigned char *)substr.c_str(), &endpos);
						for(i=0; i<endpos/2; i++)
						{
							ch = 0;
							memcpy(&ch,dst+2*i, 2);
							fname += ch;
						}
					}
					break;
				case LASTNAME_INDEX:
					substr = tstr.substr(1, tstr.length()-3);
					if(substr.size() > 0)
					{
						gb2312_to_ucs2(dst, BUFSIZE, (unsigned char *)substr.c_str(), &endpos);
						for(i=0; i<endpos/2; i++)
						{
							ch = 0;
							memcpy(&ch,dst+2*i, 2);
							lname += ch;
						}
					}
					break;
				case COMPANY_INDEX:
					substr = tstr.substr(1, tstr.length()-3);
					if(substr.size() > 0)
					{
						gb2312_to_ucs2(dst, BUFSIZE, (unsigned char *)substr.c_str(), &endpos);
						//adv_pb_item.m_company_name = (wchar_t*)dst;
						for(i=0; i<endpos/2; i++)
						{
							//ch = *(short*)(dst+2*i);
							ch = 0;
							memcpy(&ch,dst+2*i, 2);
							adv_pb_item.m_company_name += ch;
						}
					}
					break;
				case FAX_INDEX:
					substr = tstr.substr(1, tstr.length()-3);
					adv_pb_item.m_fax_num = substr.c_str();
					break;
				case OFFICE_INDEX:
					substr = tstr.substr(1, tstr.length()-3);
					adv_pb_item.m_office_num = substr.c_str();
					break;
				case HOME_INDEX:
					substr = tstr.substr(1, tstr.length()-3);
					adv_pb_item.m_home_num = substr.c_str();
					break;
				case MOBILE_INDEX:
					substr = tstr.substr(1, tstr.length()-3);
					mobile = substr.c_str();
					break;
				case EMAIL_CH_INDEX:
					substr = tstr.substr(1, tstr.length()-3);
					adv_pb_item.m_email = substr.c_str();
					break;
				case BIRTHDAY_CH_INDEX:
				{
					int year = 0;
					int month = 0;
					int day =0;
					substr = tstr.substr(1, tstr.length()-3);
					CString birthday(substr.c_str());
					_stscanf(birthday, _T("%04d-%02d-%02d"), &year, &month, &day);					
					// check if before 1900/01/01
					if(year > 1900)
					{
						adv_pb_item.m_birth_year = year;
						adv_pb_item.m_birth_month = month;
						adv_pb_item.m_birth_day = day;
					}	
				}
					break;
			}
			tstr = "";
		}
		else if(c == '\n' && quo%2 == 0)
		{
			bTitle = false;
			tstr = "";
			quo = 0;
			comma = 0;
			memset(dst, 0, BUFSIZE);
			//one line parse done
			if(!fname.IsEmpty() || !lname.IsEmpty())
			{
				PhoneBookItem *pb = new PhoneBookItem;
				pb->SetPhone(lname+fname, mobile, PBCHS_UCS2);
				pb->SetAdvPBItem(adv_pb_item);	
				/*	
				 if(++nCount > PBIMPORTCOUNT)
				 {
				 delete pb;
				 fclose(fd);
				 return PBIMPORT_FULL;
				 }
				 */	
				m_pPcFolder->InsertItem(pb);
				fname.Empty();
				lname.Empty();
				mobile.Empty();
				adv_pb_item.Clear();
			}
		}				
	}
	fclose(fd);		
	return PBIMPORT_OK;
}

- (bool)ExportPhonebook:(NSString*)strFile;
{
	const char* filename = NULL;
	if (strFile)
		filename = [strFile UTF8String]; 
	
	kal_uint32 endpos;
	
	FILE *fd = fopen(filename, "wb");
	if(fd == nil)
		return false;

	unsigned char src[BUFSIZE];
	unsigned char dst[BUFSIZE];
	char input[BUFSIZE];
	NSString *path = [[NSBundle mainBundle] pathForResource:@"ContactField"
													 ofType:@"plist"];
	
	NSDictionary *pField = [NSDictionary dictionaryWithContentsOfFile:path];
	
	for(int i=1; i<=[pField count]; i++)
	{
		NSString *str = [pField objectForKey:[NSString stringWithFormat:@"%d", i]];	
		NSInteger nLen = [str length];
		NSInteger j = 0;
		for(; j<nLen; j++)
		{
			unichar ch = [str characterAtIndex:j];
			src[2*j]   = (ch>>0)&0xFF;
			src[2*j+1] = (ch>>8)&0xFF;
		}
		
		src[2*j]   = 0;
		src[2*j+1] = 0;
		
		ucs2_to_gb2312(dst, BUFSIZE, src, &endpos);
		
		if(i != [pField count])
			sprintf(input, "\"%s\",", dst);
		else
			sprintf(input, "\"%s\"\r\n", dst);
		
		fwrite(input, 1, strlen(input), fd);
	}
	
	
	bool ret = false;
	
	if((ret = [self WriteContact:m_pHansetFolder Handle:fd]))
	{
		ret = [self WriteContact:m_pPcFolder Handle:fd];
	}
	
/*	if(!ret)
		ShowMessageBox(APPHANDLE, IDS_ERROR_PBEXPORT);*/
	
	fclose(fd);
	
	return true;
}

- (bool)WriteContact:(PbFolder*)pFolder Handle:(FILE*)fd
{
	PhoneBookItem *pItem = NULL;
	ItemPos pos = pFolder->GetFirstItemPos();
	
	while(pos != 0)
	{
		pItem = pFolder->GetNextItem(pos);
		
		if(pItem != NULL)
		{
			char input[BUFSIZE];
			unsigned char dst[BUFSIZE];
			unsigned char src[BUFSIZE];
			unsigned int endpos;
			std::string strInput;
			
			CString fname, lname, company, fax, office, home, mobile, email, birthday;			
			Advanced_PBItem_Struct adv_pb_item;
			
			pItem->GetPhone(fname, mobile);
			pItem->GetAdvPBItem(adv_pb_item);
			
			for(int i=1; i<=BIRTHDAY_CH_INDEX; i++)
			{
				switch(i)
				{
					case 1:
						strcpy(input, "\"\",");
						break;
					case FIRSTNAME_INDEX:
						[self MultibyteToSinglebyte:fname To:src];
						ucs2_to_gb2312(dst, BUFSIZE, src, &endpos);
						sprintf(input,"\"%s\",", dst);
						break;
					case LASTNAME_INDEX:
						strcpy(input, "\"\",");
						break;
					case COMPANY_INDEX:
						[self MultibyteToSinglebyte:adv_pb_item.m_company_name To:src];
						ucs2_to_gb2312(dst, BUFSIZE, src, &endpos);
						sprintf(input,"\"%s\",", dst);
						break;
					case FAX_INDEX:
						[self MultibyteToSinglebyte:adv_pb_item.m_fax_num To:src];
						ucs2_to_gb2312(dst, BUFSIZE, src, &endpos);
						sprintf(input,"\"%s\",", dst);
						break;
					case OFFICE_INDEX:
						[self MultibyteToSinglebyte:adv_pb_item.m_office_num To:src];
						ucs2_to_gb2312(dst, BUFSIZE, src, &endpos);
						sprintf(input,"\"%s\",", dst);
						break;
					case HOME_INDEX:
						[self MultibyteToSinglebyte:adv_pb_item.m_home_num To:src];
						ucs2_to_gb2312(dst, BUFSIZE, src, &endpos);
						sprintf(input,"\"%s\",", dst);
						break;
					case MOBILE_INDEX:	
						[self MultibyteToSinglebyte:mobile To:src];
						ucs2_to_gb2312(dst, BUFSIZE, src, &endpos);
						sprintf(input,"\"%s\",", dst);
						break;
					case EMAIL_CH_INDEX:
						[self MultibyteToSinglebyte:adv_pb_item.m_email To:src];
						ucs2_to_gb2312(dst, BUFSIZE, src, &endpos);
						sprintf(input,"\"%s\",", dst);
						break;
					case BIRTHDAY_CH_INDEX:
						sprintf(input, "\"%04d-%02d-%02d\",\r\n", adv_pb_item.m_birth_year, adv_pb_item.m_birth_month, adv_pb_item.m_birth_day);
						break;	
					default:
						strcpy(input, ",");
				}
				
				/*if(fwrite(input, 1, strlen(input), fd) != strlen(input))
				 return false;*/
				
				fwrite(input, 1, strlen(input), fd);
			}			
		}	
	}
	
	pFolder->CloseNextItem(pos);
	
	return true;
}

- (void)MultibyteToSinglebyte:(CString &)str To:(unsigned char*)buf
{
	size_t nLen = str.GetLength();
	
	for(size_t i=0; i<nLen; i++)
	{
		buf[2*i]   = (str[i]>>0)&0xFF;
		buf[2*i+1] = (str[i]>>8)&0xFF;
	}
	buf[2*nLen]   = 0;
	buf[2*nLen+1] = 0;
}

- (bool)IsModelDlg
{
	return m_bModelDlg;
}

@end
