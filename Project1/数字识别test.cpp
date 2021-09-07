#include<iostream>
#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;

int getColSum(Mat src, int col)//统计所有列像素的总和
{
	int sum = 0;
	int height = src.rows;
	int width = src.cols;
	for (int i = 0; i < height; i++)
	{
		sum = sum + src.at <uchar>(i, col);
	}
	return sum;
}

int getRowSum(Mat src, int row)//统计所有行像素的总和
{
	int sum = 0;
	int height = src.rows;
	int width = src.cols;
	for (int i = 0; i < width; i++)
	{
		sum += src.at <uchar>(row, i);
	}
	return sum;
}


void cutTop(Mat& src, Mat& dstImg)//上下切割
{
	int top, bottom;
	top = 0;
	bottom = src.rows;

	int i;
	for (i = 0; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);//统计所有行像素的总和
										 //cout <<i<<" th "<< colValue << endl;
		if (colValue>0)//扫描直到行像素的总和大于0时，记下当前位置top
		{
			top = i;
			break;
		}
	}
	for (; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);//统计所有行像素的总和
										 //cout << i << " th " << colValue << endl;
		if (colValue == 0)//继续扫描直到行像素的总和等于0时，记下当前位置bottom
		{
			bottom = i;
			break;
		}
	}

	int height = bottom - top;
	Rect rect(0, top, src.cols, height);
	dstImg = src(rect).clone();
}

int cutLeft(Mat& src, Mat& leftImg, Mat& rightImg)//左右切割
{
	int left, right;
	left = 0;
	right = src.cols;

	int i;
	for (i = 0; i < src.cols; i++)
	{
		int colValue = getColSum(src, i);//统计所有列像素的总和
										 //cout <<i<<" th "<< colValue << endl;
		if (colValue>0)//扫描直到列像素的总和大于0时，记下当前位置left
		{
			left = i;
			break;
		}
	}
	if (left == 0)
	{
		return 1;
	}

	//继续扫描
	for (; i < src.cols; i++)
	{
		int colValue = getColSum(src, i);//统计所有列像素的总和
										 //cout << i << " th " << colValue << endl;
		if (colValue == 0)//继续扫描直到列像素的总和等于0时，记下当前位置right
		{
			right = i;
			break;
		}
	}
	int width = right - left;//分割图片的宽度则为right - left
	Rect rect(left, 0, width, src.rows);//构造一个矩形，参数分别为矩形左边顶部的X坐标、Y坐标，右边底部的X坐标、Y坐标（左上角坐标为0，0）
	leftImg = src(rect).clone();
	Rect rectRight(right, 0, src.cols - right, src.rows);//分割后剩下的原图
	rightImg = src(rectRight).clone();
	cutTop(leftImg, leftImg);//上下切割
	return 0;
}


void getPXSum(Mat &src, int &a)//获取所有像素点和
{
	threshold(src, src, 100, 255, CV_THRESH_BINARY);
	a = 0;
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			a += src.at <uchar>(i, j);
		}
	}
}

int  getSubtract(Mat &src, int TemplateNum) //数字识别
{
	Mat img_result;
	int min = 1000000;
	int serieNum = 0;
	for (int i = 0; i < TemplateNum; i++) {
		char name[20];
		sprintf_s(name, "D:\\1\\%dLeft.jpg", i);
		Mat Template = imread(name, CV_LOAD_IMAGE_GRAYSCALE);//读取模板
		threshold(Template, Template, 100, 255, CV_THRESH_BINARY);
		threshold(src, src, 100, 255, CV_THRESH_BINARY);
		resize(src, src, Size(32, 48), 0, 0, CV_INTER_LINEAR);
		resize(Template, Template, Size(32, 48), 0, 0, CV_INTER_LINEAR);//调整尺寸
																		//imshow(name, Template);

																		/*让需要匹配的图分别和10个模板对应像素点值相减，然后求返回图片的整个图片的像素点值得平方和，和哪个模板匹配时候返回图片的平方和最小则就可以得到结果*/
		absdiff(Template, src, img_result);//AbsDiff，OpenCV中计算两个数组差的绝对值的函数。
		int diff = 0;
		getPXSum(img_result, diff);//获取所有像素点和
		if (diff < min)//像素点对比
		{
			min = diff;
			serieNum = i;
		}
	}

	printf("最小距离是%d ", min);
	printf("匹配到第%d个模板匹配的数字是%d\n", serieNum, serieNum);
	return serieNum;
}






int main()
{

	Mat src = imread("D:\\12\\18.jpg", CV_LOAD_IMAGE_GRAYSCALE);//读取图片
	//Mat src1 = imread("C:\\Users\\hzxuxin\\Downloads\\标定1\\正\\1.bmp");
	resize(src, src, Size(122, 122), 1, 1);
	Mat leftImg, rightImg;
	int res = cutLeft(src, leftImg, rightImg);
	imwrite("D:\\Digital_template\\17.jpg", leftImg);
	/*
	Mat element;
	threshold(src, src, 1, 255, THRESH_BINARY + THRESH_OTSU);
	element = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(src, src, MORPH_CLOSE, element,Point(-1,-1),2);
	medianBlur(src, src, 3);*/
	return 0;
}
