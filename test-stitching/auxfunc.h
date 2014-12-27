#ifndef __AUXFUNC_H__
#define __AUXFUNC_H__

#include "cv.h"


using namespace cv;

void init_warp(Mat W, float wz, float tx, float ty);
void draw_warped_rect(Mat pImage, Rect rect, Mat W);

// Interpolates pixel intensity with subpixel accuracy.
// Abount bilinear interpolation in Wikipedia:
// http://en.wikipedia.org/wiki/Bilinear_interpolation

//template <class T>
//float interpolate(Mat pImage, float x, float y)
//{
//	// Get the nearest integer pixel coords (xi;yi).
//	int xi = cvFloor(x);
//	int yi = cvFloor(y);
//
//	float k1 = x-xi; // Coefficients for interpolation formula.
//	float k2 = y-yi;
//
//	int f1 = xi<pImage.cols-1;  // Check that pixels to the right  
//	int f2 = yi<pImage.rows-1; // and to down direction exist.
//
//	T* row1 = &CV_IMAGE_ELEM(pImage, T, yi, xi);
//	T* row2 = &CV_IMAGE_ELEM(pImage, T, yi+1, xi);
//				
//	// Interpolate pixel intensity.
//	float interpolated_value = (1.0f-k1)*(1.0f-k2)*(float)row1[0] +
//				(f1 ? ( k1*(1.0f-k2)*(float)row1[1] ):0) +
//				(f2 ? ( (1.0f-k1)*k2*(float)row2[0] ):0) +						
//				((f1 && f2) ? ( k1*k2*(float)row2[1] ):0) ;
//
//	return interpolated_value;
//
//}

	
#endif //__AUXFUNC_H__