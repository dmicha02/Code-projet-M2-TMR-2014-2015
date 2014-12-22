#include "optimization.h"
#include "correction.h"

using namespace cv;
using namespace std;

/**
 * \fn Mat interpolationMean(Mat img0)
 * \brief function to interpolate image with the mean value of neighborhood
 * \param img0 - the original image
 * \return cv::Mat - the interpolate image
 */
Mat interpolationMean(Mat img0)
{
	int i, j, k, l, cpt;
	int img_type = img0.type();
	double minValA, maxValA, s;
	Mat img;
	img=img0.clone();
	if(img_type==6) // if image type is double
	{
		minMaxLoc( img.col(2), &minValA, &maxValA, NULL, NULL, Mat() );
		// sweep image
		for(i=0;i<img.rows-2;i++)
			{
				for(j=0;j<img.cols-2;j++)
				{
					if(img.at<double>(i,j) <= (2*maxValA/255.0)) // criterion of interpolation
					{
						cpt = 1;
						s = 0;
						// sweep neighborhood
						for(k=i;k<i+3;++k)
						{
							for(l=j;l<j+3;++l)
							{
								if(img.at<double>(k,l) >= (2*maxValA/255.0)) // if this test is true : pixel with valid information
								{
									s = s + img.at<double>(k,l);
									cpt = cpt + 1;
								}
							}
						}
						img.at<double>(i,j) = s/cpt; // new value of interpolate pixel
					}
				}
			}
		return img;
	}
	else if(img_type==0) // if image type is integer
	{
		minMaxLoc( img.col(2), &minValA, &maxValA, NULL, NULL, Mat() );
		for(i=0;i<img.rows-2;i++)
			{
				for(j=0;j<img.cols-2;j++)
				{
					if(img.at<uchar>(i,j) <= (2*maxValA/255.0))
					{
						cpt = 1;
						s = 0;
						for(k=i;k<i+3;++k)
						{
							for(l=j;l<j+3;++l)
							{
								if(img.at<uchar>(k, l) >= (2 * maxValA / 255)) 
								{
									s = s + img.at<uchar>(k, l);
									cpt = cpt + 1;
								}
							}
						}
						img.at<uchar>(i, j) = s / cpt;
					}
				}
			}
		return img;
	}
}

/**
 * \fn Mat interpolationMedian(Mat img0)
 * \brief function to interpolate image with the median value of neighborhood
 * \param img0 - the original image
 * \return cv::Mat - the interpolate image
 */
Mat interpolationMedian(Mat img0)
{
	int i, j, k, l;
	double minValA, maxValA;
	Mat img;
	img=img0.clone();
	vector<double> v;
	int img_type = img0.type();
	if(img_type==6) // if image type is double
	{
		minMaxLoc( img.col(2), &minValA, &maxValA, NULL, NULL, Mat() );
		//sweep image
		for(i=0;i<img.rows-2;i++)
		{
			for(j=0;j<img.cols-2;j++)
			{
				if(img.at<double>(i,j) <= (2*maxValA/255.0)) // if this test is true : interpolation
				{
					v.clear();
					//sweep neighborhood
					for(k=i;k<i+3;++k)
					{
						for(l=j;l<j+3;++l)
						{
							if(img.at<double>(k,l) >= (2*maxValA/255.0))  // if this test is true : pixel with valide information
								v.push_back(img.at<double>(k,l));
						}
					}
					sort(v.begin(), v.end());
					if(v.size()!=0)
						img.at<double>(i,j) = v.at(v.size()/2); // value of interpolated pixel
				}
			}
		}
		return img;
	}
	else if(img_type==0) // if image type is integer
	{
		minMaxLoc( img.col(2), &minValA, &maxValA, NULL, NULL, Mat() );
		for(i=0;i<img.rows-2;i++)
		{
			for(j=0;j<img.cols-2;j++)
			{
				if(img.at<double>(i,j) <= (2*maxValA/255.0))
				{
					v.clear();
					for(k=i;k<i+3;++k)
					{
						for(l=j;l<j+3;++l)
						{
							if(img.at<double>(k,l) >= (2*maxValA/255.0)) 
								v.push_back(img.at<double>(k,l));
						}
					}
					sort(v.begin(), v.end());
					if(v.size()!=0)
						img.at<double>(i,j) = v.at(v.size()/2);
				}
			}
		}
		return img;
	}
}