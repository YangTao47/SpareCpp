#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;
Mat RotateImg(Mat image, double angle);
Mat AngelRotate(Mat src, int angle);
Mat imgRotate(cv::Mat matSrc, float angle, bool direction);
int main()
{
	int angle;
	cin >> angle;
	Mat image = imread("lena.jpg");
	Mat imageout = RotateImg(image, angle);
	imshow("test", imageout);
	waitKey(0);
	getchar();

	return 0;
}


//旋转函数 旋转方向为逆时针为正
Mat RotateImg(Mat image, double angle)
{
	/*
	对旋转的进行改进，由于图形是一个矩形，旋转后的新图像的形状是一个原图像的外接矩形
	因此需要重新计算出旋转后的图形的宽和高
	*/
	int width = image.cols;
	int height = image.rows;

	double radian = angle * CV_PI / 180.;//角度转换为弧度 sin/cos 都是用弧度计算
	double width_rotate = fabs(width*cos(radian)) + fabs(height*sin(radian));
	double height_rotate = fabs(width*sin(radian)) + fabs(height*cos(radian));

	/* 旋转中心 原图像中心点 */
	cv::Point2f center((float)width / 2.0, (float)height / 2.0);
	/* 旋转矩阵  只是进行了旋转并没有移动 */
	Mat m1 = cv::getRotationMatrix2D(center, angle, 1.0);
	/* m1为2行3列通道数为1的矩阵 变换矩阵的中心点相当于平移一样
	原图像的中心点与新图像的中心点的相对位置（原图像中心点需要平移的量） 中心点（x1，y1）（x，y）  x1-x ，，y1-y */
	m1.at<double>(0, 2) += (width_rotate - width) / 2.;
	m1.at<double>(1, 2) += (height_rotate - height) / 2.;
	Mat imgOut;
	if (image.channels() == 1)
	{
		cv::warpAffine(image, imgOut, m1, Size(width, height), cv::INTER_LINEAR, 0, Scalar(0));
	}
	else if (image.channels() == 3)
	{
		cv::warpAffine(image, imgOut, m1, Size(width, height), cv::INTER_LINEAR, 0, Scalar(0, 0, 0));
	}
	return imgOut;
}


Mat AngelRotate(Mat src, int angle)
{
	double alpha = (360 - angle) * CV_PI / 180.;//角度转换为弧度 sin/cos 都是用弧度计算
	float rotateMat[3][3] = {
		{ cos(alpha), -sin(alpha), 0, },
		{ sin(alpha),  cos(alpha), 0, },
		{ 0,0,1 } };
	int nSrcRows = src.rows;
	int nSrcCols = src.cols;


	float a1 = nSrcCols * rotateMat[0][0];
	float b1 = nSrcCols * rotateMat[1][0];
	float a2 = nSrcCols * rotateMat[0][0] + nSrcRows *rotateMat[0][1];
	float b2 = nSrcCols * rotateMat[1][0] + nSrcRows *rotateMat[1][1];
	float a3 = nSrcRows * rotateMat[0][1];
	float b3 = nSrcRows * rotateMat[1][1];

	float kxMin = min(min(min(0.0f, a1), a2), a3);
	float kxMax = max(max(max(0.0f, a1), a2), a3);
	float kyMin = min(min(min(0.0f, b1), b2), b3);
	float kyMax = max(max(max(0.0f, b1), b2), b3);
	cout << kxMax << endl;
	cout << kxMin << endl;
	cout << kyMax << endl;
	cout << kyMin << endl;
	int width = src.cols;
	int height = src.rows;


	double width_rotate = fabs(width*cos(alpha)) + fabs(height*sin(alpha));
	double height_rotate = fabs(width*sin(alpha)) + fabs(height*cos(alpha));

	Mat dst(height_rotate, width_rotate, src.type(), Scalar(0));
	for (int i = 0; i < height_rotate; i++)
	{
		for (int j = 0; j < width_rotate; j++)
		{
			int x = (j + kxMin)* rotateMat[0][0] - (i + kyMin) * rotateMat[0][1];
			int y = -(j + kxMin) * rotateMat[1][0] + (i + kyMin) * rotateMat[1][1];
			if (x >= 0 && x < nSrcCols && y >= 0 && y < nSrcRows)
			{
				dst.at<Vec3b>(i, j) = src.at<Vec3b>(y, x);
			}

		}
	}
	return dst;
}

Mat imgRotate(Mat matSrc, float angle, bool direction)
{
	float theta = angle * CV_PI / 180.0;
	int nRowsSrc = matSrc.rows;
	int nColsSrc = matSrc.cols;
	//创建中点坐标
	int centerX = nColsSrc / 2;  //200
	int centerY = nRowsSrc / 2;  //112
								 // 如果是顺时针旋转
	if (!direction)
		theta = 2 * CV_PI - theta;
	// 全部以逆时针旋转来计算
	// 逆时针旋转矩阵
	float matRotate[3][3]{
		{ std::cos(theta), -std::sin(theta), 0 },
		{ std::sin(theta), std::cos(theta), 0 },
		{ 0, 0, 1 }
	};
	float pt[4][2]{
		{ 0, (float)nRowsSrc },
		{ (float)nColsSrc, (float)nRowsSrc },
		{ (float)nColsSrc, 0 }
	};
	for (int i = 0; i < 4; i++)
	{
		float x = pt[i][0]  * matRotate[0][0] + pt[i][1]  * matRotate[1][0];
		float y = pt[i][0]  * matRotate[0][1] + pt[i][1]  * matRotate[1][1];
		pt[i][0] = x;
		pt[i][1] = y;
	}
	// 计算出旋转后图像的极值点和尺寸
	float fMin_x = min(min(min(pt[0][0], pt[1][0]), pt[2][0]), (float)0.0);
	float fMin_y = min(min(min(pt[0][1], pt[1][1]), pt[2][1]), (float)0.0);
	float fMax_x = max(max(max(pt[0][0], pt[1][0]), pt[2][0]), (float)0.0);
	float fMax_y = max(max(max(pt[0][1], pt[1][1]), pt[2][1]), (float)0.0);
	int nRows = cvRound(fMax_y - fMin_y + 0.5) + 1;
	int nCols = cvRound(fMax_x - fMin_x + 0.5) + 1;
	int nMin_x = cvRound(fMin_x + 0.5);
	int nMin_y = cvRound(fMin_y + 0.5);

	cout << fMin_x << endl;
	cout << fMin_y << endl;
	// 拷贝输出图像
	cv::Mat matRet(nRows, nCols, matSrc.type(), cv::Scalar(0));
	for (int j = 0; j < nRows; j++)
	{
		for (int i = 0; i < nCols; i++)
		{
			// 计算出输出图像在原图像中的对应点的坐标，然后复制该坐标的灰度值
			// 因为是逆时针转换，所以这里映射到原图像的时候可以看成是，输出图像
			// 到顺时针旋转到原图像的，而顺时针旋转矩阵刚好是逆时针旋转矩阵的转置
			// 同时还要考虑到要把旋转后的图像的左上角移动到坐标原点。
			int x = (i + nMin_x) * matRotate[0][0] + (j + nMin_y) * matRotate[0][1];
			int y = (i + nMin_x) * matRotate[1][0] + (j + nMin_y) * matRotate[1][1];
			if (x >= 0 && x < nColsSrc && y >= 0 && y < nRowsSrc)
			{
				matRet.at<cv::Vec3b>(j, i) = matSrc.at<cv::Vec3b>(y, x);
			}
		}
	}
	return matRet;
}
