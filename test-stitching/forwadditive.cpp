// Module: forwadditive.cpp
// Brief:  Contains implementation of forwards additive image alignment algorithm.
// Date:  December 2014

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <cv.h>			// Include header for computer-vision part of OpenCV.
#include <highgui.h>	// Include header for GUI part of OpenCV.
#include "auxfunc.h"    // Header for our warping functions.
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
// Lucas-Kanade method
// @param[in] pImgT   Template image T
// @param[in] omega   Rectangular template region
// @param[in] pImgI   Another image I

void align_image_forwards_additive(Mat pImgT, Rect omega, Mat pImgI)
{
	// Some constants for iterative minimization process.
	const float EPS = 1E-5f; // Threshold value for termination criteria.
	const int MAX_ITER = 200;  // Maximum iteration count.
	
	// Here we will store internally used images.
	Mat pGradIx;	   // Gradient of I in X direction.
	Mat pGradIy;	   // Gradient of I in Y direction.
	pGradIx = pGradIx.zeros(pImgI.rows, pImgI.cols, CV_32F);
	pGradIy = pGradIy.zeros(pImgI.rows, pImgI.cols, CV_32F);

	// Here we will store matrices.
	Mat W;  // Current value of warp W(x,p)
	Mat X;  // Point in coordinate frame of T.
	Mat Z;  // Point in coordinate frame of I.
	
	Mat H;  // Hessian
	Mat iH; // Inverse of Hessian
	Mat b;  // Vector in the right side of the system of linear equations.
	Mat delta_p; // Parameter update value.
	
	// Create matrices.
	W = W.zeros(3, 3, CV_32F);
	X = X.zeros(3, 1, CV_32F);
	Z = Z.zeros(3, 1, CV_32F);

	H = H.zeros(3, 3, CV_32F);
	iH = iH.zeros(3, 3, CV_32F);
	b = b.zeros(3, 1, CV_32F);
	delta_p = delta_p.zeros(3, 1, CV_32F);

	// Create gradient images.
	pGradIx = pGradIx.zeros(pImgI.rows, pImgI.cols, CV_32F);
	pGradIy = pGradIy.zeros(pImgI.rows, pImgI.cols, CV_32F);

	// Calculate gradient of I.
	Sobel(pImgI, pGradIx, CV_32F, 1, 0, 3, 0.125);
	Sobel(pImgI, pGradIy, CV_32F, 0, 1, 3, 0.125);

	// Here we will store parameter approximation. 
	float wz_a=0, tx_a=0, ty_a=0;

	// Here we will store current mean error value.
	float mean_error = 0;

	// Iterate
	int iter = 0, pixel_count, i, j; // number of current iteration
	while(iter < MAX_ITER)
	{
		iter++; // Increment iteration counter
		mean_error = 0; // Set mean error value with zero
		pixel_count = 0; // Count of processed pixels
		
		init_warp(W, wz_a, tx_a, ty_a); // Init warp W(x, p)
		H = H.zeros(3, 3, CV_32F);
		b = b.zeros(3, 1, CV_32F);

		
		// (u,v) - pixel coordinates in the coordinate frame of T.
		int u, v;
		// Walk through pixels in the template T.
		for(i=0;i<omega.height;i++)
		{
			u = i + omega.x;
			for(j=0;j<omega.width;j++)
			{
				v = j + omega.y;
				// Set vector X with pixel coordinates (u,v,1)
				X.at<float>(0,0) = u;
				X.at<float>(1,0) = v;
				X.at<float>(2,0) = 1.0;

				// Warp Z=W*X
				gemm(W, X, 1, 0, 0, Z);

				// (u2,v2) - pixel coordinates in the coordinate frame of I.
				float u2, v2;
				
				// Get coordinates of warped pixel in coordinate frame of I.
				u2 = Z.at<float>(0, 0);
				v2 = Z.at<float>(1, 0);

				// Get the nearest integer pixel coords (u2i;v2i).
				int u2i = cvFloor(u2);
				int v2i = cvFloor(v2);

				if(u2i>=0 && u2i<pImgI.cols && // check if pixel is inside I.
					v2i>=0 && v2i<pImgI.rows)
				{
					pixel_count++;

					// Evaluate gradient of I at W(x,p) with subpixel accuracy
					float Ix = pGradIx.at<float>(u2, v2);
					float Iy = pGradIy.at<float>(u2, v2);

					// Calculate steepest descent image's element.
					float stdesc[3]; // an element of steepest descent image
					stdesc[0] = (float)(-v * Ix + u * Iy);
					stdesc[1] = (float)Ix;
					stdesc[2] = (float)Iy;

					// Calculate intensity of a transformed pixel with sub-pixel accuracy
					float I2 = pImgI.at<uchar>(u2, v2);
					
					
					// Calculate image difference D = T(x)-I(W(x,p)).
					//float tmp = pImgT.at<uchar>(u-omega.x , v-omega.y);
					float D = pImgT.at<uchar>(u-omega.x , v-omega.y) - I2;
					
					// Update mean error value.
					mean_error += fabs(D);

					// Add a term to b matrix.					
					b.at<float>(0, 0) += stdesc[0] * D;
					b.at<float>(1, 0) += stdesc[1] * D;
					b.at<float>(2, 0) += stdesc[2] * D;
					
					// Add a term to Hessian.
					int l,m;
					for(l=0;l<3;l++)
					{
						for(m=0;m<3;m++)
						{
							H.at<float>(l, m) += stdesc[l] * stdesc[m];
						}
					}
				}	
			}
		}

		// Finally, calculate mean error.
		if(pixel_count!=0)
			mean_error /= pixel_count;

		// Invert Hessian.
		iH = H.inv();

		// Find parameter increment. 
		gemm(iH, b, 1, 0, 0, delta_p);
		float delta_wz = delta_p.at<float>(0, 0);
		float delta_tx = delta_p.at<float>(1, 0);
		float delta_ty = delta_p.at<float>(2, 0);

		// Update parameter vector approximation.
		wz_a += delta_wz;
		tx_a += delta_tx;
		ty_a += delta_ty;

		// Print diagnostic information to screen.
		printf("iter=%d dwz=%f dtx=%f dty=%f mean_error=%f\n", 
			iter, delta_wz, delta_tx, delta_ty, mean_error);

		// Check termination critera.
		if(fabs(delta_wz)<EPS && fabs(delta_tx)<EPS && fabs(delta_ty)<EPS) break;
	}


	// Print summary.
	printf("===============================================\n");
	printf("Algorithm: forward additive.\n");
	printf("Iteration count: %d\n", iter);
	printf("Approximation: wz_a=%f tx_a=%f ty_a=%f\n", wz_a, tx_a, ty_a);
	printf("Epsilon: %f\n", EPS);
	printf("Resulting mean error: %f\n", mean_error);
	printf("===============================================\n");

	// Show result of image alignment.
	init_warp(W, wz_a, tx_a, ty_a);
	draw_warped_rect(pImgI, omega, W);
	pImgT = pImgT(omega);
	imshow("template", pImgT);
	imshow("image", pImgI);
	//cvResetImageROI(pImgT);

	// Free used resources and exit.
	W.release();
	X.release();
	Z.release();
	H.release();
	iH.release();
	b.release();
	delta_p.release();
}