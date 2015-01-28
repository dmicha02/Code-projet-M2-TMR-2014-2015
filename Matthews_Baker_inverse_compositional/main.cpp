//**
// * \file      main.cpp
// * \version   0.1
// * \details    Programme de test de l'alignement d'images à partir de l'algorithme compositionnel inverse de Matthews Baker
// */
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;

static void help()
{
    cout << "\nRebuildind endomicroscopic images.\n"\
		"M2 TMR Project 2014-2015\n"\
		"Matthews-Baker inverse compositionnel algorithm\n"\
		"**********************************\n\n"<< endl;
}

Mat get_Jacobian(int, int);
Mat get_dwarp_estimate(Vec6f);
Mat align_image(Mat, Mat, Rect);

int main()
{
	help();
	/// Declaration
	Mat img, W_estimate, warped_img, template_img, target_img, images;
	/// Init originale image, template and warped image
	img = imread("img1.jpg"); //load original image
	img.convertTo(img, CV_32F); //convert in float32
	cvtColor(img, img, CV_RGB2GRAY); //convert in grayscale image
	img = img / 255.0;
	Rect rec_tmp(Point(img.rows/6,img.cols/6), Point(5*img.rows/6,5*img.cols/6)); //template rectangle
	template_img = img(Range(img.rows/6,5*img.rows/6),Range(img.cols/6,5*img.cols/6)); //template_img
	target_img = imread("img2.jpg");
	target_img.convertTo(target_img, CV_32F); //convert in float32
	cvtColor(target_img, target_img, CV_RGB2GRAY); //convert in grayscale image
	target_img = target_img / 255.0;
	/// Display originale image, warped image and template
	hconcat(img, target_img, images);
	rectangle(images, rec_tmp, 1, 1, 8);
	imshow("Original image (on the left) and Warped image (on the right)",images);
	/// Matthews-Baker algorithm
	W_estimate = align_image(target_img, template_img, rec_tmp);
	/// Affichage reel warp matrix and estimate warp matrix
	cout << "Estimate matrix of warp by Matthews-Baker algorithm : \n" << W_estimate << endl;
	waitKey(0);
	return 0;
}

Mat get_Jacobian(int x, int y)
{
	Mat J;
	J = J.zeros(2,6,CV_32F);
	J.at<float>(0, 0) = x;
	J.at<float>(0, 1) = y;
	J.at<float>(0, 2) = 1;
	J.at<float>(1, 3) = x;
	J.at<float>(1, 4) = y;
	J.at<float>(1, 5) = 1;
	return J;
}

Mat get_dwarp_estimate(Vec6f dp)
{
	Mat dw;
	dw = Mat::eye(3,3,CV_32F);
	dw.at<float>(0,0) += dp[0];
	dw.at<float>(0,1) += dp[1];
	dw.at<float>(0,2) += dp[2];
	dw.at<float>(1,0) += dp[3];
	dw.at<float>(1,1) += dp[4];
	dw.at<float>(1,2) += dp[5];
	return dw;
}

Mat align_image(Mat target_img, Mat template_img, Rect rect)
{
	const int MAX_ITER = 200;
	const float EPS = 1e-3f;
	/// Initialisation
	int nbparam = 6;
	Mat warp_estimate, dwarp_estimate, idwarp_estimate;
	Mat Hessian, iHessian;
	Mat gradientX, gradientY;
	vector<Mat> steepest_descent_image;
	warp_estimate = warp_estimate.eye(3, 3, CV_32F);
	warp_estimate.at<float>(0,2) += rect.x;
	warp_estimate.at<float>(1,2) += rect.y;
	dwarp_estimate = dwarp_estimate.zeros(3, 3, CV_32F);
	idwarp_estimate = idwarp_estimate.zeros(3, 3, CV_32F);
	Hessian = Hessian.zeros(nbparam, nbparam, CV_32F);
	iHessian = iHessian.zeros(nbparam, nbparam, CV_32F);
	for(int i=0;i<nbparam;i++)
		steepest_descent_image.push_back(Mat(template_img.rows, template_img.cols, CV_32F));
	/// Calcul of gradient with Sobel filters
	Sobel(template_img, gradientX, CV_32F, 1, 0, 3, 0.125, 0, BORDER_DEFAULT);
    Sobel(template_img, gradientY, CV_32F, 0, 1, 3, 0.125, 0, BORDER_DEFAULT);
	/// Calcul of Hessian matrix
	Vec2f dT;
	Vec6f v_steepest;
	Mat Jacobian(2, nbparam, CV_32F);
	for(int y=0;y<template_img.rows;y++)
	{
        for(int x=0;x<template_img.cols;x++)
		{
			dT(0) = gradientX.at<float>(y,x);
			dT(1) = gradientY.at<float>(y,x);
			/// Compute Jacobian
            Jacobian = get_Jacobian(x,y);
			/// Compute the steepest descent images
			steepest_descent_image[0].at<float>(y, x) = dT(0) * Jacobian.at<float>(0,0) + dT(1) * Jacobian.at<float>(1,0);
			steepest_descent_image[1].at<float>(y, x) = dT(0) * Jacobian.at<float>(0,1) + dT(1) * Jacobian.at<float>(1,1);
			steepest_descent_image[2].at<float>(y, x) = dT(0) * Jacobian.at<float>(0,2) + dT(1) * Jacobian.at<float>(1,2);
			steepest_descent_image[3].at<float>(y, x) = dT(0) * Jacobian.at<float>(0,3) + dT(1) * Jacobian.at<float>(1,3);
			steepest_descent_image[4].at<float>(y, x) = dT(0) * Jacobian.at<float>(0,4) + dT(1) * Jacobian.at<float>(1,4);
			steepest_descent_image[5].at<float>(y, x) = dT(0) * Jacobian.at<float>(0,5) + dT(1) * Jacobian.at<float>(1,5);
			v_steepest[0] = steepest_descent_image[0].at<float>(y, x);
			v_steepest[1] = steepest_descent_image[1].at<float>(y, x);
			v_steepest[2] = steepest_descent_image[2].at<float>(y, x);
			v_steepest[3] = steepest_descent_image[3].at<float>(y, x);
			v_steepest[4] = steepest_descent_image[4].at<float>(y, x);
			v_steepest[5] = steepest_descent_image[5].at<float>(y, x);
			/// Compute Hessian
			for(int i=0;i<nbparam;i++)
				for(int j=0;j<nbparam;j++)
					Hessian.at<float>(i,j) += v_steepest[i] * v_steepest[j];
		}
	}
	/// Compute Hessian inverse
	iHessian = Hessian.inv();

	float err = 0, D = 0;
    int iter = 0;
	Mat warped_img(template_img.rows, target_img.cols, CV_32F);
	Vec6f dp, b;
	/// Iterate
	while(iter < MAX_ITER)
	{
		iter+=1; //iteration
		err=0; //error
		b = 0;
		dp = 0;
		/// Warp target image
		warpPerspective(target_img, warped_img, warp_estimate.inv(), template_img.size());
		imshow("warped_img", warped_img);
		waitKey(2);
		/// Compute the product of error image and steepest descent images
		for(int y=0;y<template_img.rows;y++)
		{
            for(int x=0;x<template_img.cols;x++)
			{
                D = warped_img.at<float>(y,x) - template_img.at<float>(y,x);
                err += abs(D);
                b[0] += D * steepest_descent_image[0].at<float>(y,x);
				b[1] += D * steepest_descent_image[1].at<float>(y,x);
				b[2] += D * steepest_descent_image[2].at<float>(y,x);
				b[3] += D * steepest_descent_image[3].at<float>(y,x);
				b[4] += D * steepest_descent_image[4].at<float>(y,x);
				b[5] += D * steepest_descent_image[5].at<float>(y,x);
			}
		}
        err /= (template_img.cols * template_img.rows);  // mean error
		/// Compute of parameters modification (delta p)
		for(int i=0;i<nbparam;i++)
			for(int j=0;j<nbparam;j++)
				dp[i] += b[j] * iHessian.at<float>(i,j);
		/// Update warp matrix
        dwarp_estimate = get_dwarp_estimate(dp);
        idwarp_estimate = dwarp_estimate.inv();
        gemm(warp_estimate,idwarp_estimate,1,NULL,0,warp_estimate);

		cout <<"iteration : "<< iter << "\tmean error : " << err << endl;
		
		/// Compute of stop criterion
		float max_err = 0;
		for(int i=1;i<nbparam;i++)
			if(abs(dp[i]) > max_err)
				max_err = abs(dp[i]);
		if (max_err <= EPS)
            break;
	}
	cout << endl;
	return warp_estimate;
}
