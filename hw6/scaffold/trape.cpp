#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat img;
Mat dst;
int diff_slider_max;
int shift_slider_max;
int diff_slider;
int shift_slider;
double diff;
double shift;

void resample(Mat const &src, Mat &dst, const Mat &param) {
    double h = src.rows;
    double w = src.cols;

    float c1 = param.at<float>(0, 0);
    float c2 = param.at<float>(1, 0);
    float c3 = param.at<float>(2, 0);
    float c4 = param.at<float>(3, 0);
    float c5 = param.at<float>(4, 0);
    float c6 = param.at<float>(5, 0);
    float c7 = param.at<float>(6, 0);
    float c8 = param.at<float>(7, 0);

    for (int i = 0; i < h; ++i) {
        for (int j = 0;j < w; ++j) {
            int x = c1 * j + c2 * i + c3 * j * i + c4;
            int y = c5 * j + c6 * i + c7 * j * i + c8;
            if (x >= w || x < 0)
                dst.at<Vec3b>(i, j) = 0;
            else if (y >= h || y < 0)
                dst.at<Vec3b>(i, j) = 0;
            else
                dst.at<Vec3b>(i, j) = src.at<Vec3b>(y, x);
        }
    }
}

Mat getParam(const double &diff, const double &shift)
{
    float v[4] = {0, 0,
        static_cast<float>(img.cols - shift),
        static_cast<float>(img.cols - shift)};

    float w[4] = {0,
        static_cast<float>(img.rows),
        static_cast<float>(0 + ceil((img.rows-diff)/2)),
        static_cast<float>(img.rows - floor((img.rows-diff)/2))};

    float x[4] = {0, 0,
        static_cast<float>(img.cols),
        static_cast<float>(img.cols)};
    float y[4] = {0,
        static_cast<float>(img.rows), 0,
        static_cast<float>(img.rows)};

    Mat B = Mat::zeros(8, 8, CV_32F);
    Mat f = Mat::zeros(8, 1, CV_32F);
    for (int i = 0; i < 4; ++i)
    {
        B.at<float>(i, 0) = v[i];
        B.at<float>(i, 1) = w[i];
        B.at<float>(i, 2) = v[i] * w[i];
        B.at<float>(i, 3) = 1;
        B.at<float>(i + 4, 4) = v[i];
        B.at<float>(i + 4, 5) = w[i];
        B.at<float>(i + 4, 6) = v[i] * w[i];
        B.at<float>(i + 4, 7) = 1;
        f.at<float>(i, 0) = x[i];
        f.at<float>(i + 4, 0) = y[i];
    }
    Mat N = B.t() * B;
    Mat t = B.t() * f;
    Mat param = N.inv()*t;
    return param;
}

void on_diff_trackbar(int val, void *userdata)
{
    diff = val;
    std::cout << "UL[" << 0 << ", " << 0 << "] "
        << "LL[" << 0 << ", " << img.rows << "] "
        << "UR[" << img.cols - shift << ", " << 0 + ceil((img.rows-diff)/2) << "] "
        << "LR[" << img.cols - shift << ", " << img.rows - floor((img.rows-diff)/2) << "]"
        << std::endl;
    Mat param = getParam(diff, shift);

    dst = Mat::zeros(img.size(), CV_8UC3);
    resample(img, dst, param);

    resize(dst, dst, Size(500, 500));
    imshow("Trape transformation", dst);
}

void on_shift_trackbar(int val, void *userdata)
{
    shift = val;
    std::cout << "UL[" << 0 << ", " << 0 << "] "
        << "LL[" << 0 << ", " << img.rows << "] "
        << "UR[" << img.cols - shift << ", " << 0 + ceil((img.rows-diff)/2) << "] "
        << "LR[" << img.cols - shift << ", " << img.rows - floor((img.rows-diff)/2) << "]"
        << std::endl;

    Mat param = getParam(diff, shift);

    dst = Mat::zeros(img.size(), CV_8UC3);
    resample(img, dst, param);

    resize(dst, dst, Size(500, 500));
    imshow("Trape transformation", dst);
}

int main()
{
    /// Initialize values
    img = imread("../../hw5/img/HW05-Part 3-03.bmp");
    dst = Mat::zeros(img.size(), CV_8UC3);
    diff = img.rows;
    shift = 0;
    diff_slider = img.rows;
    shift_slider = 0;
    diff_slider_max = img.rows;
    shift_slider_max = img.cols;

//    _fisheye(img, dst, zoom, strength);
//    resize(dst, dst, Size(500, 500));

    /// Create Windows
    resize(dst, dst, Size(500, 500));
    namedWindow("Trape transformation", 1);
    imshow("Trape transformation", dst);

    /// Create Trackbars
    createTrackbar("Diff", "Trape transformation", &diff_slider, diff_slider_max, on_diff_trackbar);
    createTrackbar("Shift", "Trape transformation", &shift_slider, shift_slider_max, on_shift_trackbar);

    waitKey(0);
    return 0;
}
