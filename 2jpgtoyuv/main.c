#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> // stat
#include <dirent.h> // openDIR
#include <errno.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include "libyuv.h"

#define MVLOG_UNIT_NAME yuv
#include "mvLog.h"

#define PATH_NUMS 5
#undef  PATH_MAX
#define PATH_MAX 256
 
// 时间函数
typedef struct timespec highres_time_t;

static inline void highres_gettime(highres_time_t *ptr) {
    clock_gettime(CLOCK_REALTIME, ptr);
}




static inline double highres_elapsed_ms(highres_time_t *start, highres_time_t *end) {
    struct timespec temp;
    if((end->tv_nsec - start->tv_nsec) < 0) {
        temp.tv_sec = end->tv_sec - start->tv_sec - 1;
        temp.tv_nsec = 1000000000 + end->tv_nsec-start->tv_nsec;
    } else {
        temp.tv_sec = end->tv_sec - start->tv_sec;
        temp.tv_nsec = end->tv_nsec - start->tv_nsec;
    }
    return (double)(temp.tv_sec * 1000) + (((double)temp.tv_nsec) * 0.000001);
}

int check_littel_endian()
{
	int ret = -1;
	union{
		int i;
		char c;
	}test;
	
	test.i = 0x01;
	ret = test.c;
	return ret == 1 ? 1 : 0;
}
int  Load_image(char * file,int index)
{
	char path[256];
	int ret = 0;
	int width,height,cp;
	int w,h;
	unsigned char * img,*imgresize;
	unsigned char * y  = NULL;
	unsigned char * uv = NULL;
	FILE * fp = NULL;
	
	if(access("./yuv_image",F_OK) != 0)
	{
		ret = mkdir("./yuv_image",0755);
		if(ret < 0)
		{
			perror("mkdir : ");
		}
	}
	
	img = stbi_load(file,&width,&height,&cp,3);
	printf(" %d , %s, w %d, h %d,cp %d\n",index,file,width,height,cp);
	if( width % 4 != 0 && height % 4 != 0)
	{
		return -1;
		w   = (width + 3) >> 2 << 2;
		h   = (height + 3) >> 2 << 2;

		if(img)
			free(img);
	}
	else
	{
		w = width;
		h = height;
		imgresize = (unsigned char *) malloc(w * h * 3);
		stbir_resize_uint8(img,width,height,0,imgresize,w,h,0,3);
	}
	if(img)
		free(img);
	img = (unsigned char *) malloc(w * h * 4);
	if(check_littel_endian())
	{
		RGB24ToARGB(imgresize,w * 3,img,w * 4,w,h);
		printf(" To transfer RGB24ToARGB\n");
	}
	else
	{
		RAWToARGB(imgresize,w * 3,img,w * 4,w,h);
		printf(" To transfer RAWToARGB\n");
	}

	if(imgresize)
		free(imgresize);
	
	imgresize = img;
	
	sprintf(path,"%s/%dx%d_%04d.yuv","yuv_image",w,h,index);
	mvLog(MVLOG_DEBUG,"T %s to %s  ... \n",file,path);
	if(!y)
	{
		y  = (unsigned char *)malloc(w * h * 3 / 2);
		uv = y + w * h;
	}
	ARGBToNV12(imgresize,4 * w,y,w,uv,w / 2,w,h);
	
	fp = fopen(path,"w");
	if(!fp)
	{
		mvLog(MVLOG_ERROR,"fopen %s failed : %s\n",path,strerror(errno));
		return -1;
	}
	
	fwrite(y,1,w * h,fp);
	fwrite(uv,1,w * h / 2,fp);
	fclose(fp);
	
	return 0;
}


int imageFilter(const struct dirent *pDir)
{
	int ret = -1;
	char * p = NULL;
	char ext[3][10] = {".jpg",".png",".bmp"};
	int nums = 3, i;
	
	if( pDir->d_type != DT_REG)
	{
		return 0;
	}
	
	ret = 0;
	for(i = 0; i < nums;i ++)
	{
		p = strstr(pDir->d_name,ext[i]);
		if(p)
		{
			ret = 1;
			break;
		}	
	}
	printf("%s \n",pDir->d_name);
	return ret;
}

int Lookup_image(char * path)
{
	int nums = 0,i;
	char path_1[1024];
	struct dirent ** list;
	
	nums = scandir(path,&list,imageFilter,alphasort);
	printf(" ### %d need to be transfered \n",nums);
	for(i = 0;i < nums; i ++)
	{
		//printf(" %d, %s \n",i,list[i]->d_name);
		sprintf(path_1,"%s/%s",path,list[i]->d_name);
		Load_image(path_1,i);
	}
	return 0;
}

// To do read image,transfer to yuv data.
int main(int argc,const char **argv)
{
	char path[PATH_NUMS][PATH_MAX];
	int i = 1;
	
	mvLogLevelSet(MVLOG_DEBUG);
	mvLogDefaultLevelSet(MVLOG_DEBUG);
	
	if(argc < 2)
	{
		fprintf(stderr,"Usage : %s path \n",argv[0]);
		exit(0);
	}
	
	for( i = 1; i < argc;i ++)
	{
		sprintf(path[i],"%s",argv[i]);
		mvLog(MVLOG_DEBUG," %s  \n",path[i]);
		printf(" ## %s \n",path[i]);
		Lookup_image(path[i]);
	}
	printf("\n");
	
	return 0;
}
