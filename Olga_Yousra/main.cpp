#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>//Sobel 


#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>


#include <stdlib.h>
#include <stdio.h>
#include <xxx.h>
#include <invc.h>
#include <time.h>



using namespace cv;
using namespace std;

int main()
{

Mat pImgI_1 = imread("C:/Users/Olga/Desktop/Projet_M2/octobre/1/001.bmp");
Mat pImgI = imread("C:/Users/Olga/Desktop/Projet_M2/octobre/1/001.bmp");

	
	


Mat pImgT;
	pImgT.create( pImgI_1.size(), CV_8U );
	//imshow("pImgT",pImgT);
	//namedWindow("template", WINDOW_AUTOSIZE);
	//imshow("template",pImgT);
cv::Rect omega = cv::Rect(110, 100, 200, 150);


align_image_inverse_compositional(pImgT, omega, pImgI);
	waitKey();
	
    return 0;
	//system("PAUSE");
}
