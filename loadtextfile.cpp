#include "loadtextfile.h"
using namespace std;
using namespace cv;

/**
 * \fn Mat load_textfile2Mat(string filename)
 * \brief load the text file of analogic data in a matrix
 * \param filename - the path of the file to open
 * \return Mat - the matrix of file
 */
Mat load_textfile2Mat(string filename)
{
	int count = 1, erreur_line = 1;
	Mat data(125000, 3, CV_32F);
	ifstream file(filename, ios::in, ios::binary);
	string line, data_x, data_y, data_a;
	file >> data_x >> data_y >> data_a;
	data.at<float>(0,0) = atof(data_x.c_str());
	data.at<float>(0,1) = atof(data_y.c_str());
	data.at<float>(0,2) = atof(data_a.c_str());
	while (getline(file, line))
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
	return data(Range(0, erreur_line), Range(0, 3));
}