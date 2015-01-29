/**
 * \file      loadtextfile.cpp
 * \author    Michaud Dorian
 * \version   0.1
 * \date      16/12/2014
 * \brief     Fichier de chargement des donnees
 *
 * \details    Ce fichier contient la fonction de chargement des donnees dans une matrice
 *                 pour ne pas manipuler un fichier.
 */
#include "loadtextfile.h"
using namespace std;
using namespace cv;

Mat load_textfile2Mat(string filename){
	int count=1, erreur_line=1;
	Mat data(125000, 3, CV_32F);
	ifstream file(filename, ios::in);
	string line, data_x, data_y, data_a;
	file >> data_x >> data_y >> data_a;
	data.at<float>(0,0) = atof(data_x.c_str());
	data.at<float>(0,1) = atof(data_y.c_str());
	data.at<float>(0,2) = atof(data_a.c_str());
	while ( getline( file, line ) ) 
	{ 

		file >> data_x >> data_y >> data_a;
		if (data_x=="0" && data_y=="0")
		{
			erreur_line = count;
			break;
		}
		data.at<float>(count,0) = atof(data_x.c_str());
		data.at<float>(count,1) = atof(data_y.c_str());
		data.at<float>(count,2) = atof(data_a.c_str());
		count++;
	}
	file.close();
	Mat M(erreur_line, 3, CV_32F);
	data(Range(0, erreur_line), Range(0, 3)).copyTo(M);
	return M;
}