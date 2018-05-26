#include <stdio.h>
#ifdef _DEBUG
//#include <vld.h>
#endif
#include "FileSystemFactory.h"

void RestoreFile(CBaseFileSystem *prmFileSystem, CBaseFileObject *prmFileObject)
{
	char	*tmpBuf = (char*)malloc(1024*1024);
	memset(tmpBuf, 0, 1024*1024);
	UINT64	tmpFileSize = prmFileObject->GetFileSize();
	UINT64	tmpBytesRead = 0;
	FILE	*fp = NULL;
	fopen_s(&fp, "F:\\test.zip", "wb");
	while (tmpBytesRead < tmpFileSize)
	{
		UINT64  tmpVal = prmFileSystem->ReadFileContent(prmFileObject, (UCHAR*)tmpBuf, tmpBytesRead, 1024*1024);
		if (tmpVal == 0)
		{
			break;
		}
		tmpBytesRead += tmpVal;
		fwrite(tmpBuf, 1, (DWORD)tmpVal, fp);
	}
	free(tmpBuf);
	fclose(fp);
}

int main()
{
	CBaseFileSystem	*fileSystem = CFileSystemFactory::GetFileSystem(TEXT("G:\\"));
	if (fileSystem == NULL)
	{
		return -1;
	}
	vector<CBaseFileObject *> fileArray;
	fileSystem->GetDeletedFiles(fileArray);
	if (fileArray.size() > 0)
	{
		RestoreFile(fileSystem, fileArray[0]);
	}
	delete fileSystem;
	for (int i = 0; i < fileArray.size(); i++)
	{
		fileArray[i]->Destroy();
	}
	return 0;
}