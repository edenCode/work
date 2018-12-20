/*
 *	os_thread.h
 *
 *  Created on: 2017年4月1日
 *  Author: wangwei
 */

#ifndef SYSTEM_THREAD_HH
#define SYSTEM_THREAD_HH
#include <Windows.h>

class CThread
{
public:
	CThread();

	virtual void Run();

	virtual ~CThread();
public:

	//启动线程
	virtual int	Start();
	//结束线程
	virtual void Stop(HANDLE hEvent);

	virtual bool IsRun();
#ifdef WIN32
	virtual int	Start(LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter);
#else
	virtual int	Start(void *(*ThreadFun) (void *),void* arg);
#endif // WIN32
protected:
#ifdef WIN32
	HANDLE	m_hThread;
	static  DWORD WINAPI ThreadProc(void *pParam);
#else
	pthread_t		m_hThread;
	pthread_mutex_t m_stMutex;
	static void* ThreadProc(void* pParam);
#endif
	bool			m_bThreadEnd;
};

#endif
