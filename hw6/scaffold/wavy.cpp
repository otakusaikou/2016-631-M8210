#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat img;
Mat dst;
int ampl_slider_max;
int freq_slider_max;
int ampl_slider;
int freq_slider;
double ampl;
double freq;

void resample(Mat const &src, Mat &dst, const double &ampl, const double &freq) {
    double h = src.rows;
    double w = src.cols;

    for (int i = 0; i < h; ++i) {
        for (int j = 0;j < w; ++j) {
            int x = j - ampl * sin(freq * i);
            int y = i - ampl * sin(freq * j);
            if (x >= w || x < 0)
                dst.at<Vec3b>(i, j) = 0;
            else if (y >= h || y < 0)
                dst.at<Vec3b>(i, j) = 0;
            else
                dst.at<Vec3b>(i, j) = src.at<Vec3b>(y, x);
        }
    }
}


void on_ampl_trackbar(int val, void *userdata)
{
    ampl = val;
    std::cout << ampl << " " << freq << std::endl;

    dst = Mat::zeros(img.size(), CV_8UC3);
    resample(img, dst, ampl, freq);

    resize(dst, dst, Size(500, 500));
    imshow("Trape transformation", dst);
}

void on_freq_trackbar(int val, void *userdata)
{
    freq = 0.1 * static_cast<double>(val) / freq_slider_max;
    std::cout << ampl << " " << freq << std::endl;

    dst = Mat::zeros(img.size(), CV_8UC3);
    resample(img, dst, ampl, freq);

    resize(dst, dst, Size(500, 500));
    imshow("Trape transformation", dst);
}

int main()
{
    /// Initialize values
    img = imread("../img/IP_dog.bmp");
    dst = Mat::zeros(img.size(), CV_8UC3);
    ampl = 1;
    freq = 1;
    ampl_slider = 0;
    freq_slider = 0;
    ampl_slider_max = 100;
    freq_slider_max = 100;

    /// Create Windows
    resize(dst, dst, Size(500, 500));
    namedWindow("Trape transformation", 1);
    imshow("Trape transformation", dst);

    /// Create Trackbars
    createTrackbar("Amplitude", "Trape transformation", &ampl_slider, ampl_slider_max, on_ampl_trackbar);
    createTrackbar("Frequency", "Trape transformation", &freq_slider, freq_slider_max, on_freq_trackbar);

    waitKey(0);
    return 0;
}
