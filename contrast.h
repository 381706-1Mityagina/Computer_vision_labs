#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <ctime>
#include <random>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;

int minimum(std::vector<int>* grayscale_image) {
	return *std::min_element(grayscale_image->begin(), grayscale_image->end());
}

int maximum(std::vector<int>* grayscale_image) {
	return *std::max_element(grayscale_image->begin(), grayscale_image->end());
}

int use_formula(int value, int max_value, int min_value) {
	// The conversion of brightness levels is carried out according to the formula:
	// g[i] = a + b * f[i]
	// where f[i] is the old brightness value of the i-th pixel, g[i] is the new
	// value, a, b are the coefficients.
	// We choose a and b so that g[min] = 0, g[max] = 255

	if (max_value < min_value)
		throw - 1;

	float a = (-1) * (static_cast<float>(255) / (max_value - min_value)) * min_value;
	float b = static_cast<float>(255) / (max_value - min_value);

	if (max_value == min_value)
		return 0;
	else
		return (a + b * value);
}

std::vector<int> contrast_increase(std::vector<int> grayscale_image, int width, int height) {
	int size = width * height, error = 0;

	if (size <= 0) {
		error = 1;
	}
	switch (error) {
	case 1:
		throw std::runtime_error("size <= 0");
	}
	std::vector<int> output(grayscale_image);
	int min = minimum(&grayscale_image);
	int max = maximum(&grayscale_image);

	for (int i = 0; i < size; i++) {
		output[i] = use_formula(grayscale_image[i], max, min);
	}
	return output;
}

void histStretch(Mat src, Mat dst)
{
	int gray[256]{ 0 };
	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			gray[(int)src.at<uchar>(y, x)]++;
		}
	}
	std::vector<int> fake = std::vector<int>(src.rows * src.cols);
	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			fake[y * src.cols + x] = src.at<uchar>(y, x);
		}
	}
	std::cout << minimum(&fake) << "\n";
	std::cout << maximum(&fake) << "\n";
	std::vector<int> out = contrast_increase(fake, src.cols, src.rows);

	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			dst.at<uchar>(y, x) = (uchar)out[y * src.cols + x];
		}
	}
}

void drawHist(const Mat src, Mat dst)
{
	int histSize = 256;
	float histMax = 0;
	for (int i = 0; i < histSize; i++) {
		float temp = src.at<float>(i);
		if (histMax < temp) {
			histMax = temp;
		}
	}

	float scale = (0.9 * 256) / histMax;
	for (int i = 0; i < histSize; i++) {
		int intensity = static_cast<int>(src.at<float>(i) * scale);
		line(dst, Point(i, 255), Point(i, 255 - intensity), Scalar(0));
	}
}
