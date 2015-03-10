#ifndef TEMPLATEMATCHING_H
#define	TEMPLATEMATCHING_H
#include "matcher.h"

class TemplateMatching : public matcher {
public:
    TemplateMatching(cv::Rect initialPosition, cv::InputArray img_fusion, Matching_Mode mode = SEQUENCE, int template_size = 250);
    TemplateMatching(const TemplateMatching& orig);
    virtual ~TemplateMatching();

    virtual bool match(cv::InputArray img);
    virtual void blend(cv::InputOutputArray img_fusion);
private:
    int template_size;
};

#endif	/* TEMPLATEMATCHING_H */

