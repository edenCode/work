#ifndef H26X_ENCODE_DECODE_HH
#define H26X_ENCODE_DECODE_HH

#ifdef H26XVIDEO_EXT_API
#define H26XVIDEO_API extern "C" __declspec(dllexport)
#else
#define H26XVIDEO_API extern "C" __declspec(dllimport)
#endif

typedef long video_encoder_t;
typedef long video_decoder_t;

typedef enum tagVIDEO_CODEC_TYPE
{
	VIDEO_CODEC_H264 = 1,				
	VIDEO_CODEC_H265 = 2	
}VIDEO_CODEC_TYPE;


#define	H26X_ERR_OK					 0
#define H26X_ERR_INVALIDPARAM		-1
#define H26X_ERR_ALLOCMEM			-2

typedef void  (__stdcall *encoderstream_callback)(unsigned char* frame,int nlen,bool bframe,void* user);

//==============================================
/// @brief: ����豸�Ƿ�֧��Ӳ������
//==============================================
H26XVIDEO_API bool __stdcall check_suppor_thwaccel();
//==============================================
/// @brief: ������Ƶ������
/// @param: void* hwnd ���ڵ�ַ
/// @param: uint32_t codec_type ��Ƶ�����ʽ
/// @return:��Ƶ���������
//==============================================
H26XVIDEO_API video_decoder_t __stdcall create_video_decoder(void* hwnd,int codec_type);

//==============================================
/// @brief: ������Ƶ������
/// @param: video_encoder_t handle ��Ƶ���������
/// @return:0-�ɹ� other-ʧ��
//==============================================
H26XVIDEO_API int __stdcall destroy_video_decoder(video_decoder_t handle);

//==============================================
/// @brief: ��Ƶ����
/// @param: video_decoder_t handle ��Ƶ���������
/// @param: const unsigned char* bytes �����������
/// @param: unsigned int usize ����������ݳ���
/// @param: bframe �Ƿ�������һ֡(�ڲ��Դ����棬֡��������true)
/// @return:0-�ɹ� other-ʧ��
//==============================================
H26XVIDEO_API int  __stdcall video_decoder_decode(video_decoder_t handle,const char* bytes,int nlen,bool bframe);

//==============================================================================
/// @brief: ������Ƶ������
/// @param: int codec_type			��������
/// @param: int width				��Ƶ���
/// @param: int height				��Ƶ�߶�
/// @param: int bitrate				��������
/// @param: int framerate			֡��
/// @return:��Ƶ���������
///=============================================================================
H26XVIDEO_API video_encoder_t __stdcall create_video_encoder(int codec_type,int width, int height, int bitrate, int framerate);

//==============================================================================
/// @brief: ������Ƶ������
/// @param: video_encoder_t handle ��Ƶ���������
/// @return:H26X_ERR_OK ���ٳɹ�����ʧ��
///=============================================================================
H26XVIDEO_API int __stdcall destroy_video_encoder(video_encoder_t handle);

//==============================================================================
/// @brief: ���ñ�����Ƶ�ص�
/// @param: video_encoder_t handle			��Ƶ���������
/// @param: encoderstream_callback callback ����ص�����
/// @param: void* user						�û��Զ���ָ��
///=============================================================================
H26XVIDEO_API int __stdcall encoder_setcallback(video_encoder_t handle,encoderstream_callback callback,void* user);

//==============================================================================
/// @brief: ����ǿ�ƹؼ�֡
/// @param: video_encoder_t handle			��Ƶ���������
/// @param: bool bkeyframe					true �ؼ� false Ĭ�ϵ�ǰ֡
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_ctrlkey(video_encoder_t handle,bool bkeyframe);

//==============================================================================
/// @brief: ������Ƶ����
/// @param: video_encoder_t handle			��Ƶ���������
/// @param: const char* in_nv12				NV12 ��Ƶ����
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_encode(video_encoder_t handle,const char* in_nv12);

//==============================================================================
/// @brief: ��ȡH264������Ϣ����
/// @param: video_encoder_t handle			��Ƶ���������
/// @param: char* sps						sps ��Ϣ����
/// @param: char* pps						pps ��Ϣ����
/// @param: int* nsps						nsps ��Ϣ���ݴ�С
/// @param: int* npps						npps ��Ϣ���ݴ�С
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_h264headers(video_encoder_t handle,char* sps,char* pps,int* nsps,int* npps);

//==============================================================================
/// @brief: ��ȡH265������Ϣ����
/// @param: video_encoder_t handle			��Ƶ���������
/// @param: char* vps						vps ��Ϣ����
/// @param: char* sps						sps ��Ϣ����
/// @param: char* pps						pps ��Ϣ����
/// @param: int* nvps						vps ��Ϣ���ݴ�С
/// @param: int* nsps						nsps ��Ϣ���ݴ�С
/// @param: int* npps						npps ��Ϣ���ݴ�С
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_h265headers(video_encoder_t handle,char* vps,char* sps,char* pps,int* nvps,int* nsps,int* npps);

//==============================================================================
/// @brief: ��ȡ�ڲ����뻺���ַ
/// @param: video_encoder_t handle			��Ƶ���������
/// @param: unsigned char** Y				���뻺��Y��ַ
/// @param: unsigned char** UV				���뻺��UV��ַ
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_getbuffer(video_encoder_t handle,unsigned char** Y,unsigned char** UV);

//==============================================================================
/// @brief: ������Ƶ����  һ���video_encoder_getbuffer�ɶԳ���
/// @param: video_encoder_t handle			��Ƶ���������
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_encbuffer(video_encoder_t handle);

#endif