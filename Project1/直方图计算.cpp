#define INPUT_TITLE "input image"
#define OUTPUT_TITLE "直方图计算"

#include<iostream>
#include<opencv2\opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat src, dst;
	//加载图像
	src = imread("C:/Users/hzxuxin/Pictures/Saved Pictures/test.jpg");
	if (!src.data)
	{
		cout << "ERROR : could not load image.";
		return -1;
	}
	namedWindow(INPUT_TITLE, CV_WINDOW_AUTOSIZE);
	namedWindow(OUTPUT_TITLE, CV_WINDOW_AUTOSIZE);

	imshow(INPUT_TITLE, src);

	//分通道显示
	vector<Mat> bgr_planes;
	split(src, bgr_planes);

	//设定像素取值范围
	int histSize = 256;
	float range[] = { 0,256 };
	const float *histRanges = { range };

	//三个通道分别计算直方图
	Mat b_hist, g_hist, r_hist;
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRanges, true, false);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRanges, true, false);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRanges, true, false);

	//创建直方图画布并归一化处理
	int hist_h = 400;
	int hist_w = 512;
	int bin_w = hist_w / histSize;
	Mat histImage(hist_w, hist_h, CV_8UC3, Scalar(0, 0, 0));
	normalize(b_hist, b_hist, 0 , hist_h, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, hist_h, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, hist_h, NORM_MINMAX, -1, Mat());

	//render histogram chart  在直方图画布上画出直方图
	for (int i = 0; i < histSize; i++)
	{
		line(histImage, Point((i - 1)*bin_w, hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point((i)*bin_w, hist_h - cvRound(b_hist.at<float>(i))), Scalar(255, 0, 0), 2, LINE_AA);
		line(histImage, Point((i - 1)*bin_w, hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point((i)*bin_w, hist_h - cvRound(g_hist.at<float>(i))), Scalar(0, 255, 0), 2, LINE_AA);
		line(histImage, Point((i - 1)*bin_w, hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point((i)*bin_w, hist_h - cvRound(r_hist.at<float>(i))), Scalar(0, 0, 255), 2, LINE_AA);
	}


	imshow(OUTPUT_TITLE, histImage);

	waitKey(0);
	return 0;
}
