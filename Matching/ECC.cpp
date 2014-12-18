#include "ECC.h"

using namespace cv;

ECC::ECC(Rect initialPosition, InputArray img_fusion, Matching_Mode mode, int template_size) : matcher(initialPosition, img_fusion, mode) {
    transformation = Mat::eye(2, 3, CV_32F);
    this->template_size = template_size;
}

ECC::~ECC() {
}

//TODO : reduire images/utilisation d'une pyramide
bool ECC::match(InputArray img) {
    this->img = img.getMat();

    Rect selection;
    selection.x = (this->last_img.cols / 2)-((template_size / 2));
    selection.y = (this->last_img.rows / 2)-((template_size / 2));
    selection.height = template_size;
    selection.width = template_size;

    Mat transfo = Mat::eye(2, 3, CV_32F);
    double correl = 0;
    switch (mode) {
        case SEQUENCE:
            try {
                correl = findTransformECC(this->last_img, this->img, transfo, MOTION_AFFINE, TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 50, 0.001));
            } catch (cv::Exception& e) {
                const char* err_msg = e.what();
                std::cout << "exception caught: " << err_msg << std::endl;
                waitKey();
            }
            std::cout << correl << std::endl;
            std::cout << transfo << std::endl;
            break;
            //TODO : Panorama
    }
    //if (correl < 0.9) return false;

    position.x = last_position.x - transfo.at<float>(0, 2);
    position.y = last_position.y - transfo.at<float>(1, 2);

    transfo.at<float>(0, 2) = 0;
    transfo.at<float>(1, 2) = 0;
    if (mode == SEQUENCE) {
        Matx13f tmp(0, 0, 1);
        vconcat(transformation, tmp, transformation);
        vconcat(transfo, tmp, transformation);
        transformation = transfo * transformation;
    } else {
        transformation = transfo;
    }
    next_img();
    //waitKey();
    return true;
}

void ECC::blend(InputOutputArray img_fusion) {
    Mat imgW, gradientW;

    warpAffine(img, imgW, transformation, img.size());
    warpAffine(gradient, gradientW, transformation, img.size());
    doBlend(img_fusion, gradientW, imgW);
}