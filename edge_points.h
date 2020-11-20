#include "methods.h"

void Moravec(Mat srcImage) {
	int kSize = 5;
	//int threshold = 140000;
	int threshold = 20000;  // board
	int r = kSize / 2;
	int nCount = 0;

	Mat resMorMat = srcImage.clone(), gray;
	cvtColor(srcImage, gray, COLOR_BGR2GRAY);
	Point* pPoint = new Point[srcImage.rows * srcImage.cols];

	for (int i = r; i < srcImage.rows - r + 1; i++) {
		for (int j = r; j < srcImage.cols - r + 1; j++) {
			int wV1, wV2, wV3, wV4;
			wV1 = wV2 = wV3 = wV4 = 0;
			for (int k = -r; k <= r; k++) {
				for (int m = -r; m <= r; m++) {
					if (j + m + 1 >= srcImage.cols || i + k >= srcImage.rows || j + m >= srcImage.cols) {
						continue;
					}
					wV1 += pow((gray.at<uchar>(i + k, j + m + 1) - gray.at<uchar>(i + k, j + m)), 2);

					if (j + m >= srcImage.cols || i + k + 1 >= srcImage.rows || i + k >= srcImage.rows) {
						continue;
					}
					wV2 += pow((gray.at<uchar>(i + k + 1, j + m) - gray.at<uchar>(i + k, j + m)), 2);

					if (i + k + 1 >= srcImage.rows || j + m + 1 >= srcImage.cols || i + k >= srcImage.rows || j + m >= srcImage.cols) {
						continue;
					}
					wV3 += pow((gray.at<uchar>(i + k + 1, j + m + 1) - gray.at<uchar>(i + k, j + m)), 2);

					if (i + k + 1 >= srcImage.rows || j + m - 1 < 0 || j + m >= srcImage.cols || i + k >= srcImage.rows) {
						continue;
					}
					wV4 += pow((gray.at<uchar>(i + k + 1, j + m - 1) - gray.at<uchar>(i + k, j + m)), 2);
				}
			}
			int value = min(min(wV1, wV2), min(wV3, wV4));
			if (value > threshold) {
				if (j < srcImage.rows && i < srcImage.cols) {
					pPoint[nCount] = Point(j, i);
					nCount++;
				}
			}
		}
	}
	for (int i = 0; i < nCount; i++) {
		circle(resMorMat, pPoint[i], 2, Scalar(0, 0, 255), 1, 8, 0);
	}
	namedWindow("Moravec", WINDOW_AUTOSIZE);
	imshow("Moravec", resMorMat);
	waitKey(0);
}
