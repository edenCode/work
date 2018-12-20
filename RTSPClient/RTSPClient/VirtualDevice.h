#ifndef TESTRTSPSOURCE_H
#define TESTRTSPSOURCE_H

#include "Live555Client.h"
#include "../H26XDecode/H26XCode.h"

class CVirtualDevice : public Live555Client 
{
public:
	CVirtualDevice();

	virtual ~CVirtualDevice();

	virtual void HandleRTPData(uint8_t* p_buffer,int i_size,int i_truncated_bytes,int64_t pts, int64_t dts);
	
	virtual void OnRecvSDPInfo(LiveTrack* track);

	virtual int  OpenChannel(void* hwnd,const char*url,bool btcp);

	virtual void CloseChannel();
private:
	
	HWND				m_hwnd;
	bool				m_bHassps;
	video_decoder_t		m_decoder_t;
};

#endif // !defined(AFX_TESTSOURCE_H__9BEEDC31_1CF6_4164_91EB_981C958E1389__INCLUDED_)
