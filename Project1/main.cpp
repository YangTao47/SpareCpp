#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
#include"DigitalRecognition.h"
#include"CellBoardCoordinates.h"
using namespace cv;
using namespace std;
using namespace std::chrono;

int main()
{
	DigitalRecognition test;
	vector<CellBoardCoordinates> cell_board_point_vector;
	Mat image = imread("C:/Users/hzxuxin/Downloads/�궨1/ƫ��/2.bmp", CV_8UC1);
	auto start_time = high_resolution_clock::now();

	cell_board_point_vector = test.extractCellCoordinates(image);
	for (int i = 0; i < cell_board_point_vector.size(); i++)
	{
		cout << cell_board_point_vector[i].boardCoordinates << endl;
		cout << cell_board_point_vector[i].cellCoordinates << endl;
	}

	time_point<high_resolution_clock> end_time = high_resolution_clock::now();
	milliseconds processing_time = duration_cast<milliseconds>(end_time - start_time);
	cout << "��ʱ" << processing_time.count() << "����" << endl;
	system("pause");
	return 0;
}