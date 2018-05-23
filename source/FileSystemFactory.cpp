#include "FileSystemFactory.h"
#include "ntfs.h"
#include "fat32.h"

CBaseFileSystem	*CFileSystemFactory::GetFileSystem(const TCHAR *prmDisk)
{
	//读取分区第一个扇区的数据，用于判断分区的文件系统类型
	UCHAR		tmpBuf[512] = { 0 };
	//ntfs文件格式标识
	UCHAR    ntfsSign[4] = { 0x4e, 0x54, 0x46, 0x53 };

	//fat32文件格式标识
	UCHAR    fat32Sign[5] = { 0x46, 0x41, 0x54, 0x33, 0x32 };

	IBaseReader	*tmpReader = new CSectorReader;
	if (!tmpReader->OpenDevice(prmDisk))
	{
		delete tmpReader;
		return NULL;
	}
	if (tmpReader->ReadSector(0, 512, tmpBuf) != 512)
	{
		delete tmpReader;
		return NULL;
	}
	CBaseFileSystem	*fileSystem = NULL;
	if (memcmp(&tmpBuf[3], ntfsSign, 4) == 0)
	{
		fileSystem = new CNtfsFileSystem(tmpReader);
		fileSystem->SetStartSector(0);
		UINT64 *tmpTotalSectors = (UINT64*)(tmpBuf + 0x28);
		fileSystem->SetTotalSector(*tmpTotalSectors);
		fileSystem->Init();
	}
	else if (memcmp(&tmpBuf[0x52], fat32Sign, 5) == 0)
	{
		fileSystem = new CFat32FileSystem(tmpReader);
		fileSystem->SetStartSector(0);
		UINT32 *tmpTotalSectors = (UINT32*)(tmpBuf + 0x20);
		fileSystem->SetTotalSector(*tmpTotalSectors);
		fileSystem->Init();
	}
	return fileSystem;
}