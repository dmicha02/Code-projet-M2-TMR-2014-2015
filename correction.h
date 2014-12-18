#ifndef CORRECTION_H
#define	CORRECTION_H
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo/photo.hpp"
#include <algorithm>
#include <iostream> 
void Interpolation(cv::Mat img, cv::Mat& dst);
void swirl_init(cv::Size img_size, cv::Point center, float radius, float rotation, float strength, cv::Mat& swirl_data);
void swirl(cv::Mat img, cv::Mat& dst, cv::Mat& swirl_data);
cv::Mat interpolationMean(cv::Mat);
cv::Mat interpolationMedian(cv::Mat);
#endif	/* CORRECTION_H */

