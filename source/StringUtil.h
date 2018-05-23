/***************************************************
*
*	@Version:	1.0
*	@Author:	Mengxl
*	@Date:		2017-8-27 22:11
*	@File:		StringUtil.h
*
****************************************************/

#ifndef STRING_UTIL_INCLUDE_H
#define STRING_UTIL_INCLUDE_H

#include <windows.h>
#include <tchar.h>
#include <vector>

using namespace std;

namespace	commutil
{
	class CStringUtil
	{
	public:
		//无参构造函数
		CStringUtil();
		//参数为char* 或wchar_t*型的字符串
		CStringUtil(LPCTSTR prmStr);
		//参数为char*或wchar_t*，从prmStr字符串的第prmStart个字
		//符开始截取，共截取prmLen个字符
		CStringUtil(LPCTSTR prmStr,int prmStart,int prmLen);
		//拷贝构造，将另一个CStringUtil对象作为当前字符串
		CStringUtil(const CStringUtil &prmObj);
		//赋值构造
		CStringUtil & operator=(const CStringUtil &prmObj);
		//析构
		~CStringUtil();

		//向当前字符串对象中追加一个char*或wchar_t*表示的字符串
		CStringUtil	&Append(LPCTSTR prmStr);

		//向当前字符串对象中追回一个char*或wchar_t表示的字符串
		//@prmStartPos从字符串的第prmStartPos字符开始截取
		//@prmLen，从prmStartPos开始截取prmLen个字符
		CStringUtil	&Append(LPCTSTR prmStr, UINT32 prmStartPos, UINT32 prmLen);
#ifdef _UNICODE
		//如果当前环境选择的是Unicode编码，可以向当前对象追加char*型字符串
		CStringUtil	&Append(char *prmStr);
#else
		//如果当前选择的是ANSI编码，支持向当前对象添加wchar_t*型字符串
		CStringUtil	&Append(wchar_t *prmStr);
#endif
		//将另一个CStringUtil对象所表示字符串追加到当前对象中
		CStringUtil	&Append(const CStringUtil &prmObj);
		//向当前字符串对象追加一个字符
		CStringUtil	&Append(TCHAR prmChar);
		//将prmNum所表示的数字作为字符串追加到当前对象中
		CStringUtil	&Append(int prmNum);

		CStringUtil &operator+=(LPCTSTR prmStr);
#ifdef _UNICODE
		CStringUtil	&operator+=(char *prmStr);
#else
		CStringUtil	&operator+=(wchar_t *prmStr);
#endif
		CStringUtil &operator+=(const CStringUtil &prmObj);
		CStringUtil &operator+=(TCHAR prmChar);
		CStringUtil &operator+=(int prmNum);

		CStringUtil operator+(LPCTSTR prmStr);
#ifdef _UNICODE
		CStringUtil	operator+(char *prmStr);
#else
		CStringUtil	operator+(wchar_t *prmStr);
#endif
		CStringUtil operator+(const CStringUtil &prmObj);
		CStringUtil operator+(TCHAR prmChar);
		CStringUtil operator+(int prmNum);

		//清空当前对象中的字符串信息，但不释放所分配的内存。
		void	Empty();
		//字符串是否为空
		BOOL	IsEmpty();
		//字符串是否相等
		BOOL	operator==(const CStringUtil &prmObj);
		//字符串是否不等
		BOOL	operator!=(const CStringUtil &prmObj);
		//当前字符串是否小于prmObj所表示字符串
		BOOL	operator<(const CStringUtil &prmObj);
		//当前字符串是否小于等于prmObj所表示字符串
		BOOL	operator<=(const CStringUtil &prmObj);
		//当前字符串是否大于prmObj所表示字符串
		BOOL	operator>(const CStringUtil &prmObj);
		//当前字符串是否大于等于prmObj所表示字符串
		BOOL	operator>=(const CStringUtil &prmObj);

		//字符串不区分大小写比较
		int		CompareNoCase(const TCHAR *prmStr);
		//字符串不区分大小写比较
		int		CompareNoCase(const CStringUtil &prmObj);
		//字符串只比较prmLen个字符,区分大小写
		int		CompareNChar(const TCHAR *prmStr,size_t prmLen);
		//字符串只比较prmLen个字符,区分大小写
		int		CompareNChar(const CStringUtil &prmObj,size_t prmLen);
		//字符串只比较prmLen个字符，不区分大小写
		int		CompareNCharNoCase(const TCHAR *prmStr,size_t prmLen);
		//字符串只比较prmLen个字符，不区分大小写
		int		CompareNCharNoCase(const CStringUtil &prmObj,size_t prmLen);

		//从字符串左边开始截取prmLen个字符
		CStringUtil Left(int prmLen);
		//从字符串右边开始载取prmLen个字符
		CStringUtil	Right(int prmLen);
		//截取从prmStart开始到prmEnd结束的字符串，范围[prmStart,prmEnd)
		CStringUtil Mid(int prmStart,int prmEnd);

		//数组下标引用，获取第prmIndex个字符
		TCHAR	&operator[](int prmIndex);
		//获取第prmIndex个字符
		TCHAR	GetAt(int prmIndex);

		//获取字符串实际缓冲区地址
		LPCTSTR GetString()const {return m_Buf;}
		//获取字符串长度
		int		GetLength(){return m_BufLen;}

		//从当前字符串中查找prmStr子串，如果找到，则返回首字符所在下标值，否则返回-1
		//查找方式为从左向右开始查
		int		FindString(const TCHAR *prmStr);
		//从当前字符串中查找prmObj所表示子串，如果找到，则返回首字符所有下标值，否则返回-1
		//查找方式为从左向右开始查
		int		FindString(const CStringUtil &prmObj);
		//从当前字符串中查找PrmStr子串，如果找到，则返回首字符所在下标值，否则返回-1
		//查找方式为从右向左查找
		int		RFindString(const TCHAR *prmStr);
		//从当前字符串中查找PrmStr子串，如果找到，则返回首字符所在下标值，否则返回-1
		//查找方式为从右向左查找
		int		RFindString(const CStringUtil &prmObj);
		//从左向右查找当前字符串中是否有字符prmChar，如果有则返回字符出现地址，否则返回NULL
		TCHAR	*StrChar(const TCHAR prmChar);
		//从右向左查找当前字符串中是否有字符prmChar，如果有则返回字符出现地址，否则返回NULL
		TCHAR	*StrrChar(const TCHAR prmChar);

		//当前字符串是否以prmStr开始
		BOOL	StartWith(const TCHAR *prmStr);
		//当前字符串是否以prmObj所表示字符串开始
		BOOL	StartWith(const CStringUtil &prmObj);
		//当前字符串是否以prmStr结束
		BOOL	EndWith(const TCHAR *prmStr);
		//当前字符串是否以prmObj所表示字符串结束
		BOOL	EndWith(const CStringUtil &prmObj);

		//当前字符串是否以prmStr开始，忽略大小写
		BOOL	StartWithNoCase(const TCHAR *prmStr);
		//当前字符串是否以prmObj所表示字符串开始，忽略大小写
		BOOL	StartWithNoCase(const CStringUtil &prmObj);
		//当前字符串是否以prmStr结束，忽略大小写
		BOOL	EndWithNoCase(const TCHAR *prmStr);
		//当前字符串是否以prmObj所表示字符串结束，忽略大小写
		BOOL	EndWithNoCase(const CStringUtil &prmObj);

		//
		CStringUtil	&Trim();

		//将字符串所有小写字符转换为大写字符
		void	ToUpper();

		//将字符串所有大写字符转换为小写字符
		void	ToLower();

		void	SplitString(vector<CStringUtil> &prmArray,LPCTSTR prmSplitStr);

		CStringUtil	ReplaceStr(TCHAR *prmSrc, TCHAR	*prmDst);

		//格式化字符串
		void		Format(TCHAR *prmForamt, ...);
	private:
		//重新分配prmNewSize个字符串空间
		BOOL	Assign(size_t prmNewSize);
		//比较两个字符串，相等返回0，小于返回-1，大于返回1
		int		Compare(const TCHAR *prmStr1,const TCHAR *prmStr2);
		//判断是否为空格字符
		BOOL	IsSpaceChar(int prmCharVal);
	private:
		TCHAR		*m_Buf;//用于存放实际字符串
		UINT32		m_BufLen;//用于表示字符串实际长度
		UINT32		m_Capacity;//当前分配的字符实际可容纳多少字符
	};
}

#endif