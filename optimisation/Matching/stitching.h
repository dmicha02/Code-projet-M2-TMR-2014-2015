//#ifndef STITCHING_H
//#define	STITCHING_H
//
//#include <iostream>
//#include <string>
//#include <fstream>
//#include <stdlib.h>
//#include <algorithm>
//#include "opencv2/core/core.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
//using namespace std;
//using namespace cv;
//
//Mat get_Jacobian(int, int);
//Mat get_dwarp_estimate(Vec6f);
//Mat align_image(Mat, Mat, Rect);
//
//#endif /* STITCHING_H */


#ifndef STITCHING_H
#define	STITCHING_H

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;

Mat get_Jacobian(int, int);
Mat get_dwarp_estimate(Vec6f);
UMat align_image(UMat, UMat, Rect);

#endif /* STITCHING_H */