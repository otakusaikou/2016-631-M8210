#ifndef RECTDETECT_H
#define RECTDETECT_H

#include <vector>
#include <opencv2/opencv.hpp>

class rectDetect
{
    public:
        rectDetect();

        std::vector<cv::Point2f> inner;

        std::vector< std::vector<cv::Point2f> > vRect;

        void detect(std::vector<cv::Point> vPSt,
            std::vector<cv::Point> vPEd,
            std::vector<int> vT,
            std::vector<int> vR);

        float getArea(const std::vector<cv::Point2f> &rect);

        float getPeri(const std::vector<cv::Point2f> &rect);

    private:
        void findParallel(const std::vector<int> &vT, std::vector< std::vector<int> > &vPara);

        void findPerpendicular(const std::vector<int> &vT, const std::vector< std::vector<int> > &vPara, std::vector< std::vector<int> > &vPerp);

        cv::Point2f getIntersect(const cv::Point &pSt1, const cv::Point &pEd1, const cv::Point &pSt2, const cv::Point &pEd2);

        void sortPolygon(std::vector<cv::Point2f> &rect);

        bool isInRect(const std::vector<cv::Point2f> &rect, const cv::Point2f &pt);
};
#endif /* ifndef RECTDETECT_H */
