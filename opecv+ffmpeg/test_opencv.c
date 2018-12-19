#include <iostream>
#include <opencv2/opencv.hpp>
#include <GL/glut.h>

using namespace cv;
using namespace std;
Point p1,p2;
//typedef void (*MouseCallback)(int event, int x, int y, int flags, void* userdata);

void onMouse(int event, int x, int y, int flags, void* userdata)
{
	Mat frame = *(Mat *)userdata;
	Mat img2;
	
	if(event == EVENT_LBUTTONDOWN)
	{
		cout << "Left Button down ,x" << x <<" y"<<y <<" flags"<< flags <<endl;
		//selectROI(frame);
		p1.x = x;
		p1.y = y;
	}
	else if(event == EVENT_RBUTTONDOWN)
	{
		cout << "Right Button down ,x" << x <<" y"<<y <<" flags"<< flags <<endl;
	}
	else if(event == EVENT_LBUTTONUP)
	{
		cout << "Left Button up ,x" << x <<" y"<<y <<" flags"<< flags <<endl;
		p2.x = x;
		p2.y = y;
		
		//rectangle(frame,p1,p2,Scalar(255,0,255),2,8,0);
		//imshow("Video",frame);
	}
	else if(event == EVENT_RBUTTONUP)
	{
		cout << "Left Button up ,x" << x <<" y"<<y <<" flags"<< flags <<endl;
	}
	
	if(event == EVENT_MOUSEMOVE)
	{
		cout << "Mouse move ,x" << x <<" y"<<y <<" flags"<< flags <<endl;
		//if(flags == 1)
		{
			p2.x = x;
			p2.y = y;
			frame.copyTo(img2);
			
			rectangle(img2,p1,p2,Scalar(255,0,255),2,8,0); // opencv_imgproc
			imshow("Video",img2);
		}
	}
	
}


 void on_opengl(void* param)
{
        glLoadIdentity();

        glTranslated(0.0, 0.0, -1.0);

        glRotatef( 55, 1, 0, 0 );
        glRotatef( 45, 0, 1, 0 );
        glRotatef( 0, 0, 0, 1 );

        static const int coords[6][4][3] = {
            { { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
            { { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
            { { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
            { { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
            { { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
            { { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
        };

        for (int i = 0; i < 6; ++i) {
                    glColor3ub( i*20, 100+i*10, i*42 );
                    glBegin(GL_QUADS);
                    for (int j = 0; j < 4; ++j) {
                            glVertex3d(0.2 * coords[i][j][0], 0.2 * coords[i][j][1], 0.2 * coords[i][j][2]);
                    }
                    glEnd();
        }
}
//extern "C"{
int main(int argc,char * argv[])
{
	 Mat frame; // opecv_core
		 //读取视频
	 VideoCapture video(argv[1]); // opencv_videoio
	 
	 //判断视频是否打开
	 if( !video.isOpened())
		 return 0;
	namedWindow("Video",WINDOW_OPENGL | WINDOW_AUTOSIZE | WINDOW_GUI_EXPANDED);
	setMouseCallback("Video",onMouse,&frame); // opencv_highgui
	setOpenGlContext("Video");
	setOpenGlDrawCallback("Video",on_opengl,NULL);
	updateWindow("Video");
	waitKey(0);
	while(1)
	{
		video >> frame;
	
		imshow("Video",frame);
		
		//waitKey(0);
		waitKey(0);
	}
	
	return 0;
}

//};