#include "methods.h"
#include <queue>

// Theory : https://medium.com/towards-artificial-intelligence/what-is-canny-edge-detection-algorithm-95defef75492
//          http://justin-liang.com/tutorials/canny/

// Gradient
void gradient(const Mat& src, Mat& gradient)
{
	Mat image;
	cvtColor(src, image, COLOR_BGR2GRAY);
	gradient = Mat(image.rows, image.cols, CV_32F);

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			float ix, iy;
			if (i == 0 || i == image.rows - 1) {
				iy = 0;
			} else {
				iy = (float(image.at<uchar>(i - 1, j)) - float(image.at<uchar>(i + 1, j))) / 2;
			}
			if (j == 0 || j == image.cols - 1) {
				ix = 0;
			} else {
				ix = (float(image.at<uchar>(i, j + 1)) - float(image.at<uchar>(i, j - 1))) / 2;
			}
			gradient.at<float>(i, j) = (ix * ix + iy * iy);
		}
	}
}

// Sobel denoising
void sobel_denoise(const Mat& src, Mat& Ix, Mat& Iy, Mat& gradient)
{
	Mat image;
	cvtColor(src, image, COLOR_BGR2GRAY);
	Ix = Mat(image.rows, image.cols, CV_32F);
	Iy = Mat(image.rows, image.cols, CV_32F);
	gradient = Mat(image.rows, image.cols, CV_32F);

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			float ix, iy;
			if (i == 0 || i >= image.rows - 1 || j == 0 || j >= image.cols - 1) {
				ix = 0; iy = 0;
			} else {
				ix = ((float(image.at<uchar>(i - 1, j + 1)) - float(image.at<uchar>(i - 1, j - 1)))
					    + 2 * (float(image.at<uchar>(i, j + 1)) - float(image.at<uchar>(i, j - 1)))
					    + (float(image.at<uchar>(i + 1, j + 1)) - float(image.at<uchar>(i + 1, j - 1)))) / 8;
				iy = ((float(image.at<uchar>(i + 1, j - 1)) - float(image.at<uchar>(i - 1, j - 1)))
					    + 2 * (float(image.at<uchar>(i + 1, j)) - float(image.at<uchar>(i - 1, j)))
					    + (float(image.at<uchar>(i + 1, j - 1)) - float(image.at<uchar>(i - 1, j - 1)))) / 8;
			}
			Ix.at<float>(i, j) = ix;
			Iy.at<float>(i, j) = iy;
			gradient.at<float>(i, j) = (ix * ix + iy * iy);
		}
	}
}

// Gradient thresholding
Mat threshold(const Mat& src, float s, bool denoise = false)
{
	Mat Ix, Iy, grad;
	Mat C(src.rows, src.cols, CV_8U);

	s *= s;
	if (denoise) {
		sobel_denoise(src, Ix, Iy, grad);
	} else {
		gradient(src, grad);
	}
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			C.at<uchar>(i, j) = (grad.at<float>(i, j) > s ? 255 : 0);
		}
	}
	return C;
}

// Canny edge detector
Mat canny(const Mat& src, float s1)
{
	Mat Ix, Iy, grad2;
	sobel_denoise(src, Ix, Iy, grad2);
	Mat Max(src.rows, src.cols, CV_8U);
	Mat C(src.rows, src.cols, CV_8U);
	queue<Point> Q;

	float s2 = 3 * s1;
	s1 *= s1; s2 *= s2; // gradient is actually squared gradient
	const float tan22 = 0.414f, tan67 = 2.414f;

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			uchar mx = 0;
			float grad0 = grad2.at<float>(i, j);
			if (i > 0 && j > 0 && i < src.rows - 1 && j < src.cols - 1 && grad0 > s1) {
				float ix = Ix.at<float>(i, j), iy = Iy.at<float>(i, j);
				if (ix < 0) {
					ix = -ix; iy = -iy;
				}
				float grad_a, grad_b;
				if (iy > tan67 * ix || iy < -tan67 * ix) {
					grad_a = grad2.at<float>(i + 1, j); grad_b = grad2.at<float>(i - 1, j);
				}
				else if (iy > 0.414 * ix) {
					grad_a = grad2.at<float>(i + 1, j + 1); grad_b = grad2.at<float>(i - 1, j - 1);
				}
				else if (iy > -0.414 * ix) {
					grad_a = grad2.at<float>(i, j + 1); grad_b = grad2.at<float>(i, j - 1);
				}
				else {
					grad_a = grad2.at<float>(i + 1, j - 1); grad_b = grad2.at<float>(i - 1, j + 1);
				}
				if (grad0 > grad_a && grad0 > grad_b) {
					mx = 255;
					if (grad0 > s2) {
						Q.push(Point(j, i));
					}
				}
			}
			Max.at<uchar>(i, j) = mx;
		}
	}
	
	C.setTo(0);
	while (!Q.empty()) {
		int j = Q.front().x; 
		int i = Q.front().y;
		Q.pop();
		C.at<uchar>(i, j) = 255;
		for (int a = max(0, i - 1); a < min(i + 2, src.rows); a++) {
			for (int b = max(0, j - 1); b < min(j + 2, src.cols); b++) {
				if (!C.at<uchar>(a, b) && Max.at<uchar>(a, b)) {
					Q.push(Point(b, a));
				}
			}
		}
	}

	return C;
}
