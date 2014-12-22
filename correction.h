#ifndef CORRECTION_H
#define	CORRECTION_H
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo/photo.hpp"
#include <algorithm>
#include <iostream> 
using namespace cv;

Mat interpolationMean(Mat);
Mat interpolationMedian(Mat);
#endif	/* CORRECTION_H */

