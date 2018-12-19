#include <stdio.h>


int NV12ToBGR( unsigned char* ydata,unsigned char * uvdata, unsigned char* bgr, int w, int h );

#define YUV_TO_R(y, u, v) ((((y)<<8) + 360*((v)-128)) >> 8)
#define YUV_TO_G(y, u, v) ((((y)<<8) -  88*((u)-128) - 184*((v)-128)) >> 8)
#define YUV_TO_B(y, u, v) ((((y)<<8) + 455*((u)-128)) >> 8)

#define CLAMP(x, l, h) (((x)<(l)) ? (l) : ((x)>(h)) ? (h) : (x))

int NV12ToBGR( unsigned char* ydata,unsigned char * uvdata, unsigned char* bgr, int w, int h )
{
	int i, j, m;
	int y, u, v;
    unsigned char *yp, *cp;
	
 //   if (w<1 || h<1 || yuv==NULL || bgr==NULL)
 //       return 0;
 	if(!ydata || !uvdata) return -1;
	
    yp = ydata; // Y plane, Size=WxH
    cp = uvdata; // UV interleaved plane, Size=(W/2)x(H/2)
    for (j=0; j<h; j+=2) {
    	for (i=0; i<w; i+=2) {
			u = *cp++;
			v = *cp++;
			m = w*j+i; // (Y00, U00, V00) --> P00
			y = yp[m]; 
			bgr[m*3+0] = CLAMP(YUV_TO_B(y, u, v), 0, 255);
			bgr[m*3+1] = CLAMP(YUV_TO_G(y, u, v), 0, 255);
			bgr[m*3+2] = CLAMP(YUV_TO_R(y, u, v), 0, 255);
			m ++;      // (Y01, U00, V00) --> P01
			y = yp[m];
			bgr[m*3+0] = CLAMP(YUV_TO_B(y, u, v), 0, 255);
			bgr[m*3+1] = CLAMP(YUV_TO_G(y, u, v), 0, 255);
			bgr[m*3+2] = CLAMP(YUV_TO_R(y, u, v), 0, 255);
			m += w;    // (Y11, U00, V00) --> P11
			y = yp[m]; 
			bgr[m*3+0] = CLAMP(YUV_TO_B(y, u, v), 0, 255);
			bgr[m*3+1] = CLAMP(YUV_TO_G(y, u, v), 0, 255);
			bgr[m*3+2] = CLAMP(YUV_TO_R(y, u, v), 0, 255);
			m --;      // (Y10, U00, V00) --> P10
			y = yp[m]; 
			bgr[m*3+0] = CLAMP(YUV_TO_B(y, u, v), 0, 255);
			bgr[m*3+1] = CLAMP(YUV_TO_G(y, u, v), 0, 255);
			bgr[m*3+2] = CLAMP(YUV_TO_R(y, u, v), 0, 255);
    	}
    }
	
    return 0;
}