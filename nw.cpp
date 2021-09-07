#include <iostream>
#include<opencv2\opencv.hpp>
#include<opencv2\core\core.hpp>
using namespace std;
using namespace cv;
Mat test();

int main()
{
	
	Mat image=test();
	cout << image.channels() << endl;
	imshow("result", image);
	waitKey(27);
	system("pause");
	return 0;

}

Mat test()
{
	Mat imageSource = imread("C:\\Users\\hzxuxin\\Pictures\\Saved Pictures\\test.jpg");
	//imshow("原始图像", imageSource);

	Mat image;
	//GaussianBlur(imageSource, image, Size(3, 3), 10);//高斯滤波
	//Canny(imageSource, image, 80, 150);
	//medianBlur(image, image, 1);
	GaussianBlur(imageSource, image, Size(3, 3), 50);
	Canny(imageSource, image, 80, 200);
	medianBlur(image, image, 1);


	Mat structElement1 = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	Mat structElement2 = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	

	waitKey(27);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point());
	Mat imageContours = Mat::zeros(image.size(), CV_8UC1);
	Mat Contours = Mat::zeros(image.size(), CV_8UC1); //绘制

	for (int i = 0; i < contours.size(); i++)
	{
		//contours[i]代表的是第i个轮廓，contours[i].size()代表的是第i个轮廓上所有的像素点数
		for (int j = 0; j < contours[i].size(); j++)
		{
			//绘制出contours向量所有的像素点
			Point P = Point(contours[i][j].x, contours[i][j].y);
			Contours.at<uchar>(P) = 255;
		}

		//绘制轮廓
		drawContours(imageContours, contours, i, Scalar(255), 1, 8, hierarchy);
	}
	//imshow("Contours Image CV_TRET_TREE", imageContours); //轮廓
	//imshow("Point of Contours CV_CHAIN_APPROX_NONE", Contours); //向量contours内保存的所有轮廓点集
	//waitKey(0);
	return imageContours;
}





