
#ifndef LIVE555_CLIENT_H__
#define LIVE555_CLIENT_H__

#include <stdint.h>
#include <string>
#include <vector>
#include "thread.h"

#define RTSP_OK			(0)
#define RTSP_TIMEOUT	(1)
#define RTSP_EOF		(2)
#define RTSP_ERR		(3)
#define RTSP_USR_STOP	(4)

class RTSPClient;
class MediaSession;

class Live555Client : public CThread
{
public:
    class LiveTrack {
    public:
        struct media_format {

			std::string	type;		/*"audio" "video" "text"*/
			std::string codec;		/*"H264","H265"*/
			std::string extra;		/*sps pps*/
            uint32_t i_bitrate;

            struct {
                int i_rate         ;
                int i_channels    ;
                int i_bitspersample;
            } audio;

            struct {
                int i_width ;
                int i_height;
            } video;
		};
	public:
        Live555Client* parent;

        void* media_sub_session;

        bool            b_muxed;
        bool            b_quicktime;
        bool            b_discard_trunc;

        uint8_t         *p_buffer;
        unsigned int    i_buffer;

        bool            b_rtcp_sync;
        char            waiting;
        int64_t         i_pts;
        double          f_npt;

        media_format    fmt;

    public:
        LiveTrack(Live555Client* p_sys, void* sub, int buffer_size);
        virtual ~LiveTrack();
        int init();

        bool isWaiting() { return waiting != 0; }
        void doWaiting(char val) { waiting = val; }
        bool isMuxed() { return b_muxed; }
        bool isQuicktime() { return b_quicktime; }
        bool discardTruncated() { return b_discard_trunc; }
        void* getMediaSubsession() { return media_sub_session; }
        const char* getSessionId() const;
        const char* getSessionName() const;
        const char* getUrl() const;
        void setNPT(double npt) {f_npt = npt;}
        double getNPT() {return f_npt;}

        media_format& getFormat() { return fmt; }

        uint8_t* buffer() { return p_buffer; }
        unsigned int buffer_size() { return i_buffer; }
        static void streamRead(void *opaque, unsigned int i_size,unsigned int i_truncated_bytes, struct timeval pts,unsigned int duration );
        static void streamClose(void *opaque );
    };
private:
    void* env;
    void* scheduler;
	RTSPClient* rtsp;
	MediaSession* m_pMediaSession;

    char			 event_rtsp;
    char			 event_data;

    bool			 b_get_param;
    bool			 b_error;
	int				 live555ResultCode;/* live555返回的http状态值 */
    unsigned int     i_timeout;     /* session timeout value in seconds */
    bool             b_timeout_call;/* mark to send an RTSP call to prevent server timeout */

    int64_t          i_pcr; /* The clock */
    double           f_seekTime;
    double           f_npt;
    double           f_npt_length;
    double           f_npt_start;

    bool             b_multicast;
    bool             b_no_data;     /* if we never received any data */
    int              i_no_data_ti;  /* consecutive number of TaskInterrupt */

    bool             b_rtsp_tcp;
    bool             b_is_stoped;

    unsigned short   u_port_begin;  /* RTP port that client will be use */

    std::string		sdp;
    std::string		user_agent;
	std::string		user_name;
	std::string		password;

	std::vector<LiveTrack*> listTracks;

    static void taskInterruptData(void *opaque );

    static void taskInterruptRTSP(void *opaque );

    bool waitLive555Response( int i_timeout = 0 /* ms */ );

    int  setup();

    int demux(void);

    int open(std::string Uri);

    int play(void);

    void stop(void);

public:
    Live555Client(void);

	virtual void Run(void);

    virtual ~Live555Client(void);

    int64_t getCurrentTime() { return (int64_t)(f_npt * 1000000.0); }

    int64_t getStartTime() { return (int64_t)(f_npt_start * 1000000.0); }

    void setUser(const char* user_name, const char* password);

    void setUserAgent(const char* user_agent);
	/* double ports ,like 7000-7001, another for rtcp*/
	void setDestination(std::string Addr, int DstPort);

	void clearDestination();

    void setRTPPortBegin(unsigned short port_begin) { u_port_begin = port_begin; }

    unsigned short getRTPPortNoUse() { return u_port_begin; }

    //流信息
    virtual void OnRecvSDPInfo(LiveTrack* track) {}

    //一帧数据
    virtual void HandleRTPData(uint8_t* p_buffer, int i_size, int i_truncated_bytes, int64_t pts, int64_t dts) {}
   
	virtual void onResetPcr(){}

    // callback functions
    void continueAfterDESCRIBE( int result_code, char* result_string );
   
	void continueAfterOPTIONS( int result_code, char* result_string );
 
	void live555Callback( int result_code );
   
	void onStreamRead(LiveTrack* track, unsigned int i_size,unsigned int i_truncated_bytes, struct timeval pts,unsigned int duration );
    
	void onStreamClose(LiveTrack* track);
protected:
	int		PlayRtsp(std::string Uri,bool bUseTcp);

};

#endif//LIVE555_CLIENT_H__
