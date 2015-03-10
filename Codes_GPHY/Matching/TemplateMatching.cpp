#include "TemplateMatching.h"
#include <opencv2/opencv.hpp>

using namespace cv;

TemplateMatching::TemplateMatching(Rect initialPosition, InputArray img_fusion, Matching_Mode mode, int template_size) : matcher(initialPosition, img_fusion, mode) {
    this->template_size = template_size;
}

TemplateMatching::~TemplateMatching() {
}

bool TemplateMatching::match(InputArray img) {
    this->img = img.getMat();

    Rect selection;
    selection.x = (this->img.cols / 2)-((template_size / 2));
    selection.y = (this->img.rows / 2)-((template_size / 2));
    selection.height = template_size;
    selection.width = template_size;

    Mat result;
    double minVal;
    Point minLoc;

    Mat eqImg, last_imgEq;
    equalizeHist(this->img(selection), eqImg);

    switch (mode) {
        case SEQUENCE:
            equalizeHist(this->last_img, last_imgEq);

            matchTemplate(last_imgEq, eqImg, result, TM_SQDIFF_NORMED);
            minMaxLoc(result, &minVal, 0, &minLoc, 0);
            if (minVal > 0.5) return false;
            position.x = last_position.x + (minLoc.x - selection.x);
            position.y = last_position.y + (minLoc.y - selection.y);
            break;
        case PANORAMA:
            int searchSize = this->img.cols / 4;

            Point tl, br;
            tl.x = max<int>(last_position.x - searchSize, 0);
            tl.y = max<int>(last_position.y - searchSize, 0);
            br.x = min<int>(last_position.x + this->img.cols + searchSize, last_img_fusion.cols);
            br.y = min<int>(last_position.y + this->img.rows + searchSize, last_img_fusion.rows);
            Rect search(tl, br);

            equalizeHist(last_img_fusion(search), last_imgEq);

            matchTemplate(last_imgEq, eqImg, result, TM_SQDIFF_NORMED);
            minMaxLoc(result, &minVal, 0, &minLoc, 0);
            if (minVal > 0.5) return false;
            position.x = tl.x + (minLoc.x - selection.x);
            position.y = tl.y + (minLoc.y - selection.y);
            break;
    }

    next_img();
    return true;
}

void TemplateMatching::blend(InputOutputArray img_fusion) {
    doBlend(img_fusion, gradient, img);
}