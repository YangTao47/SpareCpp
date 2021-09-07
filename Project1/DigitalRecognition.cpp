#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
#include"DigitalRecognition.h"
using namespace cv;
using namespace std;

vector<CellBoardCoordinates> DigitalRecognition::extractCellCoordinates(const Mat& Image)
{
	CellBoardCoordinates cell_board_point;
	vector<CellBoardCoordinates> cell_board_point_vector;
	vector<vector<Point>> grid_corner;
	Point cell_point;
	Point board_point;
	findCellBox1(Image, grid_corner);
	for (int i = 0; i < grid_corner.size(); i++)
	{
		cell_point = getBoardCoordinates(Image, grid_corner, i);
		board_point = grid_corner[i][0];
		cell_board_point.cellCoordinates = cell_point;
		cell_board_point.boardCoordinates = board_point;
		cell_board_point_vector.push_back(cell_board_point);
	}
	return cell_board_point_vector;
}

void DigitalRecognition::findCellBox1(const Mat& src, vector<vector<Point>>& grid_corner)
{
	Mat gray_image, binary_image, reverse_image, denoise_image, output, small_image;
	vector<Point> cross_point;
	CV_Assert(src.channels() == 1);
	//Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	medianBlur(src, gray_image, 13);
	//morphologyEx(gray_image, gray_image, MORPH_CLOSE, element, Point(-1, -1), 3);
	adaptiveThreshold(gray_image, binary_image, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 1333, 1);
	reverseImage(binary_image, reverse_image);
	copyMakeBorder(reverse_image, reverse_image, expandPixels, expandPixels, expandPixels, expandPixels, BORDER_REPLICATE);
	copyMakeBorder(binary_image, binary_image, expandPixels, expandPixels, expandPixels, expandPixels, BORDER_REPLICATE);

	denoiseImage(reverse_image, binary_image, denoise_image);
	resize(denoise_image, small_image, Size(int(small_scale * denoise_image.cols), int(small_scale * denoise_image.rows)));
	threshold(small_image, small_image, 1, 255, THRESH_BINARY + THRESH_OTSU);

	output = computeScoreMap(small_image, Size(roi_scale, roi_scale / 2), Size(roi_scale / 2, roi_scale));
	resize(output, output, Size(int(big_scale * output.cols), int(big_scale * output.rows)));
	computeCrossPoint(output, cross_point);
	sortCorners(cross_point, gray_image, grid_corner);

	/*for (int i = 0; i < cross_point.size(); i++) {
	circle(gray_image, cross_point[i], 10, Scalar(255), 5);
	}*/
}

/*黑底白噪图像，默认黑色为背景色  有必要缩小取噪声吗？？*/
void DigitalRecognition::findNoiseMask(const Mat& src, int area, Mat& mask)
{
	Mat labels, stats, centroids;
	CV_Assert(src.channels() == 1);
	int num = connectedComponentsWithStats(src, labels, stats, centroids, 4);
	vector<int> p(num);
	for (int m = 0; m < num; m++) {
		if (stats.at<int>(m, CC_STAT_AREA) < area) {
			p[m] = 255;
		}
		else
		{
			p[m] = 0;
		}
	}
	for (int y = 0; y < mask.rows; y++)
	{
		for (int x = 0; x < mask.cols; x++)
		{
			int label = labels.at<int>(y, x);//注意labels是int型，不是uchar
			mask.at<uchar>(y, x) = p[label];
		}
	}
}

void DigitalRecognition::reverseImage(const Mat& src, Mat& dst)
{
	Mat img_result;
	uchar lutData[256];
	for (int i = 0; i < 256; i++)
	{
		lutData[i] = 255 - i;
	}
	Mat lut(1, 256, CV_8UC1, lutData);
	LUT(src, lut, dst);
}

void DigitalRecognition::denoiseImage(const Mat& reverse_image, const Mat& binary_image, Mat& denoise_image)
{
	Mat mask(reverse_image.rows, reverse_image.cols, CV_8UC1);
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	findNoiseMask(reverse_image, connectedComponentsArea1, mask);
	Mat mv_black_noise = binary_image + mask;
	morphologyEx(mv_black_noise, mv_black_noise, MORPH_CLOSE, element, Point(-1, -1), 10);

	findNoiseMask(mv_black_noise, connectedComponentsArea2, mask);
	Mat mv_white_noise = mv_black_noise - mask;
	morphologyEx(mv_white_noise, denoise_image, MORPH_CLOSE, element, Point(-1, -1), 5);
}

void DigitalRecognition::computeCrossPoint(const Mat& src, vector<Point>& cross_points)
{
	Mat canny_image;
	int rows = src.rows;
	int cols = src.cols;
	vector<Point> temp;
	vector<vector<Point>> contours;
	threshold(src, src, 200, 255, THRESH_BINARY);
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			if (src.at<uchar>(r, c) == 255) {
				temp.push_back(Point(c, r));
			}
		}
	}
	//sort(temp.begin(), temp.end(), cmp);
	Canny(src, canny_image, 1, 3);
	findContours(canny_image, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++) {
		Point2f temp;
		float temp_offset = (float)expandPixels;
		float temp_radiu;
		minEnclosingCircle(contours[i], temp, temp_radiu);
		// 将原点移回原图左上角
		cross_points.push_back(temp - Point2f(temp_offset, temp_offset));
	}
}

Mat DigitalRecognition::computeScoreMap(Mat& gray_image, Size& ksize1, Size& ksize2)
{
	Mat cache_mat1, cache_mat2, dst;
	gray_image.convertTo(gray_image, CV_32F);
	blur(gray_image, cache_mat1, ksize1);
	blur(gray_image, cache_mat2, ksize2);

	replacePixels(cache_mat1);

	replacePixels(cache_mat2, false);

	dst = cache_mat1 & cache_mat2;
	return dst;
}


/* 图片不能过度旋转 */
void DigitalRecognition::replacePixels(Mat& image, bool dst1)
{
	int rows = image.rows;
	int cols = image.cols;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (image.at<float>(i, j) < 110 || image.at<float>(i, j) > 134) {
				image.at<float>(i, j) = 0;
			}
		}
	}
	threshold(image, image, 100, 255, THRESH_BINARY);
	image.convertTo(image, CV_8UC1);

	if (dst1) {
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				if (i + roi_scale >= rows) {
					if ((image.at<uchar>(i, j) - image.at<uchar>(rows - 1, j) != 0) || (image.at<uchar>(i, j) - image.at<uchar>(i - roi_scale, j) == 255)) {
						image.at<uchar>(i, j) = 0;
					}
				}
				else if (image.at<uchar>(i, j) - image.at<uchar>(i + roi_scale, j) != 0) {
					image.at<uchar>(i, j) = 0;
				}
			}
		}
	}

	else {
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				if (j + roi_scale >= cols) {
					if ((image.at<uchar>(i, j) - image.at<uchar>(i, cols - 1) != 0) || (image.at<uchar>(i, j) - image.at<uchar>(i, j - roi_scale) == 255)) {
						image.at<uchar>(i, j) = 0;
					}
				}
				else if (image.at<uchar>(i, j) - image.at<uchar>(i, j + roi_scale) != 0) {
					image.at<uchar>(i, j) = 0;
				}
			}
		}
	}

}

void DigitalRecognition::sortCorners(vector<Point> corners, Mat& showimage, vector<vector<Point>>& grid_corner)
{
	vector<vector<Point>>P;
	//寻找左上右上
	for (int i = 0; i < corners.size(); i++)
	{
		vector<Point>p1;
		for (int j = 0; j < corners.size(); j++)
		{
			if (corners[i].x - corners[j].x> -720 && corners[i].x - corners[j].x < -640 && corners[i].y - corners[j].y < 25 && corners[i].y - corners[j].y > -25)
			{
				p1.push_back(corners[i]);
				p1.push_back(corners[j]);
				P.push_back(p1);
			}
		}
	}
	for (int i = 0; i < P.size(); i++) {

		circle(showimage, P[i][0], 10, Scalar(255), 5);
	}

	//右下
	for (int i = 0; i < P.size(); i++)
	{
		Point p1;
		for (int j = 0; j < corners.size(); j++)
		{
			if (P[i][0].x - corners[j].x> -720 && P[i][0].x - corners[j].x < -640 && P[i][0].y - corners[j].y < -640 && P[i][0].y - corners[j].y > -720)
			{
				P[i].push_back(corners[j]);
			}
		}
	}

	//左下
	for (int i = 0; i < P.size(); i++)
	{
		Point p1;
		for (int j = 0; j < corners.size(); j++)
		{
			if (P[i][0].x - corners[j].x> -25 && P[i][0].x - corners[j].x < 25 && P[i][0].y - corners[j].y < -650 && P[i][0].y - corners[j].y > -720)
			{
				P[i].push_back(corners[j]);
			}
		}
	}
	for (int i = 0; i < P.size(); i++) {

		cout << P[i].size() << endl;
	}

	/* 防止有容器中出现不齐4个角点的情况引起程序崩溃 */
	for (int i = 0; i < P.size(); i++)
	{
		cout << P[i].size() << endl;
		if (P[i].size() == 4)
		{
			grid_corner.push_back(P[i]);
		}
	}

	/******************在传入的图中画角点****************************/
	/*
	for (int i = 0; i < P1.size(); i++)
	{
	for (int j = 0; j < 4; j++)
	{
	circle(showimage, P1[i][j], 10, Scalar(0, 0, 255), -1, 8, 0);
	}
	}*/
}