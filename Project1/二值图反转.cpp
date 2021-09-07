#include<iostream>
#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;


void reversalImage();

int main()
{

	TemplateMatch();
	system("pause");
	return 0;
}

void reversalImage()
{

	Mat img_result;
	for (int i = 0; i < 10; i++)
	{
		char name[200];
		sprintf_s(name, "D:\\templates\\%d.bmp", i);
		Mat Template = imread(name, CV_LOAD_IMAGE_GRAYSCALE);
		uchar lutData[256];
		for (int i = 0; i < 256; i++)
		{
			lutData[i] = 255-i;
		}
		Mat lut(1, 256, CV_8UC1, lutData);
		LUT(Template, lut, img_result);
		int j = i + 10;
		char name1[200];
		sprintf_s(name1, "D:\\templates\\%d.bmp", j);
		imwrite(name1, img_result);
		
	}
	
}
