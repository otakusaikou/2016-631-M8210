#ifndef UTILS_H
#define UTILS_H
#include <opencv2/opencv.hpp>

class utils
{
public:
    utils();

    enum CvtMode {RGB2CMY, RGB2HSI, RGB2XYZ, RGB2Lab, RGB2YUV};
    void convertMat(const cv::Mat &src, cv::Mat &dst, const CvtMode &mode);
};

#endif // UTILS_H
