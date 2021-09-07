#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
using namespace cv;
using namespace std;
using namespace std::chrono;
void findcorners(Mat& denoise_image, vector<Point>& resize_leftup, vector<vector<Point>>& corners);
void slideWindow(const Mat& small_image, vector<Point>& left_up);
void findSmallRoi(const vector<Point>& left_up, const Mat& small_image, vector<Point>& roi_left_up);
vector<Point> conv_findCorners(Mat&src);
Mat conv(const Mat& src);
void findRect(vector<Mat>& roi, int i, Rect& small_rect_roi);
void conv_NMS(vector<Point> &p);
bool calculateWhiltBlackRatio(Mat& roi_mat);
void NMS(vector<Point> &roi_left_up, vector<Point>& roi_leftup);
void resizeRoi(vector<Point>& leftup_nms, vector<Point>& resize_leftup);
void conv_findCorners(Mat& src, Point& cross_point, Rect& roi_rect_info);
//void  findCrossTemp(Mat& dst1, Mat& dst2);
Mat computeScoreMap(Mat& gray_image, Size& ksize1, Size& ksize2);
void replacePixels(Mat& image, bool dst1 = true);

const int connectedComponentsArea1 = 60000;
const int connectedComponentsArea2 = 30000;
const int expandPixels = 200;
const int rect_width = 5;
const int rect_height = 10;
float small_scale = 0.5f;
float big_scale = 1.f / small_scale;
const int roi_scale = 120;
struct roiRectInfo
{
	vector<Rect> roi_rect_vector;
	vector<Mat> roi;
}roi_rect_info;

bool cmp(Point a, Point b)
{
	if (abs(a.x - b.x) < 8)
	{
		return a.y < b.y;
	}
	else
	{
		return a.x < b.x;
	}
}

/*黑底白噪图像，默认黑色为背景色  有必要缩小取噪声吗？？*/
void findNoiseMask(const Mat& src, int area, Mat& mask)
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

void reverseImage(const Mat& src, Mat& dst)
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

void denoiseImage(const Mat& reverse_image, const Mat& binary_image, Mat& denoise_image)
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

void findCellBox1(const Mat& src)
{
	Mat gray_image, binary_image, reverse_image, denoise_image, output, small_image;
	vector<Point> cross_point;
	vector<Point> candidate;
	vector<Point> leftup_non_nms;
	vector<Point> leftup_nms;
	CV_Assert(src.channels() == 1);
	src.copyTo(gray_image);
	medianBlur(gray_image, gray_image, 13);
	adaptiveThreshold(gray_image, binary_image, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 1333, 1);
	reverseImage(binary_image, reverse_image);
	copyMakeBorder(reverse_image, reverse_image, expandPixels, expandPixels, expandPixels, expandPixels, BORDER_REPLICATE);
	copyMakeBorder(binary_image, binary_image, expandPixels, expandPixels, expandPixels, expandPixels, BORDER_REPLICATE);

	denoiseImage(reverse_image, binary_image, denoise_image);
	resize(denoise_image, small_image, Size(int(small_scale * denoise_image.cols), int(small_scale * denoise_image.rows)));
	threshold(small_image, small_image, 1, 255, THRESH_BINARY + THRESH_OTSU);

	Mat dst1, dst2;
	dst1 = computeScoreMap(small_image, Size(roi_scale, roi_scale/2), Size(roi_scale / 2, roi_scale));
	

	
	//findCrossTemp(dst1, dst2);
	//   手动滑窗法
	/*slideWindow(small_image, candidate);
	findSmallRoi(candidate, small_image, leftup_non_nms);
	NMS(leftup_non_nms, leftup_nms);
	vector<Point> resize_leftup(leftup_nms.size());
	resizeRoi(leftup_nms, resize_leftup);

	vector<vector<Point>> corners;
	findcorners(denoise_image, resize_leftup, corners);*/

	//  conv 方法
	/*output = conv(denoise_image);
	cross_point = conv_findCorners(output);
	conv_NMS(cross_point);*/
}

void findcorners(Mat& denoise_image, vector<Point>& resize_leftup, vector<vector<Point>>& corners)
{
	vector<Rect> smll_rect_roi;
	vector<Point> cross_point;
	for (int i = 0; i < resize_leftup.size(); i++) {
		Rect roi_rect(resize_leftup[i].x, resize_leftup[i].y, big_scale * rect_height, big_scale * rect_height);
		roi_rect_info.roi.push_back(denoise_image(roi_rect));
		roi_rect_info.roi_rect_vector.push_back(roi_rect);
	}
	for (int i = 0; i < roi_rect_info.roi.size(); i++)
	{
		Point temp;
		Mat conv_kernel_1 = (Mat_<float>(3, 3) << -0.2, -0.2, -0.2, 0.25, 0.25, -0.2, 0.25, 0.25, -0.2);
		Mat conv_kernel_2 = (Mat_<float>(3, 3) << -0.2, 0.25, 0.25, -0.2, 0.25, 0.25, -0.2, -0.2, -0.2);
		Mat conv_kernel_3 = (Mat_<float>(3, 3) << 0.25, 0.25, -0.2, 0.25, 0.25, -0.2, -0.2, -0.2, -0.2);
		Mat conv_kernel_4 = (Mat_<float>(3, 3) << -0.2, -0.2, -0.2, -0.2, 0.25, 0.25, -0.2, 0.25, 0.25);
		Mat ans1;
		Mat ans2;
		Mat ans3;
		Mat ans4;
		filter2D(roi_rect_info.roi[i], ans1, -1, conv_kernel_1);  // 0.1ms
		filter2D(roi_rect_info.roi[i], ans2, -1, conv_kernel_2);  // 0.1ms
		filter2D(roi_rect_info.roi[i], ans3, -1, conv_kernel_3);  // 0.1ms
		filter2D(roi_rect_info.roi[i], ans4, -1, conv_kernel_4);  // 0.1ms

		Mat ans = ans1 | ans2 | ans3 | ans4;
		Rect roi_rect = roi_rect_info.roi_rect_vector[i];
		conv_findCorners(ans, temp, roi_rect);
		cross_point.push_back(temp);
	}
}

void conv_findCorners(Mat& src, Point& cross_point, Rect& roi_rect_info)
{
	vector<Point> temp_points;
	for (int r = 0; r < src.rows; r++)
	{
		uchar* data = src.ptr<uchar>(r);
		for (int c = 0; c < src.cols; c++)
		{
			if ((*data) == 255) {
				temp_points.push_back(Point(c, r));
			}
			data++;
		}
	}
	Point roi_xy_info(roi_rect_info.x, roi_rect_info.y);
	cross_point = (temp_points[0] + temp_points[1]) / 2 + roi_xy_info - Point(200,200);
}

void findRect(vector<Mat>& roi, int i, Rect& small_rect_roi)
{
	int roi_cols = roi[i].cols;
	int roi_rows = roi[i].rows;
	int p1 = 0, p2 = 0;
	int p3 = 0, p4 = 0;
	if (roi[i].at<uchar>(0, 0) == 255) {
		for (int i = 0; i < roi_cols; i++) {
			if (roi[i].at<uchar>(0, i) == 0) {
				p1 = i;
			}
		}
		for (int j = 0; j < roi_rows; j++) {
			if (roi[i].at<uchar>(j, roi_cols) == 255) {
				p2 = j;
			}
		}
		for (int i = 0; i < roi_rows; i++) {
			if (roi[i].at<uchar>(i, 0) == 0) {
				p3 = i;
			}
		}
		for (int j = 0; j < roi_cols; j++) {
			if (roi[i].at<uchar>(roi_rows, j) == 255) {
				p4 = j;
			}
		}


		Rect black_area1(p1, 0, (roi_cols - p1), p2);
		Rect black_area2(0, p3, (roi_rows - p3), p4);
		small_rect_roi = black_area1&black_area2;
	}
	else{
		for (int i = 0; i < roi_cols; i++) {
			if (roi[i].at<uchar>(0, i) == 255) {
				p1 = i;
			}
		}
		for (int j = 0; j < roi_rows; j++) {
			if (roi[i].at<uchar>(j, 0) == 255) {
				p2 = j;
			}
		}
		for (int i = 0; i < roi_rows; i++) {
			if (roi[i].at<uchar>(i, roi_cols) == 0) {
				p3 = i;
			}
		}
		for (int j = 0; j < roi_cols; j++) {
			if (roi[i].at<uchar>(roi_rows, j) == 0) {
				p4 = j;
			}
		}


		Rect black_area1(p1, 0, (roi_cols - p1), p2);
		Rect black_area2(0, p3, (roi_rows - p3), p4);
		small_rect_roi = black_area1&black_area2;
	}
	
}

void slideWindow(const Mat& small_image, vector<Point>& left_up)
{
	for (int c = 0; c < small_image.cols - 2 * rect_width; c++)
	{
		for (int r = 0; r < small_image.rows - 2 * rect_height; r++)
		{
			Rect roi_rect1(c, r, rect_width, rect_height);
			Rect roi_rect2(c + rect_width, r, rect_width, rect_height);
			Mat roi_mat1 = small_image(roi_rect1).clone();
			Mat roi_mat2 = small_image(roi_rect2).clone();
			bool left_find = false;
			bool right_find = false;
			left_find = calculateWhiltBlackRatio(roi_mat1);
			right_find = calculateWhiltBlackRatio(roi_mat2);
			//cout << left_find << "  " << right_find << endl;
			if (left_find == true && right_find == true) {
				left_up.push_back(Point(c, r));
			}
		}
	}
}

/*交换候选滑窗窗口宽高，继续遍历  返回左上角*/
void findSmallRoi(const vector<Point>& left_up, const Mat& small_image, vector<Point>& roi_left_up)
{
	for (int i = 0; i < left_up.size(); i++) {
		int c = left_up[i].x;
		int r = left_up[i].y;
		Rect roi_rect1(c, r, rect_height, rect_width);
		Rect roi_rect2(c, r + rect_width, rect_height, rect_width);
		Mat roi_mat1 = small_image(roi_rect1).clone();
		Mat roi_mat2 = small_image(roi_rect2).clone();
		bool left_find = false;
		bool right_find = false;
		left_find = calculateWhiltBlackRatio(roi_mat1);
		right_find = calculateWhiltBlackRatio(roi_mat2);
		//cout << left_find << "  " << right_find << endl;
		if (left_find == true && right_find == true) {
			roi_left_up.push_back(Point(c, r));
		}
	}
	sort(roi_left_up.begin(), roi_left_up.end(), cmp);
}

bool calculateWhiltBlackRatio(Mat& roi_mat)
{
	float black = .0f, white = .0f;
	float ratio;
	for (int c = 0; c < roi_mat.cols; c++)
	{
		for (int r = 0; r < roi_mat.rows; r++)
		{
			if (roi_mat.at<uchar>(r, c) == 255)
			{
				black++;
			}
			else if (roi_mat.at<uchar>(r, c) == 0)
			{
				white++;
			}
		}
	}

	if (white == 0 || black == 0) {
		ratio = 0;
	}
	else {
		ratio = black / white;
	}
	//cout << ratio << endl;
	if (ratio < 1.3 && ratio > 0.6) {
		return true;
	}
	return false;
}

Mat conv(const Mat& src)
{
	Mat conv_kernel_1 = (Mat_<float>(3, 3) << -0.5, 0, 0.5, 0, 0, 0, 0.5, 0, -0.5);
	Mat conv_kernel_2 = (Mat_<float>(3, 3) << 0.5, 0, -0.5, 0, 0, 0, -0.5, 0, 0.5);
	Mat ans1;
	Mat ans2;
	filter2D(src, ans1, -1, conv_kernel_1);  // 0.1ms
	filter2D(src, ans2, -1, conv_kernel_2);  // 0.1ms
	Mat ans = ans1 | ans2;
	return ans;
}

vector<Point> conv_findCorners(Mat& src)
{
	vector<Point> cross_point;
	for (int r = 0; r < src.rows; r++)
	{
		uchar* data = src.ptr<uchar>(r);
		for (int c = 0; c < src.cols; c++)
		{
			Point p;
			if ((*data) == 255) {
				p.x = c;
				p.y = r;
				cross_point.push_back(p);
			}
			data++;
		}
	}
	return cross_point;
}

void conv_NMS(vector<Point> &p)
{
	vector<int> temp;
	const int error = 5;
	for (int i = 0; i < p.size(); i++)
	{
		for (int j = i + 1; j < p.size(); j++)
		{
			int x_offset = abs(p[i].x - p[j].x);
			int y_offset = abs(p[i].y - p[j].y);
			if (x_offset < error && y_offset < error)
			{
				temp.push_back(j);
			}
		}
	}
}
void NMS(vector<Point> &roi_left_up, vector<Point>& roi_leftup)
{
	vector<Point> temp_vector;
	temp_vector.push_back(roi_left_up[0]);
	int j = 0;
	for (int i = 1; i < roi_left_up.size(); i++)
	{
		int diff_x = roi_left_up[i].x - temp_vector[j].x;
		int diff_y = roi_left_up[i].y - temp_vector[j].y;
		bool judge_x = diff_x > 10 || diff_x < -10;
		bool judge_y = diff_y > 10 || diff_y < -10;
		if (judge_x || judge_y) {
			temp_vector.push_back(roi_left_up[i]);
			j++;
		}
	}
	for (int i = 0; i < temp_vector.size(); i++)
	{
		Point temp_point;
		temp_point.x = temp_vector[i].x;
		temp_point.y = temp_vector[i].y;
		roi_leftup.push_back(temp_point);
	}
}

void resizeRoi(vector<Point>& leftup_nms, vector<Point>& resize_leftup)
{
	for (int i = 0; i < leftup_nms.size(); i++)
	{
		Point temp;
		temp.x = static_cast<int>(10 * leftup_nms[i].x);// 之前扩边，在这里减回去
		temp.y = static_cast<int>(10 * leftup_nms[i].y);
		resize_leftup[i] = temp;
	}
}



Mat computeScoreMap(Mat& gray_image, Size& ksize1, Size& ksize2)
{
	Mat cache_mat1, cache_mat2, dst11;
	gray_image.convertTo(gray_image, CV_32F);
	blur(gray_image, cache_mat1, ksize1);
	blur(gray_image, cache_mat2, ksize2);

	replacePixels(cache_mat1);
	replacePixels(cache_mat2, false);

	dst11 = cache_mat1 & cache_mat2;

	return dst11;
}



void replacePixels(Mat& image, bool dst1)
{
	int rows = image.rows;
	int cols = image.cols;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (image.at<float>(i, j) < 117 || image.at<float>(i, j) > 137) {
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
					break;
				}
				if (image.at<uchar>(i, j) - image.at<uchar>(i + roi_scale, j) != 0) {
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
					break;
				}
				if (image.at<uchar>(i, j) - image.at<uchar>(i , j + roi_scale) != 0) {
					image.at<uchar>(i, j) = 0;
				}
			}
		}
	}

}

int main()
{
	Mat image = imread("C:/Users/hzxuxin/Downloads/标定1/偏右/2.bmp", CV_8UC1);
	findCellBox1(image);
	system("pause");
	return 0;
}
