#include "File.h"

#include "commutil.h"

CBaseFileObject::CBaseFileObject()
{
	m_fileExtent = NULL;
}

CBaseFileObject::~CBaseFileObject()
{
	File_Content_Extent_s	*p = m_fileExtent;
	File_Content_Extent_s	*q = p;
	while (p != NULL)
	{
		p = p->next;
		delete q;
		q = p;
	}
}

void	CBaseFileObject::SetFileName(const CStringUtil &prmFileName)
{
	m_fileName = prmFileName;
}

const CStringUtil &CBaseFileObject::GetFileName()
{
	return m_fileName;
}

void	CBaseFileObject::SetFileStartSector(UINT64 prmSector)
{
	m_startSector = prmSector;
}

UINT64	CBaseFileObject::GetFileStartSector()
{
	return m_startSector;
}

void CBaseFileObject::SetFileType(FILE_OBJECT_TYPE	prmObjectType)
{
	m_objectType = prmObjectType;
}

FILE_OBJECT_TYPE	CBaseFileObject::GetFileType()
{
	return m_objectType;
}

void CBaseFileObject::SetFileSize(UINT64 prmFileSize)
{
	m_fileSize = prmFileSize;
}

UINT64	CBaseFileObject::GetFileSize()
{
	return m_fileSize;
}

void CBaseFileObject::Destroy()
{
	delete this;
}