#include "utils.h"
#include <QDebug>

using namespace std;
using namespace cv;

utils::utils()
{
}

//
// Compute 8 parameters for trapezoidal transformation
//
void utils::getParam(const float &width, const float &height, const float &length, const float &shift, Mat &param)
{
    // Coordinates of four corner (v, w) from destination image
    float v[4] = {0, 0,
        static_cast<float>(width - shift),
        static_cast<float>(width - shift)};
    float w[4] = {0,
        static_cast<float>(height),
        static_cast<float>(0 + ceil((height-length)/2)),
        static_cast<float>(height - floor((height-length)/2))};

    // Coordinates of four corner (x, y) from source image
    float x[4] = {0, 0,
        static_cast<float>(width),
        static_cast<float>(width)};
    float y[4] = {0,
        static_cast<float>(height), 0,
        static_cast<float>(height)};

    // The transformation model is :
    // x = v * c1 + w * c2 + v * w * c3 + c4
    // y = v * c5 + w * c6 + v * w * c7 + c8

    // Format coefficient matrix B and constant matrix f
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

    // Solve unknown parameters
    Mat N = B.t() * B;
    Mat t = B.t() * f;
    param = N.inv() * t;
}

//
// Resample a new image with trapezoidal transformation
//
void utils::resampleTrap(Mat const &src, Mat &dst, const Mat &param)
{
    float height = src.rows;
    float width = src.cols;

    float c1 = param.at<float>(0, 0);
    float c2 = param.at<float>(1, 0);
    float c3 = param.at<float>(2, 0);
    float c4 = param.at<float>(3, 0);
    float c5 = param.at<float>(4, 0);
    float c6 = param.at<float>(5, 0);
    float c7 = param.at<float>(6, 0);
    float c8 = param.at<float>(7, 0);

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0;j < width; ++j)
        {
            // Compute new coordinates
            int x = c1 * j + c2 * i + c3 * j * i + c4;
            int y = c5 * j + c6 * i + c7 * j * i + c8;

            // Ignore coordinates which are not within the source image extent
            if (x >= width || x < 0)
                dst.at<Vec3f>(i, j) = 0;
            else if (y >= height || y < 0)
                dst.at<Vec3f>(i, j) = 0;
            else
                dst.at<Vec3f>(i, j) = src.at<Vec3f>(y, x);
        }
    }
}

//
// Resample a new image with wavy transformation
//
void utils::resampleWavy(Mat const &src, Mat &dst, const float &ampl, const float &freq)
{
    double height = src.rows;
    double width = src.cols;

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0;j < width; ++j)
        {
            int x = j - ampl * sin(freq * i);
            int y = i - ampl * sin(freq * j);
            if (x >= width || x < 0)
                dst.at<Vec3f>(i, j) = 0;
            else if (y >= height || y < 0)
                dst.at<Vec3f>(i, j) = 0;
            else
                dst.at<Vec3f>(i, j) = src.at<Vec3f>(y, x);
        }
    }
}

//
// Resample a new image with circular transformation
//
void utils::resampleCircular(Mat const &src, Mat &dst, const float &zoom, const float &strength)
{
    double height = src.rows;
    double width = src.cols;

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0;j < width; ++j)
        {
            // Normalize and translate the (x, y) coordinates to -1~1
            double ny = ((2 * i) / height) - 1;
            double nx = ((2 * j) / width) - 1;

            // Compute distance from image center
            double r = sqrt(nx * nx + ny * ny) / strength;

            // Discard pixels outside from circle
            if (0.0 <= r && r <= 1.0)
            {
                // Compute the new distance
                double nr = (r/zoom + (1.0 - sqrt(1.0 - r * r))) / 2.0;

                // Discard radius greater than 1.0
                if (nr <= 1.0)
                {
                    // Angle for polar coordinate system
                    double theta = atan2(ny, nx);

                    // New (x, y) coordinates with new distance in same angle
                    int x2 = static_cast<int>(((nr * cos(theta) + 1) * width) / 2.0);
                    int y2 = static_cast<int>(((nr * sin(theta) + 1) * height) / 2.0);
                    if (x2 >= width || x2 < 0)
                        dst.at<Vec3f>(i, j) = 0;
                    else if (y2 >= height || y2 < 0)
                        dst.at<Vec3f>(i, j) = 0;
                    else
                        dst.at<Vec3f>(i, j) = src.at<Vec3f>(y2, x2);
                }
            }
        }
    }
}
