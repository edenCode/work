#ifndef RTSPCLIENTPLUGEDLL_HH
#define RTSPCLIENTPLUGEDLL_HH


 #ifdef RTSPCLIENTDLL_API
	#define RTSPCLIENT_API extern "C"	__declspec(dllexport)
#else
	 #define RTSPCLIENT_API extern "C"	__declspec(dllimport)
 #endif

RTSPCLIENT_API long _stdcall create_rtsp_stream();

RTSPCLIENT_API void _stdcall destroy_rtsp_stream(long nChannel);

RTSPCLIENT_API int _stdcall  open_rtsp_stream(long nChannel,void* hwnd,const char* url,bool btcp);



#endif


