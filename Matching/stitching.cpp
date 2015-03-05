#include "stitching.h"
using namespace cv;
using namespace std;

/**
 * \fn Mat get_Jacobian(int x, int y)
 * \brief function to get Jacobian matrix for 6 parameters
 * \param x,y - the coordinates of the position to get jacobian
 * \return cv::Mat - the Jacobian
 */
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

/**
 * \fn Mat get_dwarp_estimate(Vec6f dp)
 * \brief function to update 6 warp parameters 
 * \param dp - vector of 6 float corresponding to the necessary update of parameters
 * \return cv::Mat - the new warp matrix
 */
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

/**
 * \fn Mat align_image(Mat target_img, Mat template_img, Rect rect)
 * \brief function of Matthews-Baker inverse compositionnal
 * \param target_img - the warped image
 * \param template_img - the template image
 * \param rect - rectangle corresponding to the template
 * \return cv::Mat - the estimated warp matrix
 */
Mat align_image(Mat target_img, Mat template_img, Rect rect)
{
	const int MAX_ITER = 200;
	const float EPS = 1e-4f;
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
		//imshow("warped_img", warped_img);
		//waitKey(2);
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

		//cout <<"iteration : "<< iter << "\tmean error : " << err << endl;
		
		/// Compute of stop criterion
		float max_err = 0;
		for(int i=1;i<nbparam;i++)
			if(abs(dp[i]) > max_err)
				max_err = abs(dp[i]);
		if (max_err <= EPS)
            break;
	}
	//cout << endl;
	warp_estimate.at<float>(0,2) -= rect.x;
	warp_estimate.at<float>(1,2) -= rect.y;
	return warp_estimate;
}

/*
 * \fn Mat merge_image(Mat img1, Mat img2)
 * \brief function to merge two images
 * \param img1, img2 - the two images to merge
 * \return Mat - the result image of merging
 */
Mat merge_image(Mat img1, Mat img2)
{
	Mat result_img(img1.rows,img1.cols,CV_32FC1);
	double minVal1, minVal2, maxVal1, maxVal2;
	minMaxLoc(img1, &minVal1, &maxVal1);
	minMaxLoc(img2, &minVal2, &maxVal2);
	for (int i=0;i<img1.rows;i++)
	{
		for (int j=0;j<img1.cols;j++)
		{
			if (img1.at<float>(i,j) <= (10*maxVal1/255.0) || img2.at<float>(i,j) <= (10*maxVal2/255.0))
			{
				result_img.at<float>(i,j)=0;
			}
			else if(img1.at<float>(i,j) <= (10*maxVal1/255.0) || img2.at<float>(i,j) > (10*maxVal2/255.0))
			{
				result_img.at<float>(i,j) = img2.at<float>(i,j);
			}
			else if(img2.at<float>(i,j) <= (10*maxVal2/255.0) || img1.at<float>(i,j) > (10*maxVal1/255.0))
			{
				result_img.at<float>(i,j) = img1.at<float>(i,j);
			}
			else
			{
				result_img.at<float>(i,j) = (img1.at<float>(i,j) + img2.at<float>(i,j)) / 2.0;
			}
		}
	}
	return result_img;
}

/**
 * \fn Mat crop_image(Mat img)
 * \brief function to crop image to extract only the valid part (pixels with information)
 * \param img - the original image
 * \return Mat - the result image
 */
Mat crop_image(Mat img)
{
	int min_i=0, max_i=0, min_j=0, max_j=0;
	for(int i=0;i<img.rows;i++)
		for(int j=0;j<img.cols;j++)
			if(img.at<float>(i,j)!=0)
			{
				min_i = i;
				break;
			}
	for(int i=img.rows-1;i>-1;i--)
		for(int j=img.cols-1;j>-1;j--)
			if(img.at<float>(i,j)!=0)
			{
				max_i = i;
				break;
			}
	for(int j=0;j<img.cols;j++)
		for(int i=img.rows-1;i>-1;i--)
			if(img.at<float>(i,j)!=0)
			{
				min_j = j;
				break;
			}
	for(int j=img.cols-1;j>-1;j--)
		for(int i=0;i<img.rows;i++)
			if(img.at<float>(i,j)!=0)
			{
				max_j = j;
				break;
			}
	Rect r_crop(Point(min_j, min_i), Point(max_j, max_i));
	Mat result;
	result  = img(r_crop);
	return result;
}