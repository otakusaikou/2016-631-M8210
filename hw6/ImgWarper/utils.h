#ifndef UTILS_H
#define UTILS_H
#include <opencv2/opencv.hpp>

class utils
{
public:
    utils();

    enum pos {TOP, BOTTOM, LEFT, RIGHT};

    void getParam(const float &height, const float &width, const float &length, const float &shift, cv::Mat &param, const pos &edgePos);

    void resampleTrap(cv::Mat const &src, cv::Mat &dst, const cv::Mat &param);

    void resampleWavy(cv::Mat const &src, cv::Mat &dst, const float &ampl, const float &freq);

    void resampleCircular(cv::Mat const &src, cv::Mat &dst, const float &zoom, const float &strength);
};

#endif // UTILS_H
