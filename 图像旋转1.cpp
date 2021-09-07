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


//��ת���� ��ת����Ϊ��ʱ��Ϊ��
Mat RotateImg(Mat image, double angle)
{
	/*
	����ת�Ľ��иĽ�������ͼ����һ�����Σ���ת�����ͼ�����״��һ��ԭͼ�����Ӿ���
	�����Ҫ���¼������ת���ͼ�εĿ�͸�
	*/
	int width = image.cols;
	int height = image.rows;

	double radian = angle * CV_PI / 180.;//�Ƕ�ת��Ϊ���� sin/cos �����û��ȼ���
	double width_rotate = fabs(width*cos(radian)) + fabs(height*sin(radian));
	double height_rotate = fabs(width*sin(radian)) + fabs(height*cos(radian));

	/* ��ת���� ԭͼ�����ĵ� */
	cv::Point2f center((float)width / 2.0, (float)height / 2.0);
	/* ��ת����  ֻ�ǽ�������ת��û���ƶ� */
	Mat m1 = cv::getRotationMatrix2D(center, angle, 1.0);
	/* m1Ϊ2��3��ͨ����Ϊ1�ľ��� �任��������ĵ��൱��ƽ��һ��
	ԭͼ������ĵ�����ͼ������ĵ�����λ�ã�ԭͼ�����ĵ���Ҫƽ�Ƶ����� ���ĵ㣨x1��y1����x��y��  x1-x ����y1-y */
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
	double alpha = (360 - angle) * CV_PI / 180.;//�Ƕ�ת��Ϊ���� sin/cos �����û��ȼ���
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
	//�����е�����
	int centerX = nColsSrc / 2;  //200
	int centerY = nRowsSrc / 2;  //112
								 // �����˳ʱ����ת
	if (!direction)
		theta = 2 * CV_PI - theta;
	// ȫ������ʱ����ת������
	// ��ʱ����ת����
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
	// �������ת��ͼ��ļ�ֵ��ͳߴ�
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
	// �������ͼ��
	cv::Mat matRet(nRows, nCols, matSrc.type(), cv::Scalar(0));
	for (int j = 0; j < nRows; j++)
	{
		for (int i = 0; i < nCols; i++)
		{
			// ��������ͼ����ԭͼ���еĶ�Ӧ������꣬Ȼ���Ƹ�����ĻҶ�ֵ
			// ��Ϊ����ʱ��ת������������ӳ�䵽ԭͼ���ʱ����Կ����ǣ����ͼ��
			// ��˳ʱ����ת��ԭͼ��ģ���˳ʱ����ת����պ�����ʱ����ת�����ת��
			// ͬʱ��Ҫ���ǵ�Ҫ����ת���ͼ������Ͻ��ƶ�������ԭ�㡣
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
