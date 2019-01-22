#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include "RTSPClient.hh"
#include "strDup.hh"
#include "base64.hh"

#include "mp4v2.h"
typedef struct stList{
    int sizel;
    char * buffer;
}List;

char* cmd[] = {"OPTIONS","DESCRIBE","SETUP","PLAY"};
char const * rtspURL = "RTSP://192.168.0.193:8080/h264";
int get_sdp = -1;
char * sdp = NULL;
char * frame = NULL;
int  frame_size = -1;
int maxsize = 2*1024*1024;
int setup = 1;

int w_index = 0;
int r_index = 0;

int fps = 0;
int width = 0;
int height = 0;

char sps[1024] = {'\0'};
int sps_size = 0;

char pps[1024] = {'\0'};
int pps_size = 0;

int result = -1;
int64_t start_time = -1;

// save 10 frames
char * buffer[10] = {NULL};
List list_[10];

int is_rEnd = 0;
int buffer_remalloc(int size)
{
    int i = 0;
    for(i = 0;i < sizeof(buffer);i ++)
    {
        buffer[i] = (char *)realloc(buffer[i],size);
        if(!buffer[i])
        {
           printf(" buffer malloc failed \n");
        }

        list_[i].buffer = buffer[i];
        list_[i].sizel  = size;
    }

    return i;
}

int buffer_free()
{
    int i = 0;
    for(i = 0;i < sizeof(buffer);i ++)
    {

        if(!buffer[i])
            free(buffer[i]);

        list_[i].buffer = NULL;
        list_[i].sizel  = 0;
    }


}

void response(RTSPClient * client,int result_code,char * result_str)
{
    size_t size = strlen(result_str);
    if(result_code == 0)
    {
        printf(" %s ,result str : %s \n",cmd[setup - 1],result_str);
        if(setup == 2)
        {
            get_sdp = 1;
            sdp = strDupSize(result_str,size);
        }
    }
    else
        printf(" %s ,result error code %d \n",cmd[setup - 1],result_code);

}

void afterGettingFunc(void* clientData, unsigned frameSize,
                unsigned numTruncatedBytes,
                struct timeval presentationTime,
                unsigned durationInMicroseconds)
{
    int * ff = (int *)clientData;
    char * tmp = NULL;
    result = frameSize;
    start_time = presentationTime.tv_sec * 1000000 + presentationTime.tv_usec;
    if(numTruncatedBytes > 0)
    {
       printf("Currnet size : %d ,numTruncatedBytes %d,framesize %d \n",maxsize,numTruncatedBytes,frameSize);
       if(frameSize > maxsize)
            buffer_remalloc(frameSize);
    }

    list_[w_index].sizel = frameSize;
    w_index ++; // got a picture
    if(w_index == r_index)
    {
        *ff = 1;
    }
    
}

void onCloseFunc(void * data)
{

    int * ff = (int *)data;

    *ff = 99;
    is_rEnd = 1;
}


#define H264_START_CODE 0x000001
uint32_t h264_find_next_start_code (uint8_t *pBuf,
                    uint32_t bufLen)
{
  uint32_t val;
  uint32_t offset;

  offset = 0;
  if (pBuf[0] == 0 && pBuf[1] == 0 && pBuf[2] == 0 && pBuf[3] == 1) {
    pBuf += 4;
    offset = 4;
  } else if (pBuf[0] == 0 && pBuf[1] == 0 && pBuf[2] == 1) {
    pBuf += 3;
    offset = 3;
  }
  val = 0xffffffff;
  while (offset < bufLen - 3) {
    val <<= 8;
    val |= *pBuf++;
    offset++;
    if (val == H264_START_CODE) {
      return offset - 4;
    }
    if ((val & 0x00ffffff) == H264_START_CODE) {
      return offset - 3;
    }
  }
  return 0;
}

char * skip_startCode(char * bufffer)
{

    int offset;
    if (bufffer[0] == 0 && bufffer[1] == 0 && bufffer[2] == 0 && bufffer[3] == 1) {
        offset = 4;
    } else if (bufffer[0] == 0 && bufffer[1] == 0 && bufffer[2] == 1) {
        offset = 3;
    }

    return bufffer + offset;
}

int parse_nalu(char * data,int size,int & nal_size)
{
    int i = 0;
    int found = 0;
    int j = 0;
    
    while(1)
    {
        if(data[i] == 0 && data[i + 1] == 0 && data[ i + 2] == 1)
        {
            if(found)
            {
                nal_size = i - j;
                break;
            }
                
            found = 1;
            i = i + 3;
            j = i;
        }
        else  if(data[i] == 0 && data[i + 1] == 0 && data[ i + 2] == 0 && data[i + 4] == 1)
        {
            if(found)
            {
                nal_size = i - j;
                break;
            }
                
            found = 1;
            i = i + 4;
            j = i;
        }
    }

    return 0;
}

void writer_mp4(void * arg)
{
    int count = 0;
    int nal_type = -1;
    int nal_size = -1;
    char *  p = NULL;
    MP4FileHandle  handle = MP4Create("./test.mp4",0);

    MP4SetTimeScale(handle,90000);

    MP4TrackId videoid =  MP4AddH264VideoTrack(handle,90000,90000 / 25,width,height,sps[0],sps[1],sps[2],3);

    p = base64Decode(sps,sps_size);

    MP4AddH264SequenceParameterSet(handle,videoid,p,sps_size);

    p = base64Decode(pps,pps_size);
    MP4AddH264PictureParameterSet(handle,videoid,p,pps_size);
    
    while(1)
    {
        if(r_index == w_index){
            if(count  ++ > 20)
                break;
            sleep(1);
            continue;
        }
        
        p  = buffer[r_index];
        nal_size = h264_find_next_start_code(p,list_[r_index].sizel);

        p[0] = (nal_size&0xff000000)>>24;
        p[1] = (nal_size&0x00ff0000)>>16;
        p[2] = (nal_size&0x0000ff00)>>8;
        p[3] = nal_size&0x000000ff;

        p = skip_startCode(p);

        
        nal_type = p[0] & 0x1f;

        MP4WriteSample(handle,videoid,(const uint8_t *)list_[r_index].buffer,list_[r_index].sizel,MP4_INVALID_DURATION,0,nal_type == 5);

        r_index = (r_index ++) % 10;
    }

    is_rEnd = 1;
}


void parse_spsORpps(char * sdp,char *  sps,char *  pps)
{
    int size = strlen(sdp);
    int i = 0;
    char * tmp = sdp;

    for(i = 0;i < size;i ++)
    {
        if(*tmp ++ ==',')
        {
            *tmp = '\0';
            strcpy(sps,sdp);
            strcpy(pps,sdp + strlen(sps) + 1);
        }
    }
}

void handle_response(void * arg)
{
    char * ff = (char *) arg;

    *ff = 1;
}
int handle_callback(TaskScheduler * ts)
{
     char watch_dog = 0;
     BasicTaskScheduler *  scheduler = (BasicTaskScheduler *) ts;
     scheduler->scheduleDelayedTask(30 * 1000,handle_response,&watch_dog);

     scheduler->doEventLoop(&watch_dog);

     return watch_dog >= 1 ? 1 : 0;
}

int main(int argc,char ** argv)
{
    pthread_t tid;
    int doNext = -1;
    Authenticator * authenticator = NULL;
    int port  = 0; // this port for RTCP
    BasicTaskScheduler *  scheduler= BasicTaskScheduler::createNew();
    BasicUsageEnvironment * env = BasicUsageEnvironment::createNew(scheduler);
    if(argc > 3)
        port = atoi(argv[2]);

    buffer_remalloc(maxsize);

    pthread_create(&tid,NULL,writer_mp4,NULL);

    RTSPClient * client = RTSPClient::createNew(env,rtspURL,port,1);
restart:
    client->sendOptionsCommand(response,authenticator);

    if(handle_callback(scheduler) > 0)
    {
        
    }

    setup ++;
    client->sendDescribeCommand(response,authenticator);
    if(handle_callback(scheduler) > 0)
    {
        
    }

    setup ++;

    if(get_sdp)
    {
        MediaSubsession * sub;
        MediaSession * session;
        session = MediaSession::createNew(env,sdp);
        

        MediaSubsessionIterator * it = new MediaSubsessionIterator(session);
        for(it; ; it ++)
        {
            if(it->next())
            {
               sub = it->next();
               printf("mediaName : %s \nCodeName ； %s \n",sub->mediumName(),sub->codecName());
            }
            else
                break;
        }

        if(sub)
            client->sendSetupCommand(sub,response,False,true,False,authenticator);
        if(handle_callback(scheduler) > 0)
        {
            
        }

        width = sub->videoWidth();
        height = sub->videoHeight();
        fps    = sub->videoFPS();
        printf("The video width %d,height %d,FPS %d \n",width,height,fps);
        if(result >= 0)
            client->sendPlayCommand(sub,response,start_time);
        if(handle_callback(scheduler) > 0)
        {
            
        }

        if(sub && strcmp(sub->codecName(),"H264"))
        {
           const char * tmp = sub->fmtp_spropparametersets();
           printf("sps base64 encode ； %s \n",tmp);
           // 在sdp中带有srs-respond-porp 属性，其中 sps 和 pps 是以，分隔的。
           parse_spsORpps((char *)tmp,(char *)(&sps[0]),(char *)(&pps[0]));
           sps_size = strlen(sps) + 1;
           pps_size = strlen(pps) + 1;
                     
            while(1)
            {

                if(doNext == 99)
                {
                    break;
                }

                w_index = w_index % sizeof(buffer);
                /*
                * 获取一帧数据
                **/
                sub->readSource()->getNextFrame((unsigned char *)buffer[w_index],maxsize,afterGettingFunc,&doNext,onCloseFunc,&doNext);

                handle_callback(scheduler);
            }
        }
           
            
    }
    else
    {
        setup = 0;
       
        // if get sdp failed,to reconnect.
        goto restart;
    }

    printf("waiting record over \n");
    while(!is_rEnd);
    printf("file record over \n");
    buffer_free();
    
    return 0;
}

