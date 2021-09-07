#include<iostream>
#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;

int getColSum(Mat src, int col)//ͳ�����������ص��ܺ�
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

int getRowSum(Mat src, int row)//ͳ�����������ص��ܺ�
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


void cutTop(Mat& src, Mat& dstImg)//�����и�
{
	int top, bottom;
	top = 0;
	bottom = src.rows;

	int i;
	for (i = 0; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);//ͳ�����������ص��ܺ�
										 //cout <<i<<" th "<< colValue << endl;
		if (colValue>0)//ɨ��ֱ�������ص��ܺʹ���0ʱ�����µ�ǰλ��top
		{
			top = i;
			break;
		}
	}
	for (; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);//ͳ�����������ص��ܺ�
										 //cout << i << " th " << colValue << endl;
		if (colValue == 0)//����ɨ��ֱ�������ص��ܺ͵���0ʱ�����µ�ǰλ��bottom
		{
			bottom = i;
			break;
		}
	}

	int height = bottom - top;
	Rect rect(0, top, src.cols, height);
	dstImg = src(rect).clone();
}

int cutLeft(Mat& src, Mat& leftImg, Mat& rightImg)//�����и�
{
	int left, right;
	left = 0;
	right = src.cols;

	int i;
	for (i = 0; i < src.cols; i++)
	{
		int colValue = getColSum(src, i);//ͳ�����������ص��ܺ�
										 //cout <<i<<" th "<< colValue << endl;
		if (colValue>0)//ɨ��ֱ�������ص��ܺʹ���0ʱ�����µ�ǰλ��left
		{
			left = i;
			break;
		}
	}
	if (left == 0)
	{
		return 1;
	}

	//����ɨ��
	for (; i < src.cols; i++)
	{
		int colValue = getColSum(src, i);//ͳ�����������ص��ܺ�
										 //cout << i << " th " << colValue << endl;
		if (colValue == 0)//����ɨ��ֱ�������ص��ܺ͵���0ʱ�����µ�ǰλ��right
		{
			right = i;
			break;
		}
	}
	int width = right - left;//�ָ�ͼƬ�Ŀ����Ϊright - left
	Rect rect(left, 0, width, src.rows);//����һ�����Σ������ֱ�Ϊ������߶�����X���ꡢY���꣬�ұߵײ���X���ꡢY���꣨���Ͻ�����Ϊ0��0��
	leftImg = src(rect).clone();
	Rect rectRight(right, 0, src.cols - right, src.rows);//�ָ��ʣ�µ�ԭͼ
	rightImg = src(rectRight).clone();
	cutTop(leftImg, leftImg);//�����и�
	return 0;
}


void getPXSum(Mat &src, int &a)//��ȡ�������ص��
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

int  getSubtract(Mat &src, int TemplateNum) //����ʶ��
{
	Mat img_result;
	int min = 1000000;
	int serieNum = 0;
	for (int i = 0; i < TemplateNum; i++) {
		char name[20];
		sprintf_s(name, "D:\\1\\%dLeft.jpg", i);
		Mat Template = imread(name, CV_LOAD_IMAGE_GRAYSCALE);//��ȡģ��
		threshold(Template, Template, 100, 255, CV_THRESH_BINARY);
		threshold(src, src, 100, 255, CV_THRESH_BINARY);
		resize(src, src, Size(32, 48), 0, 0, CV_INTER_LINEAR);
		resize(Template, Template, Size(32, 48), 0, 0, CV_INTER_LINEAR);//�����ߴ�
																		//imshow(name, Template);

																		/*����Ҫƥ���ͼ�ֱ��10��ģ���Ӧ���ص�ֵ�����Ȼ���󷵻�ͼƬ������ͼƬ�����ص�ֵ��ƽ���ͣ����ĸ�ģ��ƥ��ʱ�򷵻�ͼƬ��ƽ������С��Ϳ��Եõ����*/
		absdiff(Template, src, img_result);//AbsDiff��OpenCV�м������������ľ���ֵ�ĺ�����
		int diff = 0;
		getPXSum(img_result, diff);//��ȡ�������ص��
		if (diff < min)//���ص�Ա�
		{
			min = diff;
			serieNum = i;
		}
	}

	printf("��С������%d ", min);
	printf("ƥ�䵽��%d��ģ��ƥ���������%d\n", serieNum, serieNum);
	return serieNum;
}






int main()
{

	Mat src = imread("D:\\12\\18.jpg", CV_LOAD_IMAGE_GRAYSCALE);//��ȡͼƬ
	//Mat src1 = imread("C:\\Users\\hzxuxin\\Downloads\\�궨1\\��\\1.bmp");
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
