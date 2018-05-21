#include <stdio.h>
#include "FileSystem.h"

int main()
{
	IBaseReader	*tmpReader = new CSectorReader;
	if (!tmpReader->OpenDevice(TEXT("D:\\")))
	{
		delete tmpReader;
		return -1;
	}
	
	return 0;
}