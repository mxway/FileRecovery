/***************************************************
*
*	@Version:	1.0
*	@Author:	Mengxl
*	@Date:		2017-11-1 22:30
*	@File:		ntfs.h
*
****************************************************/
#ifndef NTFS_INCLUDE_H
#define NTFS_INCLUDE_H
#include "FileSystem.h"
#include "commutil.h"



class CNtfsFileSystem:public CBaseFileSystem
{
public:
	CNtfsFileSystem(IBaseReader *prmReader = NULL);

	~CNtfsFileSystem();
	/*************************************
	*
	*	函数名：	Init
	*	函数说明：	文件分区的初始化，如fat32系统载入
	*				分区引导记录信息，具体视不同的文件系统
	*				实现不同。如果文件系统不需要进行初始化，
	*				该函数的实现体可以为空。
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

	/*************************************
	*
	*	函数名：	GetMFTRunList
	*	函数说明：	获取$MFT文件的0x80属性值，根据根据0x80运行数据生成链表
	*				用于实现根据参考号查找文件簇号
	*	返回值：	void
	*
	**************************************/
	void	GetMFTRunList();

	/*************************************
	*
	*	函数名：	GetAttrValue
	*	函数说明：	获取文件MFT头中指定属性的值
	*	参数描述：	@param prmAttrTitle[输入参数]，待获取的属性值如0x30,0x80等
	*	参数描述：	@param prmBuf[输入参数]文件的MFT标识值
	*	参数描述：	@ prmAttrValue[输出参数]获取到的相应属性对应的数据放入该缓冲区
	*	返回值：	成功找到相应属性返回1，否则返回0
	*
	**************************************/
	UINT32	GetAttrValue(NTFS_ATTRDEF prmAttrTitle,UCHAR prmBuf[],UCHAR *prmAttrValue);

	/*************************************
	*
	*	函数名：	GetAttrFromAttributeList
	*	函数说明：	从$0x20属性表中读取指定属性类型的值
	*	参数描述：	@param prmAttrType[输入参数]待查找属性类型
	*	参数描述：	@param prmOffset[输入参数]从prmAttrList的prmOffset出开始查找
	*					因为一个属性表中可能存在多个相同类型的属性
	*	参数描述：	@param prmAttrList[输入参数]属性表数据缓冲区
	*	参数描述：	@param prmAttrValue[输出参数]查找到的属性数据放入该缓冲区
	*	返回值：	找到指定类型属性返回该属性类型在prmAttrList中的偏移值，否则返回0
	*
	**************************************/
	UINT32	GetAttrFromAttributeList(NTFS_ATTRDEF prmAttrType,UINT32 prmOffset,UCHAR *prmAttrList,UCHAR *prmAttrValue);

	/*************************************
	*
	*	函数名：	GetDataRunList
	*	函数说明：	获取数据运行流
	*	参数描述：	@param prmBuf[输入参数]存放mft记录头的1024字节
	*	参数描述：	@param prmRunListOffset[输入参数]运行流数据在prmBuf中的偏移值
	*	参数描述：	@param prmList[输出参数]获取好的运行流放入到该链表中
	*	返回值：	void
	*
	**************************************/
	void	GetDataRunList(UCHAR *prmBuf,UINT16 prmRunListOffset,Ntfs_Data_Run **prmList);

	/*************************************
	*
	*	函数名：	FreeRunList
	*	函数说明：	释放数据运行流占用的内存空间
	*	参数描述：	@param prmList[输入参数]运行流链表
	*	返回值：	void
	*
	**************************************/
	void	FreeRunList(Ntfs_Data_Run	*prmList);

	/*************************************
	*
	*	函数名：	GetExtendMFTAttrValue
	*	函数说明：	从扩展mft记录头中获取指定属性
	*	参数描述：	@param prmSeqNum[输入参数]扩展mft参考号
	*	参数描述：	@param prmAttrType[输入参数]待获取属性类型
	*	参数描述：	@param prmAttrValue[输出参数]获取到的属性放入该缓冲区中
	*	返回值：	如果找到prmAttrType在mft记录头的值返回1，否则返回0
	*
	**************************************/
	UINT32	GetExtendMFTAttrValue(UINT64 prmSeqNum,NTFS_ATTRDEF prmAttrType,UCHAR *prmAttrValue);

	/*************************************
	*
	*	函数名：	GetOffsetByMFTRef
	*	函数说明：	根据mft参考号计算在文件分区中的偏移值
	*	参数描述：	@param prmSeqNo[输入参数]mft参考号
	*	返回值：	返回mft参考号对应的mft记录头在分区中的偏移字节
	*
	**************************************/
	UINT64	GetOffsetByMFTRef(UINT64 prmSeqNo);

	/*************************************
	*
	*	函数名：	GetFileFromIndexRoot
	*	函数说明：	从MFT记录头内部分配的索引中解析出子文件/文件夹名
					从MFT属性中获取的0x90属性值
	*	参数描述：	@param prmAttrValue[输入参数]0x90内部分配索引属性值
	*	参数描述：	@param * prmFileArray[输出参数]从分区中解析出的文件放入到该变量中
	*	返回值：	void
	*
	**************************************/
	void	GetFileFromIndexRoot(UCHAR *prmAttrValue,vector<CBaseFileObject*> *prmFileArray);

	/*************************************
	*
	*	函数名：	GetFileFromAllocIndex
	*	函数说明：	从外部分配的索引项中找子文件/文件夹
	*	参数描述：	@param prmAttrValue[in]输入参数，0xA0属性值
	*	参数描述：	@param prmFileArray[输出参数]获取的文件放入到该变量中
	*	返回值：	void
	*
	**************************************/
	void	GetFileFromAllocIndex(UCHAR *prmAttrValue,vector<CBaseFileObject*> *prmFileArray);
	
	/*************************************
	*
	*	函数名：	ParseFileFromIndex
	*	函数说明：	从索引表项中解析出所有的文件名
	*	参数描述：	@param prmBuf[输入参数]文件索引项属性值
	*	参数描述：	@param prmOffset[输入参数]第一个索引的偏移值
	*	参数描述：	@param prmBufLen[输入参数]最后一索引尾的偏移值
	*	参数描述：	@param prmFileArray[输出参数]获取的文件放入该变量中
	*	返回值：	void
	*
	**************************************/
	void	ParseFileFromIndex(UCHAR *prmBuf,UINT16 prmOffset,UINT32 prmBufLen,vector<CBaseFileObject*> *prmFileArray);

	/*************************************
	*
	*	函数名：	GetFileWin32Name
	*	函数说明：	获取文件的win32名称，从mft中解析出来的有可能是dos名，这个不是我们想要的
	*	参数描述：	@param prmOffset[输入参数]mft在分区的偏移值
	*	返回值：	返回文件的win32命名空间名称
	*
	**************************************/
	CStringUtil	GetFileWin32Name(UINT64 prmOffset);

	/*************************************
	*
	*	函数名：	IsFileExists
	*	函数说明：	文件是否已经存在
	*	参数描述：	@prmStartSector[输入参数] 文件是否已经存在
	*	参数描述：	@param prmFileArray[输入参数]已解析的文件数组
	*	返回值：	如果文件已经存在prmFileArray数组中返回TRUE,否则返回FALSE
	*
	**************************************/
	BOOL	IsFileExists(UINT64 prmStartSector,vector<CBaseFileObject*> *prmFileArray);
	
	/*************************************
	*
	*	函数名：	GetFileExtent
	*	函数说明：	获取文件占用的簇信息
	*	参数描述：	@param prmBuf[输入参数]文件的mft记录头信息
	*	参数描述：	@param prmMftSector[输入参数]文件的mft起始扇区
	*	参数描述：	@param prmFileExtent[输出参数]获取的文件内容簇信息以链表形式放入该变量
	*	返回值：	void
	*
	**************************************/
	void	GetFileExtent(UCHAR *prmBuf,UINT64 prmMftSector,File_Content_Extent_s **prmFileExtent);

	/*************************************
	*
	*	函数名：	ReadFileContent
	*	函数说明：	读取ntfs分区中文件内容的私有函数
	*	参数描述：	@param prmDstBuf[输出参数]将读取的文件内容放入该缓冲区中
	*	参数描述：	@param prmByteOff[输入参数]从文件内容的prmByteOff偏移处开始读取数据
	*	参数描述：	@param prmByteToRead[输入参数]从文件中读取多少字节的数据
	*	参数描述：	@param prmFileSize[输入参数]文件大小
	*	参数描述：	@param prmFilExtent[输入参数]文件内容所占簇链表
	*	返回值：	返回成功写入到prmDstBuf缓冲区的字节数
	*
	**************************************/
	UINT64	ReadFileContent(UCHAR prmDstBuf[],UINT64 prmByteOff,UINT64 prmByteToRead,
		UINT64 prmFileSize,File_Content_Extent_s *prmFileExtent);

	/*************************************
	*
	*	函数名：	FreeFileExtent
	*	函数说明：	释放文件所占用的簇链表
	*	参数描述：	@param prmFileExtent[输入参数]文件内容所占用的簇链表
	*	返回值：	void
	*
	**************************************/
	void  FreeFileExtent(File_Content_Extent_s *prmFileExtent);

	/*************************************
	*
	*	函数名：	GetOffsetByFileName
	*	函数说明：	根据文件名找到mft记录头的偏移值
	*	参数描述：	@param prmParentFileOffset[输入参数]父文件的mft记录头
					的偏移值
	*	参数描述：	@param prmFileName[输入参数]文件名
	*	返回值：	返回文件在分区的偏移值，如果没有找到返回0
	*
	**************************************/
	UINT64	GetOffsetByFileName(UINT64 prmParentFileOffset,const CStringUtil	&prmFileName);

	/*************************************
	*
	*	函数名：	GetOffsetFromRootByFileName
	*	函数说明：	从0x90索引项中根据文件名查找文件mft偏移值
	*	参数描述：	@param prmAttrValue[输入参数]父目录mft记录头中的0x90索引值
	*	参数描述：	@param prmFileName[输入参数]待查找文件名
	*	返回值：	返回文件的mft记录头偏移值，如果没有找到文件返回0
	*
	**************************************/
	UINT64	GetOffsetFromRootByFileName(UCHAR *prmAttrValue,const CStringUtil &prmFileName);

	/*************************************
	*
	*	函数名：	GetOffsetFromAllocByFileName
	*	函数说明：	从外部索引根据文件名查找mft偏移值
	*	参数描述：	@param prmAttrValue[输入参数]父文件的0xA0属性值
	*	参数描述：	@param prmFileName[输入参数]待查找文件名
	*	返回值：	返回文件mft记录头偏移值，如果没有找到文件返回0
	*
	**************************************/
	UINT64	GetOffsetFromAllocByFileName(UCHAR *prmAttrValue,const CStringUtil &prmFileName);

	/*************************************
	*
	*	函数名：	GetOffsetByFileNameInIndex
	*	函数说明：	从索引项中根据文件名获取文件的mft偏移值
	*	参数描述：	@param prmBuf[输入参数]存放索引项缓冲区
	*	参数描述：	@param prmOffset[输入参数]第一个索引项的偏移值
	*	参数描述：	@param prmBufLen[输入参数]最后一个索引项尾部偏移值
	*	参数描述：	@param prmFileName[输入参数]待查找文件名
	*	返回值：	返回文件的mft记录头偏移值，如果没有找到文件返回0
	*
	**************************************/
	UINT64	GetOffsetByFileNameInIndex(UCHAR *prmBuf,UINT16 prmOffset,UINT32 prmBufLen,const CStringUtil &prmFileName);

	/*************************************
	*
	*	函数名：	GetFileSize
	*	函数说明：	获取文件的大小，
	*	参数描述：	@param prmMFTRecord[输入参数]文件mft记录头数据
	*	返回值：	返回文件的大小
	*
	**************************************/
	UINT64	GetFileSize(UCHAR *prmMFTRecord);

	/*************************************
	*
	*	函数名：	GetFileType
	*	函数说明：	从mft记录头中找到文件的类型（文件/文件夹）
	*	参数描述：	@param prmMFTRecord[输入参数]文件的mft记录头信息
	*	返回值：	返回FILE_OBJECT_TYPE_FILE(文件)或FILE_OBJECT_TYPE_DIRECTORY(文件夹)
	*
	**************************************/
	FILE_OBJECT_TYPE	GetFileType(UCHAR *prmMFTRecord);
private:
	UINT64		m_mftStartCluster;//$MFT开始簇号
	UINT64		m_clustersPerIndex;//每个索引占用簇数
	Ntfs_Data_Run	*m_mftRunList;//MFT的数据运行项，用于实现根据参考号进行文件mft查找
};
#endif