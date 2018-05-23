#include <stdio.h>
#ifdef _DEBUG
#include <vld.h>
#endif
#include "FileSystemFactory.h"

int main()
{
	CBaseFileSystem	*fileSystem = CFileSystemFactory::GetFileSystem(TEXT("F:\\"));
	vector<CBaseFileObject *> fileArray;
	fileSystem->GetDeletedFiles(fileArray);

	delete fileSystem;
	for (int i = 0; i < fileArray.size(); i++)
	{
		fileArray[i]->Destroy();
	}
	return 0;
}