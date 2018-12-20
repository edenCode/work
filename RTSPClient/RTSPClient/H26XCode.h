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
/// @brief: 检测设备是否支持硬件加速
//==============================================
H26XVIDEO_API bool __stdcall check_suppor_thwaccel();
//==============================================
/// @brief: 创建视频解码器
/// @param: void* hwnd 窗口地址
/// @param: uint32_t codec_type 视频编码格式
/// @return:视频解码器句柄
//==============================================
H26XVIDEO_API video_decoder_t __stdcall create_video_decoder(void* hwnd,int codec_type);

//==============================================
/// @brief: 销毁视频解码器
/// @param: video_encoder_t handle 视频解码器句柄
/// @return:0-成功 other-失败
//==============================================
H26XVIDEO_API int __stdcall destroy_video_decoder(video_decoder_t handle);

//==============================================
/// @brief: 视频解码
/// @param: video_decoder_t handle 视频解码器句柄
/// @param: const unsigned char* bytes 待解码的数据
/// @param: unsigned int usize 待解码的数据长度
/// @param: bframe 是否是完整一帧(内部自带缓存，帧结束设置true)
/// @return:0-成功 other-失败
//==============================================
H26XVIDEO_API int  __stdcall video_decoder_decode(video_decoder_t handle,const char* bytes,int nlen,bool bframe);

//==============================================================================
/// @brief: 创建视频编码器
/// @param: int codec_type			编码类型
/// @param: int width				视频宽度
/// @param: int height				视频高度
/// @param: int bitrate				编码码率
/// @param: int framerate			帧率
/// @return:视频编码器句柄
///=============================================================================
H26XVIDEO_API video_encoder_t __stdcall create_video_encoder(int codec_type,int width, int height, int bitrate, int framerate);

//==============================================================================
/// @brief: 销毁视频编码器
/// @param: video_encoder_t handle 视频编码器句柄
/// @return:H26X_ERR_OK 销毁成功其他失败
///=============================================================================
H26XVIDEO_API int __stdcall destroy_video_encoder(video_encoder_t handle);

//==============================================================================
/// @brief: 设置编码视频回调
/// @param: video_encoder_t handle			视频编码器句柄
/// @param: encoderstream_callback callback 编码回调函数
/// @param: void* user						用户自定义指针
///=============================================================================
H26XVIDEO_API int __stdcall encoder_setcallback(video_encoder_t handle,encoderstream_callback callback,void* user);

//==============================================================================
/// @brief: 编码强制关键帧
/// @param: video_encoder_t handle			视频编码器句柄
/// @param: bool bkeyframe					true 关键 false 默认当前帧
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_ctrlkey(video_encoder_t handle,bool bkeyframe);

//==============================================================================
/// @brief: 编码视频数据
/// @param: video_encoder_t handle			视频编码器句柄
/// @param: const char* in_nv12				NV12 视频数据
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_encode(video_encoder_t handle,const char* in_nv12);

//==============================================================================
/// @brief: 获取H264编码信息数据
/// @param: video_encoder_t handle			视频编码器句柄
/// @param: char* sps						sps 信息数据
/// @param: char* pps						pps 信息数据
/// @param: int* nsps						nsps 信息数据大小
/// @param: int* npps						npps 信息数据大小
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_h264headers(video_encoder_t handle,char* sps,char* pps,int* nsps,int* npps);

//==============================================================================
/// @brief: 获取H265编码信息数据
/// @param: video_encoder_t handle			视频编码器句柄
/// @param: char* vps						vps 信息数据
/// @param: char* sps						sps 信息数据
/// @param: char* pps						pps 信息数据
/// @param: int* nvps						vps 信息数据大小
/// @param: int* nsps						nsps 信息数据大小
/// @param: int* npps						npps 信息数据大小
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_h265headers(video_encoder_t handle,char* vps,char* sps,char* pps,int* nvps,int* nsps,int* npps);

//==============================================================================
/// @brief: 获取内部编码缓存地址
/// @param: video_encoder_t handle			视频编码器句柄
/// @param: unsigned char** Y				编码缓存Y地址
/// @param: unsigned char** UV				编码缓存UV地址
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_getbuffer(video_encoder_t handle,unsigned char** Y,unsigned char** UV);

//==============================================================================
/// @brief: 编码视频数据  一般和video_encoder_getbuffer成对出现
/// @param: video_encoder_t handle			视频编码器句柄
///=============================================================================
H26XVIDEO_API int __stdcall video_encoder_encbuffer(video_encoder_t handle);

#endif