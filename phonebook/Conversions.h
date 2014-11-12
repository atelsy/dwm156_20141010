#ifndef _CONVERSION_H
#define _CONVERSION_H

#ifdef _WIN32
#include "StdAfx.h"
#else
#include "Win2Mac.h"
#endif

#define kal_uint16 unsigned short
#define kal_uint8  unsigned char
#define kal_int32  int
#define kal_uint32 unsigned int
#define kal_int16  short

#define gb2312_to_ucs2 mmi_chset_text_to_ucs2_str_ex
#define ucs2_to_gb2312 mmi_chset_ucs2_to_text_str_ex


kal_uint16 mmi_chset_text_to_ucs2_str_ex(
										 kal_uint8 *dest,
										 kal_int32 dest_size,
										 kal_uint8 *src,
										 kal_uint32 *src_end_pos);

kal_uint16 mmi_chset_ucs2_to_text_str_ex(
										 kal_uint8 *dest,
										 kal_int32 dest_size,
										 kal_uint8 *src,
										 kal_uint32 *src_end_pos);
#endif