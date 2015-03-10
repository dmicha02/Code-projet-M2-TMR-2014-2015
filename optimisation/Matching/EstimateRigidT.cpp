#include "EstimateRigidT.h"

using namespace cv;

EstimateRigidT::EstimateRigidT(Rect initialPosition, InputArray img_fusion, Matching_Mode mode) : matcher(initialPosition, img_fusion, mode) {
    transformation = Mat::eye(2, 3, CV_64F);
}

EstimateRigidT::~EstimateRigidT() {
}

bool EstimateRigidT::match(InputArray img) {
    this->img = img.getMat();

    Mat transfo;
    switch (mode) {
        case SEQUENCE:
            transfo = estimateRigidTransform(this->img, this->last_img, false);
            break;
        case PANORAMA:
            transfo = estimateRigidTransform(this->img, last_img_fusion(Rect(last_position, this->img.size())), false);
            break;
    }
    if (transfo.rows != 2 && transfo.cols != 3) return false;


    position.x = last_position.x + transfo.at<double>(0, 2);
    position.y = last_position.y + transfo.at<double>(1, 2);

    transfo.at<double>(0, 2) = 0;
    transfo.at<double>(1, 2) = 0;
    if (mode == SEQUENCE) {
        Matx13d tmp(0, 0, 1);
        vconcat(transformation, tmp, transformation);
        vconcat(transfo, tmp, transformation);
        transformation = transfo * transformation;
    } else {
        transformation = transfo;
    }
    next_img();
    return true;
}

void EstimateRigidT::blend(InputOutputArray img_fusion) {
    Mat imgW, gradientW;

    warpAffine(img, imgW, transformation, img.size());
    warpAffine(gradient, gradientW, transformation, img.size());
    doBlend(img_fusion, gradientW, imgW);
}