/**
 * \file      loadtextfile.h
 * \author    Michaud Dorian
 * \version   0.1
 * \date      16/12/2014
 * \brief     Header du fichier loadtextfile.cpp
 */
#ifndef LOADTEXTFILE_H
#define	LOADTEXTFILE_H

#include <iostream>
#include <string>
#include <fstream>
#include "opencv2/core/core.hpp"

cv::Mat load_textfile2Mat(std::string);

#endif	/* LOADTEXTFILE_H */