#include "RTSPClient.h"
#include "VirtualDevice.h"

long _stdcall create_rtsp_stream()
{
	return long(new CVirtualDevice());
}

int _stdcall open_rtsp_stream(long nChannel,void* hwnd,const char* url,bool btcp)
{
	if(nChannel){
		CVirtualDevice* pServer = (CVirtualDevice*)nChannel;
		return pServer->OpenChannel(hwnd,url,btcp);
	}
	return -1;
}

void _stdcall destroy_rtsp_stream(long nChannel)
{
	if(nChannel){
		CVirtualDevice* pServer = (CVirtualDevice*)nChannel;
		delete pServer;
	}
}