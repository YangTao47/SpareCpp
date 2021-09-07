#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
using namespace cv;
using namespace std;

int thresh = 30;
int main()
{
	Point2f p1;
	Mat g_srcImage1, g_grayImage;
	Mat scaledImage, CannyImg, box;
	vector<Point2i> p;
	vector<Vec4i> Lines;
	double hminVal = 0, hmaxVal = 0;
	double wminVal = 0, wmaxVal = 0;
	float circle_radius = 0;
	Mat g_srcImage = imread("C:\\Users\\hzxuxin\\Downloads\\标定1\\左移\\1.bmp");
	Mat create_img(g_srcImage.rows, g_srcImage.cols,CV_8UC3, Scalar(0, 0, 0));
	g_srcImage1 = g_srcImage.clone();
	cvtColor(g_srcImage1, g_grayImage, COLOR_BGR2GRAY);
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	adaptiveThreshold(g_grayImage, g_grayImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 1333, 1);
	morphologyEx(g_grayImage, g_grayImage, MORPH_CLOSE, element, Point(-1, -1), 13);
	morphologyEx(g_grayImage, g_grayImage, MORPH_OPEN, element, Point(-1, -1), 25);
	Canny(g_grayImage, CannyImg, 140, 250, 3);
	/*********************************************************************************************************/
	/* 角点检测 */
	vector<vector<Point2f>>P;
	vector<Point2f> corners;//提供初始角点的坐标位置和精确的坐标的位置
	int maxcorners = 200;
	double qualityLevel = 0.5;  //角点检测可接受的最小特征值
	double minDistance = 50;	//角点之间最小距离
	int blockSize = 3;//计算导数自相关矩阵时指定的领域范围
	double  k = 0.04; //权重系数
	goodFeaturesToTrack(CannyImg, corners, maxcorners, qualityLevel, minDistance, Mat(), blockSize, false, k);
	for (int i = 0; i < corners.size(); i++)
	{
		circle(g_srcImage, corners[i], 10, Scalar(0,0,255), -1, 8, 0);
		//cout << "角点坐标：" << corners[i] << endl;
	}


	for (int i = 0; i < corners.size(); i++)
	{
		vector<Point2f>p1;
		for (int j = 0; j < corners.size(); j++)
		{
			if (corners[i].x - corners[j].x > 650 && corners[i].x - corners[j].x < 700 && corners[i].y - corners[j].y > 650 && corners[i].y - corners[j].y < 700)
			{
				p1.push_back(corners[i]);
				p1.push_back(corners[j]);
				P.push_back(p1);
			}
			
		}
	}
	/*P[i][3]是右上角，P[i][1]是左上角，P[i][0]是右下角，P[i][2]左下角*/
	for (int i = 0; i < P.size(); i++)
	{
		Point2f p1;
		for (int j = 0; j < corners.size(); j++) 
		{
			if (P[i][1].x- corners[j].x> -700 && P[i][1].x - corners[j].x < -650 && P[i][1].y - corners[j].y < 20 && P[i][1].y - corners[j].y > -20)
			{
				P[i].push_back(corners[j]);
			}
			if (P[i][1].x - corners[j].x> -20 && P[i][1].x - corners[j].x < 20 && P[i][1].y - corners[j].y < -650 && P[i][1].y - corners[j].y > -700)
			{
				P[i].push_back(corners[j]);
			}
		}
		cout << P[i] << endl;
	}
	for (int i = 0; i < P.size(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			circle(g_srcImage1, P[i][j], 10, Scalar(0, 0, 255), -1, 8, 0);
		}
	}
	//cout <<"完整方格数量"<< P.size() << endl;
	return 0;
}





/*  利用透视变换先把提出来的矩形弄正了 再进行数字识别*/