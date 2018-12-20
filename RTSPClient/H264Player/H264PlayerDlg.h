
#pragma once
#include "H26XCode.h"

#define DEMO_DECODE_SAMPLE		0
#define DEMO_CODECH264_SAMPLE	1

class CH264PlayerDlg : public CDialogEx
{

public:
	virtual void Run();

	CH264PlayerDlg(CWnd* pParent = NULL);	


	enum { IDD = IDD_H264PLAYER_DIALOG };

	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	


// й╣ож
protected:
	HICON m_hIcon;
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	HANDLE	m_hThread;
	bool	m_bThreadEnd;
	video_decoder_t m_hdecoder_t;
	static DWORD WINAPI ThreadProc(LPVOID lpParam);

	static void  __stdcall encodecallback(unsigned char* frame,int nlen,bool bframe,void* user);
};
