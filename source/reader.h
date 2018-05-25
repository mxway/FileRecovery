/***************************************************
*
*	@Version:	1.0
*	@Author:	Mengxl
*	@Date:		2018-5-20 22:00
*	@File:		reader.h
*	用于定义以扇区读取数据的类
*
****************************************************/

#ifndef READER_INCLUDE_H
#define READER_INCLUDE_H
#include <windows.h>

class IBaseReader
{
public:
	virtual	~IBaseReader(){}
	
	/*************************************
	*
	*	函数名：	OpenDevice
	*	函数说明：	打开一个分区或磁盘，用于提供读取数据来源
	*	参数描述：	@param prmDevice[输入参数]表示设备的标识符
	*	返回值：	打开成功返回true，否则返回false
	*
	**************************************/
	virtual	bool	OpenDevice(const TCHAR *prmDevice) = 0;

	/*************************************
	*
	*	函数名：	ReadSector
	*	函数说明：	读取虚拟磁盘中的数据
	*	参数描述：	@param prmStartSector[输入参数]虚拟磁盘中的待读取起始扇区号
	*	参数描述：	@param prmBytesToRead[输入参数]待读取的字节总数
	*	参数描述：	@param prmBuf[输出参数]将成功读取的数据写入到该缓冲区中
	*	返回值：	返回成功写入到prmBuf中的字节数
	*
	**************************************/
	virtual	UINT64	ReadSector(UINT64 prmStartSector, UINT64 prmBytesToRead, UCHAR *prmBuf) = 0;
};

//该类为IBaseReader的一个子类，用于在windows下对分区按钮扇区进行读取
class CSectorReader:public IBaseReader
{
public:
	CSectorReader();

	~CSectorReader();
	/*************************************
	*
	*	函数名：	OpenDevice
	*	函数说明：	打开一个分区或磁盘，用于提供读取数据来源
	*	参数描述：	@param prmDevice[输入参数]表示设备的标识符
	*	返回值：	打开成功返回true，否则返回false
	*
	**************************************/
	virtual	bool OpenDevice(const TCHAR *prmDevice);

	/*************************************
	*
	*	函数名：	ReadSector
	*	函数说明：	读取虚拟磁盘中的数据
	*	参数描述：	@param prmStartSector[输入参数]虚拟磁盘中的待读取起始扇区号
	*	参数描述：	@param prmBytesToRead[输入参数]待读取的字节总数
	*	参数描述：	@param prmBuf[输出参数]将成功读取的数据写入到该缓冲区中
	*	返回值：	返回成功写入到prmBuf中的字节数
	*
	**************************************/
	virtual	UINT64	ReadSector(UINT64 prmStartSector, UINT64 prmBytesToRead, UCHAR *prmBuf);
private:
	HANDLE	m_diskHandle;//打开分区的句柄
};

#endif