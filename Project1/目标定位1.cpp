#include<iostream>
using namespace std;
#include<opencv2\opencv.hpp>
using namespace cv;
Mat read_Bmp();

int main()
{

	read_Bmp();

	system("pasue");
	return 0;
}

Mat read_Bmp()
{
	Mat dst1,mask;
	Mat image=imread("C:\\Users\\hzxuxin\\Pictures\\Saved Pictures\\lcd.bmp");
	image.copyTo(mask);
	//cout << image.size();
	image.copyTo(dst1);
	cvtColor(image, image, CV_BGR2GRAY);
	threshold(image, image, 21, 255, THRESH_BINARY + THRESH_OTSU);

	//Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	//morphologyEx(image, image, MORPH_CLOSE, element, Point(-1, -1), 13);
	//morphologyEx(image, image, MORPH_OPEN, element, Point(-1, -1), 15);
	vector<vector<Point>> contours;
	findContours(image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	cout << contours.size() << endl;
	drawContours(mask, contours, -1, Scalar(0, 0, 255), 5);
	RotatedRect rect = minAreaRect(contours[0]);
	Point2f P[4];
	rect.points(P);
	for (int j = 0; j <= 3; j++)
	{
		cout << P[j] << endl;
		//cout << P[(j + 1) % 4] << endl;
		line(dst1, P[j], P[(j + 1) % 4], Scalar(0, 0, 255), 10);
	}
	imwrite("result.jpg", dst1);
	return dst1;
}
