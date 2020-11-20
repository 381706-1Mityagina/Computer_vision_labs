#include <iostream>
#include "contrast.h"
#include "canny.h"
#include "edge_points.h"

void grayscale(Mat image) { // перевод изображения в полутоновое
	Mat r, g, b;
	vector<Mat> channels(3); // вектор для каналов
	int N = image.cols * image.rows;

	split(image, channels); // разделяем картинку на каналы

	r = channels[0];
	g = channels[1];
	b = channels[2];

	for (int i = 0; i < image.cols; i++)
	{
		for (int j = 0; j < image.rows; j++)
		{
			auto y = r.at<uchar>(Point(i, j)) * 0.2952 + g.at<uchar>(Point(i, j)) * 0.5947 + b.at<uchar>(Point(i, j)) * 0.11;
			r.at<uchar>(Point(i, j)) = y;
			g.at<uchar>(Point(i, j)) = y;
			b.at<uchar>(Point(i, j)) = y;
			//image.at<uchar>(Point(i, j)) = y;
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

	imwrite("Histogram_stretched.jpg", out);
	namedWindow("Histogram stretch", WINDOW_AUTOSIZE);
	imshow("Histogram stretch", out);
	waitKey(0);
}

void canny_edge(const Mat& input, float s, bool denoise) {
	imshow("threshold", threshold(input, s));
	imshow("+ denoising", threshold(input, s, denoise));
	imshow("canny", canny(input, s));
}

int main()
{
	// ---------------------------------------------------------------------
	Mat image = imread("moment1.jpg", IMREAD_COLOR);
	imshow("input", image);
	// ---------------------------------------------------------------------
	//grayscale(image);
	//// ---------------------------------------------------------------------
	//contrast(image);
	//// ---------------------------------------------------------------------
	//Mat image_canny = imread("home.jpg", IMREAD_COLOR);
	//imshow("input", image_canny);
	//canny_edge(image_canny, 5, true);
	// ---------------------------------------------------------------------
	Mat image_mor = imread("board.jpg", IMREAD_COLOR);
	imshow("input", image_mor);
	Moravec(image_mor);

	waitKey(0);
	return 0;
}
