#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <random>
#include <vector>
#include <math.h>
#include <ctime>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;

void histStretch(Mat src, Mat dst);
void drawHist(const Mat src, Mat dst);
int minimum(std::vector<int>* grayscale_image);
int maximum(std::vector<int>* grayscale_image);
int use_formula(int value, int max_value, int min_value);
std::vector<int> contrast_increase(std::vector<int> grayscale_image, int width, int height);

void gradient(const Mat& input, Mat& G2);
void sobel(const Mat& input, Mat& Ix, Mat& Iy, Mat& G2);
Mat threshold(const Mat& input, float s, bool denoise);
Mat canny(const Mat& input, float s1);
