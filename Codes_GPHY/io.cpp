#include <fstream>
#include <iostream>
#include "io.h"

using namespace std;
using namespace cv;

bool writeMatToFile(InputArray in, string path) {

    Mat I = in.getMat();
    //load the matrix size
    int matWidth = I.size().width, matHeight = I.size().height;

    //read type from Mat
    int type = I.type();

    //declare values to be written
    float fvalue;
    double dvalue;
    Vec3f vfvalue;
    Vec3d vdvalue;

    //create the file stream
    ofstream file(path.c_str(), ios::out | ios::binary);
    if (!file)
        return false;

    //write type and size of the matrix first
    file.write((const char*) &type, sizeof (type));
    file.write((const char*) &matWidth, sizeof (matWidth));
    file.write((const char*) &matHeight, sizeof (matHeight));

    //write data depending on the image's type
    switch (type) {
        default:
            cerr << "Error: wrong Mat type: must be CV_32F, CV_64F, CV_32FC3 or CV_64FC3" << endl;
            break;
            // FLOAT ONE CHANNEL
        case CV_32F:
            //cout << "Writing CV_32F image" << endl;
            for (int i = 0; i < matWidth * matHeight; ++i) {
                fvalue = I.at<float>(i);
                file.write((const char*) &fvalue, sizeof (fvalue));
            }
            break;
            // DOUBLE ONE CHANNEL
        case CV_64F:
            //cout << "Writing CV_64F image" << endl;
            for (int i = 0; i < matWidth * matHeight; ++i) {
                dvalue = I.at<double>(i);
                file.write((const char*) &dvalue, sizeof (dvalue));
            }
            break;

            // FLOAT THREE CHANNELS
        case CV_32FC3:
            //cout << "Writing CV_32FC3 image" << endl;
            for (int i = 0; i < matWidth * matHeight; ++i) {
                vfvalue = I.at<Vec3f>(i);
                file.write((const char*) &vfvalue, sizeof (vfvalue));
            }
            break;

            // DOUBLE THREE CHANNELS
        case CV_64FC3:
            //cout << "Writing CV_64FC3 image" << endl;
            for (int i = 0; i < matWidth * matHeight; ++i) {
                vdvalue = I.at<Vec3d>(i);
                file.write((const char*) &vdvalue, sizeof (vdvalue));
            }
            break;

    }

    //close file
    file.close();

    return true;
}

bool readFileToMat(OutputArray out, string path) {

    //declare image parameters
    int matWidth, matHeight, type;

    //declare values to be written
    float fvalue;
    double dvalue;
    Vec3f vfvalue;
    Vec3d vdvalue;

    //create the file stream
    ifstream file(path.c_str(), ios::in | ios::binary);
    if (!file)
        return false;

    //read type and size of the matrix first
    file.read((char*) &type, sizeof (type));
    file.read((char*) &matWidth, sizeof (matWidth));
    file.read((char*) &matHeight, sizeof (matHeight));

    //change Mat type
    Mat I = out.getMat();
    I = Mat::zeros(matHeight, matWidth, type);

    //write data depending on the image's type
    switch (type) {
        default:
            cerr << "Error: wrong Mat type: must be CV_32F, CV_64F, CV_32FC3 or CV_64FC3" << endl;
            break;
            // FLOAT ONE CHANNEL
        case CV_32F:
            //cout << "Reading CV_32F image" << endl;
            for (int i = 0; i < matWidth * matHeight; ++i) {
                file.read((char*) &fvalue, sizeof (fvalue));
                I.at<float>(i) = fvalue;
            }
            break;
            // DOUBLE ONE CHANNEL
        case CV_64F:
            //cout << "Reading CV_64F image" << endl;
            for (int i = 0; i < matWidth * matHeight; ++i) {
                file.read((char*) &dvalue, sizeof (dvalue));
                I.at<double>(i) = dvalue;
            }
            break;

            // FLOAT THREE CHANNELS
        case CV_32FC3:
            //cout << "Reading CV_32FC3 image" << endl;
            for (int i = 0; i < matWidth * matHeight; ++i) {
                file.read((char*) &vfvalue, sizeof (vfvalue));
                I.at<Vec3f>(i) = vfvalue;
            }
            break;

            // DOUBLE THREE CHANNELS
        case CV_64FC3:
            //cout << "Reading CV_64FC3 image" << endl;
            for (int i = 0; i < matWidth * matHeight; ++i) {
                file.read((char*) &vdvalue, sizeof (vdvalue));
                I.at<Vec3d>(i) = vdvalue;
            }
            break;

    }

    //close file
    file.close();
    out.assign(I);
    return true;
}
