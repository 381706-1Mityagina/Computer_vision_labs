#include <iostream>
#include "contrast.h"
#include "canny.h"
#include "edge_points.h"

int MinMax(int value, int max) {
	int min = 0;
	if (value < min) return min;
	if (value > max) return max;

	return value;
}

Point getPixel(Mat grayImage, int radius, int x, int y) {
	int size = 2 * radius + 1;
	pair<int, Point>* neighbors = new pair<int, Point>[size * size];

	for (int l = -radius; l < radius + 1; l++) {
		for (int k = -radius; k < radius + 1; k++) {
			int idX = MinMax(x + k, grayImage.cols - 1);
			int idY = MinMax(y + l, grayImage.rows - 1);

			neighbors[(l + radius) * size + (k + radius)] = make_pair(static_cast<int>(grayImage.at<uchar>(idY, idX)), Point(idX, idY));
		}
	}

	sort(neighbors, neighbors + size * size, [](pair<int, Point> a, pair<int, Point> b)->bool {return a.first < b.first;});
	Point result = (neighbors[(size * size) / 2]).second;
	delete[]neighbors;

	return result;
}

void filterUsingIntegralIm(Mat image, Mat distImage, Mat grayImage, double k) {
	Mat integralImage, filtImage = image.clone();
	integral(image, integralImage);

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			int radius = min(static_cast<int>(k * distImage.at<uchar>(Point(i, j))), 4);

			Point A(MinMax(j - radius, image.cols - 1), MinMax(i - radius, image.rows - 1));
			Point B(MinMax(j + radius, image.cols - 1), MinMax(i - radius, image.rows - 1));
			Point C(MinMax(j + radius, image.cols - 1), MinMax(i + radius, image.rows - 1));
			Point D(MinMax(j - radius, image.cols - 1), MinMax(i + radius, image.rows - 1));

			int Summ = integralImage.at<int>(A) + integralImage.at<int>(C) - integralImage.at<int>(B) - integralImage.at<int>(D);

			float S = pow((2 * radius + 1), 2);
			filtImage.at<uchar>(Point(i, j)) = static_cast<uchar>(float(Summ) / S);
		}
	}

	imshow("With integral image", filtImage);
	waitKey();
}

void filterImageMedian(Mat image, Mat distImage, Mat grayImage, double k) {
	float avrgRadius = 0.0f;
	Mat medianImage = image.clone();

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			int radius = min(static_cast<int>(k * distImage.at<uchar>(Point(i, j))), 5);
			Point coords = getPixel(grayImage, radius, i, j);
			medianImage.at<uchar>(Point(i, j)) = image.at<uchar>(Point(coords.x, coords.y));
			avrgRadius += radius;
		}
	}

	namedWindow("Filter input", WINDOW_AUTOSIZE);
	imshow("Filter input", image);
	namedWindow("Filter output", WINDOW_AUTOSIZE);
	imshow("Filter output", medianImage);
	waitKey();
}

void DistTransOpenCV(Mat input) {
	Mat dst, bw;
	cvtColor(input, bw, COLOR_BGR2GRAY);
	threshold(bw, bw, 40, 255, THRESH_BINARY | THRESH_OTSU);

	distanceTransform(bw, dst, DIST_L2, 3);

	imwrite("res/distnceTransformOCV.jpg", dst);
	Mat image_canny = imread("res/distnceTransformOCV.jpg", IMREAD_COLOR);
	imshow("distnceTransform", image_canny);
}

void grayscale(Mat image) {
	Mat r, g, b;
	vector<Mat> channels(3);
	int N = image.cols * image.rows;

	split(image, channels);

	r = channels[0];
	g = channels[1];
	b = channels[2];

	for (int i = 0; i < image.cols; i++) {
		for (int j = 0; j < image.rows; j++) {
			auto y = r.at<uchar>(Point(i, j)) * 0.2952 + g.at<uchar>(Point(i, j)) * 0.5947 + b.at<uchar>(Point(i, j)) * 0.11;
			r.at<uchar>(Point(i, j)) = y;
			g.at<uchar>(Point(i, j)) = y;
			b.at<uchar>(Point(i, j)) = y;
		}
	}
	namedWindow("task 1", WINDOW_AUTOSIZE);
	imshow("task 1", r);
	waitKey(0);
}

void contrast(Mat image) {
	Mat hist1, hist2, gray;
	int histSize = 256;
	int channels[] = { 0 };
	float range[] = { 0, 255 };
	const float* histRange = { range };

	cvtColor(image, gray, COLOR_BGR2GRAY);
	Mat out = gray.clone();

	histStretch(gray, out);

	calcHist(&gray, 1, channels, Mat(), hist1, 1, &histSize, &histRange);
	Mat showHist1(256, 256, CV_8UC1, Scalar(255));
	drawHist(hist1, showHist1);

	namedWindow("Histogram(Original)", WINDOW_AUTOSIZE);
	imshow("Histogram(Original)", showHist1);
	waitKey(0);

	calcHist(&out, 1, channels, Mat(), hist2, 1, &histSize, &histRange);
	Mat showHist2(256, 256, CV_8UC1, Scalar(255));
	drawHist(hist2, showHist2);

	namedWindow("Histogram(changed)", WINDOW_AUTOSIZE);
	imshow("Histogram(changed)", showHist2);
	waitKey(0);

	imwrite("res/Histogram_stretched.jpg", out);
	namedWindow("Histogram stretch", WINDOW_AUTOSIZE);
	imshow("Histogram stretch", out);
	waitKey(0);
}

void canny_edge(const Mat& input, float s, bool denoise) {
	imshow("canny", canny(input, s));
	imwrite("res/canny.jpg", canny(input, s));
}

int main()
{
	// ---------------------------------------------------------------------
	Mat image = imread("res/moment1.jpg", IMREAD_COLOR);
	imshow("input", image);
	// ---------------------------------------------------------------------
	grayscale(image);
	// ---------------------------------------------------------------------
	contrast(image);
	// ---------------------------------------------------------------------
	Mat image_canny = imread("res/Histogram_stretched.jpg", IMREAD_COLOR);
	imshow("input", image_canny);
	canny_edge(image_canny, 5, true);
	// ---------------------------------------------------------------------
	Mat image_mor = imread("res/canny.jpg", IMREAD_COLOR);
	Moravec(image_mor);
	// ---------------------------------------------------------------------
	Mat image_dist = imread("res/Histogram_stretched.jpg", IMREAD_COLOR);
	imshow("dist input", image_dist);
	DistTransOpenCV(image_dist);
	// ---------------------------------------------------------------------
	Mat image_dist_filt = imread("res/distnceTransformOCV.jpg", IMREAD_COLOR);
	filterImageMedian(image, image_dist_filt, image_canny, 5);
	filterUsingIntegralIm(image, image_dist_filt, image_canny, 5);

	waitKey(0);
	return 0;
}
