#ifndef CORRECTION_H
#define	CORRECTION_H
#include <opencv2/opencv.hpp>

void Interpolation(cv::Mat img, cv::Mat& dst);
void swirl_init(cv::Size img_size, cv::Point center, float radius, float rotation, float strength, cv::Mat& swirl_data);
void swirl(cv::Mat img, cv::Mat& dst, cv::Mat& swirl_data);
#endif	/* CORRECTION_H */

