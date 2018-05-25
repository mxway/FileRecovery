#include "FileSystem.h"
#include "commutil.h"

CBaseFileSystem::CBaseFileSystem(IBaseReader *prmReader)
	:m_reader(prmReader)
{

}

CBaseFileSystem::~CBaseFileSystem()
{
	delete m_reader;
}

UINT64	CBaseFileSystem::ReadBuf(UCHAR prmBuf[],UINT64 prmStartSector,UINT64 prmByteToRead)
{
	//
	return m_reader->ReadSector(prmStartSector+m_startSector, prmByteToRead, prmBuf);
}
