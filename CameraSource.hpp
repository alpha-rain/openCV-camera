#ifndef _CAMERASOURCE_H_
#define _CAMERASOURCE_H_
 
#include <iostream>
#include <sstream>
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/opencv.hpp"

#include <unistd.h>
#include <boost/timer/timer.hpp>
#include <thread>
#include <mutex>
#include <assert.h>

using namespace std;
using namespace cv;

#define NONE 	0
#define USB 	1
#define IP		2
#define VIDEO	3
#define IMAGE	4


class camera
{
    private:
	int type_source;
    VideoCapture Cap;
    int Location;
    Mat Img;
    bool state;
	bool videoState;
	bool imageState;
	string outImage;
	unsigned long int count;

	int frame_width;
	int frame_height;

	string outVideo;
	VideoWriter videoOut;
	
	std::mutex mtx_;	
	shared_ptr<thread> Frame_graber;
	
	void get_frames(Mat* dst, VideoCapture* cap);
	
    public:
    enum
    {
        CM_NONE,
        CM_SINGLE
    };

     camera();
	~camera();

	void open_usb();
	void open_usb(int location);
	void open_ip(string location);//rename open stream
	void open_video(string location);
	void open_image(string location);
    void get (Mat &dst);

	void stop();

	void camerai_start(string name);
	void camerai_stop();

	void camerav_start(string name);
	void camerav_stop();
};

#endif