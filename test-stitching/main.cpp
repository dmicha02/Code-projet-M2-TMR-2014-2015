// Module: main.cpp
// Brief: Test of stiching images 
// Date:  December 2014

#include <stdio.h>
#include <cv.h>
#include "auxfunc.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include "forwadditive.h"
//#include "invcomp.h"

// Our plan:
// 
// 1. Ask user to enter image warp parameter vector p=(wz, tx, ty, s)
// 2. Define our template rectangle to be bounding rectangle of the butterfly
// 3. Warp image I with warp matrix W(p)
// 4. Show template T and image T, wait for a key press
// 5. Estimate warp parameters using Lucas-Kanade method, wait for a key press
// 6. Estimate warp parameters using Baker-Dellaert-Matthews, wait for a key press
//
using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	// Ask user to enter image warp paremeters vector.
	// p = (wz, tx, ty),
	
	float WZ=0, TX=0, TY=0;
	cout<< "Please enter WZ, TX and TY, separated by space.\n"\
			"Example: -0.01 5 -3\n"	<<endl; 
	scanf("%f %f %f", &WZ, &TX, &TY);

	// Here we will store our images.
	Mat pColorPhoto; // Photo of a butterfly on a flower.
	Mat pGrayPhoto;  // Grayscale copy of the photo.
	Mat pImgT;	   // Template T.
	Mat pImgI;	   // Image I.

	// Here we will store our warp matrix.
	Mat W(3, 3, CV_32F);  // Warp W(p)
	
	// Create two simple windows. 
	namedWindow("template"); // Here we will display T(x).
	namedWindow("image"); // Here we will display I(x).

	// Load photo.
	pColorPhoto = imread("..\\data\\photo.jpg");

	// Convert photo to grayscale, because we need intensity values instead of RGB.	
	cvtColor(pColorPhoto, pGrayPhoto, CV_RGB2GRAY);

	// Create template T
	// Set region of interest to butterfly's bounding rectangle.
	pImgT = pGrayPhoto.clone();
	Rect omega = Rect(110, 100, 200, 150);
	
	// Create I by warping photo with sub-pixel accuracy. 
	init_warp(W, WZ, TX, TY);
	
	warpAffine(pGrayPhoto, pImgI, W(Range(0,2),Range(0,3)), pGrayPhoto.size());
	
	// Draw the initial template position approximation rectangle.
	W = W.eye(3,3,CV_32F);
	//draw_warped_rect(pImgI, omega, W);

	// Show T and I and wait for key press.
	pImgT = pImgT(omega);
	imshow("template", pImgT);
    draw_warped_rect(pImgI, omega, W);
	imshow("image", pImgI);
	printf("Press any key to start forward additive algorithm.\n");
	waitKey(0);

	// Print what parameters were entered by user.
	printf("==========================================================\n");
	printf("Ground truth:  WZ=%f TX=%f TY=%f\n", WZ, TX, TY);
	printf("==========================================================\n");

	init_warp(W, WZ, TX, TY);

	// Restore image I
	warpAffine(pGrayPhoto, pImgI, W(Range(0,2),Range(0,3)), pGrayPhoto.size());
	// Lucas-Kanade 
	align_image_forwards_additive(pImgT, omega, pImgI);

	// Restore image I
	//warpAffine(pGrayPhoto, pImgI, W(Range(0,2),Range(0,3)), pGrayPhoto.size());

	//printf("Press any key to start Baker-Dellaert-Matthews algorithm.\n");
	//waitKey();

	// Baker-Dellaert-Matthews
	//align_image_inverse_compositional(pImgT, omega, pImgI);

	printf("Press any key to exit the demo.\n");
	waitKey();

	// Release all used resources and exit
	destroyWindow("template");
	destroyWindow("image");
	pImgT.release();
	pImgI.release();
	W.release();
	
	return 0;
}


