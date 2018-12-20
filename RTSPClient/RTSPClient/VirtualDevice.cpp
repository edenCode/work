// TestSource.cpp: implementation of the CVirtualDevice class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "mfxstructures.h"
#include "VirtualDevice.h"


#ifdef _DEBUG
#pragma comment(lib,"H26XDecoded.lib")
#else
#pragma comment(lib,"H26XDecode.lib")
#endif


CVirtualDevice::CVirtualDevice()
{
	m_decoder_t = 0;
	m_bHassps = false;
	
}

CVirtualDevice::~CVirtualDevice()
{	
	CloseChannel();
	
}

int CVirtualDevice::OpenChannel(void* hwnd,const char*url,bool btcp)
{
	m_hwnd = (HWND)hwnd;
	if(RTSP_OK != PlayRtsp(url,btcp)){
		return -1;
	}
	return CThread::Start();
}

void CVirtualDevice::CloseChannel()
{
	CThread::Stop(NULL);
	if(m_decoder_t){
		destroy_video_decoder(m_decoder_t);
		m_decoder_t = 0;
	}
}

void CVirtualDevice::OnRecvSDPInfo(LiveTrack* track)
{
	LiveTrack::media_format& fmt = track->getFormat();
	if(fmt.type == "video"){
		if(fmt.codec == "H264"){
			if(m_decoder_t){
				destroy_video_decoder(m_decoder_t);
				m_decoder_t = 0;
			}
			m_decoder_t = create_video_decoder(m_hwnd,VIDEO_CODEC_H264);
		}else if (fmt.codec == "H265"){
			if(m_decoder_t){
				destroy_video_decoder(m_decoder_t);
				m_decoder_t = 0;
			}
			m_decoder_t = create_video_decoder(m_hwnd,VIDEO_CODEC_H265);
		}
	}
}

void CVirtualDevice::HandleRTPData(uint8_t* p_buffer,int i_size,int i_truncated_bytes,int64_t pts, int64_t dts)
{
	if (0 == p_buffer || 0 == i_size)
		return ;
	int nNalType = p_buffer[4]&0x1F;
	if(0x07 != nNalType && !m_bHassps){
		return ;
	}
	switch(nNalType){
	case 0x01:
	case 0x05:
		{
			video_decoder_decode(m_decoder_t,(char*)p_buffer,i_size,true);
		}
		break;
	case 0x07:
		{
			m_bHassps = true;
			video_decoder_decode(m_decoder_t,(char*)p_buffer,i_size,false);
		}
		break;
	case 0x08:
		{
			video_decoder_decode(m_decoder_t,(char*)p_buffer,i_size,false);
		}
		break;
	default:
		break;
	}
}