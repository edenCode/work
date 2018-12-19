#include <stdio.h>
#include "libyuv.h"

#define iMaxW 2048
#define iMaxH 1536

#define BF_ALIGNBYTE4(value) ((value+3)>>2)
#define BF_ALIGNBYTE8(value) ((value+7)>>3)
#define BF_ALIGNBYTE32(value) ((value+31)>>5)
#define IMAGEWIDTHBYTES(bits)  ((((bits) + 31) >> 5) << 2)

  


typedef  struct TagVRECT{     /*  矩阵结构体       */
	int   left;        //  矩阵的左边缘
	int   right;       //  矩阵的右边缘
	int   top;         //  矩阵的上边缘
	int   bottom;      //  矩阵的下边缘	
}VRECT;

typedef  struct TagVRECT_{     /*  矩阵结构体       */
	int   x;        //  矩阵的左边缘
	int   y;       //  矩阵的右边缘
	int   width;         //  矩阵的上边缘
	int   height;      //  矩阵的下边缘	
}Rect;

static void gray_nearest_scale(unsigned char  * src, unsigned char  * dst,int  srcWidth, int  srcHeight, VRECT *pROIRect,int  dstWidth, int  dstHeight);
 

#define CLAMP(a, s, m) ((a) < (s)? (s) : ((a) > (m) ? (m) : (a)))
int NV12CutImage(unsigned char * src, unsigned char * dest, int srcW, int srcH, int x0, int y0, int x1, int y1)
{
    int dstw = 0;
    int dsth = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int srcwh = 0;
    int dstwh = 0;

    dstw = x1 - x0 + 1;
    dsth = y1 - y0 + 1;

    unsigned char * srcUV = src + srcW * srcH;
    unsigned char * destUV = dest + dstw * dsth;

    for (i = y0; i <= y1; i++)
    {
        for (j = x0; j <= x1; j++)
        {
            dest[(i - y0) * dstw + (j - x0)] = src[i * srcW + j];

            destUV[((i - y0) / 2 * (dstw / 2) + (j - x0) / 2) * 2 + 0] = srcUV[((i / 2) * (srcW / 2) + (j / 2)) * 2 + 0];

            destUV[((i - y0) / 2 * (dstw / 2) + (j - x0) / 2) * 2 + 1] = srcUV[((i / 2) * (srcW / 2) + (j / 2)) * 2 + 1];

        }
    }


    return 0;

}

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
#if 0
int main()
{
	int retval = 0;
	FILE * fp = NULL;
	char buffer[2048 * 1536 * 2]; // 栈空间被使用完导致段错误。
	char buffer2[2048 * 1536 * 2];
	VRECT rect;
	printf("test yuv ... \n");
	fp = fopen("2048x1536_0001.yuv","r");
	if(!fp)
	{
		printf("file open failed \n");
		return -1;
	}
	
	retval = fread(buffer,1,iMaxW * iMaxH,fp);
	printf("fread %d \n",retval);
	if(retval > 0) fclose(fp);
	
	printf("gray_nearest_scale start ... \n");
	rect.left = 0;
	rect.top = 0;
	rect.right = 2048;
	rect.bottom = 1536;
	
	gray_nearest_scale(buffer,buffer2,iMaxW,iMaxH,&rect,300,300);
	
	printf("gray_nearest_scale end  ... \n");
	
	fp = fopen("300x300_y.yuv","w");
	if(fp)
	{
		
		fwrite(buffer2,1,300 * 300,fp);
		fclose(fp);
	}
	return 0;
}
#else

char buffer[2048 * 1536 * 2];
char buffer2[2048 * 1536 * 2];
char bgr[iMaxH * iMaxW * 4];
int main()
{
	int retval = -1;
	int size = 0;
	int width = 0;
	int height = 0;
	Rect roi;
	printf("hello world \n");
	
	//	int retval = 0;
	FILE * fp = NULL;

	VRECT rect;
	printf("test yuv ... \n");
	fp = fopen("2048x1536_0001.yuv","r");
	if(!fp)
	{
		printf("file open failed \n");
		return -1;
	}
	
	retval = fread(buffer,1,iMaxW * iMaxH,fp);
	printf("fread %d \n",retval);
	if(retval > 0) fclose(fp);
	 //NV12ToARGB(const uint8* src_y, int src_stride_y,
     //          const uint8* src_uv, int src_stride_uv,
     //          uint8* dst_argb, int dst_stride_argb,
     //          int width, int height);
	printf("NV12ToARGB start ... \n");
	//NV12ToARGB(buffer,iMaxW * iMaxH,buffer + iMaxH * iMaxW,iMaxH * iMaxW / 2,bgr,iMaxW,iMaxW,iMaxH);
	ConvertToARGB(buffer,iMaxW * iMaxH * 3 / 2,bgr,iMaxW * 4,0,0,iMaxW,iMaxH,iMaxW,iMaxH,0,FOURCC_CM24);
	printf("NV12ToARGB end ... \n");
	fp = fopen("2048x1536_img.bgr","w");
	if(fp)
	{
		fwrite(bgr,1,iMaxH * iMaxW * 3,fp);
		fclose(fp);
	}
	
	NV12CutImage(buffer,buffer2,iMaxW,iMaxH,0,((iMaxH /3 )/ 2 )*2,(iMaxW / 2) * 2 - 1,(iMaxH  / 2) * 2 - 1);
	width = (((iMaxW / 2) * 2 - 1) + 1);
	height = (((iMaxH  / 2) * 2 - 1) - ((iMaxH /3 )/ 2 )*2+ 1);
	//size = ;
	printf("width %d ,height %d \n",width,height);
	fp = fopen("cut_img.yuv","w");
	if(fp)
	{
		fwrite(buffer2,1,width * height * 3 / 2,fp);
		fclose(fp);
	}

	printf("gray_nearest_scale start ... \n");
	rect.left = 0;
	rect.top = 0;
	rect.right = 2048;
	rect.bottom = 1536;
	
	gray_nearest_scale(buffer,buffer2,iMaxW,iMaxH,&rect,300,300);
	
	printf("gray_nearest_scale end  ... \n");
	
	fp = fopen("300x300_y.yuv","w");
	if(fp)
	{
		
		fwrite(buffer2,1,300 * 300,fp);
		fclose(fp);
	}
	
	

	return 0;
}
#endif

#if 1
static void gray_nearest_scale(unsigned char  * src, unsigned char  * dst,int  srcWidth, int  srcHeight, VRECT *pROIRect,int  dstWidth, int  dstHeight)
{
    int  sw = srcWidth;  //register keyword is for local var to accelorate
    int  sh = srcHeight;
    int  dw = dstWidth;
    int  dh = dstHeight;
    int  y, x;
    int  ROIRectW, ROIRectH;
    int  yStart,xStart;

    unsigned long int srcy, srcx;
    unsigned long int xrIntFloat_16;
    unsigned long int yrIntFloat_16;

    unsigned char * dst_y_slice = dst; //memory start pointer of dest y
    unsigned char * src_y_slice;

    VRECT ROIRect;
	//printf("file %s,line %d,fuction %s [src width %d,height %d] [dst width %d,height %d]\n",__FILE__,__LINE__,__func__,srcWidth,srcHeight,dstWidth,dstHeight);
    if(pROIRect == NULL)
    {
        ROIRect.left   = 0;
        ROIRect.top    = 0;
        ROIRect.right = (sw>>2)<<2;
        ROIRect.bottom = ((sh>>1)<<1);
    }
    else
    {

        ROIRect.left = ((BF_ALIGNBYTE4(pROIRect->left))<<2);
        ROIRect.top = ((pROIRect->top>>1)<<1);
        ROIRect.right = ((BF_ALIGNBYTE4(pROIRect->right))<<2);
        ROIRect.bottom = ((pROIRect->bottom>>1)<<1);
    }

	printf("file %s,line %d,fuction %s [pROIRect left %d,top %d,right %d,bottom %d]\n",__FILE__,__LINE__,__func__,pROIRect->left,pROIRect->top,pROIRect->right,pROIRect->bottom);
    ROIRectW = ROIRect.right  - ROIRect.left;
    ROIRectH = ROIRect.bottom - ROIRect.top;

    yStart = ROIRect.top;
    xStart = ROIRect.left;

    xrIntFloat_16 = (ROIRectW << 16) / dw + 1; //better than float division
    yrIntFloat_16 = (ROIRectH << 16) / dh + 1;

    dw = BF_ALIGNBYTE4(dw);
    dw = (dw<<2);
	printf("file %s,line %d,fuction %s [Roi x %ld,y %ld,w %ld,h %ld] [xstart %d,ystart %d,xrIntFloat_16 %d,yrIntFloat_16 %d] [dw %d,dh %d] \n",__FILE__,__LINE__,__func__,ROIRect.left,ROIRect.top,ROIRectW,ROIRectH,xStart,yStart,xrIntFloat_16,yrIntFloat_16,dw,dh);
    for (y = 0; y < dh; ++y)
    {
        srcy = (y * yrIntFloat_16) >> 16;
        //src_y_slice = src + srcy * sw;
      // printf("%s,%d ,src %p,srcy %d \n",__func__,__LINE__,src,srcy);
        src_y_slice = src + yStart * sw + xStart + srcy * sw; // (src + yStart * sw + xStart)图锟斤拷指锟斤拷锟狡讹拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷辖锟斤拷锟绞硷拷悖拷锟絪rcy * sw锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷诖锟斤拷锟?
		//printf("%s,%d ,src %p,srcy %d,src_y_slice %p \n",__func__,__LINE__,src,srcy,src_y_slice);
        //for(x = 0; x < (dw & ~7); ++x)
        for(x = 0; x < dw; ++x)
        {
            srcx = (x * xrIntFloat_16) >> 16;
            dst_y_slice[x] = src_y_slice[srcx];
         }
         dst_y_slice += dw;
    }
}
//end
#endif