// VideoStatic.cpp : implementation file
//

#include "stdafx.h"
#include "Player.h"
#include "VideoStatic.h"

#include "RTSPClient.h"

#ifdef _DEBUG
#pragma comment(lib,"RTSPClientd.lib")
#else
#pragma comment(lib,"RTSPClient.lib")
#endif

CVideoStatic::CVideoStatic()
{
	m_hPlayer = 0;
}

CVideoStatic::~CVideoStatic()
{
	if(m_hPlayer){
		destroy_rtsp_stream(m_hPlayer);
		m_hPlayer = 0;
	}
}

int	CVideoStatic::OpenChannel(const char* url)
{
	if(0 == m_hPlayer){
		m_hPlayer = create_rtsp_stream();
		return open_rtsp_stream(m_hPlayer,GetSafeHwnd(),url,false);
	}
	return -1;
}

BEGIN_MESSAGE_MAP(CVideoStatic, CStatic)
	//{{AFX_MSG_MAP(VideoStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CVideoStatic::OnPaint() 
{
	CRect Rect;
	CPaintDC pDC(this); 
	GetClientRect(&Rect);
	CBrush tempBrush(RGB(10,10,10));
	pDC.FillRect(Rect, &tempBrush);
}

BOOL CVideoStatic::Create(CWnd* pParentWnd) 
{
	RECT Rect = {4,4,16,16};
	return CStatic::Create(NULL,WS_CHILD|WS_VISIBLE,Rect, pParentWnd);
}

BOOL CVideoStatic::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CStatic::OnEraseBkgnd(pDC);
}
