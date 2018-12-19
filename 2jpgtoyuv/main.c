#include <stdio.h>
#include <time.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include "libyuv.h"


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

// To do read image,transfer to yuv data.
int main()
{
	
	
	
}
