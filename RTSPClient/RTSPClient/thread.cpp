/*
 * os_thread.cpp
 *
 *  Created on: 2016年12月13日
 *  Author: wangwei
 */
#include "thread.h"


CThread::CThread()
{
	m_hThread = 0;
	m_bThreadEnd = true;
}

CThread::~CThread()
{

}

int CThread::Start()
{
	m_bThreadEnd = false;
#ifdef WIN32
	DWORD dwThread = 0;
	m_hThread = CreateThread(NULL,0,ThreadProc,this, 0, &dwThread);
#else
	pthread_create(&m_hThread, 0, ThreadProc, this);
#endif
	return 0;
}

void CThread::Stop(HANDLE hEvent)
{
    m_bThreadEnd = true;
	if(NULL != hEvent){
		SetEvent(hEvent);
	}
#ifdef WIN32
    if(m_hThread){
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = 0;
    }
#else
    if(m_hThread){
    	pthread_join(m_hThread, 0);
    	m_hThread = 0;
    }
#endif
}

void CThread::Run()
{
#ifndef WIN32
	pthread_exit(0);
#endif

}

#ifdef WIN32
int	CThread::Start(LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter)
{
	DWORD dwThread = 0;
	m_bThreadEnd = false;
	m_hThread = CreateThread(NULL,0,lpStartAddress,lpParameter, 0, &dwThread);
	return 0;
}

DWORD WINAPI CThread::ThreadProc(void *pParam)
{
	CThread *pthis = (CThread*)pParam;
	pthis->Run();
	return 0;
}

#else

int	CThread::Start(void *(*ThreadFun) (void *),void* arg)
{
	m_bThreadEnd = false;
	pthread_create(&m_hThread, 0, ThreadFun,arg);
	return 0;
}

void* CThread::ThreadProc(void* pParam)
{
	CThread *pthis = (CThread*)pParam;
	pthis->Run();
	return 0;
}
#endif

bool CThread::IsRun()
{
	return !m_bThreadEnd;
}
