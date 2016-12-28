//*********************************************************************************
// This program use source code of hough transformation from Bruno Keymolen's blog
// (https://goo.gl/OGBZp5)
//
// The following are other references that I referred to:
//   Rectangle detection: https://goo.gl/QaTn3M
//   Determine whether point is in Rectangle: https://goo.gl/xQ1rVb
//
//*********************************************************************************
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <map>
#include <iostream>

#include <opencv2/opencv.hpp>

#include "rectDetect.h"
#include "hough.h"

using namespace cv;
using namespace std;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

const char* CW_IMG_ORIGINAL = "Result";
const char* CW_IMG_EDGE	= "Canny Edge Detection";

void usage(char * s)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "%s -s <source file> [-t <threshold>] - hough transform. build: %s-%s \n", s, __DATE__, __TIME__);
    fprintf(stderr, "   s: path image file\n");
    fprintf(stderr, "   t: hough threshold\n");
    fprintf(stderr, "\nexample:  ./rectDetect -s ../img/rects.bmp -t 40\n");
    fprintf(stderr, "\n");
}

keymolen::Hough doTransform(const string &file_path, int &threshold)
{
    Mat img_edge;
    Mat img_blur;

    Mat img_ori = imread(file_path, 1);
    blur(img_ori, img_blur, Size(5,5));
    Canny(img_blur, img_edge, 100, 150, 3);

    int w = img_edge.cols;
    int h = img_edge.rows;

    //Transform
    keymolen::Hough hough;
    hough.Transform(img_edge.data, w, h);

    if(threshold == 0)
        threshold = w>h?w/4:h/4;

    while(1)
    {
        Mat img_res = img_ori.clone();

        //Search the accumulator
        vector< pair< pair<int, int>, pair<int, int> > > lines = hough.GetLines(threshold);

        //Draw the results
        vector< pair< pair<int, int>, pair<int, int> > >::iterator it;
        for(it = lines.begin(); it != lines.end(); it++)
        {
            line(img_res, Point(it->first.first, it->first.second), Point(it->second.first, it->second.second), Scalar(0, 0, 255), 2, 8);
        }

        //Visualize all
        int aw, ah, maxa;
        aw = ah = maxa = 0;
        const unsigned int* accu = hough.GetAccu(&aw, &ah);

        for(int p = 0; p < (ah*aw); p++)
        {
            if((int)accu[p] > maxa)
                maxa = accu[p];
        }
        double contrast = 1.0;
        double coef = 255.0 / (double)maxa * contrast;

        Mat img_accu(ah, aw, CV_8UC3);
        for(int p = 0; p < (ah*aw); p++)
        {
            unsigned char c = (double)accu[p] * coef < 255.0 ? (double)accu[p] * coef : 255.0;
            img_accu.data[(p*3)+0] = 255;
            img_accu.data[(p*3)+1] = 255-c;
            img_accu.data[(p*3)+2] = 255-c;
        }

        imshow(CW_IMG_EDGE, img_edge);
        imshow(CW_IMG_ORIGINAL, img_res);

        char c = waitKey(360000);
        if(c == '+')
            threshold += 5;
        if(c == '-')
            threshold -= 5;
        if(c == 27 || c == 'y')
            break;
    }
    return hough;
}

void doDetect(const string &file_path, vector<Point> vPSt, vector<Point> vPEd, vector<int> vT, vector<int> vR)
{
    Mat img_ori = imread(file_path, 1);

    namedWindow(CW_IMG_EDGE, WINDOW_AUTOSIZE);

    rectDetect rD;
    rD.detect(vPSt, vPEd, vT, vR);

    // Output the area and perimeter of the two rectangle
    auto rect1 = rD.vRect[0];
    auto rect2 = rD.vRect[1];
    auto inner = rD.inner;
    cout << rD.getArea(rect1) + rD.getArea(rect2) - rD.getArea(inner) << endl;
    cout << rD.getPeri(rect1) + rD.getPeri(rect2) - rD.getPeri(inner) << endl;

    rect1.push_back(rect1[0]);
    for(auto itPt = rect1.begin(); itPt != rect1.end() - 1; itPt++)
    {
        line(img_ori, (*itPt), *(itPt+1), Scalar(0, 0, 255), 2, 8);
    }

    rect2.push_back(rect2[0]);
    for(auto itPt = rect2.begin(); itPt != rect2.end() - 1; itPt++)
    {
        line(img_ori, (*itPt), *(itPt+1), Scalar(0, 255, 0), 2, 8);
    }

    inner.push_back(inner[0]);
    for(auto itPt = inner.begin(); itPt != inner.end() - 1; itPt++)
    {
        line(img_ori, (*itPt), *(itPt+1), Scalar(255, 0, 0), 2, 8);
    }

    imshow(CW_IMG_ORIGINAL, img_ori);
    waitKey(0);
}

int main(int argc, char *argv[])
{
    string img_path;
    int threshold = 0;

    int c;
    while (((c = getopt(argc, argv, "s:t:?"))) != -1)
    {
        switch (c)
        {
            case 's':
                img_path = optarg;
                break;
            case 't':
                threshold = atoi(optarg);
                break;
            case '?':
            default:
                usage(argv[0]);
                return -1;
        }
    }

    if(img_path.empty())
    {
        usage(argv[0]);
        return -1;
    }

    namedWindow(CW_IMG_EDGE, WINDOW_AUTOSIZE);
    namedWindow(CW_IMG_ORIGINAL, WINDOW_AUTOSIZE);

    auto hough = doTransform(img_path, threshold);

    vector<int> vT;
    vector<int> vR;

    for (auto &e : hough.vTR)
    {
        vT.push_back(e.first);
        vR.push_back(e.second);
    }

    auto lines = hough.GetLines(threshold);
    vector<Point> vPSt;
    vector<Point> vPEd;

    for(auto it = lines.begin(); it != lines.end(); it++)
    {
        vPSt.push_back(Point(it->first.first, it->first.second));
        vPEd.push_back(Point(it->second.first, it->second.second));
    }

    doDetect(img_path, vPSt, vPEd, vT, vR);

    return 0;
}
