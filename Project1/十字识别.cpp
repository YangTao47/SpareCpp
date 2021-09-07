#include<iostream>
#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;
void Dectector(Mat &frame);

int main()
{
	Mat image = imread("C:\\Users\\hzxuxin\\Downloads\\��Ʒ\\ƫ����\\2.bmp");
	Mat copyImage;
	image.copyTo(copyImage);
	Mat gray_image;
	Mat creatPicture(image.rows, image.cols, CV_8UC3, Scalar(255, 255, 255));
	//cout << image.cols << " " << creatPicture.cols << endl;
	//cout << image.rows << " " << creatPicture.rows << endl;
	resize(image, image, Size(0, 0), 0.5, 0.5, CV_INTER_LINEAR);
	cvtColor(image, gray_image, CV_RGB2GRAY);

	//�˲�����
	Mat blur_image;
	GaussianBlur(gray_image, blur_image, Size(3, 3), 0, 0);
	//Canny��Ե���
	Mat canny_image;
	Canny(blur_image, canny_image, 100, 150, 3);
	//��������
	Mat contour_image;
	contour_image = canny_image.clone();
	vector<vector<Point>>contours;
	findContours(contour_image, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
	//��������
	drawContours(contour_image, contours, -1, Scalar(255), 1);
	//�����հ�ͼ����Ϊǰ��
	//����
	vector<Point> rectPoint;
	for (int i = 0; i < contours.size(); i++)
	{
		//��þ������Χ��
		Rect r = boundingRect(Mat(contours[i]));

		//RotatedRect r = minAreaRect(Mat(contours[i]));
		//cout << "contours" << i << "height=" << r.height << "width =" << r.width << "rate =" << ((float)r.width / r.height) << endl;

		//���ݾ��ο�߱Ⱥ����ɸѡ����
		if ((float)r.width / r.height >= 0.9 && (float)r.width / r.height <= 1.2 && (float)r.width * r.height >= 9000 && (float)r.width * r.height <= 15000)
		{
			cout << "i" << i << endl;
			cout << "r.x = " << r.x << "  r.y  = " << r.y << "rate =" << ((float)r.width / r.height) << " area = " << ((float)r.width * r.height) << endl;

			Point p1, p2, p3, p4;
			p1.x = r.x;
			p1.y = r.y;
			p2.x = r.x + r.width;
			p2.x = r.y;
			p3.x = r.x + r.width;
			p3.y = r.y + r.height;
			p4.x = r.x;
			p4.y = r.y + r.height;
			rectPoint.push_back(p1);
			rectPoint.push_back(p2);
			rectPoint.push_back(p3);
			rectPoint.push_back(p4);
			//������
			rectangle(image, r, Scalar(0, 0, 255), 2);
			//cout << rectPoint.front() << endl;

		}
	}

	


	namedWindow("���", CV_WINDOW_NORMAL);
	cvNamedWindow("����", CV_WINDOW_NORMAL);
	imshow("����", copyImage);
	imshow("���", image);
	
	waitKey(0);
	system("pause");
	return 0;
}
void Dectector(Mat &frame)
{
	Mat gray_image;
	cvtColor(frame, gray_image, CV_RGB2GRAY);

	//�˲�����
	Mat blur_image;
	GaussianBlur(gray_image, blur_image, Size(3, 3), 0, 0);
	//Canny��Ե���
	Mat canny_image;
	Canny(blur_image, canny_image, 100, 150, 3);
	imshow("Canny", canny_image);

	//��������
	Mat contour_image;
	contour_image = canny_image.clone();
	vector<vector<Point>>contours;
	findContours(contour_image, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);

	//��������
	drawContours(contour_image, contours, -1, Scalar(255), 1);

	//����

	vector<Point> rectPoint;
	for (int i = 0; i < contours.size(); i++)
	{
		//��þ������Χ��
		Rect r = boundingRect(Mat(contours[i]));

		//RotatedRect r = minAreaRect(Mat(contours[i]));
		//cout << "contours" << i << "height=" << r.height << "width =" << r.width << "rate =" << ((float)r.width / r.height) << endl;

		//���ݾ��ο�߱Ⱥ����ɸѡ����
		if ((float)r.width / r.height >= 1.5 && (float)r.width / r.height <= 2.2 && (float)r.width * r.height >= 3000 && (float)r.width * r.height <= 10000)
		{
			cout << "r.x = " << r.x << "  r.y  = " << r.y << "rate =" << ((float)r.width / r.height) << " area = " << ((float)r.width * r.height) << endl;

			Point p1, p2, p3, p4;
			p1.x = r.x;
			p1.y = r.y;
			p2.x = r.x + r.width;
			p2.x = r.y;
			p3.x = r.x + r.width;
			p3.y = r.y + r.height;
			p4.x = r.x;
			p4.y = r.y + r.height;
			rectPoint.push_back(p1);
			rectPoint.push_back(p2);
			rectPoint.push_back(p3);
			rectPoint.push_back(p4);
			//������
			rectangle(frame, r, Scalar(0, 0, 255), 2);

		}
	}
	namedWindow("���", CV_WINDOW_NORMAL);
	imshow("���", frame);
}


