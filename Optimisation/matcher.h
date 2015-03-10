#ifndef MATCHER_H
    #define	MATCHER_H

    #include <opencv2/opencv.hpp>
    //#include <opencv2/nonfree/nonfree.hpp>
    #include <vector>

    #define GRID_SIZE 4
//#define SHOW_REDONDANCE

typedef enum Matching_Mode {
    SEQUENCE,
    PANORAMA
} Matching_Mode;

typedef struct gridVariables {
    cv::Point min;
    cv::Point max;
    cv::Point current;
} gridVariables;

class matcher {
public:
    matcher(cv::Rect initialPosition, cv::InputArray img_fusion, Matching_Mode mode = SEQUENCE);
    matcher(const matcher& orig);
    virtual ~matcher();

    virtual bool match(cv::InputArray img) = 0;
    virtual void blend(cv::InputOutputArray img_fusion) = 0;
    void build_final(std::string final_filename);

    cv::Mat transformation;
protected:
    void update_redondance(cv::InputArray image, cv::Point position, cv::InputArray gradient = cv::noArray());
    void next_img();
    void doBlend(cv::InputOutputArray img_fusion, cv::InputArray gradient, cv::InputArray img);

    cv::Mat last_img, img, last_img_fusion, redondance;
    cv::Point2f position, last_position;
    Matching_Mode mode;

    cv::Mat gradient;
private:
    gridVariables grid;
    void merge_redondance(cv::OutputArray merged);
    void unmerge_redondance(cv::InputArray merged, cv::OutputArray red, cv::OutputArray fusion);
    void gridUI(cv::InputOutputArray img_fusion);
};

#endif	/* MATCHER_H */

