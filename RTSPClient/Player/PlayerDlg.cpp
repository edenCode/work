
// PlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CPlayerDlg::CPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPlayerDlg::IDD, pParent)
{
	m_nPlayCount = MAX_DISPLAYER_WND;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPlayerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CPlayerDlg 消息处理程序
BOOL CPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//MoveWindow(0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CreateImageDlgs();

	LoadConfig();

	ShowImageDlgs();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPlayerDlg::OnPaint()
{
	
	CPaintDC dc(this); // 用于绘制的设备上下文
	CDialogEx::OnPaint();
	ShowImageDlgs();
}

HCURSOR CPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPlayerDlg::ShowImageDlgs()
{
	int i;
	RECT Rect = {0};

	GetClientRect(&Rect);

	int nCaleCount = m_nPlayCount;
	RECT m_ImageRect[MAX_DISPLAYER_WND] = {0};

	double ImageWidth = (double)(Rect.right - Rect.left - (sqrt((double)nCaleCount) * 2 + 2)) / sqrt((double)nCaleCount);
	double ImageHeight = (double)(Rect.bottom - Rect.top - (sqrt((double)nCaleCount) * 2 + 2)) / sqrt((double)nCaleCount);
	Rect.left += 2;
	Rect.top += 2;
	for(i = 0 ; i < nCaleCount ; i++)
	{
		int nCol = i % (int)sqrt((double)nCaleCount);
		int nRow = i / (int)sqrt((double)nCaleCount);
		m_ImageRect[i].left = Rect.left + (long)(ImageWidth * nCol + nCol * 2 + 0.5);
		m_ImageRect[i].top=Rect.top+(long)(ImageHeight*nRow + nRow * 2 + 0.5);
		m_ImageRect[i].right=m_ImageRect[i].left+(long)(ImageWidth + 0.5);
		m_ImageRect[i].bottom=m_ImageRect[i].top+(long)(ImageHeight + 0.5);
	}

	for(i = 0 ; i < m_nPlayCount ; i++)
	{
		m_pImageDlgs[i]->MoveWindow(&m_ImageRect[i]);
		m_pImageDlgs[i]->ShowWindow(SW_SHOW);
	}
}

void CPlayerDlg::CreateImageDlgs()
{
	for (int i = 0; i < MAX_DISPLAYER_WND;i++){
		m_pImageDlgs[i] = new CVideoStatic();
		m_pImageDlgs[i]->Create(this);
	}
}

void CPlayerDlg::LoadConfig()
{
	char stemp[16] = {0};
	char sUser[32] = {0};
	char sPassword[32] = {0};
	char sMediaurl[128] = {0};
	char sFilePath[MAX_PATH] = {0};
	GetModuleFileName(NULL,sFilePath,MAX_PATH);
	for (size_t i = strlen(sFilePath); i > 0; i--){
		if(sFilePath[i] == '\\'){
			strcat(sFilePath,"localset.ini");
			break;
		}
		sFilePath[i] = '\0';
	}

	m_nPlayCount = GetPrivateProfileInt("RTSPClient","PlayCount",MAX_DISPLAYER_WND,sFilePath);
	if (m_nPlayCount > MAX_DISPLAYER_WND){
		m_nPlayCount = MAX_DISPLAYER_WND;
	}
	for (int i = 0; i < m_nPlayCount;i++){
		sprintf(stemp,"url%d",i+1);
		GetPrivateProfileString("RTSPClient",stemp,"",sMediaurl,128,sFilePath);
		
		m_pImageDlgs[i]->OpenChannel(sMediaurl);
	}
}

void CPlayerDlg::ReleaseImageDlgs()
{
	for (int i = 0; i < MAX_DISPLAYER_WND;i++){
		m_pImageDlgs[i]->DestroyWindow();
		delete m_pImageDlgs[i];
		m_pImageDlgs[i] = 0;
	}
}

void CPlayerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	ReleaseImageDlgs();
}


void CPlayerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	MoveWindow(0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	CDialogEx::OnLButtonDblClk(nFlags, point);
}
