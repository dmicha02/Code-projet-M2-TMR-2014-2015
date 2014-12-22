#include <cstdlib>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <timer.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "optimization.h"
#include "correction.h"
#include "Matching/TemplateMatching.h"
#include "Matching/EstimateRigidT.h"
#include "Matching/ECC.h"
#include "loadtextfile.h"
#include "data2img.h"

using namespace std;
using namespace cv;

/**
 * \fn static void help()
 * \brief Description of program for users 
 * 
 */
static void help()
{
	cout << "Endomicroscopic images reconstruction program\n"\
				"\n"<<endl;
}

/**
 * \fn int main (int argc, char** argv)
 * \brief Main program.
 *
 * \return EXIT_SUCCESS - Normal stop of program.
 */
int main(int argc, char** argv) {

	help();
//****************************************************************************************************************
//								Images reconstruction with analogic datas
//****************************************************************************************************************/
	//Mat data, img, img_reconstr;
	//
	////Load text file in a cv::Mat
	//data = load_textfile2Mat("Papier.txt"); //chargement du fichier contenant les données analogiques
	//
	//timer__("Time of images reconstruction")
	//{
	//	//cv::Mat of file to image form (cv::Mat 250*250)
	//	timer__("File matrix to an image form")
	//		img = data2img(data);
	//	
	//	//Mean Interpolation
	//	timer__("Mean Interpolation of image")
	//		img_reconstr = interpolationMean(img);

	//	////Median Interpolation
	//	//timer__("Median Interpolation of image")
	//	//	img_reconstr = interpolationMedian(img);
	//}
	//imshow("Originale image", img);
	//imshow("Mean Interpolate", img_reconstr);
	////imshow("Median Interpolate", img_reconstr);

/****************************************************************************************************************
											Stiching images
****************************************************************************************************************/
    string path;
    if (argc < 2) {
        path = "../img/Endoscope non lineaire_champ 250um/";
    } else {
        path = string(argv[1]);
    }
    
 
    VideoCapture cap(path + "%03d.jpg");
    cap.set(CAP_PROP_FPS, 8);
    namedWindow("Camera", WINDOW_AUTOSIZE);
    namedWindow("Panorama", WINDOW_AUTOSIZE);

    Mat img;

    //Process on first image
    while (!cap.read(img));
    cvtColor(img, img, COLOR_BGR2GRAY);
	img = interpolationMean(img); // reconstruction of image
    
	// init for template matching
    Mat img_fusion = Mat::zeros(768 * 2, 1024 * 2, CV_8U);
    Rect img_center_fusion;
    img_center_fusion.x = img_fusion.cols / 2 - img.cols / 2;
    img_center_fusion.y = img_fusion.rows / 2 - img.rows / 2;
    img_center_fusion.width = img.cols;
    img_center_fusion.height = img.rows;
    img.copyTo(img_fusion(img_center_fusion));
    TemplateMatching m(img_center_fusion, img_fusion, SEQUENCE);

	//Process on other images
    while (true) {

        if (cap.read(img)) {
            cvtColor(img, img, COLOR_BGR2GRAY);
			//imshow("Original image", img);
            timer__("Correction") {
                img = interpolationMean(img);
            }

            bool matched;
            timer__("Matching")
				matched = m.match(img);

            if (likely(matched))
                timer__("Fusion")
					m.blend(img_fusion);

            Mat img_fusion_mini;
            resize(img_fusion, img_fusion_mini, Size(), 0.5, 0.5, INTER_LINEAR);

            imshow("Panorama", img_fusion_mini);
            imshow("Camera", img);
            waitKey();
        }
        if (waitKey(30) >= 0) break;
    }

    if (argc < 3) {
        m.build_final("out.png");
    } else {
        m.build_final(string(argv[2]));
    }

	waitKey(0);    
    return EXIT_SUCCESS;
}

