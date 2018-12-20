
// H264PlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "H264Player.h"
#include "H264PlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef _DEBUG
#pragma comment(lib,"H26XDecode.lib")
#else
#pragma comment(lib,"H26XDecoded.lib")
#endif

CH264PlayerDlg::CH264PlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CH264PlayerDlg::IDD, pParent)
{
	m_hdecoder_t = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CH264PlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CH264PlayerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CH264PlayerDlg 消息处理程序

BOOL CH264PlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);


	m_hThread = NULL;
	m_bThreadEnd = true;


	if (check_suppor_thwaccel())
	{
		DWORD dwThread = 0;
#if DEMO_DECODE_SAMPLE
		m_hdecoder_t = create_video_decoder(GetSafeHwnd(),VIDEO_CODEC_H264);
#else
#if DEMO_CODECH264_SAMPLE
		m_hdecoder_t = create_video_encoder(VIDEO_CODEC_H264,1920,1080,4096,25);
#else
		m_hdecoder_t = create_video_encoder(VIDEO_CODEC_H265,1920,1080,4096,25);
#endif
		encoder_setcallback(m_hdecoder_t,encodecallback,this);
#endif
	
		m_hThread = CreateThread(NULL,0,ThreadProc,this,NULL,&dwThread);

	}
	return TRUE; 
}

void CH264PlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CH264PlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CH264PlayerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	WaitForSingleObject(m_hThread,-1);
	if (m_hdecoder_t)
	{
#if DEMO_DECODE_SAMPLE
		destroy_video_decoder(m_hdecoder_t);
#else
		destroy_video_encoder(m_hdecoder_t);
#endif
		m_hdecoder_t = 0;
	}
}

int FindStartCode(unsigned char *H264Frame, int zeros_in_startcode)
{
	int info;
	int i;

	info = 1;
	for (i = 0; i < zeros_in_startcode; i++)
		if(H264Frame[i] != 0)
			info = 0;

	if(H264Frame[i] != 1)
		info = 0;
	return info;
}

int getNextNal(FILE* inpf, unsigned char* Buf,int* nNal)
{
	int pos = 0;
	while(!feof(inpf) && (Buf[pos++]=fgetc(inpf))==0);

	int StartCodeFound = 0;
	int info2 = 0;
	int info3 = 0;

	while (!StartCodeFound)
	{
		if (feof (inpf))
		{
			//			return -1;
			return pos-1;
		}
		Buf[pos++] = fgetc (inpf);
		info3 = FindStartCode(&Buf[pos-4], 3);
		if(info3 != 1)
			info2 = FindStartCode(&Buf[pos-3], 2);
		StartCodeFound = (info2 == 1 || info3 == 1);
	}
	fseek (inpf, -4, SEEK_CUR);
	*nNal = (Buf[4] & 0x1F);
	return pos - 4;
}

void CH264PlayerDlg::Run()
{
#if DEMO_DECODE_SAMPLE
	int nNal = 0;
	int nDatalen = 0;
	bool bKeyframe = false;

	FILE* pFile = fopen("F:\\images\\1920x1080.264","rb");
	unsigned char* H264Frame = (unsigned char*)malloc(0x100000);

	while (!feof(pFile) || !m_bThreadEnd){
		int nByte = getNextNal(pFile,H264Frame,&nNal);

		if(nNal== 0x07 ){
			video_decoder_decode(m_hdecoder_t,(char*)H264Frame,nByte,false);
		}else if(0x08 == nNal){
			video_decoder_decode(m_hdecoder_t,(char*)H264Frame,nByte,false);
		}else if(0x05 == nNal){
			video_decoder_decode(m_hdecoder_t,(char*)H264Frame,nByte,true);
			Sleep(40);
		}
		else if (0x01 == nNal){
			video_decoder_decode(m_hdecoder_t,(char*)H264Frame,nByte,true);
			Sleep(40);
		}	
	}
	free(H264Frame);
	fclose(pFile);
#else
	char vps[128] = {0};
	char sps[128] = {0};
	char pps[128] = {0};
	int nvps = 0,nsps = 0,npps = 0;
#if DEMO_CODECH264_SAMPLE
	video_encoder_h264headers(m_hdecoder_t,sps,pps,&nsps,&npps);
#else
	video_encoder_h265headers(m_hdecoder_t,vps,sps,pps,&nvps,&nsps,&npps);
#endif

	FILE* pFile = fopen("F:\\images\\1920x1080.yuv","rb");
	char* buffer = (char*)malloc(1920*1080*3/2);
	int i = 10;
	while (m_hdecoder_t && !feof(pFile)){

		fread(buffer,1920*1080*3/2,1,pFile);
		video_encoder_encode(m_hdecoder_t,buffer);
		if(--i == 0){
			video_encoder_ctrlkey(m_hdecoder_t,true);
		}
		Sleep(1);
	}
	if(m_hdecoder_t){
		destroy_video_encoder(m_hdecoder_t);
		m_hdecoder_t = 0;
	}
	free(buffer);
#endif

}

 void __stdcall CH264PlayerDlg::encodecallback(unsigned char* frame,int nlen,bool bframe,void* user)
 {

 }

DWORD WINAPI CH264PlayerDlg::ThreadProc(LPVOID lpParam)
{
	CH264PlayerDlg* pthis = (CH264PlayerDlg*)lpParam;
	pthis->Run();
	return 0;
}