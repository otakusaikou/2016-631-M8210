#include <iostream>
#include <fstream>
#include <cstring>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

//
// Read text file
//
std::ifstream &openFileIn(std::ifstream &fin, const std::string &fileName)
{
    fin.close();                    // Close in case it was already open
    fin.clear();                    // Clear any existing errors
    fin.open(fileName.c_str());     // Open the file with given file name
    return fin;
}

//
// Split the file name and extension
//
void splitFileName(const std::string &fileName, std::string &file, std::string &ext)
{
    std::size_t filePtr = fileName.find_last_of("/\\");     // Get start position of file name
    std::size_t extPtr = fileName.find_last_of(".");        // Get start position of file extension
    file = fileName.substr(filePtr + 1, extPtr - filePtr - 1);
    ext = fileName.substr(extPtr + 1);
}

int main(int argc, char const* argv[])
{
    // Check the number of input argument
    if (argc == 1) {
        std::cerr << "Complain: I cannot find the file" << std::endl << std::endl;
        return -1;
    }
    std::string txtName = argv[1];

    // Open text file
    std::ifstream fin;
    openFileIn(fin, txtName);
    if (!openFileIn(fin, txtName)) {
        std::cerr << "Complain: I cannot find the file" << std::endl << std::endl;
    }

    // Split the file name
    std::string fileIn;
    std::string extIn;
    splitFileName(txtName, fileIn, extIn);

    // Inirialize new image
    int dim = std::stoi(extIn);
    cv::Mat img = cv::Mat::zeros(dim, dim, CV_8U);

    // Write the value to image
    unsigned char value;
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            fin >> value;
            if (value >= 'A') {
                value -= ('A' - '9' - 1);
            }
            value -= '0';
            img.at<uchar>(i, j) = value * 8;
        }
    }
    fin.close();

    // Save the image
    cv::imwrite(fileIn + ".jpg", img);

    return 0;
}
