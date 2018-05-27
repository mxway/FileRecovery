
// DemoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "FileRecovery/FileSystemFactory.h"


// CDemoDlg 对话框
class CDemoDlg : public CDialogEx
{
// 构造
public:
	CDemoDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CDemoDlg();
// 对话框数据
	enum { IDD = IDD_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_diskList;
	CListCtrl m_deletedFileList;
	CBaseFileSystem	*m_fileSystem;
	vector<CBaseFileObject*>	m_deleteFileArray;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
private:
	
	/*************************************
	*
	*	函数名：	RestoreFile
	*	函数说明：	将文件恢复到指定目录
	*	参数描述：	prmFileSystem[输入参数]删除文件所在文件系统对象
	*	参数描述：	prmFileObject[输入参数]删除文件对象
	*	参数描述：	restorFileName[输入参数]恢复文件绝对路径
	*	返回值：	void
	*
	**************************************/
	void		RestoreFile(CBaseFileSystem *prmFileSystem, CBaseFileObject *prmFileObject,
		CString &restorFileName);
};
