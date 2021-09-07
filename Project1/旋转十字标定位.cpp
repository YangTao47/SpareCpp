#include<iostream>
#include<opencv2\opencv.hpp>
#include<chrono>
using namespace std::chrono;
using namespace std;
using namespace cv;
Point LocateCrossMark(const Mat& image);

/* 20ms */
int main()
{

	Mat image = imread("C:\\Users\\hzxuxin\\Downloads\\产品\\正\\1.bmp");
	Point2f p1;
	auto start_time = high_resolution_clock::now();
	for (int i = 0; i <= 10; i++) {
		LocateCrossMark(image);
	}
	time_point<high_resolution_clock> end_time = high_resolution_clock::now();
	milliseconds processing_time = duration_cast<milliseconds>(end_time - start_time)/10;
	cout << processing_time.count() << endl;
	system("pause");
	return 0;
}
Point LocateCrossMark(const Mat& Image)
{
	Point2f p1;
	Mat gray_image;
	Mat box;
	double hminVal = 0, hmaxVal = 0;
	double wminVal = 0, wmaxVal = 0;
	float circle_radius = 0;
	//Mat contour_image;
	//Mat canny_image;
	//Mat blur_image;
	cvtColor(Image, gray_image, CV_RGB2GRAY);
	//滤波处理
	//GaussianBlur(gray_image, gray_image, Size(3, 3),0,0);
	threshold(gray_image, gray_image, 1, 255, THRESH_OTSU + THRESH_BINARY);
	//Canny边缘检测
	//Canny(gray_image, gray_image, 100, 150, 3);
	//查找轮廓
	vector<vector<Point>>contours;
	findContours(gray_image, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
	int contours_size = (int)contours.size();
	//cout << Mat(contours[0]) << endl;
	//矩形操作
	for (int i = 0; i < contours_size; i++)
	{
		//获得矩形外包围框
		RotatedRect r = minAreaRect(Mat(contours[i]));
		boxPoints(r, box);
		minMaxIdx(box.col(0), &hminVal, &hmaxVal);
		minMaxIdx(box.col(1), &wminVal, &wmaxVal);
		double height = hmaxVal - hminVal;
		double width = wmaxVal - wminVal;
		//根据矩形宽高比和面积筛选矩形
		if ((float)width / height >= 0.9 && (float)width / height <= 1.2 && (float)width * height >= 40000 && (float)width * height <= 50000)
		{
			minEnclosingCircle(box, p1, circle_radius);
			cout << "十字标中心位置" << p1 << endl;
			circle(Image, p1, (int)circle_radius, Scalar(0, 0, 255));
		}
	}
	/*namedWindow("reutl", CV_WINDOW_NORMAL);
	imshow("reutl", Image);
	waitKey(0);*/
	return p1;
}


