#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef  struct TagVRECT_{     /*  矩阵结构体       */
	int   x;        //  矩阵的左边缘
	int   y;       //  矩阵的右边缘
	int   width;         //  矩阵的上边缘
	int   height;      //  矩阵的下边缘	
}Rect;

int Nv12_roi( unsigned char * src,unsigned char *dest,int swidth,int sheight,Rect * pRect)
{
	int i,j ;
	unsigned char * suv = NULL;
	unsigned char * duv = NULL;
	
	unsigned char * src_y = NULL;
	unsigned char * dst_y = NULL;
	
	unsigned char * src_uv = NULL;
	unsigned char * dst_uv = NULL;
	
	if(!src || !dest)
		return -1;
	
	suv = src + swidth * sheight;
	duv = dest + pRect->width * pRect->height;
	
	src_y = src + swidth * pRect->y + pRect->x;
	dst_y = dest;
	
	src_uv = suv + swidth * pRect->y / 2 + pRect->x;
	dst_uv = duv;
	
	
	for( i = 0;i < pRect->height; i ++)
	{
		memcpy(dst_y + i * pRect->width,src_y + i * swidth,pRect->width);
	}
	
	for(j = 0;j < pRect->height / 2; j ++)
	{
		memcpy(dst_uv + j * pRect->width,src_uv + j * swidth,pRect->width);
	}
	return 0;
}

int main(int argc,char * argv[])
{
	int iRet = 0;
	int width = 0;
	int height = 0;
	unsigned char * buffer = NULL;
	FILE * fp = NULL;
	Rect rect;
	char file[48];
	char * temp = NULL;
	int i = 0;
	
	int dwidth = -1;
	int dheight = -1;
	
	if(argc  < 4)
	{
		printf("Usage ：%s  yuv width height \n",argv[0]);
		return -1;
	}
	
	width = atoi(argv[2]);
	height = atoi(argv[3]);
	printf("width %d,height %d \n",width,height);
	
	if(!buffer)
	{
		buffer =(unsigned char *)malloc(width * height * 3);
		temp = (unsigned char *)malloc(width * height * 3);
	}
	
	fp = fopen(argv[1],"r");
	if(fp)
	{
		if(!buffer) return -1;
		
		iRet = fread(buffer,1,width * height * 3 / 2,fp);
		printf("read %d bytes \n",iRet);
	}
	
	fclose(fp);
	fp = NULL;
	
	dwidth = width / 2;
	dheight = height / 2;
	
	rect.x = 0;
	rect.y = 0;
	rect.width = (dwidth / 4) * 4;
	rect.height = (dheight / 4 ) * 4;
	
	
	
	sprintf(file,"%d_%d_%d.yuv",rect.width,rect.height,i);
	printf(" Rect x %d ,y %d,width %d,height %d \n",rect.x,rect.y,rect.width,rect.height);
	Nv12_roi(buffer,temp,width,height,&rect);
	fp = fopen(file,"w");
	if(fp)
	{
		fwrite(temp,1,rect.width * rect.height * 3 /2,fp);
		fclose(fp);
	}
	
	i++;
	rect.x = dwidth;
	rect.y = 0;
	sprintf(file,"%d_%d_%d.yuv",rect.width,rect.height,i);
	printf(" Rect x %d ,y %d,width %d,height %d \n",rect.x,rect.y,rect.width,rect.height);
	Nv12_roi(buffer,temp,width,height,&rect);
	fp = fopen(file,"w");
	if(fp)
	{
		fwrite(temp,1,rect.width * rect.height * 3 /2,fp);
		fclose(fp);
	}
	
	i++;
	rect.x = 0;
	rect.y = dheight;
	sprintf(file,"%d_%d_%d.yuv",rect.width,rect.height,i);
	printf(" Rect x %d ,y %d,width %d,height %d \n",rect.x,rect.y,rect.width,rect.height);
	Nv12_roi(buffer,temp,width,height,&rect);
	fp = fopen(file,"w");
	if(fp)
	{
		fwrite(temp,1,rect.width * rect.height * 3 /2,fp);
		fclose(fp);
	}
	
	
	i++;
	rect.x = dwidth;
	rect.y = dheight;
	sprintf(file,"%d_%d_%d.yuv",rect.width,rect.height,i);
	printf(" Rect x %d ,y %d,width %d,height %d \n",rect.x,rect.y,rect.width,rect.height);
	Nv12_roi(buffer,temp,width,height,&rect);
	fp = fopen(file,"w");
	if(fp)
	{
		fwrite(temp,1,rect.width * rect.height * 3 /2,fp);
		fclose(fp);
	}
	return 0;
}