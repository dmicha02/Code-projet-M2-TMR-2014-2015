#ifndef STITCHING_H
#define	STITCHING_H

#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;

Mat get_Jacobian(int, int);
Mat get_dwarp_estimate(Vec6f);
Mat align_image(Mat, Mat, Rect);
Mat merge_image(Mat, Mat);
Mat crop_image(Mat);

#endif /* STITCHING_H */