#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
using namespace cv;
using namespace std;
using namespace std::chrono;

int main()
{


	Mat image = imread("C:/Users/hzxuxin/Downloads/±ê¶¨1/×óÒÆ/1.bmp", CV_8UC1);
	line(image, Point(0, 500), Point(image.cols, 500), Scalar(0, 0, 255), 2);
	line(image, Point(0, 1000), Point(image.cols, 1000), Scalar(0, 0, 255), 2);
	line(image, Point(0, 1500), Point(image.cols, 1500), Scalar(0, 0, 255), 2);
	line(image, Point(0, 2000), Point(image.cols, 2000), Scalar(0, 0, 255), 2);
	line(image, Point(500, 0), Point(500, image.rows), Scalar(0, 0, 255), 2);
	line(image, Point(1000, 0), Point(1000, image.rows), Scalar(0, 0, 255), 2);
	line(image, Point(1500, 0), Point(1500, image.rows), Scalar(0, 0, 255), 2);
	line(image, Point(2000, 0), Point(2000, image.rows), Scalar(0, 0, 255), 2);
	system("pause");
	return 0;
}