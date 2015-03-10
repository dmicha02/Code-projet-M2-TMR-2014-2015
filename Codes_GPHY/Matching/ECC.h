#ifndef ECC_H
#define	ECC_H
#include "matcher.h"

class ECC : public matcher {
public:
    ECC(cv::Rect initialPosition, cv::InputArray img_fusion, Matching_Mode mode = SEQUENCE, int template_size = 250);
    ECC(const ECC& orig);
    virtual ~ECC();

    virtual bool match(cv::InputArray img);
    virtual void blend(cv::InputOutputArray img_fusion);

    cv::Mat transformation;
private:
    int template_size;
};

#endif	/* ECC_H */

