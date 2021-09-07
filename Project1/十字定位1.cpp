#include<iostream>
#include<opencv2\opencv.hpp>
#include<ctime>
using namespace std;
using namespace cv;

int main()
{
	clock_t start, end;
	start = clock();
	Mat image = imread("C:\\Users\\hzxuxin\\Downloads\\产品\\前移\\1.bmp");
	Mat copyImage;
	image.copyTo(copyImage);
	Mat gray_image;
	Mat creatPicture(image.rows, image.cols, CV_8UC3, Scalar(255, 255, 255));
	//cout << image.cols << " " << creatPicture.cols << endl;
	//cout << image.rows << " " << creatPicture.rows << endl;
	resize(image, image, Size(0, 0), 0.5, 0.5, CV_INTER_LINEAR);
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
	//创建空白图像作为前景
	//矩形
	vector<Point> rectPoint;
	for (int i = 0; i < contours.size(); i++)
	{	
		//获得矩形外包围框
		Rect r = boundingRect(Mat(contours[i]));

		//根据矩形宽高比和面积筛选矩形
		if ((float)r.width / r.height >= 0.9 && (float)r.width / r.height <= 1.2 && (float)r.width * r.height >= 9000 && (float)r.width * r.height <= 15000)
		{
			//cout << "r.x = " << r.x << "  r.y  = " << r.y << "rate =" << ((float)r.width / r.height) << " area = " << ((float)r.width * r.height) << endl;

			Point p1, p2, p3, p4;
			p1.x = r.x;
			p1.y = r.y;
			p2.x = r.x + r.width;
			p2.x = r.y;
			p3.x = r.x + r.width;
			p3.y = r.y + r.height;
			p4.x = r.x;
			p4.y = r.y + r.height;
			rectPoint.push_back(p1);
			rectPoint.push_back(p2);
			rectPoint.push_back(p3);
			rectPoint.push_back(p4);
			//画矩形
			rectangle(image, r, Scalar(0, 0, 255), 2);

			//cout << rectpoint.front() << endl;

		}
	}



	end = clock();
	cout << end - start << endl;
	namedWindow("检测", CV_WINDOW_NORMAL);
	cvNamedWindow("测试", CV_WINDOW_NORMAL);
	imshow("测试", copyImage);
	imshow("检测", image);

	waitKey(0);
	system("pause");
	return 0;
}