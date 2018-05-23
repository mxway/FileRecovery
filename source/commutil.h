/***************************************************
*
*	@Version:	1.0
*	@Author:	Mengxl
*	@Date:		2017-8-27 22:08
*	@File:		commutil.h
*
****************************************************/

#ifndef COMM_UTIL_INCLUDE_H
#define COMM_UTIL_INCLUDE_H
#include <windows.h>
#include<stdlib.h>

#include "StringUtil.h"

using namespace commutil;
//用于将两个字节进行大小端转换
void	rev16bit(UCHAR *value);

//用于将四个字节进行大小端转换
void	rev32bit(UCHAR *value);

//用于将8个字节进行大小端转换
void	rev64bit(UCHAR *value);

#pragma pack(push,1)

//文件数据所占用的数据块，每个文件可能占用连续的簇，也可能占用不连续的多个簇。
typedef struct _File_Content_Extent
{
	UINT64	startSector;//在分区中的起始扇区
	UINT64	totalSector;//连续的块大小，单位为扇区
	UINT8	isPersist;//文件内容是否为常驻属性，只对ntfs有效

	_File_Content_Extent *next;//指向下一个文件块
	_File_Content_Extent()
	{
		isPersist = 0;
		startSector = 0;
		totalSector = 0;
		next = NULL;
	}
}File_Content_Extent_s;
#pragma pack(pop)

#pragma pack(push,1)

typedef struct _FAT32 {
	char BS_jmpBoot[3];
	char BS_OEMName[8];
	unsigned short BPB_BytsPerSec;
	unsigned char BPB_SecPerClus;
	unsigned short BPB_ResvdSecCnt;
	unsigned char BPB_NumFATs;
	unsigned short BPB_RootEntCnt;
	unsigned short BPB_TotSec16;
	char BPB_Media;
	unsigned short BPB_FATSz16;
	unsigned short BPB_SecPerTrk;
	unsigned short NumHeads;
	unsigned int BPB_HiddSec;
	unsigned int BPB_TotSec32;
	unsigned int BPB_FATSz32;
	unsigned short BPB_flags;
	unsigned short BPB_FSVer;
	unsigned int BPB_RootClus;
	unsigned short BPB_FSInfo;
	unsigned short BPB_BkBootSec;
	char BPB_Reserved[12];
	unsigned char BS_DrvNum;
	unsigned char BS_Reserved1;
	char BS_BootSig;
	unsigned int BS_VolID;
	char BS_VolLAB[11];
	char BS_FilSysType[8];
	char code[420];
	char bootsig[2];
}FAT32_s;

typedef struct dir_entry {
	unsigned char   name[11];/* name and extension */
	unsigned char   attr;           /* attribute bits */
	unsigned char   lcase;          /* Case for base and extension */
	unsigned char   ctime_cs;       /* Creation time, centiseconds (0-199) */
	unsigned short  ctime;          /* Creation time */
	unsigned short  cdate;          /* Creation date */
	unsigned short  adate;          /* Last access date */
	unsigned short  starthi;        /* High 16 bits of cluster in FAT32 */
	unsigned short  time, date, start;/* time, date and first cluster */
	unsigned int    size;           /* file size (in bytes) */
}DIR_ENTRY_s;

//文件结束簇标志
#define EOC 0x0FFFFFFF

typedef struct dir_long_entry {
	unsigned char    id;             /* sequence number for slot */
	unsigned char    name0_4[10];    /* first 5 characters in name */
	unsigned char    attr;           /* attribute byte */
	unsigned char    reserved;       /* always 0 */
	unsigned char    alias_checksum; /* checksum for 8.3 alias */
	unsigned char    name5_10[12];   /* 6 more characters in name */
	unsigned short   start;         /* starting cluster number, 0 in long slots */
	unsigned char    name11_12[4];   /* last 2 characters in name */
}DIR_LONG_ENTRY_s;

#pragma pack(pop)

#pragma pack(push,1)

#define MFTREFMASK	0xFFFFFFFFFFFF

//枚举类型,枚举ntfs所有的属性类型
enum NTFS_ATTRDEF
{
	ATTR_STANDARD = 0x10,//标准信息属性
	ATTR_ATTRIBUTE_LIST = 0x20,//属性列表
	ATTR_FILE_NAME = 0x30,//文件名
	ATTR_VOLUME_VERSION = 0x40,//卷版本属性
	ATTR_SECURITY_DESCRIPTOR = 0x50,//安全对象描述
	ATTR_VOLUME_NAME = 0x60,//卷名称
	ATTR_VOLUME_INFOMATION = 0x70,//卷信息
	ATTR_DATA = 0x80,//数据属性
	ATTR_INDEX_ROOT = 0x90,//索引根目录属性
	ATTR_INDEX_ALLOCATION = 0xA0,//外部索引根目录
	ATTR_BITMAP = 0xB0,//位图
	ATTR_SYMLINK = 0xC0,//符号链接
	ATTR_HPFS_EXTENDED_INFO = 0xD0,//hpfs扩展信息
	ATTR_HPFS_EXTENDED = 0xE0,//hpfs扩展
	ATTR_PROPERTY = 0xF0,//所有权设置属性
	ATTR_LOG_STREAM = 0x100//日志作用流属性
};

typedef struct _NTFS_Data_Run
{
	UINT64				lcn;   //数据流的逻辑簇号
	UINT64				vcn;	//数据流的虚拟簇号
	UINT64				length;	//长度，表示该数据流占用多少个簇
	_NTFS_Data_Run		*next;
	_NTFS_Data_Run()
	{
		lcn = 0;
		vcn = 0;
		length = 0;
		next = NULL;
	}
}Ntfs_Data_Run;
#pragma pack(pop)

#endif