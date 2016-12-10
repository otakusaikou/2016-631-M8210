#ifndef UTILS_H
#define UTILS_H
#include <QDebug>
#include <vector>
#include <opencv2/opencv.hpp>

class utils
{
public:
    utils();

    enum CvtMode {RGB2CMY, RGB2HSI, RGB2XYZ, RGB2Lab, RGB2YUV};

    void convertMat(const cv::Mat &src, cv::Mat &dst, const CvtMode &mode);

    void getFCImg(const cv::Mat &src, cv::Mat &dst, const cv::Mat &cmap);

    void imgClustering(const cv::Mat &src, cv::Mat &dst, int &nclusters, bool &hasBlur, int &kSize);
};

#endif // UTILS_H
