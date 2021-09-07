#include<iostream>
using namespace std;
#include<opencv2\opencv.hpp>
using namespace cv;


Mat SegmentImage(const Mat &srcImage)
{
	Mat mask;
	cvtColor(srcImage, srcImage, CV_BGR2HSV);
	//GaussianBlur(image, image, Size(3, 3), 0);
	//threshold(image, image, 200, 250, THRESH_BINARY);
	//cout << image.channels() << endl;
	double low_H = 90; //156
	double low_S = 43;
	double low_V = 46;
	double high_H = 124; //180
	double high_S = 255;
	double high_V = 255;
	//使用inrange函数，将在这个蓝青色的颜色提取出来，得到mask矩阵
	inRange(srcImage, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask);
	GaussianBlur(mask, mask, Size(3, 3), 0);
	cvtColor(mask, mask, CV_GRAY2BGR);
	cvtColor(srcImage, srcImage, CV_HSV2BGR);
	addWeighted(srcImage, 1, mask, 1, 1, srcImage);
	//GaussianBlur(image, image, Size(3, 3), 0);
	return srcImage;

}


int main()
{
	Mat image;
	image = imread("test.jpg");
	Mat result;
	result = SegmentImage(image);
	imwrite("result.jpg", result);
	system("pause");
	return 0;
}
#include<iostream>
#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;
Mat SegmentImage(const Mat &srcImage);
void dasdasd();
void on_bilateralFilterTrackbar(int, void*);

int main()
{

	Mat src;
	src = imread("test.jpg");
	Mat dst;
	dst = SegmentImage(src);
	imwrite("result.jpg", dst);
	return 0;
}

Mat SegmentImage(const Mat &src)
{
	Mat mask, hsvImage, filteredImage;
	double low_H = 89, low_S = 43, low_V = 46, high_H = 124, high_S = 255, high_V = 255;
	bilateralFilter(src, filteredImage, 17, 77, 11);
	//medianBlur(filteredImage, filteredImage, 5);
	cvtColor(filteredImage, hsvImage, CV_BGR2HSV);
	/* 使用inrange函数，将在这个蓝青色的颜色提取出来，得到mask矩阵 */
	inRange(hsvImage, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask);
	cvtColor(mask, mask, CV_GRAY2BGR);
	addWeighted(src, 1, mask, 1, 1, src);
	//medianBlur(src, src, 5);
	return src;
}

//定义全局变量
const int g_ndMaxValue = 500;
const int g_nsigmaColorMaxValue = 500;
const int g_nsigmaSpaceMaxValue = 500;
int g_ndValue;
int g_nsigmaColorValue;
int g_nsigmaSpaceValue;



void dasdasd()
{

	//定义输出图像窗口属性和轨迹条属性
	namedWindow("双边滤波图像", WINDOW_AUTOSIZE);
	g_ndValue = 10;
	g_nsigmaColorValue = 10;
	g_nsigmaSpaceValue = 10;

	char dName[20];
	sprintf_s(dName, "邻域直径 %d", g_ndMaxValue);

	char sigmaColorName[20];
	sprintf_s(sigmaColorName, "sigmaColor %d", g_nsigmaColorMaxValue);

	char sigmaSpaceName[20];
	sprintf_s(sigmaSpaceName, "sigmaSpace %d", g_nsigmaSpaceMaxValue);

	//创建轨迹条
	createTrackbar(dName, "双边滤波图像", &g_ndValue, g_ndMaxValue, on_bilateralFilterTrackbar);
	on_bilateralFilterTrackbar(g_ndValue, 0);

	createTrackbar(sigmaColorName, "双边滤波图像", &g_nsigmaColorValue,
		g_nsigmaColorMaxValue, on_bilateralFilterTrackbar);
	on_bilateralFilterTrackbar(g_nsigmaColorValue, 0);

	createTrackbar(sigmaSpaceName, "双边滤波图像", &g_nsigmaSpaceValue,
		g_nsigmaSpaceMaxValue, on_bilateralFilterTrackbar);
	on_bilateralFilterTrackbar(0, 0);

	waitKey(0);

}

void on_bilateralFilterTrackbar(int, void*)
{
	Mat src;
	src = imread("test.jpg");
	Mat dst;
	Mat mask, hsvImage, filteredImage;
	double low_H = 89, low_S = 43, low_V = 46, high_H = 124, high_S = 255, high_V = 255;
	bilateralFilter(src, filteredImage, g_ndValue, g_nsigmaColorValue, g_nsigmaSpaceValue);
	cvtColor(filteredImage, hsvImage, CV_BGR2HSV);
	inRange(hsvImage, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask);
	cvtColor(mask, mask, CV_GRAY2BGR);
	addWeighted(src, 1, mask, 1, 1, dst);
	imshow("双边滤波图像", dst);
}
