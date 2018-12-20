
// PlayerDlg.h : ͷ�ļ�
//

#pragma once
#include "VideoStatic.h"

#define		MAX_DISPLAYER_WND	16
// CPlayerDlg �Ի���
class CPlayerDlg : public CDialogEx
{
// ����
public:
	CPlayerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PLAYER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual void LoadConfig();
	virtual void ShowImageDlgs();
	virtual void CreateImageDlgs();
	virtual void ReleaseImageDlgs();
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
