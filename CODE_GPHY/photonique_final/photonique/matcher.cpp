#include "optimization.h"
#include "matcher.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <timer.h>
#include "io.h"

using namespace std;
using namespace cv;

matcher::matcher(Rect initialPosition, InputArray img_fusion, Matching_Mode mode) {
    last_img_fusion = img_fusion.getMat();
    last_position = initialPosition.tl();
    last_img = last_img_fusion(initialPosition);

    //Premier canal : somme
    //Deuxième canal : nombre de valeurs
    redondance.create(last_img_fusion.size(), CV_32FC2);
    update_redondance(last_img, initialPosition.tl());

    this->mode = mode;

    gradient = imread("gradient.png", IMREAD_GRAYSCALE);
    resize(gradient, gradient, initialPosition.size());
    gradient.convertTo(gradient, CV_32F, 1.0 / 255.0);

    grid.min = grid.current = Point(0, 0);
    grid.max = Point(GRID_SIZE - 1, GRID_SIZE - 1);
}

matcher::~matcher() {
}

void matcher::update_redondance(InputArray _img, Point position, InputArray _gradient) {
    Mat gradient = _gradient.getMat(), img = _img.getMat();
#pragma omp parallel for collapse(2) schedule(dynamic, 50)
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            if (img.at<uchar>(y, x) > 0) {
                if (gradient.data && redondance.at<Vec2f>(y + position.y, x + position.x)[1] > 0) {
                    redondance.at<Vec2f>(y + position.y, x + position.x)[0] += (1.0 - gradient.at<float>(y, x)) * img.at<uchar>(y, x);
                } else {
                    redondance.at<Vec2f>(y + position.y, x + position.x)[0] += img.at<uchar>(y, x);
                }
                redondance.at<Vec2f>(y + position.y, x + position.x)[1] += 1;
            }
        }
    }
#ifdef SHOW_REDONDANCE
    namedWindow("Redondance Somme", WINDOW_AUTOSIZE);
    namedWindow("Redondance Nombre", WINDOW_AUTOSIZE);
    Mat somme, nombre;
    vector<Mat> channels(2);
    split(redondance, channels);
    normalize(channels[0], somme, 0, 255, NORM_MINMAX, CV_8U);
    normalize(channels[1], nombre, 0, 255, NORM_MINMAX, CV_8U);
    imshow("Redondance Somme", somme);
    imshow("Redondance Nombre", nombre);
    imwrite("red_somme.png", somme);
    imwrite("red_nombre.png", nombre);
    //waitKey();
#endif
}

void matcher::merge_redondance(OutputArray merged) {
    Mat channels[3];
    split(redondance, channels);
    last_img_fusion.convertTo(channels[2], CV_32F, 1, 0);
    merge(channels, 3, merged);
}

void matcher::unmerge_redondance(InputArray merged, OutputArray red, OutputArray fusion) {
    Mat m = merged.getMat();
    Mat channels[3];
    split(m, channels);
    channels[2].convertTo(fusion, CV_8U, 1, 0);
    merge(channels, 2, red);
}

void matcher::next_img() {
    last_position = position;
    last_img = img;
}

void matcher::doBlend(InputOutputArray _img_fusion, InputArray _gradient, InputArray _img) {
    Mat img = _img.getMat();
    Mat img_fusion = _img_fusion.getMat();

    Point tl;
    Point br;
    tl.x = min<int>(cvRound(position.x), 0);
    tl.y = min<int>(cvRound(position.y), 0);
    br.x = max<int>(cvRound(position.x) + img.cols, img_fusion.cols);
    br.y = max<int>(cvRound(position.y) + img.rows, img_fusion.rows);
    Rect taille(tl, br);

    if (unlikely(taille.size() != img_fusion.size())) {
        timer__("Grid shift")
        gridUI(img_fusion);
        _img_fusion.assign(img_fusion);
    }

    Rect selection = Rect(position, img.size());
    Mat dst = img_fusion(selection);

    update_redondance(img, selection.tl(), _gradient);


#pragma omp parallel for collapse(2) schedule(dynamic, 50)
    for (int y = 0; y < dst.rows; y++) {
        for (int x = 0; x < dst.cols; x++) {
            Vec2f red_pix = redondance.at<Vec2f>(position.y + y, position.x + x);
            if (red_pix[1] > 0)
                dst.at<uchar>(y, x) = red_pix[0] / red_pix[1];
        }
    }

    last_img_fusion = img_fusion;
}

string getFilename(Point p) {
    stringstream filename;
    filename << p.x << " " << p.y << ".dat";
    return filename.str();
}

#define XCELL(n) (img_fusion.cols / GRID_SIZE)*(n)
#define YCELL(n) (img_fusion.rows / GRID_SIZE)*(n)
#define CELL(x,y) Rect(XCELL((x)), YCELL((y)), XCELL(1), YCELL(1))
#define SAVE(x,y) writeMatToFile(merged(CELL((x),(y))), getFilename(grid.current + Point((x), (y))))
#define LOAD(xSrc,ySrc,xDst,yDst) readFileToMat(merged_next(CELL((xDst),(yDst))), getFilename(grid.current + Point((xSrc), (ySrc))))

void matcher::gridUI(InputOutputArray _img_fusion) {
    Rect img_rect(position, img.size());
    Rect invar, paste_invar;

    Mat img_fusion = _img_fusion.getMat();

    Point ori_invar;

    Mat merged, merged_next = Mat::zeros(img_fusion.size(), CV_32FC3);
    merge_redondance(merged);

    if (img_rect.tl().x < 0 && img_rect.tl().y < 0) { //Si on décale en haut à gauche
        /*
                ##X
                ##X
                XXX
         */
        invar.x = 0;
        invar.y = 0;
        invar.height = YCELL(GRID_SIZE - 1);
        invar.width = XCELL(GRID_SIZE - 1);

        ori_invar = Point(XCELL(1), YCELL(1));

#pragma omp parallel for collapse(2) firstprivate(merged)
        for (int y = 0; y < GRID_SIZE; y++)
            for (int x = 0; x < GRID_SIZE; x++) {
                if (x == GRID_SIZE - 1 || y == GRID_SIZE - 1)
                    SAVE(x, y);
                if (x == 0 || y == 0)
                    LOAD(x - 1, y - 1, x, y);
            }

        grid.current.x--;
        grid.current.y--;

    } else if (img_rect.br().x > img_fusion.cols && img_rect.tl().y < 0) { //Si on décale en haut à droite
        /*
                X##
                X##
                XXX
         */
        invar.x = XCELL(1);
        invar.y = 0;
        invar.height = YCELL(GRID_SIZE - 1);
        invar.width = XCELL(GRID_SIZE - 1);

        ori_invar = Point(XCELL(0), YCELL(1));

#pragma omp parallel for collapse(2) firstprivate(merged)
        for (int y = 0; y < GRID_SIZE; y++)
            for (int x = 0; x < GRID_SIZE; x++) {
                if (x == 0 || y == GRID_SIZE - 1)
                    SAVE(x, y);
                if (y == 0 || x == GRID_SIZE - 1)
                    LOAD(x + 1, y - 1, x, y);
            }

        grid.current.x++;
        grid.current.y--;

    } else if (img_rect.br().x > img_fusion.cols && img_rect.br().y > img_fusion.rows) { //Si on décale en bas à droite
        /*
                XXX
                X##
                X##
         */
        invar.x = XCELL(1);
        invar.y = YCELL(1);
        invar.height = YCELL(GRID_SIZE - 1);
        invar.width = XCELL(GRID_SIZE - 1);

        ori_invar = Point(XCELL(0), YCELL(0));

#pragma omp parallel for collapse(2) firstprivate(merged)
        for (int y = 0; y < GRID_SIZE; y++)
            for (int x = 0; x < GRID_SIZE; x++) {
                if (x == 0 || y == 0)
                    SAVE(x, y);
                if (x == GRID_SIZE - 1 || y == GRID_SIZE - 1)
                    LOAD(x + 1, y + 1, x, y);
            }

        grid.current.x++;
        grid.current.y++;

    } else if (img_rect.tl().x < 0 && img_rect.br().y > img_fusion.rows) { //Si on décale en bas à gauche
        /*
                XXX
                ##X
                ##X
         */
        invar.x = XCELL(GRID_SIZE - 1);
        invar.y = YCELL(GRID_SIZE - 1);
        invar.height = YCELL(GRID_SIZE - 1);
        invar.width = XCELL(GRID_SIZE - 1);

        ori_invar = Point(XCELL(1), YCELL(0));

#pragma omp parallel for collapse(2) firstprivate(merged)
        for (int y = 0; y < GRID_SIZE; y++)
            for (int x = 0; x < GRID_SIZE; x++) {
                if (y == 0 || x == GRID_SIZE - 1)
                    SAVE(x, y);
                if (x == 0 || y == GRID_SIZE - 1)
                    LOAD(x - 1, y + 1, x, y);
            }

        grid.current.x--;
        grid.current.y++;

    } else if (img_rect.br().x > img_fusion.cols) { //dépassement à droite sans coin
        /*
                X##
                X##
                X##
         */
        invar.x = XCELL(1);
        invar.y = 0;
        invar.height = YCELL(GRID_SIZE);
        invar.width = XCELL(GRID_SIZE - 1);

        ori_invar = Point(XCELL(0), YCELL(0));

#pragma omp parallel for firstprivate(merged)
        for (int i = 0; i < GRID_SIZE; i++) {
            SAVE(0, i);
            LOAD(GRID_SIZE, i, GRID_SIZE - 1, i);
        }

        grid.current.x++;

    } else if (img_rect.br().y > img_fusion.rows) { // dépassement en bas sans coin
        /*
                XXX
                ###
                ###
         */
        invar.x = 0;
        invar.y = YCELL(1);
        invar.height = YCELL(GRID_SIZE - 1);
        invar.width = XCELL(GRID_SIZE);

        ori_invar = Point(XCELL(0), YCELL(0));

#pragma omp parallel for firstprivate(merged)
        for (int i = 0; i < GRID_SIZE; i++) {
            SAVE(i, 0);
            LOAD(i, GRID_SIZE, i, GRID_SIZE - 1);
        }

        grid.current.y++;

    } else if (img_rect.tl().x < 0) { // dépassement à gauche sans coin
        /*
                ##X
                ##X
                ##X
         */
        invar.x = 0;
        invar.y = 0;
        invar.height = YCELL(GRID_SIZE);
        invar.width = XCELL(GRID_SIZE - 1);

        ori_invar = Point(XCELL(1), YCELL(0));

#pragma omp parallel for firstprivate(merged)
        for (int i = 0; i < GRID_SIZE; i++) {
            SAVE(GRID_SIZE - 1, i);
            LOAD(-1, i, 0, i);
        }

        grid.current.x--;

    } else if (img_rect.tl().y < 0) { // dépassement en haut sans coin
        /*
                ###
                ###
                XXX
         */
        invar.x = 0;
        invar.y = 0;
        invar.height = YCELL(GRID_SIZE - 1);
        invar.width = XCELL(GRID_SIZE);

        ori_invar = Point(XCELL(0), YCELL(1));
#pragma omp parallel for firstprivate(merged)
        for (int i = 0; i < GRID_SIZE; i++) {
            SAVE(i, GRID_SIZE - 1);
            LOAD(i, -1, i, 0);
        }

        grid.current.y--;

    }
    Mat img_fusion_next, redondance_next;
    unmerge_redondance(merged_next, redondance_next, img_fusion_next);
    //Mat img_fusion_next = Mat::zeros(img_fusion.size(), CV_8U);
    Rect selection(ori_invar.x, ori_invar.y, invar.width, invar.height);
    cout << img_fusion_next.channels() << img_fusion.channels() << endl;
    img_fusion(invar).copyTo(img_fusion_next(selection));
    img_fusion = img_fusion_next;
    _img_fusion.assign(img_fusion);

    //Mat redondance_next = Mat::zeros(redondance.size(), CV_32FC2);
    redondance(invar).copyTo(redondance_next(selection));
    redondance = redondance_next;

    position = (Point2f) ori_invar - (Point2f) invar.tl() + position;
    last_position = (Point2f) ori_invar - (Point2f) invar.tl() + last_position;

    grid.min.x = MIN(grid.min.x, grid.current.x);
    grid.min.y = MIN(grid.min.y, grid.current.y);
    grid.max.x = MAX(grid.max.x, grid.current.x);
    grid.max.y = MAX(grid.max.y, grid.current.y);
}

void matcher::build_final(string final_filename) {
    Mat img_fusion = last_img_fusion;
    int sizeX = MAX(grid.max.x - grid.min.x, 3);
    int sizeY = MAX(grid.max.y - grid.min.y, 3);
    Mat final_merged = Mat::zeros(YCELL(sizeY + 1), XCELL(sizeX + 1), CV_32FC3);
    Rect selection(0, 0, XCELL(1), YCELL(1));

    for (int y = grid.min.y; y <= grid.max.y; y++)
        for (int x = grid.min.x; x <= grid.max.x; x++) {
            string filename = getFilename(Point(x, y));
            selection.x = XCELL(x - grid.min.x);
            selection.y = YCELL(y - grid.min.y);
            readFileToMat(final_merged(selection), filename);
            remove(filename.c_str());
        }
    Mat final_red, final_fusion;
    unmerge_redondance(final_merged, final_red, final_fusion);

    selection.x = XCELL(grid.current.x - grid.min.x);
    selection.y = YCELL(grid.current.y - grid.min.y);
    selection.width = img_fusion.cols;
    selection.height = img_fusion.rows;
    img_fusion.copyTo(final_fusion(selection));

    //TODO : Autocrop ?

    imwrite(final_filename, final_fusion);
}