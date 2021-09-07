#include<iostream>
#include<opencv2\opencv.hpp>
#include<chrono>
using namespace std::chrono;
using namespace std;
using namespace cv;
Point LocateCrossMark(const Mat& image);

int main()
{

	Mat image = imread("C:\\Users\\hzxuxin\\Downloads\\产品\\偏右旋\\2.bmp");
	auto start_time = high_resolution_clock::now();
	time_point<high_resolution_clock> end_time = high_resolution_clock::now();
	milliseconds processing_time = duration_cast<milliseconds>(end_time - start_time);
	cout << processing_time.count() << endl;
	system("pause");
	return 0;
}
Point LocateCrossMark(const Mat& image)
{
	//resize(image, image, Size(0, 0), 0.5, 0.5, CV_INTER_LINEAR);
	Mat gray_image;
	cvtColor(image, gray_image, CV_RGB2GRAY);
	//滤波处理
	Mat blur_image;
	GaussianBlur(gray_image, blur_image, Size(3, 3), 0, 0);
	//Canny边缘检测
	Mat canny_image;
	Canny(blur_image, canny_image, 100, 150, 3);
	//查找轮廓
	Mat contour_image;
	contour_image = canny_image.clone();
	vector<vector<Point>>contours;
	findContours(contour_image, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
	//绘制轮廓
	drawContours(contour_image, contours, -1, Scalar(255), 1);

	//cvtColor(image, gray_image, CV_GRAY2BGR);
	//矩形
	for (int i = 0; i < contours.size(); i++)
	{
		//获得矩形外包围框
		RotatedRect r = minAreaRect(Mat(contours[i]));
		Mat box;
		boxPoints(r, box);
		double hminVal = 0, hmaxVal = 0;
		double wminVal = 0, wmaxVal = 0;
		int    hminIdx[2] = {}, hmaxIdx[2] = {};	// minnimum Index, maximum Index
		int    wminIdx[2] = {}, wmaxIdx[2] = {};	// minnimum Index, maximum Index
		minMaxIdx(box.col(0), &hminVal, &hmaxVal, hminIdx, hmaxIdx);
		minMaxIdx(box.col(1), &wminVal, &wmaxVal, wminIdx, wmaxIdx);
		double height = hmaxVal - hminVal;
		double width = wmaxVal - wminVal;
		//根据矩形宽高比和面积筛选矩形
		if ((float)width / height >= 0.9 && (float)width / height <= 1.2 && (float)width * height >= 40000 && (float)width * height <= 50000)
		{
			//cout << "r.x = " << r.x << "  r.y  = " << r.y << "rate =" << ((float)r.width / r.height) << " area = " << ((float)r.width * r.height) << endl;
			//画矩形

			/*for (int i = 0; i < 4; i++)
			{
			Point p1 = box.row(i);
			int j = (i + 1) % 4;
			Point p2 = box.row(j);
			line(image, p1, p2, Scalar(0, 0, 255), 1);
			}*/

			Point2f p1;
			float radius = 0;
			minEnclosingCircle(box, p1, radius);
			//cout << "矩形角点位置" << box << endl;
			cout << "十字标中心位置" << p1 << endl;
			//cout << "圆的半径" << radius << endl;
			circle(image, p1, radius, Scalar(0, 0, 255));
		}
	}
	/*cvNamedWindow("image", CV_WINDOW_NORMAL);
	imshow("image", image);
	waitKey(0);*/
	//return image;
}


