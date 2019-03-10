#include "StringUtil.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include "StringUtil.h"


#include "commutil.h"

using namespace std;
using namespace commutil;

//无参构造函数
CStringUtil::CStringUtil() :m_Buf(NULL), m_BufLen(0), m_Capacity(0)
{
	Assign(10);
}

//参数为char* 或wchar_t*型的字符串
CStringUtil::CStringUtil(LPCTSTR prmStr) : m_Buf(NULL), m_BufLen(0), m_Capacity(0)
{
	Assign(10);
	this->Append(prmStr);
}

//参数为char*或wchar_t*，从prmStr字符串的第prmStart个字
//符开始截取，共截取prmLen个字符
CStringUtil::CStringUtil(LPCTSTR prmStr,int prmStart,int prmLen):m_Buf(0),m_BufLen(0),m_Capacity(0)
{
	Assign(10);
	int tmpLen = _tcslen(prmStr);
	if(prmStart + prmLen>tmpLen)
	{
		prmLen = m_BufLen-prmStart;
	}
	if(Assign(prmLen))
	{
		memcpy(m_Buf,((char*)prmStr+prmStart*sizeof(TCHAR)),prmLen*sizeof(TCHAR));
		m_BufLen = prmLen;
	}
}

//拷贝构造，将另一个CStringUtil对象作为当前字符串
CStringUtil::CStringUtil(const CStringUtil &prmObj) : m_Buf(NULL), m_BufLen(0), m_Capacity(0)
{
	Assign(10);
	this->Append(prmObj);
}

//赋值构造
CStringUtil &CStringUtil::operator=(const CStringUtil &prmObj)
{
	if (this==&prmObj)
	{
		return *this;
	}
	if(m_Buf==NULL || m_Capacity==0)
	{
		this->Assign(10);
	}
	else
	{
		memset(m_Buf,0,m_BufLen*sizeof(TCHAR));
		m_BufLen = 0;
	}
	this->Append(prmObj);
	return *this;
}

//析构
CStringUtil::~CStringUtil()
{
	free(m_Buf);
	m_Buf = NULL;
	m_BufLen = 0;
	m_Capacity = 0;
}

//向当前字符串对象中追加一个char*或wchar_t*表示的字符串
CStringUtil	&CStringUtil::Append(LPCTSTR prmStr)
{
	if (prmStr == NULL)
	{
		Assign(10);
		return *this;
	}
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen + m_BufLen > m_Capacity)
	{
		size_t tmpNewSize = m_BufLen;
		//选择长度较大的以2倍长度进行扩展，否则分配的空间可能不够
		if (tmpLen > tmpNewSize)
		{
			tmpNewSize = tmpLen;
		}
		//以将近两部的容量扩充字符串容量
		tmpNewSize = tmpNewSize * 2;
		Assign(tmpNewSize);
	}
	//确保空间足够
	if (tmpLen + m_BufLen <= m_Capacity)
	{
		memcpy(((char*)m_Buf + m_BufLen * sizeof(TCHAR)), prmStr, tmpLen * sizeof(TCHAR));
		m_BufLen += tmpLen;
	}
	return *this;
}

//向当前字符串对象中追回一个char*或wchar_t表示的字符串
//@prmStartPos从字符串的第prmStartPos字符开始截取
//@prmLen，从prmStartPos开始截取prmLen个字符
CStringUtil	&CStringUtil::Append(LPCTSTR prmStr, UINT32 prmStartPos, UINT32 prmLen)
{
	//要截取的字符串个数过大于字符串实际长度
	if ( (prmLen + prmStartPos > _tcslen(prmStr)) || prmLen==-1) 
	{
		prmLen = _tcslen(prmStr) - prmStartPos;
	}
	if (prmLen >= m_Capacity-m_BufLen)
	{
		this->Assign( (m_Capacity+prmLen)*2);
		/*if (this->Assign(m_Capacity * 2))
		{
			memcpy(m_Buf + m_BufLen, prmStr + prmStartPos, prmLen * sizeof(TCHAR));
			m_BufLen += prmLen;
		}*/
	}
	if (m_BufLen + prmLen <= m_Capacity)
	{
		memcpy(m_Buf + m_BufLen, prmStr + prmStartPos, prmLen * sizeof(TCHAR));
		m_BufLen += prmLen;
	}
	return *this;
}

#ifdef _UNICODE
//如果当前环境选择的是Unicode编码，可以向当前对象追加char*型字符串
CStringUtil	&CStringUtil::Append(char *prmStr)
{
	//获取将prmStr ANSI编码转换为Unicode编码需要多少长度
	int tmpLen = ::MultiByteToWideChar(CP_ACP, 0, prmStr, -1, NULL, 0);
	//tmpLen = (tmpLen + 1) * sizeof(TCHAR);
	TCHAR	*tmpBuf = (TCHAR*)malloc((tmpLen + 1) * sizeof(TCHAR));
	if (tmpBuf == NULL)
	{
		return *this;
	}
	memset(tmpBuf, 0, (tmpLen + 1) * sizeof(TCHAR));
	//将ANSI转换为Unicode字符串
	::MultiByteToWideChar(CP_ACP, 0, prmStr, -1, tmpBuf, tmpLen);
	//转换后的字符串追加到当前对象中
	this->Append(tmpBuf);
	free(tmpBuf);
	return *this;
}
#else
//如果当前选择的是ANSI编码，支持向当前对象添加wchar_t*型字符串
CStringUtil	&CStringUtil::Append(wchar_t *prmStr)
{
	//获取将Unicode转换为ANSI字符串需要多少字符
	int tmpLen = ::WideCharToMultiByte(CP_ACP, 0, prmStr, -1, NULL, 0, 0, 0);
	char	*tmpBuf = (char*)malloc(tmpLen + 2);
	if (tmpBuf == NULL)
	{
		return *this;
	}
	memset(tmpBuf, 0, tmpLen + 2);
	//Unicode转换为ANSI字符串
	::WideCharToMultiByte(CP_ACP, 0, prmStr, -1, tmpBuf, tmpLen, 0, 0);
	//转换后的字符串追加到当前对象中
	this->Append(tmpBuf);
	free(tmpBuf);
	return *this;
}
#endif

//将另一个CStringUtil对象所表示字符串追加到当前对象中
CStringUtil	&CStringUtil::Append(const CStringUtil &prmObj)
{
	if (this == &prmObj)
	{
		return *this;
	}
	this->Append(prmObj.m_Buf);
	return *this;
}

//向当前字符串对象追加一个字符
CStringUtil	&CStringUtil::Append(TCHAR prmChar)
{
	TCHAR	tmpStr[2] = { prmChar };
	this->Append(tmpStr);
	return *this;
}

//将prmNum所表示的数字作为字符串追加到当前对象中
CStringUtil	&CStringUtil::Append(int prmNum)
{
	char	tmpBuf[MAX_PATH] = { 0 };
	_snprintf_s(tmpBuf, MAX_PATH, "%d", prmNum);
	this->Append(tmpBuf);
	return *this;
}

CStringUtil &CStringUtil::operator+=(LPCTSTR prmStr)
{
	this->Append(prmStr);
	return *this;
	//return this->Append(prmStr);
}
#ifdef _UNICODE
CStringUtil	&CStringUtil::operator+=(char *prmStr)
{
	this->Append(prmStr);
	return *this;
}
#else
CStringUtil	&CStringUtil::operator+=(wchar_t *prmStr)
{
	this->Append(prmStr);
	return *this;
}
#endif
CStringUtil &CStringUtil::operator+=(const CStringUtil &prmObj)
{
	this->Append(prmObj);
	return *this;
}
CStringUtil &CStringUtil::operator+=(TCHAR prmChar)
{
	this->Append(prmChar);
	return *this;
}
CStringUtil &CStringUtil::operator+=(int prmNum)
{
	this->Append(prmNum);
	return *this;
}

CStringUtil CStringUtil::operator+(LPCTSTR prmStr)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmStr);
	return tmpString;
}
#ifdef _UNICODE
CStringUtil	CStringUtil::operator+(char *prmStr)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmStr);
	return tmpString;
}
#else
CStringUtil	CStringUtil::operator+(wchar_t *prmStr)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmStr);
	return tmpString;
}
#endif
CStringUtil CStringUtil::operator+(const CStringUtil &prmObj)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmObj);
	return tmpString;
}
CStringUtil CStringUtil::operator+(TCHAR prmChar)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmChar);
	return tmpString;
}
CStringUtil CStringUtil::operator+(int prmNum)
{
	CStringUtil	tmpString = *this;
	tmpString.Append(prmNum);
	return tmpString;
}

//清空当前对象中的字符串信息，但不释放所分配的内存。
void CStringUtil::Empty()
{
	memset(m_Buf,0,sizeof(TCHAR)*m_BufLen);
	m_BufLen = 0;
	m_Capacity = 0;
}

//字符串是否为空
BOOL CStringUtil::IsEmpty()
{
	return m_BufLen==0;
}

//字符串是否相等
BOOL CStringUtil::operator ==(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)==0);
}

//字符串是否不等
BOOL CStringUtil::operator!=(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)!=0);
}

//当前字符串是否小于prmObj所表示字符串
BOOL CStringUtil::operator<(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)<0);
}

//当前字符串是否小于等于prmObj所表示字符串
BOOL CStringUtil::operator <=(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)<=0);
}

//当前字符串是否大于prmObj所表示字符串
BOOL CStringUtil::operator>(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)>0);
}

//当前字符串是否大于等于prmObj所表示字符串
BOOL CStringUtil::operator>=(const CStringUtil &prmObj)
{
	return (Compare(m_Buf,prmObj.m_Buf)>=0);
}

//字符串不区分大小写比较
int		CStringUtil::CompareNoCase(const TCHAR *prmStr)
{
	return (_tcsicmp(m_Buf,prmStr));
}

//字符串不区分大小写比较
int		CStringUtil::CompareNoCase(const CStringUtil &prmObj)
{
	return (_tcsicmp(m_Buf,prmObj.m_Buf));
}

//字符串只比较prmLen个字符,区分大小写
int		CStringUtil::CompareNChar(const TCHAR *prmStr,size_t prmLen)
{
	return (_tcsncmp(m_Buf,prmStr,prmLen));
}

//字符串只比较prmLen个字符,区分大小写
int		CStringUtil::CompareNChar(const CStringUtil &prmObj,size_t prmLen)
{
	return (_tcsncmp(m_Buf,prmObj.m_Buf,prmLen));
}

//字符串只比较prmLen个字符，不区分大小写
int		CStringUtil::CompareNCharNoCase(const TCHAR *prmStr,size_t prmLen)
{
	return (_tcsnicmp(m_Buf,prmStr,prmLen));
}

//字符串只比较prmLen个字符，不区分大小写
int		CStringUtil::CompareNCharNoCase(const CStringUtil &prmObj,size_t prmLen)
{
	return (_tcsnicmp(m_Buf,prmObj.m_Buf,prmLen));
}

//从字符串左边开始截取prmLen个字符
CStringUtil CStringUtil::Left(int prmLen)
{
	size_t tmpLen = prmLen;
	if(tmpLen>m_BufLen || prmLen==-1)
	{
		tmpLen = m_BufLen;
	}
	return CStringUtil(m_Buf,0,prmLen);
}

//从字符串右边开始载取prmLen个字符
CStringUtil	CStringUtil::Right(int prmLen)
{
	size_t tmpLen = prmLen;
	if(tmpLen<0 || tmpLen>m_BufLen)
	{
		tmpLen = m_BufLen;
	}
	return CStringUtil(m_Buf,m_BufLen-tmpLen,tmpLen);
}

//截取从prmStart开始到prmEnd结束的字符串，范围[prmStart,prmEnd)
CStringUtil CStringUtil::Mid(int prmStart,int prmEnd)
{
	if(prmStart>prmEnd || prmStart>=(int)m_BufLen)
	{
		return CStringUtil(_T(""));
	}
	return CStringUtil(m_Buf,prmStart,prmEnd-prmStart+1);
}

//数组下标引用，获取第prmIndex个字符
TCHAR	&CStringUtil::operator[](int prmIndex)
{
	if(prmIndex<0 || prmIndex>=(int)m_BufLen)
	{
		//throw std::exception(_T("Out of Range"));
		throw std::out_of_range("Out of Range");
	}
	return (*(m_Buf + prmIndex));
}

//获取第prmIndex个字符
TCHAR	CStringUtil::GetAt(int prmIndex)
{
	if(prmIndex<0 || prmIndex>=(int)m_BufLen)
	{
		throw std::out_of_range("Out of Range");
	}
	return (*(m_Buf + prmIndex));
}

//从当前字符串中查找prmStr子串，如果找到，则返回首字符所在下标值，否则返回-1
//查找方式为从左向右开始查
int		CStringUtil::FindString(const TCHAR *prmStr)
{
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return -1;
	}
	for (size_t i = 0; i <= m_BufLen - tmpLen; i++)
	{
		if(memcmp(m_Buf+i,prmStr,tmpLen*sizeof(TCHAR)) == 0)
		{
			return i;
		}
		/*if (this->CompareNChar(prmStr, tmpLen) == 0)
		{
			return i;
		}*/
	}
	return -1;
}

//从当前字符串中查找prmObj所表示子串，如果找到，则返回首字符所有下标值，否则返回-1
//查找方式为从左向右开始查
int		CStringUtil::FindString(const CStringUtil &prmObj)
{
	return this->FindString(prmObj.m_Buf);
}

//从当前字符串中查找PrmStr子串，如果找到，则返回首字符所在下标值，否则返回-1
//查找方式为从右向左查找
int		CStringUtil::RFindString(const TCHAR *prmStr)
{
	size_t	tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return -1;
	}
	for (int i = (int)(m_BufLen - tmpLen); i >= 0; i--)
	{
		/*if (this->CompareNChar(m_Buf + i, tmpLen) == 0)
		{
			return i;
		}*/
		if(memcmp(m_Buf+i,prmStr,tmpLen*sizeof(TCHAR)) ==0)
		{
			return i;
		}
	}
	return -1;
}

//从当前字符串中查找PrmStr子串，如果找到，则返回首字符所在下标值，否则返回-1
//查找方式为从右向左查找
int		CStringUtil::RFindString(const CStringUtil &prmObj)
{
	return this->RFindString(prmObj.m_Buf);
}

//从左向右查找当前字符串中是否有字符prmChar，如果有则返回字符出现地址，否则返回NULL
TCHAR	*CStringUtil::StrChar(const TCHAR prmChar)
{
	return _tcschr(m_Buf, prmChar);
}

//从右向左查找当前字符串中是否有字符prmChar，如果有则返回字符出现地址，否则返回NULL
TCHAR	*CStringUtil::StrrChar(const TCHAR prmChar)
{
	return _tcsrchr(m_Buf, prmChar);
}

//当前字符串是否以prmStr开始
BOOL	CStringUtil::StartWith(const TCHAR *prmStr)
{
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return FALSE;
	}
	return ( (this->CompareNChar(prmStr, tmpLen)) == 0);
}

//当前字符串是否以prmObj所表示字符串开始
BOOL	CStringUtil::StartWith(const CStringUtil &prmObj)
{
	return this->StartWith(prmObj.m_Buf);
}

//当前字符串是否以prmStr结束
BOOL	CStringUtil::EndWith(const TCHAR *prmStr)
{
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return FALSE;
	}
	return this->Compare(m_Buf + m_BufLen - tmpLen, prmStr)==0;
}

//当前字符串是否以prmObj所表示字符串结束
BOOL	CStringUtil::EndWith(const CStringUtil &prmObj)
{
	return this->EndWith(prmObj.m_Buf);
}

//当前字符串是否以prmStr开始，忽略大小写
BOOL	CStringUtil::StartWithNoCase(const TCHAR *prmStr)
{
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return FALSE;
	}
	return ((this->CompareNCharNoCase(prmStr, tmpLen)) == 0);
}

//当前字符串是否以prmObj所表示字符串开始，忽略大小写
BOOL	CStringUtil::StartWithNoCase(const CStringUtil &prmObj)
{
	return this->StartWithNoCase(prmObj.m_Buf);
}

//当前字符串是否以prmStr结束，忽略大小写
BOOL	CStringUtil::EndWithNoCase(const TCHAR *prmStr)
{
	size_t tmpLen = _tcslen(prmStr);
	if (tmpLen > m_BufLen)
	{
		return FALSE;
	}
	//return this->CompareNoCase(m_Buf + m_BufLen - tmpLen, prmStr) == 0;
	return (_tcsicmp(m_Buf + m_BufLen - tmpLen, prmStr) == 0);
}

//当前字符串是否以prmObj所表示字符串结束，忽略大小写
BOOL	CStringUtil::EndWithNoCase(const CStringUtil &prmObj)
{
	return this->EndWithNoCase(prmObj.m_Buf);
}

//
CStringUtil	&CStringUtil::Trim()
{
	UINT32 i = 0;
	for (i = 0; i < m_BufLen; i++)
	{
#ifdef _UNICODE
		INT16  *tmpVal = (INT16*)(m_Buf + i);
#else
		INT8	*tmpVal = (INT8*)(m_Buf + i);
#endif
		if (!this->IsSpaceChar(*tmpVal))
		{
			break;
		}
		//if(!isspace((m_Buf+m_BufLen)))
	}
	if (i > 0)
	{
		memcpy(m_Buf, m_Buf + i, (m_BufLen - i + 1)*sizeof(TCHAR));
		m_BufLen = m_BufLen - i;
	}
	for (i = m_BufLen - 1; i >= 0 && m_BufLen>0; i--)
	{
#ifdef _UNICODE
		INT16  *tmpVal = (INT16*)(m_Buf + i);
#else
		INT8	*tmpVal = (INT8*)(m_Buf + i);
#endif
		if (this->IsSpaceChar(*tmpVal))
		{
			*(m_Buf + i) = 0;
			m_BufLen--;
		}
		else
		{
			break;
		}
	}
	return *this;
}

void CStringUtil::ToUpper()
{
	for (UINT32 i = 0; i < m_BufLen; i++)
	{
#ifdef _UNICODE
		INT16  *tmpVal = (INT16*)(m_Buf + i);
#else
		INT8	*tmpVal = (INT8*)(m_Buf + i);
#endif
		if (*tmpVal >= _T('a') && *tmpVal <= _T('z'))
		{
			*tmpVal -= 32;
		}
	}
}

void CStringUtil::ToLower()
{
	for (UINT32 i = 0; i < m_BufLen; i++)
	{
#ifdef _UNICODE
		INT16  *tmpVal = (INT16*)(m_Buf + i);
#else
		INT8	*tmpVal = (INT8*)(m_Buf + i);
#endif
		if (*tmpVal >= _T('A') && *tmpVal <= _T('Z'))
		{
			*tmpVal += 32;
		}
	}
}

void CStringUtil::SplitString(vector<CStringUtil> &prmArray, LPCTSTR prmSplitStr)
{
	prmArray.clear();
	if (m_BufLen == 0)
	{
		return;
	}
	TCHAR *tmpBuf = (TCHAR*)malloc((m_BufLen + 2) * sizeof(TCHAR));
	memset(tmpBuf, 0, (m_BufLen + 2) * sizeof(TCHAR));
	memcpy(tmpBuf, m_Buf, m_BufLen * sizeof(TCHAR));
	TCHAR *tmpContext = NULL;
	TCHAR *tmpTok = _tcstok_s(tmpBuf, prmSplitStr,&tmpContext);
	while (tmpTok != NULL)
	{
		prmArray.push_back(tmpTok);
		tmpTok = _tcstok_s(NULL, prmSplitStr,&tmpContext);
	}
	free(tmpBuf);
}

CStringUtil CStringUtil::ReplaceStr(TCHAR *prmSrc, TCHAR *prmDst)
{
	CStringUtil	tmpResult;
	UINT32 tmpLastPos = 0;
	UINT32 tmpPos = 0;

	int		tmpLen = _tcslen(prmSrc);
	while (tmpPos <= m_BufLen)
	{
		BOOL	tmpFlag = TRUE;
		for(int i=0;i<tmpLen;i++)
		{
			if(*(m_Buf+tmpPos+i)!=*(prmSrc+i))
			{
				tmpFlag = FALSE;
				break;
			}
		}
		if(tmpFlag)
		{
			tmpResult.Append(m_Buf, tmpLastPos, tmpPos - tmpLastPos);
			tmpResult.Append(prmDst);
			tmpLastPos = tmpPos + tmpLen;
			tmpPos += tmpLen;
			continue;
		}
		/*if (_tcsncmp(m_Buf + tmpPos, prmSrc, _tcslen(prmSrc) * sizeof(TCHAR)) == 0)
		{
			tmpResult.Append(m_Buf, tmpLastPos, tmpPos - tmpLastPos);
			tmpResult.Append(prmDst);
			tmpLastPos = tmpPos + _tcslen(prmSrc);
			tmpPos += _tcslen(prmSrc);
			continue;
		}*/
		tmpPos++;
	}
	if (tmpLastPos < m_BufLen)
	{
		tmpResult.Append(m_Buf + tmpLastPos);
	}
	return tmpResult;
}

//字符串格式化
void CStringUtil::Format(TCHAR *prmForamt, ...)
{
	int	tmpSize = 0;
	va_list		tmpArgs;
	va_start(tmpArgs, prmForamt);
#pragma warning(push)
#pragma warning(disable:4996)
	tmpSize = _vsntprintf(NULL, 0, prmForamt, tmpArgs);

	if (m_Capacity <= (UINT32)tmpSize)
	{
		this->Assign(tmpSize + 1);
	}
	m_BufLen = _vsntprintf(m_Buf, m_Capacity - 1, prmForamt, tmpArgs);
#pragma warning(pop)
	va_end(tmpArgs);
}

//重新分配prmNewSize个字符串空间
BOOL CStringUtil::Assign(size_t prmNewSize)
{
	//多分配一个字符空间用于存放字符结尾'\0'
	TCHAR *tmpBuf = (TCHAR*)malloc( (prmNewSize+1) * sizeof(TCHAR));
	if (tmpBuf == NULL)
	{
		return FALSE;
	}
	memset(tmpBuf, 0, sizeof(TCHAR)*(prmNewSize + 1));
	if (m_Buf != NULL)
	{
		memcpy(tmpBuf, m_Buf, m_BufLen * sizeof(TCHAR));
	}
	free(m_Buf);
	//当前字符串指向新分配的空间
	m_Buf = tmpBuf;
	//新分配的大小
	m_Capacity = prmNewSize;
	return TRUE;
}

//比较两个字符串，相等返回0，小于返回-1，大于返回1
int CStringUtil::Compare(const TCHAR *prmStr1,const TCHAR *prmStr2)
{
	return (_tcscmp(prmStr1,prmStr2));
}

//判断是否为空格字符
BOOL CStringUtil::IsSpaceChar(int prmCharVal)
{
	if (prmCharVal < 255 && prmCharVal>0 && isspace(prmCharVal))
	{
		return TRUE;
	}
	return FALSE;
}