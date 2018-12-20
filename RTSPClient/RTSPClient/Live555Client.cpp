#include <UsageEnvironment.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <liveMedia.hh>
#include <liveMedia_version.hh>
#include <Base64.hh>
#include <RTSPCommon.hh>
#include "Live555Client.h"
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <sstream>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib")
#ifdef _DEBUG
#pragma comment(lib,"BasicUsageEnvironmentd.lib")
#pragma comment(lib,"UsageEnvironmentd.lib")
#pragma comment(lib,"groupsockd.lib")
#pragma comment(lib,"liveMediad.lib")
#else
#pragma comment(lib,"BasicUsageEnvironment.lib")
#pragma comment(lib,"UsageEnvironment.lib")
#pragma comment(lib,"groupsock.lib")
#pragma comment(lib,"liveMedia.lib")
#endif

#if defined(_WIN32) || defined(WIN32)
#pragma warning(disable:4996)
#endif

using namespace std;

#ifndef INT64_C
#define INT64_C(c)  c ## LL
#endif

/* All timestamp below or equal to this define are invalid/unset
 * XXX the numerical value is 0 because of historical reason and will change.*/
#define VLC_TS_INVALID INT64_C(0)
#define VLC_TS_0 INT64_C(1)

#define CLOCK_FREQ INT64_C(1000000)

unsigned char* parseH264ConfigStr(char const* configStr,unsigned int& configSize);
uint8_t * parseVorbisConfigStr(char const* configStr,unsigned int& configSize);

#if defined(_WIN32) || defined(WIN32)
char *strcasestr(const char *phaystack, const char *pneedle)
{
	register const unsigned char *haystack, *needle;
	register unsigned bl, bu, cl, cu;

	haystack = (const unsigned char *)phaystack;
	needle = (const unsigned char *)pneedle;

	bl = tolower(*needle);
	if (bl != '\0')
	{
		// Scan haystack until the first character of needle is found:
		bu = toupper(bl);
		haystack--;				/* possible ANSI violation */
		do
		{
			cl = *++haystack;
			if (cl == '\0')
				goto ret0;
		} while ((cl != bl) && (cl != bu));

		// See if the rest of needle is a one-for-one match with this part of haystack:
		cl = tolower(*++needle);
		if (cl == '\0')  // Since needle consists of only one character, it is already a match as found above.
			goto foundneedle;
		cu = toupper(cl);
		++needle;
		goto jin;

		for (;;)
		{
			register unsigned a;
			register const unsigned char *rhaystack, *rneedle;
			do
			{
				a = *++haystack;
				if (a == '\0')
					goto ret0;
				if ((a == bl) || (a == bu))
					break;
				a = *++haystack;
				if (a == '\0')
					goto ret0;
			shloop:
				;
			} while ((a != bl) && (a != bu));

		jin:
			a = *++haystack;
			if (a == '\0')  // Remaining part of haystack is shorter than needle.  No match.
				goto ret0;

			if ((a != cl) && (a != cu)) // This promising candidate is not a complete match.
				goto shloop;            // Start looking for another match on the first char of needle.

			rhaystack = haystack-- + 1;
			rneedle = needle;
			a = tolower(*rneedle);

			if (tolower(*rhaystack) == (int)a)
				do
				{
					if (a == '\0')
						goto foundneedle;
					++rhaystack;
					a = tolower(*++needle);
					if (tolower(*rhaystack) != (int)a)
						break;
					if (a == '\0')
						goto foundneedle;
					++rhaystack;
					a = tolower(*++needle);
				} while (tolower(*rhaystack) == (int)a);

				needle = rneedle;		/* took the register-poor approach */

				if (a == '\0')
					break;
		} // for(;;)
	} // if (bl != '\0')
foundneedle:
	return (char*)haystack;
ret0:
	return 0;
}
#endif

static /* Base64 decoding */
size_t vlc_b64_decode_binary_to_buffer(uint8_t *p_dst, size_t i_dst, const char *p_src)
{
	static const int b64[256] = {
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
		52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
		-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
		15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
		-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
		41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
	};
	uint8_t *p_start = p_dst;
	uint8_t *p = (uint8_t *)p_src;

	int i_level;
	int i_last;

	for (i_level = 0, i_last = 0; (size_t)(p_dst - p_start) < i_dst && *p != '\0'; p++)
	{
		const int c = b64[(unsigned int)*p];
		if (c == -1)
			break;

		switch (i_level)
		{
		case 0:
			i_level++;
			break;
		case 1:
			*p_dst++ = (i_last << 2) | ((c >> 4) & 0x03);
			i_level++;
			break;
		case 2:
			*p_dst++ = ((i_last << 4) & 0xf0) | ((c >> 2) & 0x0f);
			i_level++;
			break;
		case 3:
			*p_dst++ = ((i_last & 0x03) << 6) | c;
			i_level = 0;
		}
		i_last = c;
	}

	return p_dst - p_start;
}


static inline Boolean toBool(bool b) { return b ? True : False; } // silly, no?

class MyRTSPClient : public RTSPClient
{
protected:
    Live555Client* parent;
    Boolean fSupportsGetParameter;
	std::string destination;
	int destPort;

	PresentationTimeSessionNormalizer* PtsSessionNormalizer;
public:
    MyRTSPClient( UsageEnvironment& env, char const* rtspURL, int verbosityLevel,char const* applicationName, portNumBits tunnelOverHTTPPortNum,Live555Client *p_sys) 
        : RTSPClient( env, rtspURL, verbosityLevel, applicationName,tunnelOverHTTPPortNum
#if LIVEMEDIA_LIBRARY_VERSION_INT >= 1373932800
                   , -1
#endif
                   )
                   , parent (p_sys)
                   , fSupportsGetParameter(False)
				   , PtsSessionNormalizer(new PresentationTimeSessionNormalizer(env))
    {

    }
	virtual ~MyRTSPClient()
	{
		if (PtsSessionNormalizer) {
			Medium::close(PtsSessionNormalizer);
		}
	}

    // over
	virtual Boolean setRequestFields(RequestRecord* request,char*& cmdURL, Boolean& cmdURLWasAllocated,char const*& protocolStr,char*& extraHeaders, Boolean& extraHeadersWereAllocated);

	void setDestination(std::string Addr, int DstPort);

	PresentationTimeSubsessionNormalizer* createNewPresentationTimeSubsessionNormalizer(FramedSource* inputSource,RTPSource* rtpSource,char const* codecName);

	static void continueAfterDESCRIBE(RTSPClient* client, int result_code, char* result_string);
	static void continueAfterOPTIONS(RTSPClient* client, int result_code, char* result_string);
	static void default_live555_callback(RTSPClient* client, int result_code, char* result_string);

    void setSupportsGetParameter(Boolean val) { fSupportsGetParameter = val; }

    Boolean isSupportsGetParameter() { return fSupportsGetParameter; }
};

void MyRTSPClient::setDestination(std::string Addr, int DstPort)
{
	destination = Addr;
	destPort = DstPort;
}

PresentationTimeSubsessionNormalizer* MyRTSPClient::createNewPresentationTimeSubsessionNormalizer(FramedSource* inputSource,RTPSource* rtpSource,char const* codecName)
{
	if (PtsSessionNormalizer) {
		return PtsSessionNormalizer->createNewPresentationTimeSubsessionNormalizer(inputSource, rtpSource, codecName);
	}
	return nullptr;
}

Boolean MyRTSPClient::setRequestFields(RTSPClient::RequestRecord* request,char*& cmdURL, Boolean& cmdURLWasAllocated,char const*& protocolStr,char*& extraHeaders, Boolean& extraHeadersWereAllocated) 
{
	if ((destination.size() > 0) && (strcmp(request->commandName(), "SETUP") == 0)) {  //只覆盖SETUP消息,其它消息仍然使用RTSPClient::setRequestFields()
		extraHeaders = new char[256];
		extraHeadersWereAllocated = True;

		Boolean streamUsingTCP = (request->booleanFlags() & 0x1) != 0;
		if (streamUsingTCP) {
			if (strcmp(request->subsession()->protocolName(), "UDP") == 0)
				sprintf(extraHeaders, "Transport: RAW/RAW/UDP;unicast;interleaved=0-1;destination=%s;client_port=%d-%d\r\n",destination.c_str(), destPort, destPort + 1);
			else
				sprintf(extraHeaders, "Transport: RTP/AVP;unicast;interleaved=0-1;destination=%s;client_port=%d-%d\r\n",destination.c_str(), destPort, destPort + 1);
		}
		else {
			sprintf(extraHeaders, "Transport: RTP/TCP;unicast;interleaved=0-1;destination=%s;client_port=%d-%d\r\n",destination.c_str(), destPort, destPort + 1);
		}
		return True;
	}
    // 其他rtsp请求消息处理
	return RTSPClient::setRequestFields(request, cmdURL, cmdURLWasAllocated, protocolStr, extraHeaders, extraHeadersWereAllocated);
}

void MyRTSPClient::continueAfterDESCRIBE(RTSPClient* client, int result_code, char* result_string)
{
	MyRTSPClient* pThis = static_cast<MyRTSPClient*>(client);
	pThis->parent->continueAfterDESCRIBE(result_code, result_string);
	delete[] result_string;
}

void MyRTSPClient::continueAfterOPTIONS(RTSPClient* client, int result_code, char* result_string)
{
	MyRTSPClient* pThis = static_cast<MyRTSPClient*>(client);

	Boolean serverSupportsGetParameter = RTSPOptionIsSupported("GET_PARAMETER", result_string);
	pThis->setSupportsGetParameter(serverSupportsGetParameter);
	pThis->parent->continueAfterOPTIONS(result_code, result_string);
	delete[] result_string;
}

void MyRTSPClient::default_live555_callback(RTSPClient* client, int result_code, char* result_string)
{
	MyRTSPClient* pThis = static_cast<MyRTSPClient*>(client);
	delete[]result_string;
	pThis->parent->live555Callback(result_code);
	//这里好好处理一下转换成我们的错误值
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
Live555Client::LiveTrack::LiveTrack(Live555Client* p_sys, void* sub, int buffer_size)
	: parent(p_sys)
	, media_sub_session(sub)
	, i_buffer(buffer_size)
{
	b_quicktime = false;
	b_muxed = false;
	b_discard_trunc = false;

	waiting = 0;
	b_rtcp_sync = false;
	i_pts = VLC_TS_INVALID;
	f_npt = 0.;
}

Live555Client::LiveTrack::~LiveTrack()
{
	if (p_buffer) {
		delete[] p_buffer;
		p_buffer = NULL;
	}
}

int Live555Client::LiveTrack::init()
{
    MediaSubsession* sub = static_cast<MediaSubsession*>(media_sub_session);
    p_buffer = new uint8_t [i_buffer + 4];

    if (!p_buffer){
        return -1;
	}
	fmt.type  = sub->mediumName();
	fmt.codec = sub->codecName();
	if (fmt.type == "audio") {

		fmt.audio.i_channels = sub->numChannels();
		fmt.audio.i_rate = sub->rtpTimestampFrequency();

		if ((fmt.codec == "MPA") ||
			(fmt.codec == "MPA-ROBUST") ||
			(fmt.codec == "X-MP3-DRAFT-00") ||
			(fmt.codec == "AC3")) {
			fmt.audio.i_rate = 0;
		}
		else if (fmt.codec == "L16"){
			fmt.audio.i_bitspersample = 16;
		}
		else if (fmt.codec == "L20"){
			fmt.audio.i_bitspersample = 20;
		}
		else if (fmt.codec == "L24"){
			fmt.audio.i_bitspersample = 24;
		}
		else if (fmt.codec == "L8"){
			fmt.audio.i_bitspersample = 8;
		}
		else if (fmt.codec == "DAT12"){
			fmt.audio.i_bitspersample = 12;
		}
		else if (fmt.codec == "PCMU"){
			fmt.audio.i_bitspersample = 8;
		}
		else if (fmt.codec == "PCMA"){
			fmt.audio.i_bitspersample = 8;
		}
		else if (fmt.codec == "G726")  {
			fmt.audio.i_rate = 8000;
			fmt.audio.i_channels = 1;
			if (!strcmp(sub->codecName() + 5, "40"))
				fmt.i_bitrate = 40000;
			else if (!strcmp(sub->codecName() + 5, "32"))
				fmt.i_bitrate = 32000;
			else if (!strcmp(sub->codecName() + 5, "24"))
				fmt.i_bitrate = 24000;
			else if (!strcmp(sub->codecName() + 5, "16"))
				fmt.i_bitrate = 16000;
		}
		else if (fmt.codec == "MP4A-LATM") {
			unsigned int i_extra;
			uint8_t      *p_extra;

			if ((p_extra = parseStreamMuxConfigStr(sub->fmtp_config(),
				i_extra)))
			{
				fmt.extra = std::string((char*)p_extra, i_extra);
				delete[] p_extra;
			}
			/* Because the "faad" decoder does not handle the LATM
			* data length field at the start of each returned LATM
			* frame, tell the RTP source to omit. */
			((MPEG4LATMAudioRTPSource*)sub->rtpSource())->omitLATMDataLengthField();
		}
		else if (fmt.codec == "MPEG4-GENERIC") {
			unsigned int i_extra;
			uint8_t      *p_extra;

			if ((p_extra = parseGeneralConfigStr(sub->fmtp_config(),
				i_extra)))
			{
				fmt.extra = std::string((char*)p_extra, i_extra);
				delete[] p_extra;
			}
		}
		else if (fmt.codec == "SPEEX") {
			if (fmt.audio.i_rate == 0){
				fmt.audio.i_rate = 8000;
			}
		}
		else if (fmt.codec == "VORBIS") {
			unsigned int i_extra;
			unsigned char *p_extra;
			if ((p_extra = parseVorbisConfigStr(sub->fmtp_config(),
				i_extra)))
			{
				fmt.extra = std::string((char*)p_extra, i_extra);
				delete[] p_extra;
			}
		}

	}
	else if (fmt.type == "video") {
		if (fmt.codec == "H264")
		{
			unsigned int i_extra = 0;
			uint8_t      *p_extra = NULL;

			if ((p_extra = parseH264ConfigStr(sub->fmtp_spropparametersets(),
				i_extra)))
			{
				fmt.extra = std::string((char*)p_extra, i_extra);
				delete[] p_extra;
			}
		}
#if LIVEMEDIA_LIBRARY_VERSION_INT >= 1393372800 // 2014.02.26
		else if (fmt.codec == "H265")
		{
			unsigned int i_extra1 = 0, i_extra2 = 0, i_extra3 = 0, i_extraTot;
			uint8_t      *p_extra1 = NULL, *p_extra2 = NULL, *p_extra3 = NULL;
			p_extra1 = parseH264ConfigStr(sub->fmtp_spropvps(), i_extra1);
			p_extra2 = parseH264ConfigStr(sub->fmtp_spropsps(), i_extra2);
			p_extra3 = parseH264ConfigStr(sub->fmtp_sproppps(), i_extra3);
			i_extraTot = i_extra1 + i_extra2 + i_extra3;
			if (i_extraTot > 0)
			{
				if (p_extra1)
				{
					fmt.extra = std::string((char*)p_extra1, i_extra1);
					delete[] p_extra1;
				}
				if (p_extra2)
				{
					fmt.extra += std::string((char*)p_extra2, i_extra2);
					delete[] p_extra2;
				}
				if (p_extra3)
				{
					fmt.extra += std::string((char*)p_extra3, i_extra3);
					delete[] p_extra3;
				}
			}
		}
#endif
		else if (fmt.codec == "MP4V-ES")
		{
			unsigned int i_extra;
			uint8_t      *p_extra;

			if ((p_extra = parseGeneralConfigStr(sub->fmtp_config(),
				i_extra)))
			{
				fmt.extra = std::string((char*)p_extra, i_extra);
				delete[] p_extra;
			}
		}
		else if (!strcmp(sub->codecName(), "X-QT") ||
			!strcmp(sub->codecName(), "X-QUICKTIME") ||
			!strcmp(sub->codecName(), "X-QDM") ||
			!strcmp(sub->codecName(), "X-SV3V-ES") ||
			!strcmp(sub->codecName(), "X-SORENSONVIDEO"))
		{
			b_quicktime = true;
		}
		else if (!strcmp(sub->codecName(), "DV"))
		{
			b_discard_trunc = true;
		}
		else if (!strcmp(sub->codecName(), "THEORA"))
		{
			unsigned int i_extra;
			uint8_t      *p_extra;

			if ((p_extra = parseVorbisConfigStr(sub->fmtp_config(),
				i_extra)))
			{
				fmt.extra = std::string((char*)p_extra, i_extra);
				delete[] p_extra;
			}
		}

	}
    if( sub->rtcpInstance() != NULL )
    {
        sub->rtcpInstance()->setByeHandler( Live555Client::LiveTrack::streamClose, this );
    }

    return 0;
}

const char* Live555Client::LiveTrack::getSessionId() const
{
    MediaSubsession* sub = static_cast<MediaSubsession*>(media_sub_session);
    if (!sub)
        return "";

    return sub->sessionId();
}

const char* Live555Client::LiveTrack::getSessionName() const
{
    MediaSubsession* sub = static_cast<MediaSubsession*>(media_sub_session);
    if (!sub)
        return "";

    return sub->controlPath();
}

const char* Live555Client::LiveTrack::getUrl() const
{
    RTSPClient* client = static_cast<RTSPClient*>(parent->rtsp);

    return client->url();
}

void Live555Client::LiveTrack::streamRead(void *opaque, unsigned int i_size,unsigned int i_truncated_bytes, struct timeval pts,unsigned int duration)
{
    Live555Client::LiveTrack* pThis = static_cast<Live555Client::LiveTrack*>(opaque);
    pThis->parent->onStreamRead(pThis, i_size, i_truncated_bytes, pts, duration);
}

void Live555Client::LiveTrack::streamClose(void* opaque)
{
    Live555Client::LiveTrack* pThis = static_cast<Live555Client::LiveTrack*>(opaque);
    pThis->parent->onStreamClose(pThis);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void Live555Client::taskInterruptData( void *opaque )
{
    Live555Client *pThis = static_cast<Live555Client*>(opaque);

    pThis->i_no_data_ti++;

    /* Avoid lock */
    pThis->event_data = (char)0xff;
}

void Live555Client::taskInterruptRTSP( void *opaque )
{
    Live555Client *pThis = static_cast<Live555Client*>(opaque);

    /* Avoid lock */
    pThis->event_rtsp = (char)0xff;
}

bool Live555Client::waitLive555Response( int i_timeout /* ms */ )
{
    TaskToken task;
    BasicTaskScheduler* sch = (BasicTaskScheduler*)scheduler;
    //event_rtsp = 0;
    if( i_timeout > 0 )
    {
        /* Create a task that will be called if we wait more than timeout ms */
        task = sch->scheduleDelayedTask( i_timeout*1000,taskInterruptRTSP,this);
    }
    event_rtsp = 0;
    b_error = true;
	live555ResultCode = 0;
    sch->doEventLoop(&event_rtsp );
    //here, if b_error is true and i_live555_ret = 0 we didn't receive a response
    if( i_timeout > 0 )
    {
        /* remove the task */
        sch->unscheduleDelayedTask( task );
    }
    return !b_error;
}

#define DEFAULT_FRAME_BUFFER_SIZE 500000

int Live555Client::setup()
{
    MediaSubsessionIterator *iter   = NULL;
    MediaSubsession         *sub    = NULL;
	MyRTSPClient* client = static_cast<MyRTSPClient*>(rtsp);
    UsageEnvironment* environment = static_cast<UsageEnvironment*>(env);

    int            i_client_port;
    int            i_return = RTSP_OK;
    unsigned int   i_receive_buffer = 0;
    int            i_frame_buffer = DEFAULT_FRAME_BUFFER_SIZE;
    unsigned const thresh = 200000; /* RTP reorder threshold .2 second (default .1) */

    i_client_port = u_port_begin; //var_InheritInteger( p_demux, "rtp-client-port" );

    /* here print sdp on debug */
    printf("SDP content:\n%s", sdp.c_str());
    /* Create the session from the SDP */
    if( !(m_pMediaSession = MediaSession::createNew( *environment, sdp.c_str() ) ) )
    {
        return RTSP_ERR;
    }

    iter = new MediaSubsessionIterator( *m_pMediaSession);
    while( ( sub = iter->next() ) != NULL )
    {
        bool b_init;
        LiveTrack* tk;
        /* Value taken from mplayer */
        if( !strcmp( sub->mediumName(), "audio" ) )
            i_receive_buffer = 200000;
        else if( !strcmp( sub->mediumName(), "video" ) )
            i_receive_buffer = 2000000;
        else if( !strcmp( sub->mediumName(), "text" ) )
            ;
        else continue;

        if( i_client_port != -1 )
        {
            sub->setClientPortNum( i_client_port );
            i_client_port += 2;
        }

        if( strcasestr( sub->codecName(), "REAL" ) )
        {
            //msg_Info( p_demux, "real codec detected, using real-RTSP instead" );
            //b_real = true; /* This is a problem, we'll handle it later */
            continue;
        }

        if( !strcmp( sub->codecName(), "X-ASF-PF" ) )
            b_init = sub->initiate( 0 );
        else
            b_init = sub->initiate();

        if( b_init )
        {
			FramedFilter* normalizerFilter = client->createNewPresentationTimeSubsessionNormalizer(
			sub->readSource(), sub->rtpSource(),sub->codecName());

			sub->addFilter(normalizerFilter);

            if( sub->rtpSource() != NULL )
            {
                int fd = sub->rtpSource()->RTPgs()->socketNum();

                /* Increase the buffer size */
                if( i_receive_buffer > 0 )
                    increaseReceiveBufferTo( *environment, fd, i_receive_buffer );

                /* Increase the RTP reorder timebuffer just a bit */
                sub->rtpSource()->setPacketReorderingThresholdTime(thresh);
            }

            /* Issue the SETUP */
            if( client )
            {
                client->sendSetupCommand( *sub, MyRTSPClient::default_live555_callback, False,toBool( b_rtsp_tcp ),False/*toBool( p_sys->b_force_mcast && !b_rtsp_tcp )*/ );
                if( !waitLive555Response(200) )
                {
                    /* if we get an unsupported transport error, toggle TCP
                     * use and try again */
                    if(live555ResultCode == 461 ){
                        client->sendSetupCommand( *sub, MyRTSPClient::default_live555_callback, False,!toBool( b_rtsp_tcp ), False );
					}
					if(live555ResultCode != 461 || !waitLive555Response() ){
                        continue;
                    } else {
                        b_rtsp_tcp = !b_rtsp_tcp;
                    }
                }
            }

            /* Check if we will receive data from this subsession for
             * this track */
            if( sub->readSource() == NULL ) continue;
            if( !b_multicast )
            {
                /* We need different rollover behaviour for multicast */
                b_multicast = IsMulticastAddress( sub->connectionEndpointAddress() );
            }

            tk = new LiveTrack(this, sub, i_frame_buffer);
            if( !tk ){
                delete iter;
                return RTSP_ERR;
            }

            if (tk->init())
            {
                delete tk;
            }
            else {
                listTracks.push_back(tk);
                OnRecvSDPInfo(tk);
            }
        }
    }
    delete iter;
    
    if( !listTracks.size() ) {
        i_return = RTSP_ERR;
    }

    /* Retrieve the starttime if possible */
    f_npt_start = m_pMediaSession->playStartTime();

    /* Retrieve the duration if possible */
    if (m_pMediaSession->playEndTime() > 0){
        f_npt_length = m_pMediaSession->playEndTime();
	}
    /* */
    b_no_data = true;
    i_no_data_ti = 0;

    u_port_begin = i_client_port;

    return i_return;
}

int Live555Client::demux(void)
{
    TaskToken      task;
    MyRTSPClient*    client = static_cast<MyRTSPClient*>(rtsp);
    TaskScheduler* sch = static_cast<TaskScheduler*>(scheduler);

    if (b_is_stoped) {
        return RTSP_EOF;
    }
#if 1
    /* Check if we need to send the server a Keep-A-Live signal */
    if( b_timeout_call && client && m_pMediaSession)
    {
        char *psz_bye = NULL;
        if (client->isSupportsGetParameter())
            client->sendGetParameterCommand( *m_pMediaSession, NULL, psz_bye );
        else {
            Authenticator authenticator;
            authenticator.setUsernameAndPassword( user_name.c_str(), password.c_str() );
            client->sendOptionsCommand(NULL, &authenticator);
        }

        b_timeout_call = false;
    }
#endif

    /* First warn we want to read data */
    event_data = 0;

    for (auto it = listTracks.begin(); it != listTracks.end(); ++it)
    {
        LiveTrack *tk = *it;

        MediaSubsession* sub = static_cast<MediaSubsession*>(tk->getMediaSubsession());
        uint8_t* p_buffer = tk->buffer();

        if( tk->getFormat().codec == "AMR" || tk->getFormat().codec == "AMR-WB" )
        {
            p_buffer++;
        }
        else if( tk->getFormat().codec == "H261" || 
			     tk->getFormat().codec == "H264" || tk->getFormat().codec == "H265" )
        {
            p_buffer += 4;
        }

        if( !tk->isWaiting() )
        {
            tk->doWaiting(1);
            sub->readSource()->getNextFrame( p_buffer, tk->buffer_size(),Live555Client::LiveTrack::streamRead, tk, Live555Client::LiveTrack::streamClose, tk );
        }
    }

    /* Create a task that will be called if we wait more than 300ms */
    task = sch->scheduleDelayedTask( 300000, taskInterruptData, this );

    /* Do the read */
    sch->doEventLoop( &event_data );

    /* remove the task */
    sch->unscheduleDelayedTask( task );
#if 1
    /* Check for gap in pts value */
     for (auto it = listTracks.begin(); it != listTracks.end(); ++it)
     {
		LiveTrack *tk = *it;
		MediaSubsession* sub = static_cast<MediaSubsession*>(tk->getMediaSubsession());

		if( !tk->b_muxed && !tk->b_rtcp_sync && sub->rtpSource() && sub->rtpSource()->hasBeenSynchronizedUsingRTCP() )
		{
             onResetPcr();
             tk->b_rtcp_sync = true;
             /* reset PCR */
             tk->i_pts = VLC_TS_INVALID;
             tk->f_npt = 0.;
             i_pcr = 0;
             f_npt = 0.;
         }
     }
#endif
    if( b_multicast && b_no_data && ( i_no_data_ti > 120 ) )
    {
     return RTSP_TIMEOUT;
    }
    else if( !b_multicast && b_no_data && ( i_no_data_ti > 34 ) )
    {
     return RTSP_TIMEOUT;
    }

    if( i_no_data_ti > 33 || live555ResultCode == 454) {
        return RTSP_TIMEOUT;
    }
    return RTSP_OK;
}

void Live555Client::Run(void)
{
	uint32_t last_call_timeout= timeGetTime();

	while (!m_bThreadEnd)
	{
		uint32_t lasting = (timeGetTime() - last_call_timeout);
		if (lasting >= (i_timeout - 2)) {
			last_call_timeout= timeGetTime();
			b_timeout_call = true;
		}
		if (demux() != RTSP_OK){
			break;
		}
	}
}

Live555Client::Live555Client(void)
    : env(NULL)
    , scheduler(NULL)
    , rtsp(NULL)
    , m_pMediaSession(NULL)
    , event_rtsp(0)
    , event_data(0)
    , b_get_param(false)
    , b_error(false)
    , live555ResultCode(0)
    , i_timeout(60)
    , b_timeout_call(false)
    , i_pcr(VLC_TS_0)
    , f_seekTime(-1.0)
    , f_npt(0)
    , f_npt_length(0)
    , f_npt_start(0)
    , b_multicast(false)
    , b_no_data(false)
    , i_no_data_ti(0)
    , b_is_stoped(true)
    , u_port_begin(0)
    , user_agent("RTSPClient/1.0")
    , b_rtsp_tcp(false)
{
    // 初始化调度
	scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*(TaskScheduler*)scheduler);
}

Live555Client::~Live555Client(void)
{
	stop();
	UsageEnvironment* environment = static_cast<UsageEnvironment*>(env);
	TaskScheduler* sch = static_cast<TaskScheduler*>(scheduler);
	environment->reclaim();
	delete sch;
}

int Live555Client::PlayRtsp(std::string Uri,bool bUseTcp)
{
    int ret;
	if (Uri.length() == 0) {
		return RTSP_ERR;
	}
    if ((ret = open(Uri)) != RTSP_OK) {
        goto exit;
    }
	this->b_rtsp_tcp = bUseTcp;
	if ((ret = setup()) != RTSP_OK) {
       goto exit;
	}
    if ((ret = play()) != RTSP_OK) {
       goto exit;
    }
	return ret;
exit:
    stop();
    return ret;
}

// option-> des->setup ->play
int Live555Client::open(std::string Uri)
{
    Authenticator authenticator;
    UsageEnvironment* environment = static_cast<UsageEnvironment*>(env);
    TaskScheduler* sch = static_cast<TaskScheduler*>(scheduler);

    // new 一个RTSP对象
    rtsp = new MyRTSPClient(*environment, Uri.c_str(), 0, user_agent.c_str(), 0, this);
    if (!rtsp) {
        return RTSP_ERR;
    }

    if (user_name.length() != 0 && password.length() != 0) {
        authenticator.setUsernameAndPassword(user_name.c_str(), password.c_str());
    }

    rtsp->sendOptionsCommand(&MyRTSPClient::continueAfterOPTIONS, &authenticator);
    if (!waitLive555Response(3000)){
        return RTSP_TIMEOUT;
    }
    f_npt_start = 0;
    return RTSP_OK;
}

int Live555Client::play()
{
    /* The PLAY */
    rtsp->sendPlayCommand(*m_pMediaSession, MyRTSPClient::default_live555_callback, f_npt_start, -1, 1);

    if (!waitLive555Response(3000)){
        return RTSP_TIMEOUT;
    }

    /* Retrieve the timeout value and set up a timeout prevention thread */
    i_timeout = rtsp->sessionTimeoutParameter();
    if (i_timeout <= 0)
        i_timeout = 60; /* default value from RFC2326 */
    i_pcr = 0;

    /* Retrieve the starttime if possible */
    f_npt_start = m_pMediaSession->playStartTime();
    if (m_pMediaSession->playEndTime() > 0)
        f_npt_length = m_pMediaSession->playEndTime();

    // now create thread for get data
    b_is_stoped = false;
    b_timeout_call = true;

    return RTSP_OK;
}

void Live555Client::stop()
{
    if( rtsp && m_pMediaSession )
        rtsp->sendTeardownCommand( *m_pMediaSession, NULL );

    if( m_pMediaSession ) {
        Medium::close( m_pMediaSession );
        m_pMediaSession = NULL;
    }

    if( rtsp ) {
        RTSPClient::close( rtsp );
        rtsp = NULL;
    }

	for (size_t i = 0; i < listTracks.size(); i++) {
		delete listTracks[i];
	}
    listTracks.clear();
    u_port_begin = 0;
}

void Live555Client::setUser(const char* user_name, const char* password)
{
    this->user_name = user_name;
    this->password = password;
}

void Live555Client::setUserAgent(const char* user_agent)
{
    this->user_agent = user_agent;
}

void Live555Client::setDestination(std::string Addr, int DstPort)
{
	MyRTSPClient* client = static_cast<MyRTSPClient*>(rtsp);
	client->setDestination(Addr, DstPort);
}

void Live555Client::clearDestination()
{
	MyRTSPClient* client = static_cast<MyRTSPClient*>(rtsp);
	std::string tmpS;
	client->setDestination(tmpS, -1);
}

void Live555Client::continueAfterDESCRIBE( int result_code, char* result_string )
{
	live555ResultCode = result_code;
    if ( result_code == 0 )
    {
        char* sdpDescription = result_string;
        if( sdpDescription )
        {
            sdp = std::string( sdpDescription );
            b_error = false;
        }
    }
    else
        b_error = true;

	event_rtsp = 1;
}

void Live555Client::continueAfterOPTIONS( int result_code, char* result_string )
{
    b_get_param =
      // If OPTIONS fails, assume GET_PARAMETER is not supported but
      // still continue on with the stream.  Some servers (foscam)
      // return 501/not implemented for OPTIONS.
      result_code == 0
      && result_string != NULL
      && strstr( result_string, "GET_PARAMETER" ) != NULL;

    RTSPClient* client = static_cast<RTSPClient*>(rtsp);
    client->sendDescribeCommand( MyRTSPClient::continueAfterDESCRIBE );
}

void Live555Client::live555Callback( int result_code )
{
	live555ResultCode = result_code;
    b_error = live555ResultCode != 0;
    event_rtsp = 1;
}

void Live555Client::onStreamRead(LiveTrack* track, unsigned int i_size,unsigned int i_truncated_bytes, struct timeval pts,unsigned int duration )
{
    MediaSubsession* sub = static_cast<MediaSubsession*>(track->getMediaSubsession());
    int64_t i_pts = (int64_t)pts.tv_sec * INT64_C(1000000) + (int64_t)pts.tv_usec;

    int64_t i_dts;
    i_pts &= INT64_C(0x00ffffffffffffff);
    track->setNPT(sub->getNormalPlayTime(pts));

    if( track->isQuicktime())
    {
        QuickTimeGenericRTPSource *qtRTPSource =(QuickTimeGenericRTPSource*)sub->rtpSource();
        QuickTimeGenericRTPSource::QTState &qtState = qtRTPSource->qtState;
        uint8_t *sdAtom = (uint8_t*)&qtState.sdAtom[4];

        if(track->getFormat().type == "video") {
            if( qtState.sdAtomSize < 16 + 32 )
            {
                /* invalid */
                event_data = (char)0xff;
                track->doWaiting(0);
                return;
            }
            track->getFormat().codec = std::string((char*)sdAtom, 4);;
            track->getFormat().video.i_width  = (sdAtom[28] << 8) | sdAtom[29];
            track->getFormat().video.i_height = (sdAtom[30] << 8) | sdAtom[31];

            if( track->getFormat().codec == "avc1" )
            {
                uint8_t *pos = (uint8_t*)qtRTPSource->qtState.sdAtom + 86;
                uint8_t *endpos = (uint8_t*)qtRTPSource->qtState.sdAtom
                                  + qtRTPSource->qtState.sdAtomSize;
                while (pos+8 < endpos) {
                    unsigned int atomLength = pos[0]<<24 | pos[1]<<16 | pos[2]<<8 | pos[3];
                    if( atomLength == 0 || atomLength > (unsigned int)(endpos-pos)) break;
                    if( memcmp(pos+4, "avcC", 4) == 0 && atomLength > 8 && atomLength <= INT_MAX )
                    {
						track->getFormat().extra = std::string((char*)pos + 8, atomLength - 8);
                        break;
                    }
                    pos += atomLength;
                }
            }
            else{
				track->getFormat().extra.erase(track->getFormat().extra.size()-16, 16);
            }
        }
        else {
            if( qtState.sdAtomSize < 24 )
            {
                event_data = (char)0xff;
                track->doWaiting(0);
                return;
            }
			track->getFormat().codec = std::string((char*)sdAtom, 4);;
			track->getFormat().audio.i_bitspersample = (sdAtom[22] << 8) | sdAtom[23];
        }
    }

    if( i_truncated_bytes > 0 )
    {
        if( track->buffer_size() < 2000000 )
        {
            void *p_tmp;
            p_tmp = realloc(track->p_buffer, track->i_buffer * 2 );
            if( p_tmp != NULL ){
               track->p_buffer = (uint8_t*)p_tmp;
               track->i_buffer *= 2;
            }
        }
    }

    assert( i_size <= track->buffer_size() );
    unsigned int out_size = i_size;

    if( track->getFormat().codec == "AMR" || track->getFormat().codec == "AMR-WB"){
        AMRAudioSource *amrSource = (AMRAudioSource*)sub->readSource();
        track->buffer()[0] = amrSource->lastFrameHeader();
        out_size++;
    }
    else if( track->getFormat().codec == "H264" || track->getFormat().codec == "H265" )
    {
        /* Normal NAL type */
        track->buffer()[0] = 0x00;
        track->buffer()[1] = 0x00;
        track->buffer()[2] = 0x00;
        track->buffer()[3] = 0x01;
        out_size += 4;
    }

    if( i_pcr < i_pts ){
        i_pcr = i_pts;
    }

    if( track->getNPT() > 0 && (track->getNPT() < f_npt_length || f_npt_length <= 0 ) )
        f_npt = track->getNPT();

    i_dts = ( track->getFormat().codec == "MPV" ) ? VLC_TS_INVALID : (VLC_TS_0 + i_pts);
	if(track->getFormat().type == "video"){
		HandleRTPData(track->buffer(), out_size, i_truncated_bytes, i_pts, i_dts);
	}
    /* warn that's ok */
    event_data = (char)0xff;
    /* we have read data */
    track->doWaiting(0);
    b_no_data = false;
    i_no_data_ti = 0;

    if( i_pts > 0 && !track->b_muxed ){
		track->i_pts = i_pts;
    }
}

void Live555Client::onStreamClose(LiveTrack* track)
{
    event_rtsp = (char)0xff;
    event_data = (char)0xff;
	b_is_stoped = true;
}


unsigned char* parseH264ConfigStr(char const* configStr,unsigned int& configSize)
{
	char *dup, *psz;
	size_t i_records = 1;

	configSize = 0;

	if (configStr == NULL || *configStr == '\0')
		return NULL;

	psz = dup = strdup(configStr);

	/* Count the number of commas */
	for (psz = dup; *psz != '\0'; ++psz)
	{
		if (*psz == ',')
		{
			++i_records;
			*psz = '\0';
		}
	}

	size_t configMax = 5 * strlen(dup);
	unsigned char *cfg = new unsigned char[configMax];
	psz = dup;
	for (size_t i = 0; i < i_records; ++i)
	{
		cfg[configSize++] = 0x00;
		cfg[configSize++] = 0x00;
		cfg[configSize++] = 0x00;
		cfg[configSize++] = 0x01;

		configSize += vlc_b64_decode_binary_to_buffer(cfg + configSize,configMax - configSize, psz);

		psz += strlen(psz) + 1;
	}
	free(dup);
	return cfg;
}

uint8_t * parseVorbisConfigStr(char const* configStr,unsigned int& configSize)
{
	configSize = 0;
	if (configStr == NULL || *configStr == '\0')
		return NULL;
#if LIVEMEDIA_LIBRARY_VERSION_INT >= 1332115200 // 2012.03.20
	unsigned char *p_cfg = base64Decode(configStr, configSize);
#else
	char* configStr_dup = strdup(configStr);
	unsigned char *p_cfg = base64Decode(configStr_dup, configSize);
	free(configStr_dup);
#endif
	uint8_t *p_extra = NULL;
	/* skip header count, ident number and length (cf. RFC 5215) */
	const unsigned int headerSkip = 9;
	if (configSize > headerSkip && ((uint8_t*)p_cfg)[3] == 1)
	{
		configSize -= headerSkip;
		p_extra = new uint8_t[configSize];
		memcpy(p_extra, p_cfg + headerSkip, configSize);
	}
	delete[] p_cfg;
	return p_extra;
}
