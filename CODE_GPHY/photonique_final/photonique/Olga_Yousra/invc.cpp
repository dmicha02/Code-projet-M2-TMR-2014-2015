#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>//Sobel 


#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>


#include <stdlib.h>
#include "stdio.h"
#include <xxx.h>
#include <time.h>



using namespace cv;
using namespace std;

void align_image_inverse_compositional(cv::Mat pImgT, cv::Rect omega, cv::Mat pImgI)
{
	//int wz=0;
	//int tx=0;
	//int ty=0;
	//Mat E = Mat::eye(3, 3, CV_64F);
 //   cout << "E = " << endl << " " << E << endl << endl;
	// 
	//
	////cout << "wz = " << endl << " " << wz << endl << endl;
 //  //cv::Mat a = cv::MatE(cv::1,cv::1);
 //  
 //  //printf ("col: %d \n", E);
 //  Mat C = (Mat_<double>(3,3) << 1, -wz, tx, -wz, 1, ty, 0, 0, 1);
 //  cout << "C = " << endl << " " << C << endl << endl;
	

	// Some constants for iterative minimization process.
	const float EPS = 1E-5f; // Threshold value for termination criteria.
	const int MAX_ITER = 100;  // Maximum iteration count.

	Mat W = Mat::zeros(3,3, CV_32F);
	//Mat *pW;
	//pW=&W;
	cout << "W = " << endl << " " << W << endl << endl;
	//CV_MAT_ELEM(*pW, float, 0, 0) = 1;
	W.at<float>(0,0) =1;
	cout << "W 2= " << endl << " " << W << endl << endl;

	Mat dW = Mat::zeros(3,3, CV_32F);
cout << "dW = " << endl << " " << dW << endl << endl;
 
Mat idW = Mat::zeros(3,3, CV_32F);
cout << "idW = " << endl << " " << idW << endl << endl;

Mat X = Mat::zeros(3,1, CV_32F);
cout << "X = " << endl << " " << X << endl << endl;

Mat Z = Mat::zeros(3,1, CV_32F);
cout << "Z = " << endl << " " << Z << endl << endl;

cv::Mat *pH(0);
cv::Mat H = cv::Mat::zeros(3,3, CV_32F);
cout << "H = " << endl << " " << H << endl << endl;

Mat iH = Mat::zeros(3,3, CV_32F);
cout << "iH = " << endl << " " << iH << endl << endl;

Mat b = Mat::zeros(3,1, CV_32F);
cout << "b = " << endl << " " << b << endl << endl;

Mat delta_p = Mat::zeros(3,1, CV_32F);
cout << "delta_p = " << endl << " " << delta_p << endl << endl;
 
//Mat pImgI = imread("C:/Users/Olga/Desktop/Projet_M2/octobre/1/img_test.jpg");
//namedWindow("image", WINDOW_AUTOSIZE);
//imshow("image", pImgI);
Size image_size = pImgI.size();//taille d'image
cout << "Taille d'image = " << endl << " " << image_size << endl << endl;

//Calcul des gradients
//pGradTx = image_size
//Mat pGradTx = Mat::zeros(image_size, CV_8UC1);
//Mat pGradTy = Mat::zeros(image_size, CV_8UC1);
//cout << "pGradTy = " << endl << " " << pGradTy << endl << endl;
//Mat pImgT;
	//pImgT.create( pImgI.size(), CV_8U );
	//imshow("pImgT",pImgT);
Mat pGradTx;
Mat pGradTy;
Mat pStDesc;
//Mat(const IplImage* pGradTx,bool copyData);
IplImage* pImgIChar(0);
IplImage* pImgTChar(0);
IplImage* pGradTxChar(0);
IplImage* pGradTyChar(0);
IplImage* pStDescChar(0);

	pGradTx.create( pImgI.size(), CV_16S );
	imshow("pGradTx",pGradTx);
	
	pGradTy.create( pImgI.size(), CV_16S );
	imshow("pGradTy",pGradTy);

	pStDesc.create( pImgI.size(), CV_32F );
	imshow("pStDesc",pStDesc);


// Get current time. We will use it later to obtain total calculation time.
	clock_t start_time = clock();

Mat abs_pGradTx, abs_pGradTy;
//Calcule dradient de T
cv::Sobel(pImgT, pGradTx,pImgT.depth(), 1,0);//Gradient dans la direction X
convertScaleAbs( pGradTx, abs_pGradTx ,0.125);//Normalisation
cv::Sobel(pImgT, pGradTy,pImgT.depth(), 0,1);//Gradient dans la direction Y
convertScaleAbs( pGradTy, abs_pGradTy, 0.125 );//Normalisation

//Descente de Gradient et matrice Hessienne
//H=0;//pas sur---cvSet(H, cvScalar(0)); // Set Hessian with zeroes
//pH=&H;

int u, v;	// (u,v) - pixel coordinates in the coordinate frame of T.
//int pImage
float u2, v2; // (u2,v2) - pixel coordinates in the coordinate frame of I.
//cv::Rect omega = cv::Rect(110, 100, 200, 150);

pImgIChar = cvCreateImage(cvSize(250,250), IPL_DEPTH_8U, 1);
pImgTChar = cvCreateImage(cvSize(250,250), IPL_DEPTH_8U, 1);
pGradTxChar = cvCreateImage(cvSize(250,250), IPL_DEPTH_8U, 1);
pGradTyChar = cvCreateImage(cvSize(250,250), IPL_DEPTH_8U, 1);
pStDescChar = cvCreateImage(cvSize(250,250), IPL_DEPTH_8U, 1);


pImgIChar->imageData = (char*) pImgI.data;//conversion image Mat vers IplImage
pImgTChar->imageData = (char*) pImgT.data;
pGradTxChar->imageData = (char*) pGradTx.data;
pGradTyChar->imageData = (char*) pGradTy.data;
pStDescChar->imageData = (char*) pStDesc.data;


// Walk through pixels in the template T.
	int i, j;
	for(i=0; i<omega.width; i++)
	{
		u = i + omega.x;

		for(j=0; j<omega.height; j++)
		{
			v = j + omega.y;

			// Evaluate gradient of T.
			short Tx = CV_IMAGE_ELEM(pGradTxChar, short, v, u);	
			short Ty = CV_IMAGE_ELEM(pGradTyChar, short, v, u);	
			
			// Calculate steepest descent image's element.
			float *stdesc(0);
			stdesc= &CV_IMAGE_ELEM(pStDescChar, float, v, u*3); // an element of steepest descent image
			stdesc[0] = (float)(-v*Tx+u*Ty);
			stdesc[1] = (float)Tx;
			stdesc[2] = (float)Ty;

			// Add a term to Hessian.
			int l,m;
			for(l=0;l<3;l++)
			{
				for(m=0;m<3;m++)
				{
					//CV_MAT_ELEM(*H, float, l, m) += stdesc[l]*stdesc[m];
					H.at<float>(l,m) += stdesc[l]*stdesc[m];
				}
			}
		}	
	}

// Invert Hessian.
	double inv_res = cv::invert(H, iH);
	if(inv_res==0)
	{
		cout << "Error: Hessian is singular " << endl;//printf("Error: Hessian is singular.\n");
		return;
	}

	/*
	 *   Iteration stage.
	 */

	// Set warp with identity.
	//cvSetIdentity(W);
	W = Mat::eye(3, 3, CV_32F);

	// Here we will store current value of mean error.
	float mean_error=0;

	// Iterate
	int iter=0; // number of current iteration
	//int MAX_ITER=100;
	while(iter < MAX_ITER)
	{
		iter++; // Increment iteration counter

		mean_error = 0; // Set mean error value with zero

		int pixel_count=0; // Count of processed pixels
		
		//cvSet(b, cvScalar(0)); // Set b matrix with zeroes
			
		// Walk through pixels in the template T.
		int i, j;
		for(i=0; i<omega.width; i++)
		{
			int u = i + omega.x;

			for(j=0; j<omega.height; j++)
			{
				int v = j + omega.y;

				// Set vector X with pixel coordinates (u,v,1)
				SET_VECTOR(X, u, v);

				// Warp Z=W*X
				gemm(W, X, 1, 0, 0, Z);

				// Get coordinates of warped pixel in coordinate frame of I.
				GET_VECTOR(Z, u2, v2);

				// Get the nearest integer pixel coords (u2i;v2i).
				int u2i = cvFloor(u2);
				int v2i = cvFloor(v2);

				if(u2i>=0 && u2i<pImgI.rows && // check if pixel is inside I.
					v2i>=0 && v2i<pImgI.cols)
				{
					pixel_count++;

					// Calculate intensity of a transformed pixel with sub-pixel accuracy
					// using bilinear interpolation.
					float I2 = interpolate<uchar>(pImgIChar, u2, v2);

					// Calculate image difference D = I(W(x,p))-T(x).
					float D = I2 - CV_IMAGE_ELEM(pImgTChar, uchar, v, u);

					// Update mean error value.
					mean_error += fabs(D);

					// Add a term to b matrix.
					float* stdesc = &CV_IMAGE_ELEM(pStDescChar, float, v, u*3);
					float* pb = &b.at<float>(0,0);
					pb[0] += stdesc[0] * D;
					pb[1] += stdesc[1] * D;
					pb[2] += stdesc[2] * D;					
				}	
			}
		}

		// Finally, calculate resulting mean error.
		if(pixel_count!=0)
			mean_error /= pixel_count;

		// Find parameter increment. 
		gemm(iH, b, 1, 0, 0, delta_p);
		float delta_wz = delta_p.at<float>(0,0);
		float delta_tx = delta_p.at<float>(1,0);
		float delta_ty = delta_p.at<float>(2,0);

		init_warp(dW, delta_wz, delta_tx, delta_ty);
		// Invert warp.
		inv_res = cv::invert(dW, idW);
		if(inv_res==0)
		{
			cout << "Error: Warp is singular " << endl;//printf("Error: Warp matrix is singular.\n");
			return;
		}

		gemm(idW, W, 1, 0, 0, dW);
		//cvCopy(dW, W);
		//W = dW.clone();
		dW.copyTo(W);

		// Print diagnostic information to screen.
		//printf("iter=%d mean_error=%f\n", iter, mean_error);
		// Print diagnostic information to screen.
		printf("iter=%d dwz=%f dtx=%f dty=%f mean_error=%f\n", iter, delta_wz, delta_tx, delta_ty, mean_error);


		// Check termination critera.
		if(fabs(delta_wz)<=EPS && fabs(delta_tx)<=EPS && fabs(delta_ty)<=EPS) break;
	}

	// Get current time and obtain total time of calculation.
	clock_t finish_time = clock();
	double total_time = (double)(finish_time-start_time)/CLOCKS_PER_SEC;

	// Print summary.
	printf("===============================================\n");
	printf("Algorithm: inverse compositional.\n");
	printf("Caclulation time: %g sec.\n", total_time);
	printf("Iteration count: %d\n", iter);
	printf("Epsilon: %f\n", EPS);
	printf("Resulting mean error: %f\n", mean_error);
	printf("===============================================\n");

	//// Show result of image alignment.
	//draw_warped_rect(pImgIChar, omega, W);
	//cvSetImageROI(pImgTChar, omega);
	//namedWindow("template", WINDOW_AUTOSIZE);
	//imshow("template",pImgT);
	//namedWindow("image", WINDOW_AUTOSIZE);
	//imshow("image",pImgI);
	//cvResetImageROI(pImgTChar);



	// Show T and I and wait for key press.
	draw_warped_rect(pImgI, omega, W);
	cvSetImageROI(pImgTChar, omega);
	namedWindow("template", WINDOW_AUTOSIZE);
	cv::imshow("template", pImgT);
	namedWindow("image", WINDOW_AUTOSIZE);
	imshow("image",pImgI);
	printf("Press any key to start Lucas-Kanade algorithm.\n");
	//printf("Press any key to start Baker-Dellaert-Matthews algorithm.\n");
	waitKey(0);
	cvResetImageROI(pImgTChar);

	//// Free used resources and exit.
	//cvReleaseMat(&W);
	//cvReleaseMat(&dW);
	//cvReleaseMat(&idW);
	//cvReleaseMat(&H);
	//cvReleaseMat(&iH);
	//cvReleaseMat(&b);
	//cvReleaseMat(&delta_p);
	//cvReleaseMat(&X);
	//cvReleaseMat(&Z);

	
	//Size rows = img.size();
//cout << "Ligne d'image = " << endl << " " << rows << endl << endl;
    

  
  //E[0][0]=5;
//imshow("My Window", E);

	//waitKey();
	
    //return 0;
	//system("PAUSE");
}
