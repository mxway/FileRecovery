#include "fat32.h"
#include <shlwapi.h>
#include <windows.h>
#include <queue>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "commutil.h"

#pragma comment(lib,"shlwapi.lib")

using namespace std;

CFat32FileSystem::CFat32FileSystem(IBaseReader	*prmReader)
	:CBaseFileSystem(prmReader), m_clusterFlag(NULL)
{
	m_fatTable = NULL;
}

CFat32FileSystem::~CFat32FileSystem()
{
	free(m_fatTable);
	free(m_clusterFlag);
}

void CFat32FileSystem::Init()
{
	//读取fat32头部数据
	this->ReadBuf((UCHAR*)&m_fatSector,0,512);

	m_fatNum = m_fatSector.BPB_FATSz32 * m_fatSector.BPB_BytsPerSec / 4;
	m_fatTable = (UINT32*)malloc(sizeof(UINT32)*m_fatNum);
	memset(m_fatTable, 0, sizeof(UINT32)*m_fatNum);
	if (m_fatTable == NULL)
	{
		return;
	}
	m_clusterFlag = (UCHAR *)malloc(sizeof(UCHAR)*m_fatNum);
	if (m_clusterFlag == NULL)
	{
		return;
	}
	memset(m_clusterFlag, 0, m_fatNum);
	//读取文件分配表
	this->ReadBuf((UCHAR*)m_fatTable, m_fatSector.BPB_ResvdSecCnt, sizeof(UINT32)*m_fatNum);
	m_bytesPerSector = m_fatSector.BPB_BytsPerSec;
	m_sectorsPerCluster = m_fatSector.BPB_SecPerClus;

	//读取fat32根目录信息
	m_rootDirectory = new CBaseFileObject;
	m_rootDirectory->SetFileType(FILE_OBJECT_TYPE_ROOT);
	m_rootDirectory->SetFileSize(0);
	m_rootDirectory->SetFileName(TEXT(""));
	m_rootDirectory->SetPath(TEXT(""));
	UINT64	tmpRootStartSector = m_fatSector.BPB_FATSz32 * m_fatSector.BPB_NumFATs;
	tmpRootStartSector += m_fatSector.BPB_ResvdSecCnt;
	tmpRootStartSector += (2-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
	m_rootDirectory->SetFileStartSector(tmpRootStartSector);
}

UINT64	CFat32FileSystem::ReadFileContent(CBaseFileObject *prmFileObject, UCHAR prmDstBuf[], UINT64 prmByteOff, UINT64 prmByteToRead)
{
	if(prmFileObject->GetFileType()!=FILE_OBJECT_TYPE_FILE)
	{
		return 0;
	}
	UINT64		tmpFileSize = prmFileObject->GetFileSize();
	if(tmpFileSize>=0xFFFFFFFF)
	{
		return 0;
	}
	if(prmByteOff>=tmpFileSize)
	{
		return 0;
	}
	UINT32		tmpRemainSize = (UINT32)tmpFileSize-(UINT32)prmByteOff;
	if(tmpRemainSize<prmByteToRead)
	{
		prmByteToRead = tmpRemainSize;
	}
	UINT32		tmpClusterSize = m_fatSector.BPB_SecPerClus*m_fatSector.BPB_BytsPerSec;
	UINT32		tmpClusterNum = (UINT32)prmFileObject->GetFileStartSector();
	//根据分区的扇区号计算簇号
	tmpClusterNum -= m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
	tmpClusterNum -= m_fatSector.BPB_ResvdSecCnt;
	tmpClusterNum = tmpClusterNum/m_fatSector.BPB_SecPerClus+2;

	while(prmByteOff>=tmpClusterSize && tmpClusterNum)
	{
		prmByteOff -= tmpClusterSize;
		tmpClusterNum = this->GetNextCluster(tmpClusterNum);
	}
	if(tmpClusterNum==0)
	{
		//非法请求
		return 0;
	}
	char	*tmpBuf = (char*)malloc(tmpClusterSize);
	if(tmpBuf==NULL)
	{
		return 0;
	}
	UINT32 tmpOffset = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
	tmpOffset += m_fatSector.BPB_ResvdSecCnt;
	tmpOffset += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
	//读取prmByteOff所有fat32分区中的簇数据
	UINT64 tmpRet = this->ReadBuf((UCHAR*)tmpBuf,tmpOffset,tmpClusterSize);
	UINT64		tmpResult = 0;
	if(prmByteToRead<=tmpClusterSize-prmByteOff)
	{
		//读取的数据小于一簇大小
		memcpy(prmDstBuf,tmpBuf+(UINT32)prmByteOff,(UINT32)prmByteToRead);
		tmpResult = prmByteToRead;
		goto SUCCESS;
	}
	else
	{
		//将一簇中有用数据放入缓冲区
		memcpy(prmDstBuf,tmpBuf+(UINT32)prmByteOff,tmpClusterSize-(UINT32)prmByteOff);
		tmpResult = tmpClusterSize-prmByteOff;
		tmpClusterNum = this->GetNextCluster(tmpClusterNum);
		//减少待读取数据大小
		prmByteToRead = prmByteToRead - tmpResult;
	}
	//剩下的数据整簇读取
	for(int i=0;i<prmByteToRead/tmpClusterSize;i++)
	{
		tmpOffset = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
		tmpOffset += m_fatSector.BPB_ResvdSecCnt;
		tmpOffset += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
		this->ReadBuf(prmDstBuf+tmpResult,tmpOffset,tmpClusterSize);
		tmpResult += tmpClusterSize;
		prmByteToRead = prmByteToRead - tmpClusterSize;
		tmpClusterNum = this->GetNextCluster(tmpClusterNum);
	}
	//考虑可能还有数据需要读取，但不够一簇大小
	if(prmByteToRead>0)
	{
		tmpOffset = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
		tmpOffset += m_fatSector.BPB_ResvdSecCnt;
		tmpOffset += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
		this->ReadBuf(prmDstBuf+tmpResult,tmpOffset,prmByteToRead);
		tmpResult += prmByteToRead;
	}
SUCCESS:
	free(tmpBuf);
	return tmpResult;
}

CBaseFileObject *CFat32FileSystem::GetFileObjectByPath(LPCTSTR prmFileName)
{
	if(_tcslen(prmFileName)>MAX_PATH*4)
	{
		return NULL;
	}
	TCHAR		tmpFullFileName[MAX_PATH*4] = {0};
	memcpy(tmpFullFileName,prmFileName,_tcslen(prmFileName)*sizeof(TCHAR));
	CStringUtil		tmpString(prmFileName);
	tmpString.ReplaceStr(_T("/"),_T("\\"));
	vector<CStringUtil>		tmpStrArray;
	//将文件名以"\"进行分隔
	tmpString.SplitString(tmpStrArray,_T("\\"));
	if(tmpStrArray.size()==0)
	{
		return NULL;
	}
	TCHAR		tmpFileName[MAX_PATH] = {0};

	//逻辑簇大小
	UINT32		tmpClusterSize = m_fatSector.BPB_BytsPerSec*m_fatSector.BPB_SecPerClus;
	UCHAR		*tmpBuf = (UCHAR*)malloc(sizeof(UCHAR)*tmpClusterSize);

	DIR_ENTRY_s		*tmpDirEntry;
	//
	UINT32			tmpClusterNum = 2;
	UINT32			tmpOffset = 0;
	UINT32			i = 0;
	for(i=0;i<tmpStrArray.size();i++)
	{
		while(tmpClusterNum)
		{
			tmpOffset = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
			tmpOffset += m_fatSector.BPB_ResvdSecCnt;
			tmpOffset += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
			//每次读取一个逻辑簇大小到缓存
			this->ReadBuf(tmpBuf,tmpOffset,tmpClusterSize);
			UINT32	 j = 0;
			for(j=0;j<tmpClusterSize;j+=32)
			{
				tmpDirEntry = (DIR_ENTRY_s*)(tmpBuf+j);
				if(tmpDirEntry->name[0]==0)
				{
					break;
				}
				//记录当前文件项
				DIR_ENTRY_s	*tmpFirstEntry = tmpDirEntry;
				//找到第一个非长文件名标识的文件项
				while(tmpDirEntry->attr==0x0F)
				{
					j+=32;
					tmpDirEntry = (DIR_ENTRY_s*)(tmpBuf+j);
				}
				if(tmpDirEntry->name[0]==0xE5)//不考虑删除的文件
				{
					continue;
				}
				if(tmpFirstEntry==tmpDirEntry)
				{
					this->ParseShortFileName(tmpDirEntry,tmpFileName,MAX_PATH);
				}
				else
				{
					this->ParseLongFileName(tmpFileName,MAX_PATH,tmpFirstEntry,tmpDirEntry);
				}
				if(tmpStrArray[i].CompareNoCase(tmpFileName)==0)
				{
					break;
				}
			}
			if(j>=tmpClusterSize)
			{
				tmpClusterNum = this->GetNextCluster(tmpClusterNum);
			}
			else
			{
				tmpClusterNum = this->ParseStartCluster(tmpDirEntry);
				break;
			}
		}
	}
	
	if(i>=tmpStrArray.size())
	{
		CBaseFileObject	*tmpFileObject = new CBaseFileObject;
		if(tmpDirEntry->attr & 0x10)
		{
			tmpFileObject->SetFileSize(0);
			tmpFileObject->SetFileType(FILE_OBJECT_TYPE_DIRECTORY);
		}
		else
		{
			tmpFileObject->SetFileSize(tmpDirEntry->size);
			tmpFileObject->SetFileType(FILE_OBJECT_TYPE_FILE);
		}
		UINT32		tmpClusterNum = this->ParseStartCluster(tmpDirEntry);
		UINT32		tmpSectors = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
		tmpSectors += m_fatSector.BPB_ResvdSecCnt;
		tmpSectors += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
		tmpFileObject->SetFileStartSector(tmpSectors);
		tmpFileObject->SetFileName(tmpStrArray[tmpStrArray.size()-1]);
		::PathRemoveFileSpec(tmpFullFileName);
		tmpFileObject->SetPath(tmpFullFileName);
		free(tmpBuf);
		return tmpFileObject;
	}
	free(tmpBuf);
	return 0;
}

vector<CBaseFileObject*> *CFat32FileSystem::GetChildren(CBaseFileObject *prmParentDirectory)
{
	if(prmParentDirectory->GetFileType()==FILE_OBJECT_TYPE_FILE)
	{
		return NULL;
	}
	vector<CBaseFileObject*> *tmpArray = new vector<CBaseFileObject*>;
	DIR_ENTRY_s		*tmpDirEntry = { 0 };
	//int		tmpIndex = prmParentDirectory->GetFileType()==FILE_OBJECT_TYPE_ROOT?32:64;
	UINT32	tmpClusterNum = (UINT32)prmParentDirectory->GetFileStartSector();
	//fat32文件系统根据文件的扇区号计算文件的簇号
	tmpClusterNum -= m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
	tmpClusterNum -= m_fatSector.BPB_ResvdSecCnt;
	tmpClusterNum = tmpClusterNum/m_fatSector.BPB_SecPerClus+2;
	//每簇多少字节
	UINT32		tmpClusterSize = m_fatSector.BPB_SecPerClus*m_fatSector.BPB_BytsPerSec;
	//存放每簇的数据
	char		*tmpClusterBuf = (char*)malloc(tmpClusterSize+32);
	if(tmpClusterBuf==NULL)
	{
		return tmpArray;
	}
	memset(tmpClusterBuf,0,tmpClusterSize+32);
	CStringUtil		tmpPath = prmParentDirectory->GetPath();
	while(tmpClusterNum)
	{
		UINT32	tmpOffset = m_fatSector.BPB_FATSz32 * m_fatSector.BPB_NumFATs;
		tmpOffset += m_fatSector.BPB_ResvdSecCnt;
		tmpOffset += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
		//读取一簇到内存中，然后解析所有的文件信息
		this->ReadBuf((UCHAR*)tmpClusterBuf,tmpOffset,tmpClusterSize);
		for(UINT32 i=0;i<tmpClusterSize;i+=32)
		{
			tmpDirEntry = (DIR_ENTRY_s *)(tmpClusterBuf+i);
			if(tmpDirEntry->name[0]==0)
			{
				break;
			}
			//如果当前是卷标目录或都当前目录(.)或父目录(..)，则跳过，不做处理
			if( (tmpDirEntry->attr&0x80) || tmpDirEntry->name[0]=='.' )
			{
				continue;
			}
			DIR_ENTRY_s	*tmpFirstEntry = tmpDirEntry;
			//找到第一个非长文件名标识的Dir_Entry项
			while(tmpDirEntry->attr==0xF)
			{
				i+=32;
				tmpDirEntry = (DIR_ENTRY_s *)(tmpClusterBuf+i);
			}
			if(tmpDirEntry->name[0]==0xE5)
			{
				//删除文件标识，不要删除的文件
				continue;
			}
			CBaseFileObject	*tmpFileObject = this->ParseFileObject(tmpFirstEntry,tmpDirEntry);
			
			if(prmParentDirectory->GetFileType()!=FILE_OBJECT_TYPE_ROOT)
			{
				tmpFileObject->SetPath(tmpPath+"\\"+prmParentDirectory->GetFileName());
			}
			tmpArray->push_back(tmpFileObject);
		}
		if(tmpDirEntry->name[0]==0)
		{
			break;
		}
		tmpClusterNum = this->GetNextCluster(tmpClusterNum);
	}
	free(tmpClusterBuf);
	return tmpArray;
}

void CFat32FileSystem::GetDeletedFiles(vector<FileInfo *> &fileArray, UINT32 *prmRunningFlag)
{
	TCHAR	fileName[MAX_PATH] = { 0 };
	//FileInfo	fileInfo;
	UINT32	clusNum = 0;
	UINT64	offset = 0;
	queue<UINT32> dirs;
	//this->FreeArray(fileArray);
	dirs.push(2);//首先把root的簇号放到队列中

	UINT32	clusterSize = m_fatSector.BPB_BytsPerSec*m_fatSector.BPB_SecPerClus;
	UCHAR	*szBuf = (UCHAR*)malloc(sizeof(UCHAR)*clusterSize);
	while (!dirs.empty() && (*prmRunningFlag))
	{
		clusNum = dirs.front();
		dirs.pop();
		//遍历文件夹的第个簇
		while (*prmRunningFlag)
		{
			offset = m_fatSector.BPB_FATSz32 * m_fatSector.BPB_NumFATs;
			offset += m_fatSector.BPB_ResvdSecCnt;
			offset += (clusNum - m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
			this->ReadBuf(szBuf, offset, clusterSize);

			//遍历簇中的每个文件项
			for (UINT32 i = 0; i < clusterSize && (*prmRunningFlag); i += 32)
			{
				DIR_ENTRY_s *dirEntry = (DIR_ENTRY_s*)(szBuf + i);
				if (dirEntry->name[0] == 0)
				{
					break;
				}
				if (dirEntry->size == 0 || dirEntry->size == 0xFFFFFFFF)
				{
					//如果是文件夹，则需要分析文件夹中是否在删除的文件
					UINT32	tmpClusterNum = this->ParseStartCluster(dirEntry);
					if (tmpClusterNum != 0 && tmpClusterNum < m_fatNum && m_clusterFlag[tmpClusterNum] == 0)
					{
						dirs.push(tmpClusterNum);
						m_clusterFlag[tmpClusterNum] = 1;//避免重复对同一簇进行分析
					}
					//dirs.push(this->ParseStartCluster(dirEntry));
				}

				//记录当前文件项
				DIR_ENTRY_s *firstEntry = dirEntry;
				//找到第一个非长文件名标志的文件项
				while (dirEntry->attr == 0x0F)
				{
					i += 32;
					dirEntry = (DIR_ENTRY_s *)(szBuf + i);
				}
				//被删除的文件
				if (dirEntry->name[0] == 0xE5 && dirEntry->size != 0 && dirEntry->size != 0xFFFFFFFF)
				{
					if (firstEntry == dirEntry)
					{
						this->ParseShortFileName(dirEntry, fileName, MAX_PATH);
					}
					else
					{
						this->ParseLongFileName(fileName, MAX_PATH, firstEntry, dirEntry);
					}
					FileInfo *fileInfo = new FileInfo;
					if (fileInfo == NULL)
					{
						break;
					}
					fileInfo->fileName = fileName;
					fileInfo->fileSize = this->ParseFileSize(dirEntry);
					fileInfo->m_fileExtent = NULL;
					this->ParseFileExtent(dirEntry, &fileInfo->m_fileExtent);
					this->ParseAccessDate(dirEntry, fileInfo);
					this->ParseCreateDate(dirEntry, fileInfo);
					this->ParseModifyDate(dirEntry, fileInfo);
					fileArray.push_back(fileInfo);
				}
			}
			clusNum = this->GetNextCluster(clusNum);
			if (clusNum == 0)
			{
				break;
			}
		}
	}
	free(szBuf);
}

UINT32	CFat32FileSystem::GetNextCluster(UINT32 prmCurCluster)
{
	if (prmCurCluster >= m_fatNum)
	{
		return 0;
	}
	if (m_fatTable[prmCurCluster] == EOC || m_fatTable[prmCurCluster] == 0x0FFFFFF8)
	{
		return 0;
	}
	return m_fatTable[prmCurCluster];
}

void CFat32FileSystem::ParseShortFileName(DIR_ENTRY_s *prmDirEntry, TCHAR prmFileName[], size_t prmFileNameLength)
{
	char	tmpFileName[20] = {0};
	memset(prmFileName, 0, prmFileNameLength);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (prmDirEntry->name[i] == ' ')
		{
			break;
		}
		tmpFileName[i] = prmDirEntry->name[i];
	}
	if (prmDirEntry->name[8] != ' ')
	{
		tmpFileName[i++] = '.';
	}
	for (int j = 8; j < 11; j++)
	{
		if (prmDirEntry->name[j] == ' ')
		{
			break;
		}
		tmpFileName[i++] = prmDirEntry->name[j];
	}
#ifdef _UNICODE
	::MultiByteToWideChar(CP_THREAD_ACP,0,tmpFileName,-1,prmFileName,prmFileNameLength);
#else
	memcpy(prmFileName,tmpFileName,prmFileNameLength);
#endif
}

void CFat32FileSystem::ParseLongFileName(TCHAR prmLongFileName[], size_t prmFileNameLength, DIR_ENTRY_s *prmFirstEntry, DIR_ENTRY_s *prmLastEntry)
{
	TCHAR	tmpFileName[1024] = { 0 };
	memset(prmLongFileName, 0, prmFileNameLength);
	UINT32	index = 0;
	//长文件名的获取方式为从下向上找的顺序排列
	DIR_LONG_ENTRY_s *startEntry = (DIR_LONG_ENTRY_s*)(prmFirstEntry);
	DIR_LONG_ENTRY_s *dirLongEntry = (DIR_LONG_ENTRY_s*)(prmLastEntry);
	dirLongEntry--;
	while (startEntry != dirLongEntry)
	{
		memcpy((char*)tmpFileName + index, dirLongEntry->name0_4, 10);
		index += 10;
		memcpy((char*)tmpFileName + index, dirLongEntry->name5_10, 12);
		index += 12;
		memcpy((char*)tmpFileName + index, dirLongEntry->name11_12, 4);
		index += 4;
		dirLongEntry--;
	}
	memcpy((char*)tmpFileName + index, startEntry->name0_4, 10);
	index += 10;
	memcpy((char*)tmpFileName + index, startEntry->name5_10, 12);
	index += 12;
	memcpy((char*)tmpFileName + index, startEntry->name11_12, 4);
	index += 4;
#ifndef _UNICODE
	DWORD tmpLength = ::WideCharToMultiByte(CP_THREAD_ACP, 0, (WCHAR*)tmpFileName, -1, NULL, 0, 0, 0);
	if (tmpLength > prmFileNameLength)
	{
		return;
	}
	::WideCharToMultiByte(CP_THREAD_ACP, 0, (WCHAR*)tmpFileName, -1, prmLongFileName, prmFileNameLength, 0, 0);
#else
	memcpy(prmLongFileName,tmpFileName,_tcslen(tmpFileName)*sizeof(TCHAR));
#endif
}

UINT32	CFat32FileSystem::ParseStartCluster(DIR_ENTRY_s *prmDirEntry)
{
	UINT32 result = (prmDirEntry->starthi << 16)&(0xFFFF0000);
	result = result | (prmDirEntry->start & 0xFFFF);
	if (result == EOC || result == 0x0FFFFFF8)
	{
		return 0;
	}
	return result;
}

CBaseFileObject *CFat32FileSystem::ParseFileObject(DIR_ENTRY_s *prmFirstEntry,DIR_ENTRY_s *prmLastEntry)
{
	TCHAR		tmpFileName[1024] = {0};
	CBaseFileObject		*tmpFileObject = new CBaseFileObject;
	UINT32		tmpSectors = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
	tmpSectors += m_fatSector.BPB_ResvdSecCnt;
	if(tmpFileObject==NULL)
	{
		return tmpFileObject;
	}
	if(prmFirstEntry == prmLastEntry)
	{
		this->ParseShortFileName(prmFirstEntry,tmpFileName,sizeof(tmpFileName));
	}
	else
	{
		this->ParseLongFileName(tmpFileName,sizeof(tmpFileName),prmFirstEntry,prmLastEntry);
	}
	tmpFileObject->SetFileName(tmpFileName);
	//解析出子文件开始的簇号，转换为相对于分区偏移的扇区数进行保存
	UINT32		tmpClusterNum = this->ParseStartCluster(prmLastEntry);
	tmpSectors += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
	tmpFileObject->SetFileStartSector(tmpSectors);
	if(prmLastEntry->attr & 0x10)
	{
		//目录属性
		tmpFileObject->SetFileSize(0);
		tmpFileObject->SetFileType(FILE_OBJECT_TYPE_DIRECTORY);
	}
	else
	{
		tmpFileObject->SetFileSize(prmLastEntry->size);
		tmpFileObject->SetFileType(FILE_OBJECT_TYPE_FILE);
	}
	return tmpFileObject;
}

UINT32 CFat32FileSystem::ParseFileExtent(DIR_ENTRY_s *dirEntry, File_Content_Extent_s **prmExtent)
{
	File_Content_Extent_s *p = *prmExtent;
	UINT32	baseSector = m_fatSector.BPB_NumFATs*m_fatSector.BPB_FATSz32 + m_fatSector.BPB_ResvdSecCnt;
	UINT32	startCluster = this->ParseStartCluster(dirEntry);
	UINT32	clusterSize = m_fatSector.BPB_SecPerClus;
	if (startCluster == 0)
	{
		return 0;
	}
	if (p == NULL)
	{
		p = new File_Content_Extent_s;
		*prmExtent = p;
	}
	p->startSector = startCluster*m_fatSector.BPB_SecPerClus + baseSector - m_fatSector.BPB_RootClus*m_fatSector.BPB_SecPerClus;
	UINT32 fileSize = dirEntry->size;
	fileSize += 511;
	fileSize = fileSize & ~(511);
	p->totalSector = fileSize >> 9;

	return 1;
}

void CFat32FileSystem::ParseCreateDate(DIR_ENTRY_s *dirEntry, FileInfo *fileInfo)
{
	if (fileInfo == NULL)
	{
		return;
	}
	char	szBuf[128] = { 0 };
	USHORT	time = dirEntry->ctime;
	USHORT	date = dirEntry->cdate;
	UINT8   hour = (time & 0xF800) >> 11;
	UINT8   minute = (time & 0x7E0) >> 5;
	UINT8	second = (time & 0x1F);
	UINT8	year = (date & 0xFE00) >> 9;
	UINT8   month = (date & 0x1E0) >> 5;
	UINT8	day = (date & 0x1F);
	sprintf_s(szBuf, 128, _T("%04d-%02d-%02d %02d:%02d:%02d"), year + 1980, month, day, hour, minute, second);
	fileInfo->createDate = szBuf;
}

void CFat32FileSystem::ParseModifyDate(DIR_ENTRY_s *dirEntry, FileInfo *fileInfo)
{
	if (fileInfo == NULL)
	{
		return;
	}
	char	szBuf[128] = { 0 };
	USHORT	time = dirEntry->time;
	USHORT	date = dirEntry->date;
	UINT8   hour = (time & 0xF800) >> 11;
	UINT8   minute = (time & 0x7E0) >> 5;
	UINT8	second = (time & 0x1F);
	UINT8	year = (date & 0xFE00) >> 9;
	UINT8   month = (date & 0x1E0) >> 5;
	UINT8	day = (date & 0x1F);
	sprintf_s(szBuf, 128, _T("%04d-%02d-%02d %02d:%02d:%02d"), year + 1980, month, day, hour, minute, second);
	fileInfo->modifyDate = szBuf;
}

void CFat32FileSystem::ParseAccessDate(DIR_ENTRY_s *dirEntry, FileInfo *fileInfo)
{
	if (fileInfo == NULL)
	{
		return;
	}
	char	szBuf[128] = { 0 };
	USHORT	date = dirEntry->adate;
	UINT8	year = (date & 0xFE00) >> 9;
	UINT8   month = (date & 0x1E0) >> 5;
	UINT8	day = (date & 0x1F);
	sprintf_s(szBuf, 128, _T("%04d-%02d-%02d"), year + 1980, month, day);
	fileInfo->modifyDate = szBuf;
}