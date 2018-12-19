#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char const *argv[])
{
    int i = 0;
    printf("hello vs studio \n");
    char filename[48];
    Mat frame; // opecv_core
    Mat dst_frame;
		 //读取视频
	VideoCapture video("112432.avi"); // opencv_videoio
	 
	 //判断视频是否打开
	if( !video.isOpened())
		 return 0;
    int frames = video.get(CV_CAP_PROP_FRAME_COUNT);
    int width  = video.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = video.get(CV_CAP_PROP_FRAME_HEIGHT);
    int isRGB  = video.get(CAP_PROP_CONVERT_RGB);

    cout << "total frames : " << frames << endl;
    cout << "width : " << width << "height : " << height << "isRGB : " << isRGB << endl;
    long frameStart = 377;

    video.set(CV_CAP_PROP_POS_FRAMES,frameStart);
    i = frameStart;
    while(1)
    {
        video >> frame;

        if(frame.empty())
        {
            break;
        }

        sprintf(filename,"jpeg/test/%dx%d_%04d.jpg",frame.cols,frame.rows,i);
        imwrite(filename,frame);

        resize(frame,dst_frame,Size(2048,1536),1,0.5,INTER_LINEAR);

        sprintf(filename,"jpeg/scale/%dx%d_%04d.jpg",dst_frame.cols,dst_frame.rows,i);
        imwrite(filename,dst_frame);
        i ++;

        if(i == 398)
            break;
    }
    return 0;
}
