#ifndef ESTIMATERIGIDT_H
#define	ESTIMATERIGIDT_H
#include "matcher.h"

class EstimateRigidT : public matcher {
public:
    EstimateRigidT(cv::Rect initialPosition, cv::InputArray img_fusion, Matching_Mode mode = SEQUENCE);
    EstimateRigidT(const EstimateRigidT& orig);
    virtual ~EstimateRigidT();

    virtual bool match(cv::InputArray img);
    virtual void blend(cv::InputOutputArray img_fusion);

    cv::Mat transformation;
private:

};

#endif	/* ESTIMATERIGIDT_H */

