//*********************************************************************************
// This program use source code of hough transformation from Bruno Keymolen's blog
// (https://goo.gl/OGBZp5)
//
// The following are other references that I referred to:
//   Rectangle detection: https://goo.gl/QaTn3M
//   Determine whether point is in Rectangle: https://goo.gl/xQ1rVb
//
//*********************************************************************************
#include <iostream>
#include <vector>
#include <iterator>
#include <cmath>

#include <opencv2/opencv.hpp>

#include "rectDetect.h"

#define M_PI 3.14159265358979323846  /* pi */

using namespace cv;
using namespace std;

rectDetect::rectDetect()
{
}

void rectDetect::detect(vector<Point> vPSt, vector<Point> vPEd, vector<int> vT, vector<int> vR)
{
    // Find parallel line pairs (one by one)
    vector< vector<int> > vPara;
    findParallel(vT, vPara);

    // Find perpendicular line pairs (two by two)
    vector< vector<int> > vPerp;
    findPerpendicular(vT, vPara, vPerp);

    // Find rectangles with intersection of perpendicular line pairs
    for (auto itPerp = vPerp.begin(); itPerp != vPerp.end(); ++itPerp)
    {
        auto paraPair1 = vPara[(*itPerp)[0]];
        auto paraPair2 = vPara[(*itPerp)[1]];
        vector<Point2f> rect;
        for (auto itIdx1 = paraPair1.begin();  itIdx1 != paraPair1.end(); ++ itIdx1)
        {
            for (auto itIdx2 = paraPair2.begin();  itIdx2 != paraPair2.end(); ++ itIdx2)
            {
                Point2f pt = getIntersect(vPSt[*itIdx1], vPEd[*itIdx1], vPSt[*itIdx2], vPEd[*itIdx2]);
                rect.push_back(pt);
            }
        }
        vRect.push_back(rect);
    }

    // Sort the each rectangle point by clockwise order
    for (auto itRect = vRect.begin(); itRect != vRect.end(); ++itRect)
    {
        sortPolygon(*itRect);
    }

    // Find intersection point of two rectangles
    auto &rect1 = vRect[0];
    auto &rect2 = vRect[1];

    for (auto &pt : rect1)
        if (isInRect(rect2, pt))
            inner.push_back(pt);

    for (auto &pt : rect2)
        if (isInRect(rect1, pt))
            inner.push_back(pt);

    for (size_t i = 0; i < rect1.size(); ++i)
    {
        for (size_t j = 0; j < rect2.size(); ++j)
        {
            int idxSt1 = i;
            int idxEd1 = (i + 1) % rect1.size();
            int idxSt2 = j;
            int idxEd2 = (j + 1) % rect2.size();
            Point2f pt = getIntersect(rect1[idxSt1], rect1[idxEd1], rect2[idxSt2], rect2[idxEd2]);
            if (pt.x >= 0 && pt.y >= 0)
                inner.push_back(pt);
        }
    }
    sortPolygon(inner);
}

void rectDetect::findParallel(const vector<int> &vT, vector< vector<int> > &vPara)
{
    for (auto it1 = vT.begin(); it1 != vT.end(); ++it1)
    {
        for (auto it2 = it1 + 1; it2 != vT.end(); ++it2)
        {
            if (abs(*it1 - *it2) <= 1)
            {
                int idx1 = it1 - vT.begin();
                int idx2 = it2 - vT.begin();
                vPara.push_back(vector<int>({idx1, idx2}));
            }
        }
    }
}

void rectDetect::findPerpendicular(
    const vector<int> &vT,
    const vector< vector<int> > &vPara,
    vector< vector<int> > &vPerp)
{
    for (auto it1 = vPara.begin(); it1 != vPara.end(); ++it1)
    {
        for (auto it2 = it1 + 1; it2 != vPara.end(); ++it2)
        {
            float t1 = (vT[(*it1)[0]] + vT[(*it1)[1]]) / 2;
            float t2 = (vT[(*it2)[0]] + vT[(*it2)[1]]) / 2;
            if (abs(t1 + t2 - 180) < 10 || abs(abs(t1 - t2) - 90) < 10) {
                int idx1 = it1 - vPara.begin();
                int idx2 = it2 - vPara.begin();
                vPerp.push_back(vector<int>({idx1, idx2}));
            }
        }
    }
}

Point2f rectDetect::getIntersect(const Point &pSt1, const Point &pEd1, const Point &pSt2, const Point &pEd2)
{
    float m1 = (1.0 * (pEd1.y - pSt1.y) / (pEd1.x - pSt1.x));
    float b1 = pSt1.y - m1 * pSt1.x;

    float m2 = (1.0 * (pEd2.y - pSt2.y) / (pEd2.x - pSt2.x));
    float b2 = pSt2.y - m2 * pSt2.x;

    float x = 1.0 * (b2 - b1) / (m1 - m2);
    float y = m1 * x + b1;

    bool withInEdge1 =
        (x >= min(pSt1.x, pEd1.x)) && (x <= max(pSt1.x, pEd1.x)) &&
        (y >= min(pSt1.y, pEd1.y)) && (y <= max(pSt1.y, pEd1.y));

    bool withInEdge2 =
        (x >= min(pSt2.x, pEd2.x)) && (x <= max(pSt2.x, pEd2.x)) &&
        (y >= min(pSt2.y, pEd2.y)) && (y <= max(pSt2.y, pEd2.y));

    return (withInEdge1 && withInEdge2) ? Point2f(x, y) : Point2f(-1, -1);
}

void rectDetect::sortPolygon(vector<Point2f> &rect)
{
    // Compute the center coordinates of rectangle
    float yc = 0;
    float xc = 0;
    for (auto itPt = rect.begin(); itPt != rect.end(); ++itPt)
    {
        yc += itPt->y;
        xc += itPt->x;
    }
    yc /= rect.size();
    xc /= rect.size();

    // Compute the azimuth from the center to each corner
    vector<float> vT;
    for (auto itPt = rect.begin(); itPt != rect.end(); ++itPt)
    {
        float t = atan2(itPt->y - yc, itPt->x - xc);
        if (t < 0)
            t += 2 * M_PI;
        vT.push_back(t);
    }

    // Sort corners with azimuth
    struct ptAng
    {
        Point2f pt;
        float ang;
    };

    struct byAng {
        bool operator()(ptAng const &a, ptAng const &b) {
            return a.ang < b.ang;
        }
    };

    vector<ptAng> vPtAng;
    for (size_t i = 0; i < rect.size(); ++i)
    {
        ptAng p;
        p.pt = rect[i];
        p.ang = vT[i];
        vPtAng.push_back(p);
    }

    sort(vPtAng.begin(), vPtAng.end(), byAng());
    auto itPtAng = vPtAng.begin();
    for (auto &pt : rect)
    {
        pt = itPtAng->pt;
        itPtAng++;
    }
}

float rectDetect::getArea(const vector<Point2f> &rect)
{
    float area = 0;
    for (size_t i = 0; i < rect.size(); ++i)
    {
        int idxSt = i;
        int idxEd = (i+1) % rect.size();
        area +=
            (rect[idxSt].x*rect[idxEd].y) - (rect[idxSt].y*rect[idxEd].x);
    }
    area = 0.5 * abs(area);
    return area;
}

bool rectDetect::isInRect(const vector<Point2f> &rect, const Point2f &pt)
{
    float areaRect = getArea(rect);

    float areaSum = 0;
    for (size_t i = 0; i < rect.size(); ++i)
    {
        int idxSt = i;
        int idxEd = (i+1) % rect.size();

        areaSum +=
            0.5 * abs((rect[idxSt].x*rect[idxEd].y) - (rect[idxSt].y*rect[idxEd].x) +
            (rect[idxEd].x*pt.y) - (rect[idxEd].y*pt.x) +
            (pt.x*rect[idxSt].y) - (pt.y*rect[idxSt].x));
    }


    return (areaRect == areaSum);
}

float rectDetect::getPeri(const vector<Point2f> &rect)
{
    float peri = 0;
    for (size_t i = 0; i < rect.size(); ++i)
    {
        int idxSt = i;
        int idxEd = (i+1) % rect.size();
        peri +=
            sqrt(pow(rect[idxEd].x-rect[idxSt].x, 2) + pow(rect[idxEd].y-rect[idxSt].y, 2));
    }
    return peri;
}
