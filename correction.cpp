#include "optimization.h"
#include "correction.h"
#define M_LN2 0.693147180559945309417
using namespace cv;
using namespace std;

void Interpolation(Mat img, Mat& dst) {
#define CONNEXITE 8
#define COND_SUM(y,x) if(img.at<uchar>(y, x) > 2) {sum+=img.at<uchar>(y, x); nb++;}
#define COND_SUM2(y,x) if(img.at<uchar>(y, x) > 2) {sum+=img.at<uchar>(y, x)/2; nb+=0.5f;}

    dst = img.clone();

    Mat mask;
    mask = mask.zeros(img.size(), CV_8U);

    //circle(mask, Point(mask.cols / 2, mask.rows / 2), mask.cols / 2, 255, CV_FILLED);
    circle(mask, Point(mask.cols / 2, mask.rows / 2), mask.cols / 2, 255, 1);
    mask = (img <= 2) & mask;

#pragma omp parallel for collapse(2) schedule(dynamic, 50) firstprivate(mask, img)
    for (int y = 0; y < mask.rows; y++) {
        for (int x = 0; x < mask.cols; x++) {
            if (likely(mask.at<uchar>(y, x) == 255)) {
                float nb = 0;
                int sum = 0;
                if (x > 0)
                    COND_SUM(y, x - 1);
                if (x < mask.cols - 1)
                    COND_SUM(y, x + 1);

                if (y > 0)
                    COND_SUM(y - 1, x);
                if (y < mask.rows - 1)
                    COND_SUM(y + 1, x);
#if CONNEXITE==8
                if (x > 0 && y > 0)
                    COND_SUM(y - 1, x - 1);
                if (x < mask.cols - 1 && y < mask.rows - 1)
                    COND_SUM(y + 1, x + 1);
                if (x > 0 && y < mask.rows - 1)
                    COND_SUM(y + 1, x - 1);
                if (x < mask.cols - 1 && y > 0)
                    COND_SUM(y - 1, x + 1);
#endif
                if (nb > 0)
                    dst.at<uchar>(y, x) = cvRound((float) sum / (float) nb);
            }

        }
    }
	namedWindow( "Display window in", WINDOW_AUTOSIZE );// Create a window for display.
	imshow( "Display window in", dst );    
}


//Source : http://scikit-image.org/docs/dev/auto_examples/plot_swirl.html#the-swirl-transformation

void swirl_init(Size img_size, Point center, float radius, float rotation, float strength, Mat& swirl_data) {
    Mat cartCoordsX(img_size.height, img_size.width, CV_32F),
            cartCoordsY(img_size.height, img_size.width, CV_32F);

    float r = M_LN2 * radius / 5.0;

#pragma omp parallel for collapse(2) firstprivate(r)
    for (int y = 0; y < img_size.height; y++) {
        for (int x = 0; x < img_size.width; x++) {
            float rho = sqrt(pow((double)(x - center.x), 2) + pow((double)(y - center.y), 2));
            float theta = rotation + strength * exp(-rho / radius) + atan2((double)(y - center.y), (double)(x - center.x));
            cartCoordsX.at<float>(y, x) = center.x + rho * cos(theta);
            cartCoordsY.at<float>(y, x) = center.y + rho * sin(theta);
        }
    }

    Mat tmp;
    convertMaps(cartCoordsX, cartCoordsY, swirl_data, tmp, CV_16SC2, true);
}

void swirl(Mat img, Mat& dst, Mat& swirl_data) {
    dst.create(img.size(), img.type());
    remap(img, dst, swirl_data, Mat(), INTER_NEAREST, BORDER_CONSTANT);
}