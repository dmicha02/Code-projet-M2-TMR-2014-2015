#include "optimization.h"
#include "correction.h"

using namespace cv;
using namespace std;

/**
 * \fn Mat interpolationMean(Mat img0)
 * \brief function to interpolate image with the mean value of neighborhood
 * \param img0 - the original image CV_8U or CV_64F
 * \return cv::Mat - the interpolate image
 */
Mat interpolationMean(Mat img0)
{
	int i, j, k, l, cpt;
	int img_type = img0.type();
	double minValA, maxValA, s;
	Mat img;
	img=img0.clone();
	if(img_type == 6) // if image type is double
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
	else if(img_type == 0) // if image type is integer
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
	else
		cout << "Wrong image type in interpolation function !" << endl;
}

/**
 * \fn Mat interpolationMedian(Mat img0)
 * \brief function to interpolate image with the median value of neighborhood
 * \param img0 - the original image CV_8U or CV_64F
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
	if(img_type == 6) // if image type is double
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
	else if(img_type == 0) // if image type is integer
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
	else
		cout << "Wrong image type in interpolation function !" << endl;
}

/**
 * \fn Mat interpolationBilinear(Mat img0)
 * \brief function to interpolate image with the bilinear method
 * \param img0 - the original image CV_8U or CV_64F
 * \return cv::Mat - the interpolate image
 */
Mat interpolationBilinear(Mat img0)
{
	int i, j, k;
	double minValA, maxValA;
	Mat img, circ;
	img=img0.clone();
	Vec4d neighbors, weight;
	int img_type = img0.type();
	if(img_type == 6) // if image type is double
	{
		minMaxLoc( img.col(2), &minValA, &maxValA, NULL, NULL, Mat() );
		circ = circ.zeros(img.size(), CV_64F);
		minMaxLoc( img.col(2), &minValA, &maxValA, NULL, NULL, Mat() );
		circle(circ, Point(circ.cols  / 2 + 1, circ.rows / 2 + 1), circ.cols / 2, 1.5, -1);
		//sweep image
		for(i=1;i<img.rows-1;i++)
		{
			for(j=1;j<img.cols-1;j++)
			{
				if(img.at<double>(i,j) <= (2*maxValA/255.0)) // if this test is true : interpolation
				{
					//Neighborhood of pixel to interpolate
					neighbors[0] = img.at<double>(i-1,j);
					neighbors[1] = img.at<double>(i+1,j);
					neighbors[2] = img.at<double>(i,j-1);
					neighbors[3] = img.at<double>(i,j+1);
					//weight of each pixel in neighborhood
					weight[0] = 1;
					weight[1] = 1;
					weight[2] = 1;
					weight[3] = 1;
					for(k=0;k<4;++k)
					{
						if(neighbors[k] <= (2*maxValA/255.0))
							weight[k] = 0;
					}
					double vw_sum = 0, w_sum = 0 ;
					//Calcul of new value of pixel to interpolate
					for(k=0;k<4;++k)
					{
						vw_sum = vw_sum + weight[k] * neighbors[k];
						w_sum = w_sum + weight[k];
					}
					img.at<double>(i,j) = vw_sum / w_sum;
				}
			}
		}
		return img & circ;
	}
	if(img_type == 0) // if image type is integer
	{
		minMaxLoc( img.col(2), &minValA, &maxValA, NULL, NULL, Mat() );
		circ = circ.zeros(img.size(), CV_8U);
		minMaxLoc( img.col(2), &minValA, &maxValA, NULL, NULL, Mat() );
		circle(circ, Point(circ.cols  / 2 + 1, circ.rows / 2 + 1), circ.cols / 2, 255, -1);
		//sweep image
		for(i=1;i<img.rows-1;i++)
		{
			for(j=1;j<img.cols-1;j++)
			{
				if(img.at<uchar>(i,j) <= 2) // if this test is true : interpolation
				{
					neighbors[0] = img.at<uchar>(i-1,j);
					neighbors[1] = img.at<uchar>(i+1,j);
					neighbors[2] = img.at<uchar>(i,j-1);
					neighbors[3] = img.at<uchar>(i,j+1);
					weight[0] = 1;
					weight[1] = 1;
					weight[2] = 1;
					weight[3] = 1;
					for(k=0;k<4;++k)
					{
						if(neighbors[k] <= 2)
							weight[k] = 0;
					}
					int vw_sum = 0, w_sum = 0 ;
					for(k=0;k<4;++k)
					{
						vw_sum = vw_sum + weight[k] * neighbors[k];
						w_sum = w_sum + weight[k];
					}
					img.at<uchar>(i,j) = vw_sum / w_sum;
				}
			}
		}
		return img & circ;
	}
	else
		cout << "Wrong image type in interpolation function !" << endl;
}