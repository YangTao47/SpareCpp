#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
#include"CellBoardCoordinates.h"
using namespace cv;
using namespace std;

class DigitalRecognition
{
public:

	vector<CellBoardCoordinates> extractCellCoordinates(const Mat& Image);

private:

	struct roiRectInfo
	{
		vector<Rect> roi_rect_vector;
		vector<Mat> roi;
	}roi_rect_info;

	const int connectedComponentsArea1 = 50000;
	const int connectedComponentsArea2 = 50000;
	const int expandPixels = 200;
	const int rect_width = 5;
	const int rect_height = 10;
	float small_scale = 0.5f;
	float big_scale = 1.f / small_scale;
	const int roi_scale = 140;

	Point getBoardCoordinates(const Mat& src, vector<vector<Point>> P, int i);
	Point composeNumbers(int num1, int num2, int num3, int num4);
	int getRowSum(Mat src, int row);
	int getColSum(Mat src, int col);
	int matchTemplates(const Mat& src, int nums);
	int cutLeft(Mat& src, Mat& leftImg, Mat& rightImg);
	void cutTop(Mat& src, Mat& dstImg);

	void findCellBox1(const Mat& src, vector<vector<Point>>& grid_corner);
	void reverseImage(const Mat& src, Mat& dst);
	void findNoiseMask(const Mat& src, int area, Mat& mask);
	void denoiseImage(const Mat& reverse_image, const Mat& binary_image, Mat& denoise_image);
	Mat computeScoreMap(Mat& gray_image, Size& ksize1, Size& ksize2);
	void replacePixels(Mat& image, bool dst1 = true);
	void computeCrossPoint(const Mat& src, vector<Point>& cross_points);
	void sortCorners(vector<Point> corners, Mat& showimage, vector<vector<Point>>& grid_corner);
};
