#ifndef WINDOWS_DEFINE_HH
#define WINDOWS_DEFINE_HH

#include <process.h>
#include <Ws2tcpip.h>
#include <Windows.h> 

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif





#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"WS2_32.lib")
#pragma comment(lib,"Mswsock.lib")


#define sleep(x) 		    Sleep((x) * 1000)
#define usleep(x) 		    Sleep((x) / 1000)

#define CLOSE_SOCKET(P)						if(P != -1){closesocket(P);P = -1;}
#define SAFE_RELEASE(P)                     if (P){P->Release(); P = NULL;}
#define SAFE_CLSHANDLE(P)                   if (P){CloseHandle(P); P = NULL;}

#endif
