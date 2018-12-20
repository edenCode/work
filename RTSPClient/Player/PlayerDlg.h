
// PlayerDlg.h : 头文件
//

#pragma once
#include "VideoStatic.h"

#define		MAX_DISPLAYER_WND	16
// CPlayerDlg 对话框
class CPlayerDlg : public CDialogEx
{
// 构造
public:
	CPlayerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PLAYER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual void LoadConfig();
	virtual void ShowImageDlgs();
	virtual void CreateImageDlgs();
	virtual void ReleaseImageDlgs();
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	unsigned int	m_nPlayCount;
	CVideoStatic*	m_pImageDlgs[MAX_DISPLAYER_WND];
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
