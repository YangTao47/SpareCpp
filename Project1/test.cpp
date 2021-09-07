#include <iostream>
#include <mutex>  // ���� std::unique_lock
#include <shared_mutex>
#include <thread>
#include<opencv2\opencv.hpp>
using namespace cv;
using namespace std;

int main() {

	Mat image = imread("111.png", IMREAD_GRAYSCALE);
	vector<vector<Point>> contours;
	threshold(image, image, 1, 255, THRESH_BINARY + THRESH_OTSU);
	findContours(image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	Mat show_board = Mat::zeros(image.size(), CV_8UC1);
	drawContours(show_board, contours, -1, Scalar(255));
}

// ���ͣ���������ڵ��˻��������ɡ� thread1 ��ʼʱ�����״ν���ѭ�������� increment() ��
// ������ get() ��Ȼ���������ܴ�ӡ����ֵ�� std::cout ǰ���������� thread1 ��������
// ������ thread2 ������Ȼ���㹻ʱ��һ������ȫ������ѭ���������ٻص� thread1 ���������׸�
// ѭ�������У������մ�ӡ��ֲ��ļ�����������ֵ���� 1 �� std::cout ��������ʣ�¶���ѭ����
// ��˻����ϣ�û���̱߳��������ߣ������������Ϊ����˳��