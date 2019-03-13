
/********************************************
*	@Author: Giuseppe Sensolini Arra'		*
*											*
*		COMPUTER VISION MODULE				*
*											*
*********************************************/

#include <stdlib.h>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "ball_tracker.h"
#include "ball_physic.h"

int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

const String windowName = "Original Image";
const String windowName1 = "HSV Image";
const String windowName2 = "Thresholded Image";
const String windowName3 = "After Morphological Operations";
const String trackbarWindowName = "Trackbars";
const String gainTrackbarWindowName = "PID GAINS";


/*************************************************************************************
 *--------  drawing functions -------------------------------------------------------*
 *************************************************************************************/

void on_trackbar( int, void* ){
	//This function gets called whenever a
	// trackbar position is changed
}

String intToString(int number){
	std::stringstream ss;
	ss << number;
	return ss.str();
}

//________________________________________________________________________________
void createTrackbars(){

	//create window for trackbars
    namedWindow(trackbarWindowName, 0);

	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN");
	sprintf( TrackbarName, "H_MAX");
	sprintf( TrackbarName, "S_MIN");
	sprintf( TrackbarName, "S_MAX");
	sprintf( TrackbarName, "V_MIN");
	sprintf( TrackbarName, "V_MAX");

	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH),
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->
    createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
    createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
    createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
    createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
    createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
    createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );
}

//________________________________________________________________________________
void createGainTrackbars(PID_t* XPID, PID_t* YPID){

    namedWindow(gainTrackbarWindowName, 0);

	char TrackbarName[50];
	sprintf( TrackbarName, "X_Kp");
	sprintf( TrackbarName, "X_Ki");
	sprintf( TrackbarName, "X_Kd");
	sprintf( TrackbarName, "Y_Kp");
	sprintf( TrackbarName, "Y_Ki");
	sprintf( TrackbarName, "Y_Kd");

	createTrackbar( "X_Kp", gainTrackbarWindowName, (int*)&(XPID->Kp), P_MAX, on_trackbar );
    createTrackbar( "X_Ki", gainTrackbarWindowName, (int*)&(XPID->Ki), I_MAX, on_trackbar );
    createTrackbar( "X_Kd", gainTrackbarWindowName, (int*)&(XPID->Kd), D_MAX, on_trackbar );
	createTrackbar( "Y_Kp", gainTrackbarWindowName, (int*)&(YPID->Kp), P_MAX, on_trackbar );
    createTrackbar( "Y_Ki", gainTrackbarWindowName, (int*)&(YPID->Ki), I_MAX, on_trackbar );
    createTrackbar( "Y_Kd", gainTrackbarWindowName, (int*)&(YPID->Kd), D_MAX, on_trackbar );
}

//_ Draw version 1 ____________________________
void drawObjectV1(int x, int y, Mat &frame){
	//Draw version 1

	circle(frame,Point(x,y),24,Scalar(0,255,0),3);
	//vertical line
    if(y-25>0)
    line(frame,Point(x,0),Point(x,y-25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
    if(y+25<FRAME_HEIGHT)
    line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);

	//orizzontal line
    if(x-25>0)
    line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
    if(x+25<FRAME_WIDTH)
    line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);

	putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);

}

//_ Draw version 2 ____________________________
void drawObjectV2(Ball* ball, Mat &frame, bool noise_error){

	if(noise_error){	//////////////////////////////////////
		rectangle(	frame,
					Point(SETPOINT_X-202, SETPOINT_Y-202),
					Point(SETPOINT_X+202, SETPOINT_Y+202),
					RED, 3, LINE_8, 0);
		putText(frame, "TOO MUCH NOISE! ADJUST FILTERS", Point(180,220), 1, 1, RED , 2);
		return;
	}

	if (ball->detected){	//////////////////////////////////////
			//draw square area
		rectangle(	frame,
					Point(SETPOINT_X-202, SETPOINT_Y-202),
					Point(SETPOINT_X+202, SETPOINT_Y+202),
					CYAN, 3, LINE_8, 0);
		putText(frame, "BALL FOUND", Point(123,32), 1, 1, GREEN, 2);

		//draw setpoint area
		circle(frame, Point(SETPOINT_X,SETPOINT_Y), 2, CYAN, 3);
		rectangle(	frame,
					Point(SETPOINT_X-TOLLERANCE, SETPOINT_Y-TOLLERANCE),
					Point(SETPOINT_X+TOLLERANCE, SETPOINT_Y+TOLLERANCE),
					ORANGE, 2, LINE_4, 0);


		//draw ball lines for position spot
		rectangle(	frame,
					Point(ball->x[0]-28, ball->y[0]-28),
					Point(ball->x[0]+28, ball->y[0]+28),
					GREEN, 1, LINE_8, 0);

		//draw velocity arrow
		if (ball->v > 2.0){
			arrowedLine(frame, Point(ball->x[0], ball->y[0]), Point(ball->fx, ball->fy) , RED, 2, 8, 0 , 0.4);
		}
		//draw previous positions
		for (int i=1 ; i<8 ; i++){
			circle( frame, Point(ball->x[i], ball->y[i]), 2, ORANGE, -1, 8, 0 );
		}
		//display ball info
		line(frame, Point(ball->x[0], 0), Point(ball->x[0], FRAME_HEIGHT), BLUE, 1);
		line(frame, Point(0, ball->y[0]), Point(FRAME_WIDTH, ball->y[0]), BLUE, 1);
		putText(frame,intToString(ball->y[0]),Point(ball->x[0]+2,ball->y[0]-42),1,1,BLUE,2);
		putText(frame,intToString(ball->x[0]),Point(ball->x[0]+40,ball->y[0]+14),1,1,BLUE,2);
		return;
	}

	//////////////////////////////////////
	//draw square area
	rectangle(	frame,
		Point(SETPOINT_X-202, SETPOINT_Y-202),
		Point(SETPOINT_X+202, SETPOINT_Y+202),
		ORANGE, 3, LINE_8, 0);

	putText(frame, "BALL NOT FOUND", Point(123,32), 1, 1, RED , 2);
	return;

}

void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.

	//the element chosen here is a 3px by 3px rectangle
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));

    //dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(5,5));

	//Mat

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);

	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);

	morphologyEx(thresh, thresh, MORPH_CLOSE, Mat::ones(5, 5, CV_8U));

	//release
	erodeElement.release();
	dilateElement.release();
}

void trackFilteredObject(Ball* ball, Mat threshold, Mat &cameraFeed){
	bool noise_error = false;
	Mat temp;
	threshold.copyTo(temp);
	//temp = threshold(buildBox(b));

	//these two vectors needed for output of findContours
	std::vector< std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );

	//use moments method to find our filtered object
	double refArea = 0;

	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					ball->detected = true;
					//circleDetector(cameraFeed, threshold);
					updateBall(ball, moment.m10/area, moment.m01/area);
					refArea = area;
				}
			}
		}
		else{
			noise_error = true;
			*ball = createBall(0, 0);
		}
	}
	else{
		//restore default ball parameters
		*ball = createBall(ball->x[0], ball->y[0]);
	}

	drawObjectV2( ball, cameraFeed, noise_error);

	//release memory
	temp.release();
}

void circleDetector(Mat cameraFeed, Mat threshold){

	Mat gray;
	threshold.copyTo(gray);
	//cvtColor(threshold, gray, CV_BGR2GRAY);
	GaussianBlur( gray, gray, Size(7, 7), 1.8, 1.8 );

	std::vector<Vec3f> circles;
	HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 35, 100, 25, 10, 45);

	for(size_t i = 0 ; i < circles.size() ; i++){
		printf("%d    radius: %.1f \n", (int)i+1, circles[i][2]);

		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		short radius = cvRound(circles[i][2]);

		circle( cameraFeed, center, 3, ORANGE, -1, 8, 0 );     // circle center
		circle( cameraFeed, center, radius+1, GREEN, 2, 8, 0 );  // circle outline
  	}
	gray.release();
}

//create square box and find for ball only here
void createSquareBox(Ball* b, Mat cameraFeed, Mat threshold){
	//delete from threshol all out of box point
	//box is centered in (fx, fy)

}

void callBackFunc(int event, int x, int y, int flags, void* param){
    if  ( event == EVENT_LBUTTONUP ){
        printf("\n# MOUSE CLICK EVENT\n	x: %d\n	y: %d\n", x, y);

		// Mount back the parameters
    	mouseParams* mp = (mouseParams*)param;
		Mat HSV;
		cvtColor(mp->_mat, HSV, COLOR_BGR2HSV);
		Vec3b hsv = HSV.at<Vec3b>(x,y);

		int h = ((int)(hsv.val[0]))*(250.0/360.0);
		//int s = (int)(hsv.val[1]);
		//int v = (int)(hsv.val[2]);

		int epsilon = 9;

		*mp->_H_MIN = (h-epsilon);
		if (*mp->_H_MIN < 0)*mp->_H_MIN = 0;

		*mp->_H_MAX = (h+epsilon);
		if (*mp->_H_MAX > 255)*mp->_H_MAX = 255;

		*mp->_S_MIN = (126);
		if (*mp->_S_MIN < 0)*mp->_S_MIN = 0;

		*mp->_S_MAX = (255);
		if (*mp->_S_MAX > 255)*mp->_S_MAX = 255;

		*mp->_V_MIN = (60);
		if (*mp->_V_MIN < 0)*mp->_V_MIN = 0;

		*mp->_V_MAX = (245);
		if (*mp->_V_MAX > 255)*mp->_V_MAX = 255;

		printHSV(*mp);
    }
}

void printHSV(mouseParams mp){
	printf("\n  =======================\n");
	printf(" ||	H MIN = %d	||\n", *mp._H_MIN);
	printf(" ||	  MAX = %d	||\n", *mp._H_MAX);
	printf(" ||	S MIN = %d	||\n", *mp._S_MIN);
	printf(" ||	  MAX = %d	||\n", *mp._S_MAX);
	printf(" ||	V MIN = %d	||\n", *mp._V_MIN);
	printf(" ||	  MAX = %d	||\n", *mp._V_MAX);
	printf("  =======================\n");
}
