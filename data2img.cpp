/**
 * \file      data2img.cpp
 * \author    Michaud Dorian
 * \version   0.1
 * \date      16/12/2014
 * \brief     Fichier de mise en forme des donnees sous forme de matrice
 *
 * \details    Ce fichier contient la fonction qui va mettre en forme les donnees sous formes d'image.
 *                 
 */
#include "data2img.h"

cv::Mat data2img(cv::Mat data){
	
	double minValX, maxValX, minValY, maxValY, minValA, maxValA; 
	int i, idx, idy;
	cv::Mat Mcpt, img, X(88439,1,CV_64F), Y(88439,1,CV_64F), A(88439,1,CV_64F);
	img = img.zeros(250,250,CV_64F);
	Mcpt = Mcpt.ones(250,250,CV_64F);
	minMaxLoc( data.col(0), &minValX, &maxValX, NULL, NULL, cv::Mat() );
	minMaxLoc( data.col(1), &minValY, &maxValY, NULL, NULL, cv::Mat() );
	minMaxLoc( data.col(2), &minValA, &maxValA, NULL, NULL, cv::Mat() );
	X = data.col(0) - minValX;
	Y = data.col(1) - minValY;
	A = data.col(2) - minValA;
	minMaxLoc( X, &minValX, &maxValX, NULL, NULL, cv::Mat() );
	minMaxLoc( Y, &minValY, &maxValY, NULL, NULL, cv::Mat() );
	X = X / (maxValX/249.0);
	Y = Y / (maxValY/249.0);
	for (i=0;i<88439;++i)
	{
		idx = cvRound(X.at<double>(i));
		idy = cvRound(Y.at<double>(i));
		if(img.at<double>(idx, idy) == 0.0)
		{
			img.at<double>(idx, idy) = A.at<double>(i);
		}	
		else
		{
			img.at<double>(idx, idy) = img.at<double>(idx, idy) + A.at<double>(i);
			Mcpt.at<double>(idx, idy) = Mcpt.at<double>(idx, idy) + 1;
		}
	}
	//imshow("Display window img", img);
	return img / Mcpt;
}