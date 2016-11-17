#ifndef UTILS_H
#define UTILS_H
#include <opencv2/opencv.hpp>

class utils
{
public:
    utils();

    void multiplyMat(const cv::Mat &A, const cv::Mat &B, cv::Mat &dst);
    void getMagnitude(const cv::Mat &src, cv::Mat &dst);
    void shiftMat(cv::Mat &dst);
    void normMat(cv::Mat &dst);
    void HFiltering(const cv::Mat &src, const cv::Mat &dstFFT, cv::Mat &mask, cv::Mat &dst);

    cv::Mat getIMask(const cv::Size &size, const double &D0, const bool &isLowpass);
    cv::Mat getBMask(const cv::Size &size, const double &D0, const int &n
                     , const bool &isLowpass);

    cv::Mat getGMask(const cv::Size &size, const double &D0, const bool &isLowpass);
    cv::Mat getHMask(const cv::Size &size, const double &gammaH
                     , const double &gammaL, const double &c, const double &D0);
    cv::Mat getOptimalImg(const cv::Mat &img);
};

#endif // UTILS_H
