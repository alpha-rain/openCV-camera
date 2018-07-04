#include "stdafx.h"
#include "CameraSource.hpp"

camera::camera () 
{ 
	count = 0;
	type_source = NONE;
	videoState = false;
	imageState = false;
	Fps = 20;
	got = false;
/* throw if no camera conected Location = 0;*/
}
camera::~camera(){stop();}

void camera::get_frames(Mat *dst, VideoCapture *cap)
{
	for(;;)
	{ 
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		if(cap->isOpened())
		{
			Mat rdimg;
			if(type_source == USB)
			{
				{
					*cap>>rdimg;
					mtx_.lock();
					got = true;
					*dst = rdimg;
					mtx_.unlock();
				}
			}
			else if(type_source == IP)
			{//mpeg mode
				Cap.read(rdimg);
				mtx_.lock();
				if (!rdimg.empty()) {//don't give empty frames
				*dst = rdimg;
				got = true;
				}
				mtx_.unlock();
			}
			if(videoState == true)
			{
				videoOut.write(rdimg);
			}
			if(imageState == true)
			{
				imwrite(outImage + to_string(count) + ".jpg",rdimg);
				count++;
			}
		}
		else
		{
			assert(0);
			//*dst = imread ("test-1-opencv-with-webcam.png");
		}
		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		 auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
		 //cout <<"dur: "<<duration<<endl;
		 int dur = ((1000000/Fps) - duration);// + 3000;
		 //cout<<"dur: "<<dur<<","<<Fps<<","<<duration<<endl;
		 if(dur < 1000) dur = 1000;
		 
		 std::this_thread::sleep_for(std::chrono::microseconds(dur));
		//usleep(dur);
	}
}

#if 0
void camera::get_frames(Mat *dst, VideoCapture *cap)
{
	for(;;)
	{
		if(cap->isOpened())
		{
			mtx_.lock();
			*cap>>*dst;
			mtx_.unlock();
			
		}
		else
		{//will change
			assert(0);//*dst = imread ("test-1-opencv-with-webcam.png");
		}
		usleep(40000);
	}
}
#endif
void camera::open_usb(int location)
{ // throw if no camera conected
	assert(type_source == NONE);
    Mat tmp;
    Location = location;
    Cap.open (location);
    Cap >> tmp;
    if (tmp.cols == 0)
        state = false;
    else
        state = true;
}
void camera::open_usb()
{
	assert(type_source == NONE);
	type_source = USB;
	//type_source = USB;
    Location = 0;
    Mat tmp;
    for (int i = 0; i < 10; i++)
    {
        Cap.open (i);
        Cap >> tmp;
        if (tmp.cols > 0)
        {
			Img = tmp;//first image since if we call get to early we not have yet triggered get_frames
            Location = i;
            cout << "found camera at port: " << i << endl;
			Frame_graber.reset(new thread(&camera::get_frames, this ,&Img, &Cap));
            break;
        }
        else
        {
        	type_source = NONE;
            Cap.release ();
        }
    }
    if (tmp.cols == 0)
        state = false;
    else
        state = true;
    // assert(tmp.cols > 0);
}

Mat camera::open_ip(string location)
{
	cout<<type_source<<endl;
	assert(type_source == NONE);
	type_source = IP;
	Cap.open(location);
	if (!Cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the IP video cam" << endl;
		type_source = NONE;
        //return -1;
    }
	else
	{
		cout<<"IP address: "<<location<<" opened succesfully!"<<endl;
		Cap.read(Img);
		//Cap.open(location);
		Frame_graber.reset(new thread(&camera::get_frames, this ,&Img, &Cap));
	}
	return Img;
}

void camera::open_video(string location)
{
	assert(type_source == NONE);
	type_source = VIDEO;

	Cap.open(location);
	
	if (!Cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
		type_source = NONE;
        //return -1;
    }
	else
	{
		cout<<"Video: "<<location<<" opened succesfully!"<<endl;
		Cap.read(Img);
		Frame_graber.reset(new thread(&camera::get_frames, this ,&Img, &Cap));
	}
}

void camera::open_image(string location)
{
	assert(type_source == NONE);
	type_source = IMAGE;
}

/*bool camera::get_state ()
{
    // if(state == false)Cap.release();
    return state;
}*/

bool camera::get (Mat &dst)
{ // throw if no camera conected
	bool available = true;
	if((type_source == USB) || (type_source == IP))
	{
			mtx_.lock();
			if(got == true)
			{
				got = false;
				dst = Img.clone();
			}
			else available = false;
			mtx_.unlock();				
	}
	else if(type_source == VIDEO)
	{
		Cap.read(dst);
	}
		
   
    if (dst.cols == 0)
    {
		
		//assert(0);
		//>>>cout<<"fail to get image"<<endl;
        //state = false;
        //Cap.release ();
        //>>>dst = imread ("grayimg.jpg");
		//cout<<"nope
    }
	return available;
}

void camera::stop()
{
	type_source = NONE;
	mtx_.lock();
	Cap.release();
	Frame_graber->detach();
	mtx_.unlock();
}

void camera::camerav_start(string name)
{
	assert(type_source != NONE);

	mtx_.lock();
	frame_width  = Img.cols;
	frame_height = Img.rows;
	mtx_.unlock();
	
	VideoWriter video(name + ".avi",CV_FOURCC('P','I','M','1'), 20, Size(frame_width,frame_height), true); //CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);
	videoOut = video;
	videoState = true;
}

void camera::camerav_stop()
{
	videoState = false;
	//videoOut.close();
}

void camera::camerai_start(string name)
{
	assert(type_source != NONE);

	imageState = true;
	//mtx_.lock();
	//frame_width  = Img;
	//mtx_.unlock();
	
	//VideoWriter video(name + ".avi",CV_FOURCC('P','I','M','1'), 20, Size(frame_width,frame_height), true); //CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);
	//videoOut = video;
	//videoState = true;
}

void camera::camerai_stop()
{
	imageState = false;
	//videoState = false;
	//videoOut.close();
} 

void camera::settings(string jsonSettings)
{
	//cout<<"json wow"<<endl;
	Document jsonData;
	jsonData.Parse(jsonSettings.c_str());
	//cout<<jsonData.HasMember("FPS")<<endl;
	//cout<<"size: "<<jsonData.Size()<<endl;
	if(jsonData.HasMember("FPS"))
	{
		Fps = jsonData["FPS"].GetDouble();
		cout<<"has member: "<<jsonData["FPS"].GetDouble()<<endl;
	}
	
}

/*void camera::get_IPcamera(Mat &dst)
{
	Cap.read(dst);
}*/
