#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"

#define URL "rtsp://192.168.110.179:37441/h264"

int main(int argc,char ** argv)
{
	int retval = -1;
	
	printf(" .libavformat version : %d \n .libavformat configuration : %s\n",avformat_version(),avformat_configuration());
	printf(" .libavformat License : %s \n",avformat_license());
	printf(" .libavutil version	  : %d ,%s \n",avutil_version(),av_version_info());
	printf(" .libavutil configuration : %s \n",avutil_configuration());
	printf(" .libavutil License	  : %s \n",avutil_license());
	
	AVFormatContext * ps;
	AVDictionary *options = NULL;
	AVDictionaryEntry *t = NULL;
	AVCodecContext * pCodectx = NULL;
	AVCodec * pCodec;
	AVPacket * packet;
	
	avformat_network_init();
	printf("11111111111111111111111111 sssss\n");
	av_dict_set(&options, "timeout", "10000", 0);

	if(!options) printf("error options \n");
	ps = avformat_alloc_context();
	if(!ps) 
	{
		printf("avformat_alloc_context failed \n");
		return -1;
	}
	retval = avformat_open_input(&ps,URL,NULL,&options);
	if(retval < 0) 
	{
		printf("libavformat error : %s \n",av_err2str(retval));
		return -2;
	}

	while(t = av_dict_get(ps->metadata, "", t, AV_DICT_IGNORE_SUFFIX))
	{
		printf(" key: %s,value: %s \n",t->key,t->value);
	}
	printf("nb_streams : %d \n",ps->nb_streams);

	retval = avformat_find_stream_info(ps,NULL);
	if(retval < 0)  printf("error : %s \n",av_err2str(retval));
	
	printf("stream index : %d \n",ps->streams[0]->index);
	printf("AVRational : %f \n",av_q2d(ps->streams[0]->time_base));
	
	while(t = av_dict_get(ps->streams[0]->metadata, "", t, AV_DICT_IGNORE_SUFFIX))
	{
		printf(" key: %s,value: %s \n",t->key,t->value);
	}
	printf("1111111111111111111111111111\n");
	pCodectx = avcodec_alloc_context3(NULL);
	if(!pCodectx) printf("avcodec_alloc_context3 failed \n");
	retval = avcodec_parameters_to_context(pCodectx,ps->streams[0]->codecpar);
    if(retval < 0)  printf("error : %s \n",av_err2str(retval));
	
	pCodec = avcodec_find_decoder(pCodectx->codec_id);
	if(!pCodec)  printf("avcodec_find_decoder failed \n");
	
	retval = avcodec_open2(pCodectx,pCodec,&options);
	if(retval < 0) printf("avcodec_open2 error : %s \n",av_err2str(retval));
	
	packet = av_packet_alloc();
	
	av_init_packet(packet);
	printf("ww22222222222222222222222222222\n");
	AVFrame frame;
	while(1)
	{
		if((retval = av_read_frame(ps,packet)) < 0)
		{
			printf("av_read_frame error : %s \n",av_err2str(retval));
		}
		
		retval = avcodec_send_packet(pCodectx,packet);
		if(retval < 0)
		{
			printf("avcodec_send_packet error : %s \n",av_err2str(retval));
		}
		
		retval = avcodec_receive_frame(pCodectx,&frame);
		if(retval < 0)
		{
			printf("avcodec_receive_frame error : %s \n",av_err2str(retval));
		}
	}
	return 0;
}