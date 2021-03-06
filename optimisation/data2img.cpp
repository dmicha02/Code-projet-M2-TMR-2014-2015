#include "data2img.h"

using namespace cv;

/**
 * \fn Mat data2img(Mat data)
 * \brief function to change file matrix in image matrix
 * \param data - file matrix with analogic data
 * \return Mat - the matrix image CV_32F
 */
Mat data2img(Mat data)
{
	double minValX, maxValX, minValY, maxValY, minValA, maxValA; 
	int idx, idy, nb_line = 88439;
	Mat Mcpt, img, X(nb_line, 1, CV_32F), Y(nb_line, 1, CV_32F), A(nb_line, 1, CV_32F);
	img = img.zeros(250, 250, CV_32F);
	Mcpt = Mcpt.ones(250, 250, CV_32F);
	minMaxLoc(data.col(0), &minValX, &maxValX);
	minMaxLoc(data.col(1), &minValY, &maxValY);
	minMaxLoc(data.col(2), &minValA, &maxValA);
	X = data.col(0) - minValX;
	Y = data.col(1) - minValY;
	A = data.col(2) - minValA;
	minMaxLoc(X, &minValX, &maxValX);
	minMaxLoc(Y, &minValY, &maxValY);
	X = X / (maxValX / 249.0);
	Y = Y / (maxValY / 249.0);
	for (int i=0;i<nb_line;i++)
	{
		idx = cvRound(X.at<float>(i));
		idy = cvRound(Y.at<float>(i));
		if(img.at<float>(idx, idy) == 0.0)
		{
			img.at<float>(idx, idy) = A.at<float>(i);
		}	
		else
		{
			img.at<float>(idx, idy) = img.at<float>(idx, idy) + A.at<float>(i);
			Mcpt.at<float>(idx, idy) = Mcpt.at<float>(idx, idy) + 1;
		}
	}
	return img / Mcpt;
}