#include<iostream>
using namespace std;
#include<opencv2\opencv.hpp>
#include<opencv2\core\core.hpp>
using namespace cv;
#define WHITE   255
#define BLACK   0
void NaiveRemoveNoise(int pNum, Mat* img);


int main(int argc, char **argv)
{


	VideoCapture capture("/home/dale/cvrviz/water.avi");
	if (!capture.isOpened())
	{
		//error in opening the video input
		printf("could not load image..\n");
		return false;
	}
	Mat frame, hsv, mask, cornerpoint;
	while (ros::ok())
	{
		capture >> frame;
		if (frame.empty())
			break;

		cvtColor(frame, hsv, COLOR_BGR2HSV);
		inRange(hsv, Scalar(0, 43, 46), Scalar(26, 255, 255), mask);
		threshold(hsv, hsv, 70, 230, CV_THRESH_BINARY);
		//闭操作联通+膨胀
		dilate(mask, mask, Mat(), Point(-1, -1), 14);
		erode(mask, mask, Mat(), Point(-1, -1), 19);
		dilate(mask, mask, Mat(), Point(-1, -1), 3);

		vector<vector<Point> > cont;
		findContours(mask, cont, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		Mat draw = Mat::zeros(mask.size(), CV_8UC3);
		// imshow("mask", mask);
		vector<RotatedRect> rect(cont.size());
		Mat savepoint[4];

		Point2f dot;
		vector<Point2f> savedots;

		if (cont.size() > 1)
		{
			for (int i = 0; i < cont.size(); i++)
			{
				//找出最小旋转矩形
				rect[i] = minAreaRect(cont[i]);
				//定义矩形的4个顶点
				Point2f vertices[4];
				// 排除上面灯管干扰，计算矩形的4个顶点
				if (rect[i].center.y > 80)
				{

					rect[i].points(vertices);
				}
				for (int j = 0; j < 4; j++)
				{
					line(frame, vertices[j], vertices[(j + 1) % 4], Scalar(255, 255, 0), 0.03);
					//提取corner points
					if (vertices[j].x > 0 && abs(vertices[j].y - vertices[(j + 1) % 4].y) < 15)
					{

						dot = Point2f((vertices[j].x + vertices[(j + 1) % 4].x) / 2, (vertices[j].y + vertices[(j + 1) % 4].y) / 2);
						savedots.push_back(dot);
						// cout << dot << endl;
						circle(draw, dot, 1, Scalar(255, 0, 255), 2, 8, 0);
					}
				}
			}
			imshow("frame", frame);
			imshow("draw", draw);
			//多甲板的话，可以对一个矩形临近点进行距离限定
		}
		int keyboard = waitKey(10);
		if (keyboard == 'q' || keyboard == 27)
			break;
	}

	capture.release();
	return (0);
}


