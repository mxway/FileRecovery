#include "commutil.h"
void	rev16bit(UCHAR *value)
{
	UCHAR	szResult[2] ={0};
	szResult[0] = value[1];
	szResult[1] = value[0];
	memcpy(value,szResult,2);
}

void	rev32bit(UCHAR *value)
{
	UCHAR	szResult[4] = {0};
	szResult[0] = value[3];
	szResult[1] = value[2];
	szResult[2] = value[1];
	szResult[3] = value[0];
	memcpy(value,szResult,4);
}

void	rev64bit(UCHAR *value)
{
	UCHAR szResult[8] = {0};
	szResult[0] = value[7];
	szResult[1] = value[6];
	szResult[2] = value[5];
	szResult[3] = value[4];
	szResult[4] = value[3];
	szResult[5] = value[2];
	szResult[6] = value[1];
	szResult[7] = value[0];
	memcpy(value,szResult,8);
}