#include "auxfunc.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

// Our warp matrix looks like this one:
//
//  ! 1  -wz  tx !
//  ! wz  1   ty !
//  ! 0   0   1  !
//
using namespace cv;
void init_warp(Mat W, float wz, float tx, float ty)
{
	W.at<float>(0, 0) = 1;
	W.at<float>(1, 0) = wz;
	W.at<float>(2, 0) = 0;

	W.at<float>(0, 1) = -wz;
	W.at<float>(1, 1) = 1;
	W.at<float>(2, 1) = 0;

	W.at<float>(0, 2) = tx;
	W.at<float>(1, 2) = ty;
	W.at<float>(2, 2) = 1;
}

void draw_warped_rect(Mat pImage, Rect rect, Mat W)
{
	Point lt, lb, rt, rb;
	
	Mat X(3, 1, CV_32F);
	Mat Z(3, 1, CV_32F);

	// left-top point
	X.at<float>(0,0)= rect.x;
	X.at<float>(1,0)= rect.y; 
	X.at<float>(2,0)= 1.0;
	gemm(W, X, 1, 0, 0, Z);
	lt.x = (int)Z.at<float>(0,0);
	lt.y = (int)Z.at<float>(1,0);

	// left-bottom point
	X.at<float>(0,0)= rect.x;
	X.at<float>(1,0)= rect.y + rect.height; 
	X.at<float>(2,0)= 1.0;
	gemm(W, X, 1, 0, 0, Z);
	lb.x = (int)Z.at<float>(0,0);
	lb.y = (int)Z.at<float>(1,0);

	// right-top point
	X.at<float>(0,0)= rect.x + rect.width;
	X.at<float>(1,0)= rect.y; 
	X.at<float>(2,0)= 1.0;
	gemm(W, X, 1, 0, 0, Z);
	rt.x = (int)Z.at<float>(0,0);
	rt.y = (int)Z.at<float>(1,0);

	// right-bottom point
	X.at<float>(0,0)= rect.x + rect.width;
	X.at<float>(1,0)= rect.y + rect.height; 
	X.at<float>(2,0)= 1.0;
	gemm(W, X, 1, 0, 0, Z);
	rb.x = (int)Z.at<float>(0,0);
	rb.y = (int)Z.at<float>(1,0);

	// draw rectangle
	line(pImage, lt, rt, 255, 1, 8);
	line(pImage, rt, rb, 255, 1, 8);
	line(pImage, rb, lb, 255, 1, 8);
	line(pImage, lb, lt, 255, 1, 8);

	// release resources and exit
	X.release();
	Z.release();
}

