#include "CameraSource.hpp"

camera::camera () { count = 0;type_source = NONE;videoState = false;imageState = false;/* throw if no camera conected Location = 0;*/}
camera::~camera(){stop();}

void camera::get_frames(Mat *dst, VideoCapture *cap)
{
	for(;;)
	{
		
		if(cap->isOpened())
		{
			Mat rdimg;
			if(type_source == USB)
			{
				*cap>>rdimg;
				mtx_.lock();*dst = rdimg;mtx_.unlock();
			}
			else if(type_source == IP)
			{
				Cap.read(rdimg);
				mtx_.lock();*dst = rdimg;mtx_.unlock();
			}
			if(videoState == true)videoOut.write(rdimg);
			if(imageState == true)
			{
				imwrite(outImage + to_string(count) + ".jpg",rdimg);
				count++;
			}
		}
		else
		{
			//*dst = imread ("test-1-opencv-with-webcam.png");
		}
		usleep(40000);
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

void camera::open_ip(string location)
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
		Frame_graber.reset(new thread(&camera::get_frames, this ,&Img, &Cap));
		//Frame_graber = new std::thread(&camera::get_frames, this ,&Img, &Cap);
	}
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

void camera::get (Mat &dst)
{ // throw if no camera conected

	if((type_source == USB) || (type_source == IP))
	{
		mtx_.lock();
		dst = Img.clone();
		mtx_.unlock();
	}
	else if(type_source == VIDEO)
	{
		Cap.read(dst);
	}
		
   
    if (dst.cols == 0)
    {
		cout<<"fail to get image"<<endl;
        state = false;
        Cap.release ();
        dst = imread ("test-1-opencv-with-webcam.png");
    }
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



/*void camera::get_IPcamera(Mat &dst)
{
	Cap.read(dst);
}*/
