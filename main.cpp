﻿#include <cstdlib>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <timer.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "optimization.h"
#include "correction.h"
#include "Matching/TemplateMatching.h"
#include "Matching/EstimateRigidT.h"
#include "Matching/ECC.h"
#include "loadtextfile.h"
#include "data2img.h"
#include "stitching.h"

using namespace std;
using namespace cv;

/**
 * \fn static void help()
 * \brief Description of program for users 
 * 
 */
static void help()
{
    cout << "\nRebuildind endomicroscopic images V1.0.\n"\
		"M2 TMR Project 2014-2015\n"\
		"**********************************\n\n"<< endl;
}

/**
 * \fn int main (int argc, char** argv)
 * \brief Main program of M2 project 2014-2015 : Endomicroscopic images reconstruction
 * \return EXIT_SUCCESS - Normal stop of program.
 */
int main(int argc, char** argv) {

	help();
//****************************************************************************************************************
//								Images reconstruction with analogic datas
//****************************************************************************************************************/
	//Mat data, img, img_reconstr;
	///// Load text file in matrix
	//timer__("Load text file")
	//	data = load_textfile2Mat("Papier.txt"); //chargement du fichier contenant les données analogiques
	///// Matrix of file to an image form
	//timer__("File matrix to an image form")
	//	img = data2img(data);
	//imshow("Originale image", img);	
	///// Mean Interpolation
	//timer__("Mean Interpolation of image")
	//	img_reconstr = interpolationMean(img);
	//imshow("Mean Interpolate", img_reconstr);
	///// Median Interpolation
	//timer__("Median Interpolation of image")
	//	img_reconstr = interpolationMedian(img);
	//imshow("Median Interpolate", img_reconstr);
	///// Bilinear interpolation
	//timer__("Bilinear Interpolation of image")
	//	img_reconstr = interpolationBilinear(img);
	//imshow("Bilinear Interpolate", img_reconstr);
	//waitKey(0);

//****************************************************************************************************************
//											Stiching images
//****************************************************************************************************************/
 //   string path;
 //   if (argc < 2) {
 //       path = "../img/Endoscope non lineaire_champ 250um/";
 //   } else {
 //       path = string(argv[1]);
 //   }

	///// Initialisation
	//Mat img0, target_img, img, img2, img_pano, template_img, img_merge, W_estimate;
 //   /// Capture of images
 //   VideoCapture cap(path + "%03d.jpg");
 //   cap.set(CAP_PROP_FPS, 8);
 //   //namedWindow("Camera", WINDOW_AUTOSIZE);
	////namedWindow("Rebuilded", WINDOW_AUTOSIZE);
	////namedWindow("Panorama", WINDOW_AUTOSIZE);
 //   /// Process on first image
	//while (!cap.read(img));
	//img.convertTo(img, CV_32F); // Convert in float32
 //   cvtColor(img, img, COLOR_RGB2GRAY); // RGB -> Grayscale
	//img = img / 255.0; // Value between 0 and 1
	////imshow("Camera", img/255);
	//img0 = interpolationMean(img); // reconstruction of image
 //   //imshow("Rebuilded",img0);
	///// Init template
	//Rect rec_tmp(Point(img0.rows/6, img0.cols/6), Point(5*img0.rows/6, 5*img0.cols/6)); // Template rectangle
	//template_img = img0(Range(img0.rows/6, 5*img0.rows/6), Range(img0.cols/6, 5*img0.cols/6)); // Template image
	//img_pano = img_pano.zeros(2000,2000,CV_32F);
	/////Process on other images
 //   while (true){
 //       if (cap.read(img)){
 //           img.convertTo(img, CV_32F); // Convert in float32
	//		cvtColor(img, img, COLOR_RGB2GRAY); // RGB -> Grayscale
	//		target_img = img / 255.0; // Value between 0 and 1
	//		//imshow("Camera", img/255); // display images with artefacts
	//		/// Image reconstruction
 //           timer__("Correction")
 //               target_img = interpolationMean(target_img); // interpolation with mean value in neighborhood
	//		//imshow("Rebuilded", target_img); 
	//		/// Matthews-Baker inverse compositionnal algorithm
	//		timer__("Template Matching")
	//			W_estimate = align_image(target_img, template_img, rec_tmp); // inverse compositional algorithm
	//		//cout << W_estimate << endl;
	//		warpPerspective(target_img, img2, W_estimate.inv(), target_img.size()); // warp images
	//		//imshow("Warped", img2);
	//		/// Merge two images
	//		//addWeighted(img_merge, 0.5, img2, 0.5, 0.0, img_merge);
	//		img_merge = merge_image(img0, img2);
	//		imshow("Merge", img_merge);
	//		///Display panorama
	//		//imshow("Panorama", img_pano);
	//		///Update template image
	//		img2.copyTo(img0); // Previous image
	//		Rect rec_tmp(Point(img0.rows/6, img0.cols/6), Point(5*img0.rows/6, 5*img0.cols/6)); // Template rectangle
	//		template_img = img0(Range(img0.rows/6, 5*img0.rows/6), Range(img0.cols/6, 5*img0.cols/6)); // Template image
 //           waitKey();
 //       }
 //       if (waitKey(30) >= 0) break;
 //   }
//****************************************************************************************************************
//											Panorama
//****************************************************************************************************************/
	string path;
    if (argc < 2) {
        path = "../img/tx-2_ty-2/";
    } else {
        path = string(argv[1]);
    }
	/// Initialisation
	Mat img, img0, img_merge, target_img, warp(3,3,CV_32F);
	warp.at<float>(0,0) = 1.0;
	warp.at<float>(0,1) = 0.0;
	warp.at<float>(0,2) = 2;
	warp.at<float>(1,0) = 0.0;
	warp.at<float>(1,1) = 1.0;
	warp.at<float>(1,2) = 2.0;
	warp.at<float>(2,0) = 0.0;
	warp.at<float>(2,1) = 0.0;
	warp.at<float>(2,2) = 1.0;
	/// First image
	int num=53;
	stringstream ss;
	ss << num;
	img0 = imread(path + ss.str() + ".jpg", 0);
	img0.convertTo(img0,CV_32F);
	img0=img0/255.0;
	img_merge = img0;
    /// Capture of images
	while(true){
		num+=1;
		stringstream ss;
		ss << num;
		img = imread(path + ss.str() + ".jpg", 0);
		img.convertTo(img,CV_32F);
		img = img/255.0;
		if(img.rows == 0)
			break;
		img_merge = merge_image(img0, img, warp);
		imshow("merge",img_merge);
		img.copyTo(img0);
		waitKey(0);
	}
    return EXIT_SUCCESS;
}

