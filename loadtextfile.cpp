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

cv::Mat load_textfile2Mat(std::string filename){
	int count=1, erreur_line=1;
	cv::Mat data(125000, 3, CV_64F);
	std::ifstream file(filename, std::ios::in);
	std::string line, data_x, data_y, data_a;
	file >> data_x >> data_y >> data_a;
	data.at<double>(0,0) = atof(data_x.c_str());
	data.at<double>(0,1) = atof(data_y.c_str());
	data.at<double>(0,2) = atof(data_a.c_str());
	while ( getline( file, line ) ) 
	{ 

		file >> data_x >> data_y >> data_a;
		if (data_x=="0" && data_y=="0")
		{
			erreur_line = count;
			break;
		}
		data.at<double>(count,0) = atof(data_x.c_str());
		data.at<double>(count,1) = atof(data_y.c_str());
		data.at<double>(count,2) = atof(data_a.c_str());
		count++;
	}
	file.close();
	cv::Mat M(erreur_line, 3, CV_64F);
	data(cv::Range(0, erreur_line), cv::Range(0, 3)).copyTo(M);
	return M;
}