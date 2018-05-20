#ifndef FAT32_INCLUDE_H
#define FAT32_INCLUDE_H

#include "FileSystem.h"
#include "commutil.h"

class CFat32FileSystem:public CBaseFileSystem
{
public:
	CFat32FileSystem(IBaseReader  *prmReader=NULL);
	virtual ~CFat32FileSystem();
	/*************************************
	*
	*	函数名：	Init
	*	函数说明：	fat32分区格式初始化
	*	返回值：	void
	*
	**************************************/
	virtual		void	Init();

	/*************************************
	*
	*	函数名：	ReadFileContent
	*	函数说明：	读取文件内容
	*	参数描述：	@param prmFileObject[输入参数]待读取文件的对象
	*	参数描述：	@param prmDstBuf[输出参数]读取的内容存入到该缓冲区中
	*	参数描述：	@param prmByteOff[输入参数]表示从文件的多少节字偏移处开始读取
	*	参数描述：	@param prmByteToRead[输入参数]表示读取多少字节
	*	返回值：	返回成功写入到prmDstBuf中的字节数
	*
	**************************************/
	virtual UINT64	ReadFileContent(CBaseFileObject *prmFileObject,
		UCHAR prmDstBuf[], UINT64 prmByteOff, UINT64 prmByteToRead);

	/*************************************
	*
	*	函数名：	GetFileObjectByPath
	*	函数说明：	根据文件的路径获取文件对象
	*	参数描述：	@param prmFileName[输入参数]文件名字符串
	*	返回值：	如果在文件系统中成功找到prmFileName文件返回CBaseFileObject对象
	*				否则返回NULL
	*
	**************************************/
	virtual	CBaseFileObject	*GetFileObjectByPath(LPCTSTR prmFileName);

	/*************************************
	*
	*	函数名：	GetChildren
	*	函数说明：	获取目录中的所有子文件/文件夹对象
	*	参数描述：	@param prmParentDirectory[输入参数]父目录的对象
	*	返回值：	vector<CBaseFileObject*> *
	*
	**************************************/
	virtual	vector<CBaseFileObject*> *GetChildren(CBaseFileObject *prmParentDirectory);

private:

	/***
	*
	*获取文件的下一个簇
	*
	***/
	UINT32    GetNextCluster(UINT32	prmCurCluster);

	//解析短文件名
	void	  ParseShortFileName(DIR_ENTRY_s *prmDirEntry, TCHAR	prmFileName[], size_t prmFileNameLength);

	//解析长文件名
	void	  ParseLongFileName(TCHAR	prmLongFileName[], size_t prmFileNameLength, DIR_ENTRY_s *prmFirstEntry, DIR_ENTRY_s *prmLastEntry);
	
	//从DIR_ENTRY_s目录结构中解析出文件或文件夹的开始簇号
	UINT32	 ParseStartCluster(DIR_ENTRY_s *prmDirEntry);

	/*************************************
	*
	*	函数名：	ParseFileObject
	*	函数说明：	根据获取的fat32目录项解析出文件信息
	*	参数描述：	DIR_ENTRY_s * prmFirstEntry
	*	参数描述：	DIR_ENTRY_s * prmLastEntry
	*	返回值：	CBaseFileObject	*
	*
	**************************************/
	CBaseFileObject	*ParseFileObject(DIR_ENTRY_s *prmFirstEntry,DIR_ENTRY_s *prmLastEntry);
private:
	FAT32_s m_fatSector;
	UINT32	*m_fatTable;
	UINT32	m_fatNum;
};

#endif