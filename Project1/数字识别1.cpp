#include<iostream>
#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;
void FindCellBox(const cv::Mat& Image);

int main()
{
	Mat Image = imread("C:\\Users\\hzxuxin\\Downloads\\标定1\\偏右\\1.bmp");//3通道
	FindCellBox(Image);
	system("pause");
	return 0;
}



/* 提取输入图像中所有完整方格的四个角点坐标   直线检测做*/
void FindCellBox(const Mat& Image)
{
	Mat gray_image;
	Point2f p1;
	Mat box;
	Mat Corner_image;
	Mat create_picuture(Image.rows, Image.cols, CV_8UC3, Scalar(0, 0, 0));
	vector<Vec4i>lines;
	//namedWindow("HoughLinesP", CV_WINDOW_NORMAL);
	Corner_image = Image.clone();
	double hminVal = 0, hmaxVal = 0;
	double wminVal = 0, wmaxVal = 0;
	float circle_radius = 0;
	vector<vector<Point>>rect_corners;
	vector<vector<Point>>contours1;
	cvtColor(Image, gray_image, CV_BGR2GRAY);
	
	//GaussianBlur(gray_image, gray_image, Size(3, 3), 0, 0);
	threshold(gray_image, gray_image, 1, 255, THRESH_BINARY+THRESH_OTSU );
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(gray_image, gray_image, MORPH_CLOSE, element, Point(-1, -1), 13);
	morphologyEx(gray_image, gray_image, MORPH_OPEN, element, Point(-1, -1), 15);


	
}	

/******************************** 角点检测函数cornerHarris *****************************/















