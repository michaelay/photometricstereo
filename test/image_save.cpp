#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

int main (int argc, char** argv) {
Mat pixels;
pixels = Mat(2, 2 , CV_32FC1);
pixels.at<float>(0,0) = 0; 
pixels.at<float>(0,1) = 0.25; 
pixels.at<float>(1,0) = 0.5; 
pixels.at<float>(1,1) = 1; 
pixels *= 255;

imwrite("test.png", pixels);

Mat image = imread("test.png"); 
std::cout << image << std::endl;
return 0;
}
