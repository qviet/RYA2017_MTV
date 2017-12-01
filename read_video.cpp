#include <cv.h>
#include <highgui.h>
#include "opencv2/opencv.hpp"
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <string>
#include <vector>
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;
const string nameWindow1 = "Original Image";
const string nameWindow2 = "Trackars";
const string nameWindow3 = "Blurred Image";
const string nameWindow4 = "HSV Image";
const string nameWindow5 = "InRange Image";
const string nameWindow6 = "Result Image";

int iLastX = -1;
int iLastY = -1;
int s16_blur_val = 0;
int s16_blur_max = 255;

int s16_H_val = 14;
int s16_H_max = 173;
int s16_S_val = 54;
int s16_S_max = 255;
int s16_V_val = 138;
int s16_V_max = 255;

int s16_ths_val = 169;
int s16_ths_max = 255;

Scalar red_min  = Scalar(0,36,0);
Scalar red_max = Scalar(25, 255, 255);
void Init() {
    namedWindow(nameWindow2, 0);
    createTrackbar("Blur size", nameWindow2, &s16_blur_val, s16_blur_max);
    // H
    createTrackbar("H MIN val",nameWindow2,&s16_H_val, s16_H_max);
    createTrackbar("H MAX val",nameWindow2,&s16_H_max, s16_H_max);
    // S
    createTrackbar("S MIN val",nameWindow2,&s16_S_val, s16_S_max);
    createTrackbar("S MAX val",nameWindow2,&s16_S_max, s16_S_max);
    // V
    createTrackbar("V MIN val",nameWindow2,&s16_V_val, s16_V_max);
    createTrackbar("V MAX val",nameWindow2,&s16_V_max, s16_V_max);
}
int main(int, char**)
{
    Mat img_raw, blurred_frame, threshold, img_result, img_Grey;
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //Create a black image with the size as the camera output
    Mat imgTmp, edges;
    Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;
    bool playVideo = true;
    Init();
    VideoCapture cap("video.wmv"); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    Mat frame;
    while(1)
    {
        if(playVideo)
            cap >> frame;
        if(frame.empty())
        {
            cout<<"Empty Frame\n";
            return 0;
        }

        Mat img_HSV;
        // filter
        blur(frame, blurred_frame, Size(s16_blur_val*2+1,s16_blur_val*2+1),Point(-1,-1),BORDER_DEFAULT);
        //convert from BGR to HSV
		cvtColor(blurred_frame, img_HSV, COLOR_BGR2HSV);
		//imshow(nameWindow2,img_HSV);
		// circle color
		inRange(img_HSV,Scalar(s16_H_val,s16_S_val,s16_V_val), Scalar(s16_H_max,s16_S_max,s16_V_max), threshold);
		//morphological opening (remove small objects from the foreground)
		erode(threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
		dilate( threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

		//morphological closing (fill small holes in the foreground)
		dilate( threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
		erode(threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        	imshow(nameWindow4, threshold);

        findContours(threshold, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		//Calculate the moments of the thresholded image
		Moments oMoments = moments(threshold);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;
		 // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero
		if (dArea > 10000)
		  {
		   //calculate the position of the ball
		   int posX = dM10 / dArea;
		   int posY = dM01 / dArea;

		   if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
		   {
			//Draw a red line from the previous point to the current point
			line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,255), 2);
		   }

		   iLastX = posX;
		   iLastY = posY;
		   circle(frame, Point(iLastX-18,iLastY),10, Scalar(0,255,255),CV_FILLED, 8,0);

		   printf("gia_tri_x: %d, gia_tri_y: %d \r\n",iLastX-18,iLastY);
		   printf("error_x: %d, error_y: %d \r\n",iLastX-18-320,iLastY-240);
		  }
		imshow(nameWindow1,frame);
		char key = waitKey(30);
		if(key == 'p')
			playVideo = !playVideo;


    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

