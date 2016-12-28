#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat img;
Mat dst;
const int zoom_slider_max = 100;
const int strength_slider_max = 100;
int zoom_slider;
int strength_slider;
double zoom;
double strength;

void _fisheye(Mat const &src, Mat &dst, const double &zoom, const double &strength) {
    double h = src.rows;
    double w = src.cols;

    for (int i = 0; i < h; ++i) {
        for (int j = 0;j < w; ++j) {
            // Normalize and translate the (x, y) coordinates to -1~1
            double ny = ((2 * i) / h) - 1;
            double nx = ((2 * j) / w) - 1;

            // Compute distance from image center
            double r = sqrt(nx * nx + ny * ny) / strength;

            // Discard pixels outside from circle
            if (0.0 <= r && r <= 1.0) {
                // Compute the new distance
                double nr = (r/zoom + (1.0 - sqrt(1.0 - r * r))) / 2.0;

                // Discard radius greater than 1.0
                if (nr <= 1.0) {
                    // Angle for polar coordinate system
                    double theta = atan2(ny,nx);

                    // New (x, y) coordinates with new distance in same angle
                    int x2 = static_cast<int>(((nr * cos(theta) + 1) * w) / 2.0);
                    int y2 = static_cast<int>(((nr * sin(theta) + 1) * h) / 2.0);
                    if (x2 >= w || x2 < 0)
                        dst.at<Vec3b>(i, j) = 0;
                    else if (y2 >= h || y2 < 0)
                        dst.at<Vec3b>(i, j) = 0;
                    else
                        dst.at<Vec3b>(i, j) = src.at<Vec3b>(y2, x2);
                }
            }
        }
    }
}

void on_zoom_trackbar(int val, void *userdata)
{
    zoom = 0.5 + (static_cast<double>(val)/zoom_slider_max);
    dst = Mat::zeros(img.size(), CV_8UC3);
    _fisheye(img, dst, zoom, strength);
    resize(dst, dst, Size(500, 500));
    imshow("Circulat transformation", dst);
}

void on_strength_trackbar(int val, void *userdata)
{
    strength = 2*(static_cast<double>(val)/strength_slider_max);
    dst = Mat::zeros(img.size(), CV_8UC3);
    _fisheye(img, dst, zoom, strength);
    resize(dst, dst, Size(500, 500));
    imshow("Circulat transformation", dst);
}

int main()
{
    /// Initialize values
    zoom = 1;
    strength = 1;
    zoom_slider = 50;
    strength_slider = 50;
    img = imread("../img/IP_dog.bmp");
    dst = Mat::zeros(img.size(), CV_8UC3);

    _fisheye(img, dst, zoom, strength);
    resize(dst, dst, Size(500, 500));

    /// Create Windows
    namedWindow("Circulat transformation", 1);
    imshow("Circulat transformation", dst);

    /// Create Trackbars
    createTrackbar("Zoom", "Circulat transformation", &zoom_slider, zoom_slider_max, on_zoom_trackbar);
    createTrackbar("Strength", "Circulat transformation", &strength_slider, strength_slider_max, on_strength_trackbar);

    waitKey(0);
    return 0;
}
