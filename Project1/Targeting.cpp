#include<iostream>
#include<opencv2\opencv.hpp>
#include<chrono>
using namespace cv;
using namespace std;
using namespace std::chrono;

class ObjectTarget
{
public:
	Mat targetImage(const Mat& src)
	{
		Mat roi, dst;
		vector<Point> cross_points;
		roi = findRoi(src);
		cross_points = calculateCrossPoints(roi);
		dst = drawLines(src, cross_points);
		return dst;
	}
private:

	const int error = 2;
	const int area1 = 80000;
	const int area2 = 50;
	const float scale1 = 0.1f;
	const int scale2 =10;
	const int pixelNums = 500;
	const int thresholdValue = 254;
	Point roiTopLeftCorner;

	Mat findRoi(const Mat& src)
	{
		Mat g_bd, labels, stats, centroids, dst, rvs_Image, resize_img, src_copy;
		vector<vector<int>> roi;
		src.copyTo(src_copy);
		resize_img = resizeImage(src, scale1);
		threshold(resize_img, g_bd, 1, 255, THRESH_BINARY + THRESH_OTSU);
		rvs_Image = reverseImage(g_bd);
		roi = findConnectedComponentsArea(rvs_Image, area1);

		int x = scale2 * (roi[0][0] - error);
		int y = scale2 * (roi[0][1] - error);
		int width = scale2 * (roi[0][2] + 2 * error);
		int height = scale2 * (roi[0][3] + 2 * error);

		Rect Roi(x, y, width, height);
		roiTopLeftCorner.x = x;
		roiTopLeftCorner.y = y;
		dst = src_copy(Roi).clone();
		return dst;
	}

	vector<vector<Point>> findContoursPoints(const Mat& src)
	{
		Mat g_bd;
		Mat picture(src.rows, src.cols, CV_8UC3, Scalar(0, 0, 0));
		vector<vector<Point>> P;
		vector<Point> P1, P2, P3, P4;
		threshold(src, g_bd, 1, 255, THRESH_BINARY + THRESH_OTSU);
		g_bd = removeNoise(g_bd);
		P1 = findJumpPoint(g_bd, "up");
		P2 = findJumpPoint(g_bd, "right");
		P3 = findJumpPoint(g_bd, "down");
		P4 = findJumpPoint(g_bd, "left");

		P.push_back(P1);
		P.push_back(P2);
		P.push_back(P3);
		P.push_back(P4);

		return P;
	}

	vector<Point> calculateCrossPoints(const Mat& src)
	{
		vector<vector<Point>> P;
		vector<Point> cross_points;
		P = findContoursPoints(src);
		for (int i = 0; i < P.size(); i++)
		{
			Point p;
			int A1 = P[i][499].y - P[i][0].y;
			int B1 = P[i][0].x - P[i][499].x;
			int C1 = A1 * P[i][0].x + B1 * P[i][0].y;
			int A2 = P[(i + 1) % 4][499].y - P[(i + 1) % 4][0].y;
			int B2 = P[(i + 1) % 4][0].x - P[(i + 1) % 4][499].x;
			int C2 = A2 * P[(i + 1) % 4][0].x + B2 * P[(i + 1) % 4][0].y;
			int denominator = A1 * B2 - A2 * B1;
			float x = float(B2 * C1 - B1 * C2) / (float)denominator;
			float y = float(A1 * C2 - A2 * C1) / (float)denominator;
			p.x = (int)x;
			p.y = (int)y;
			cross_points.push_back(p);

		}
		return cross_points;
	}

	Mat resizeImage(const Mat& src, float scale)
	{
		Mat dst;
		Size size;
		size.width = int(src.cols * scale);
		size.height = int(src.rows * scale);
		resize(src, dst, size);
		return dst;
	}
	/*返回x y w h*/
	vector<vector<int>> findConnectedComponentsArea(const Mat&src, int area, string mode="findRoi")
	{
		Mat labels, stats, centroids;
		vector<vector<int>> roi;
		int num = connectedComponentsWithStats(src, labels, stats, centroids, 4);
		for (int i = 1; i < num; i++)
		{
			vector<int> candidateArea;
			if (mode == "findRoi")
			{
				if (stats.at<int>(i, CC_STAT_AREA) > area1)
				{
					for (int cols = 0; cols < 4; cols++)
					{
						candidateArea.push_back(stats.at<int>(i, cols));
					}
					roi.push_back(candidateArea);
				}

			}
			else if (mode == "removeNoise")
			{
				if (stats.at<int>(i, CC_STAT_AREA) < area2)
				{
					for (int cols = 0; cols < 4; cols++)
					{
						candidateArea.push_back(stats.at<int>(i, cols));
					}
					roi.push_back(candidateArea);
				}
			}

		}
		return roi;
	}

	Mat removeNoise(Mat& src)
	{
		vector<vector<int>> roi;
		roi = findConnectedComponentsArea(src, area2, "removeNoise");
		for (int i = 0; i < roi.size(); i++)
		{
			int x = roi[i][0];
			int y = roi[i][1];
			int width = roi[i][2];
			int height = roi[i][3];
			for (int c = x; c < x + width; c++)
			{
				for (int r = y; r < y + height; r++)
				{
					src.at<uchar>(r, c) = 0;
				}
			}
		}
		return src;
	}

	Mat reverseImage(const Mat& src)
	{
		Mat dst;
		uchar lutData[256];
		for (int i = 0; i < 256; i++)
		{
			lutData[i] = 255 - i;
		}
		Mat lut(1, 256, CV_8UC1, lutData);
		LUT(src, lut, dst);
		return dst;
	}

	Mat drawLines(const Mat& image, vector<Point> p)
	{
		Mat dst;
		image.copyTo(dst);
		if (dst.channels() == 1){
			cvtColor(dst, dst, CV_GRAY2BGR);
		}
		for (int i = 0; i < p.size(); i++)
		{
			line(dst, p[i],p[(i + 1) % 4], Scalar(0, 0, 255), 1);
		}
		return dst;
	}

	vector<Point> findJumpPoint(const Mat& src, string mode = "up")
	{
		vector<Point> P;
		int center_x = src.cols / 2;
		int center_y = src.rows / 2;
		int src_rows = src.rows;
		int src_cols = src.cols;
		CV_Assert(src.at<uchar>(center_y, center_x) == 0);
		CV_Assert(center_x + pixelNums < src.cols);
		CV_Assert(center_x - pixelNums > 0);
		CV_Assert(center_y + pixelNums < src.rows);
		CV_Assert(center_y - pixelNums > 0);
		if (src.at<uchar>(center_y, center_x) == 0)
		{
			if (mode == "up")
			{
				//cout << "up" << endl;
				for (int num = 0; num < pixelNums; num++)
				{
					Point p;
					for (int i = 0; i < src_rows / 2; i++)
					{
						int p1 = src.at<uchar>(center_y - i, center_x);
						int p2 = src.at<uchar>(center_y - i - 1, center_x);
						//cout << p2 - p1 << endl;
						if (p2 - p1 > thresholdValue)
						{
							p.x = center_x + roiTopLeftCorner.x;
							p.y = center_y - i - 1 + roiTopLeftCorner.y;
							P.push_back(p);
						}
					}
					center_x++;
				}
				for (int num = 0; num < pixelNums; num++)
				{
					Point p;
					for (int i = 0; i < src_rows / 2; i++)
					{
						int p1 = src.at<uchar>(center_y - i, center_x);
						int p2 = src.at<uchar>(center_y - i - 1, center_x);
						//cout << p2 - p1 << endl;
						if (p2 - p1 > thresholdValue)
						{
							p.x = center_x + roiTopLeftCorner.x;
							p.y = center_y - i - 1 + roiTopLeftCorner.y;
							P.push_back(p);
						}
					}
					center_x--;
				}
			}

			if (mode == "down")
			{
				//cout << "down" << endl;
				for (int num = 0; num < pixelNums; num++)
				{
					Point p;
					for (int i = 0; i < src_rows / 2 -1; i++)
					{
						int p1 = src.at<uchar>(center_y + i, center_x);
						int p2 = src.at<uchar>(center_y + i + 1, center_x);
						if (p2 - p1 > thresholdValue)
						{
							p.x = center_x + roiTopLeftCorner.x;
							p.y = center_y + i + 1 + roiTopLeftCorner.y;
							P.push_back(p);
						}
					}
					center_x++;
				}

				for (int num = 0; num < pixelNums; num++)
				{
					Point p;
					for (int i = 0; i < src_rows / 2 - 1; i++)
					{
						int p1 = src.at<uchar>(center_y + i, center_x);
						int p2 = src.at<uchar>(center_y + i + 1, center_x);
						if (p2 - p1 > thresholdValue)
						{
							p.x = center_x + roiTopLeftCorner.x;
							p.y = center_y + i + 1 + roiTopLeftCorner.y;
							P.push_back(p);
						}
					}
					center_x--;
				}
			}

			if (mode == "left")
			{
				//cout << "left" << endl;
				for (int num = 0; num < pixelNums; num++)
				{
					Point p;
					for (int i = 0; i < src_cols / 2 - 1; i++)
					{
						int p1 = src.at<uchar>(center_y, center_x - i);
						int p2 = src.at<uchar>(center_y, center_x - i - 1);
						if (p2 - p1 > thresholdValue)
						{
							p.x = center_x - i - 1 + roiTopLeftCorner.x;
							p.y = center_y + roiTopLeftCorner.y;
							P.push_back(p);
						}
					}
					center_y++;
				}
				for (int num = 0; num < pixelNums; num++)
				{
					Point p;
					for (int i = 0; i < src_cols / 2 - 1; i++)
					{
						int p1 = src.at<uchar>(center_y, center_x - i);
						int p2 = src.at<uchar>(center_y, center_x - i - 1);
						if (p2 - p1 > thresholdValue)
						{
							p.x = center_x - i - 1 + roiTopLeftCorner.x;
							p.y = center_y + roiTopLeftCorner.y;
							P.push_back(p);
						}
					}
					center_y--;
				}
			}

			if (mode == "right")
			{
				//cout << "right" << endl;
				for (int num = 0; num < pixelNums; num++)
				{
					Point p;
					for (int i = 0; i < src_cols / 2 - 1; i++)
					{
						int p1 = src.at<uchar>(center_y, center_x + i);
						int p2 = src.at<uchar>(center_y, center_x + i + 1);
						if (p2 - p1 > thresholdValue)
						{
							p.x = center_x + i + 1 + roiTopLeftCorner.x;
							p.y = center_y + roiTopLeftCorner.y;
							P.push_back(p);
						}
					}
					center_y++;
				}
				for (int num = 0; num < pixelNums; num++)
				{
					Point p;
					for (int i = 0; i < src_cols / 2 - 1; i++)
					{
						int p1 = src.at<uchar>(center_y, center_x + i);
						int p2 = src.at<uchar>(center_y, center_x + i + 1);
						if (p2 - p1 > thresholdValue)
						{
							p.x = center_x + i + 1 + roiTopLeftCorner.x;
							p.y = center_y + roiTopLeftCorner.y;
							P.push_back(p);
						}
					}
					center_y--;
				}
			}
		}
		return P;
	}
};

int main()
{
	Mat image = imread("lcd.bmp",CV_8UC1);
	Mat dst;
	ObjectTarget test;
	auto start_time = high_resolution_clock::now();

	dst = test.targetImage(image);

	time_point<high_resolution_clock> end_time = high_resolution_clock::now();
	milliseconds processing_time = duration_cast<milliseconds>(end_time - start_time);
	cout << processing_time.count() << endl;
	imwrite("result.jpg", dst);
	return 0;
}
