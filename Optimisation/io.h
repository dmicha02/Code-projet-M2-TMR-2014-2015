#ifndef IO_H
    #define	IO_H
    #include <string>
    #include <opencv2/core.hpp>

bool writeMatToFile(cv::InputArray in, std::string path);
bool readFileToMat(cv::OutputArray out, std::string path);

#endif	/* IO_H */

