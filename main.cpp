#include <cstdlib>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "optimization.h"
#include "correction.h"
#include "Matching/TemplateMatching.h"
#include "Matching/EstimateRigidT.h"
#include "Matching/ECC.h"
#include <timer.h>
using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    string path; //chemin des images
    if (argc < 2) {
        path = "../img/Endoscope non lineaire_champ 250um/"; // valeur par défaut si on ne passe pas d'argument
        //path = "../photonique/images_test/";
    } else {
        path = string(argv[1]); // si on passe un arguement cela devient le chemin
    }
    
    ////Mat swirl_data;
    ////swirl_init(Size(450, 450), Point(225, 225), 0.022 * 450, -75 / 180 * M_PI, 0, swirl_data);

    VideoCapture cap(path + "%03d.jpg");
    //VideoCapture cap(0);
    cap.set(CAP_PROP_FPS, 8);

    namedWindow("Camera", WINDOW_AUTOSIZE);
    namedWindow("Panorama", WINDOW_AUTOSIZE);

    Mat img, img2;

    //Traitement de la première image
    while (!cap.read(img));

    cvtColor(img, img2, COLOR_BGR2GRAY);

    Interpolation(img2, img);

    
    Mat img_fusion = Mat::zeros(768 * 2, 1024 * 2, CV_8U);
    Rect img_center_fusion;
    img_center_fusion.x = img_fusion.cols / 2 - img.cols / 2;
    img_center_fusion.y = img_fusion.rows / 2 - img.rows / 2;
    img_center_fusion.width = img.cols;
    img_center_fusion.height = img.rows;
    img.copyTo(img_fusion(img_center_fusion));
    TemplateMatching m(img_center_fusion, img_fusion, SEQUENCE);

    while (true) {

        if (cap.read(img)) {
            cvtColor(img, img, COLOR_BGR2GRAY);

            timer__("Correction") {
                Interpolation(img, img);
                //swirl(img, img, swirl_data);
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

