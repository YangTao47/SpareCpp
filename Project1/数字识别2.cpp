#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
using namespace cv;
using namespace std;
using namespace std::chrono;
vector<vector<Point2f>> FindCellBox(const Mat& g_srcImage);
vector<vector<Point2f>> FindCellBox1(const Mat& g_srcImage);
Point GetBoardCoordinates(const Mat& src, vector<vector<Point2f>> P, int i);
int GetRowSum(Mat src, int row);
int GetColSum(Mat src, int col);
int MatchTemplate(const Mat& src, int nums);
int CutLeft(Mat& src, Mat& leftImg, Mat& rightImg);
void CutTop(Mat& src, Mat& dstImg);
Mat trackbar(Mat& src);
Mat trackbar1(Mat& src);
Mat reversalImage(Mat& src);


struct CellBoardCoordinates
{

	Point cellCoordinates;
	Point boardCoordinates;

};
vector<CellBoardCoordinates> ExtractCellCoordinates(const Mat& Image);

int main()
{
	vector<CellBoardCoordinates> cell_board_point_vector;
	Mat Image = imread("C:\\Users\\hzxuxin\\Downloads\\�궨1\\��\\2.bmp");
	auto start_time = high_resolution_clock::now();

	ExtractCellCoordinates(Image);
	time_point<high_resolution_clock> end_time = high_resolution_clock::now();
	milliseconds processing_time = duration_cast<milliseconds>(end_time - start_time);
	cout <<"��ʱ"<< processing_time.count() <<"����"<<endl;
	
	system("pause");
	return 0;
}


vector<CellBoardCoordinates> ExtractCellCoordinates(const Mat& Image)
{
	CellBoardCoordinates cell_board_point;
	vector<CellBoardCoordinates> cell_board_point_vector;
	vector<vector<Point2f>> P;
	Point cell_point;
	Point board_point;
	P = FindCellBox1(Image);
	for (int i = 0; i < P.size(); i++)
	{
		cell_point = GetBoardCoordinates(Image, P, i);
		board_point = P[i][0];
		cell_board_point.cellCoordinates = cell_point;
		cell_board_point.boardCoordinates = board_point;
		cell_board_point_vector.push_back(cell_board_point);
	}
	return cell_board_point_vector;
}

vector<vector<Point2f>> FindCellBox(const Mat& g_srcImage)
{
	Mat g_srcImage1, g_grayImage, CannyImg, g_srcImage2;
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	vector<vector<Point2f>>P;
	vector<Point2f> corners;//�ṩ��ʼ�ǵ������λ�ú;�ȷ�������λ��
	int maxcorners = 200;
	double qualityLevel = 0.5;  //�ǵ���ɽ��ܵ���С����ֵ
	double minDistance = 50;	//�ǵ�֮����С����
	int blockSize = 3;//���㵼������ؾ���ʱָ��������Χ
	double  k = 0.04; //Ȩ��ϵ��
	g_srcImage1 = g_srcImage.clone();
	g_srcImage2 = g_srcImage.clone();
	cvtColor(g_srcImage1, g_grayImage, COLOR_BGR2GRAY);
	adaptiveThreshold(g_grayImage, g_grayImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 1333, 1);
	morphologyEx(g_grayImage, g_grayImage, MORPH_CLOSE, element, Point(-1, -1), 5);
	morphologyEx(g_grayImage, g_grayImage, MORPH_OPEN, element, Point(-1, -1), 12);
	Canny(g_grayImage, CannyImg, 140, 250, 3);
	/*********************************************************************************************************/
	/* �ǵ��� */
	goodFeaturesToTrack(CannyImg, corners, maxcorners, qualityLevel, minDistance, Mat(), blockSize, false, k);
	for (int i = 0; i < corners.size(); i++)
	{
		circle(g_srcImage2, corners[i], 10, Scalar(0, 0, 255), -1, 8, 0);
	}

	for (int i = 0; i < corners.size(); i++)
	{
		vector<Point2f>p1;
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

	
	/*P[i][1]�����Ͻǣ�P[i][0]�����Ͻǣ�P[i][2]�����½ǣ�P[i][3]���½�*/
	for (int i = 0; i < P.size(); i++)
	{
		Point2f p1;
		for (int j = 0; j < corners.size(); j++)
		{	/* ���޸Ĳ��� -700  -650*/
			if (P[i][0].x - corners[j].x> -710 && P[i][0].x - corners[j].x < -640 && P[i][0].y - corners[j].y < -640 && P[i][0].y - corners[j].y > -700)
			{
				P[i].push_back(corners[j]);
			}
		}
	}

	for (int i = 0; i < P.size(); i++)
	{
		Point2f p1;
		for (int j = 0; j < corners.size(); j++)
		{	/* ���޸Ĳ��� -700  -650*/
			if (P[i][0].x - corners[j].x> -20 && P[i][0].x - corners[j].x < 20 && P[i][0].y - corners[j].y < -650 && P[i][0].y - corners[j].y > -700)
			{
				P[i].push_back(corners[j]);
			}
		}
	}

	vector<vector<Point2f>>P1;
	for (int i = 0; i < P.size(); i++) {

		if (P[i].size() == 4) {
			P1.push_back(P[i]);
		}

	}
	for (int i = 0; i < P1.size(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			circle(g_srcImage1, P1[i][j], 10, Scalar(0, 0, 255), -1, 8, 0);
		}
	}
	return P1;
}


Point GetBoardCoordinates(const Mat& src, vector<vector<Point2f>> P, int i)
{
	Point2i p;
	Point2f srcTri[4];
	Point2f dstTri[4];
	Mat warpPerspective_mat(3, 3, CV_32FC1);
	Mat warpPerspective_dst, g_grayImage;
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	int row_col = 700;
	warpPerspective_dst = Mat::zeros(row_col, row_col, src.type());
	
	srcTri[0] = P[i][0];
	srcTri[1] = P[i][1];
	srcTri[2] = P[i][3];
	srcTri[3] = P[i][2];

	dstTri[0] = Point2f(0, 0);
	dstTri[1] = Point2f((float)(row_col - 1), 0);
	dstTri[2] = Point2f(0, (float)(row_col - 1));
	dstTri[3] = Point2f((float)(row_col - 1), (float)(row_col - 1));

	/*   ͸�ӱ任  */
	warpPerspective_mat = getPerspectiveTransform(srcTri, dstTri);
	warpPerspective(src, warpPerspective_dst, warpPerspective_mat, warpPerspective_dst.size());

	cvtColor(warpPerspective_dst, warpPerspective_dst, CV_BGR2GRAY);
	adaptiveThreshold(warpPerspective_dst, warpPerspective_dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 2111, 1);
	morphologyEx(warpPerspective_dst, warpPerspective_dst, MORPH_CLOSE, element, Point(-1, -1), 4);
	morphologyEx(warpPerspective_dst, warpPerspective_dst, MORPH_OPEN, element, Point(-1, -1), 3);
	medianBlur(warpPerspective_dst, warpPerspective_dst, 5);

	Rect rect1(145, 100, 200, 200);
	Mat rect01 = warpPerspective_dst(rect1).clone();
	Rect rect2(395, 100, 150, 190);
	Mat rect02 = warpPerspective_dst(rect2).clone();
	Rect rect3(145, 360, 150, 190);
	Mat rect03 = warpPerspective_dst(rect3).clone();
	Rect rect4(395, 360, 150, 190);
	Mat rect04 = warpPerspective_dst(rect4).clone();

	Mat leftImg01, rightImg01;
	int res01 = CutLeft(rect01, leftImg01, rightImg01);

	Mat leftImg02, rightImg02;
	int res02 = CutLeft(rect02, leftImg02, rightImg02);

	Mat leftImg03, rightImg03;
	int res03 = CutLeft(rect03, leftImg03, rightImg03);

	Mat leftImg04, rightImg04;
	int res04 = CutLeft(rect04, leftImg04, rightImg04);

	int num1 = MatchTemplate(rect01, 20);
	int num2 = MatchTemplate(rect02, 20);
	int num3 = MatchTemplate(rect03, 20);
	int num4 = MatchTemplate(rect04, 20);

	cout << num1 << endl;
	cout << num2 << endl;
	cout << num3 << endl;
	cout << num4 << endl;
	
	if (num1 == 0)
	{
		p.x = num2;
	}
	else
	{
		p.x = num1 * 10 + num2;
	}
	if (num3 == 0)
	{
		p.y = num4;
	}
	else
	{
		p.y = num3 * 10 + num4;
	}
	
	return p;
}



int MatchTemplate(const Mat& src, int nums)
{
	double minVal = 0, maxVal = 0;
	double temp = 0;
	int serieNum = 0;
	Mat img_result;
	for (int i = 0; i < nums; i++)
	{
		char name[200];
		sprintf_s(name, "D:\\templates\\%d.bmp", i);
		Mat Template = imread(name, CV_LOAD_IMAGE_GRAYSCALE);
		resize(Template, Template, Size(src.cols, src.rows));
		matchTemplate(src, Template, img_result, TM_CCOEFF_NORMED);
		minMaxIdx(img_result, &minVal, &maxVal);
		if (temp < maxVal)
		{
			temp = maxVal;
			serieNum = i;
		}
	}
	
	if (serieNum >= 10)
	{
	 serieNum -= 10;
	}
	return serieNum;
}


/**���߷���һ��**/

int GetColSum(Mat src, int col)//ͳ�����������ص��ܺ�
{
	int sum = 0;
	int height = src.rows;
	int width = src.cols;
	for (int i = 0; i < height; i++)
	{
		sum = sum + src.at <uchar>(i, col);
	}
	return sum;
}

int GetRowSum(Mat src, int row)//ͳ�����������ص��ܺ�
{
	int sum = 0;
	int height = src.rows;
	int width = src.cols;
	for (int i = 0; i < width; i++)
	{
		sum += src.at <uchar>(row, i);
	}
	return sum;
}


void CutTop(Mat& src, Mat& dstImg)//�����и�
{
	int top, bottom;
	top = 0;
	bottom = src.rows;

	int i;
	for (i = 0; i < src.rows; i++)
	{
		int colValue = GetRowSum(src, i);//ͳ�����������ص��ܺ�
										 //cout <<i<<" th "<< colValue << endl;
		if (colValue>0)//ɨ��ֱ�������ص��ܺʹ���0ʱ�����µ�ǰλ��top
		{
			top = i;
			break;
		}
	}
	for (; i < src.rows; i++)
	{
		int colValue = GetRowSum(src, i);//ͳ�����������ص��ܺ�
										 //cout << i << " th " << colValue << endl;
		if (colValue == 0)//����ɨ��ֱ�������ص��ܺ͵���0ʱ�����µ�ǰλ��bottom
		{
			bottom = i;
			break;
		}
	}
	if (top == 0)
	{

		int j;
		for (j = 0; j < src.rows; j++)
		{
			int colValue = GetRowSum(src, j);//ͳ�����������ص��ܺ�
											 //cout <<i<<" th "<< colValue << endl;
			if (colValue<src.cols*255)//ɨ��ֱ�������ص��ܺʹ���0ʱ�����µ�ǰλ��top
			{
				top = j;
				break;
			}
		}
		for (; j < src.rows; j++)
		{
			int colValue = GetRowSum(src, j);//ͳ�����������ص��ܺ�
											 //cout << i << " th " << colValue << endl;
			if (colValue == src.cols * 255)//����ɨ��ֱ�������ص��ܺ͵���0ʱ�����µ�ǰλ��bottom
			{
				bottom = j;
				break;
			}
		}
	}
	int height = bottom - top;
	if (height < 125) 
	{
		height = 125;
	}
	Rect rect(0, top, src.cols, height);
	dstImg = src(rect).clone();
}


int CutLeft(Mat& src, Mat& leftImg, Mat& rightImg)//�����и�
{
	int left, right;
	left = 0;
	right = src.cols;

	int i;
	for (i = 0; i < src.cols; i++)
	{
		int colValue = GetColSum(src, i);//ͳ�����������ص��ܺ�
										 //cout <<i<<" th "<< colValue << endl;
		if (colValue>0)//ɨ��ֱ�������ص��ܺʹ���0ʱ�����µ�ǰλ��left
		{
			left = i;
			break;
		}
	}

	//����ɨ��
	for (; i < src.cols; i++)
	{
		int colValue = GetColSum(src, i);//ͳ�����������ص��ܺ�
										 //cout << i << " th " << colValue << endl;
		if (colValue == 0)//����ɨ��ֱ�������ص��ܺ͵���0ʱ�����µ�ǰλ��right
		{
			right = i;
			break;
		}
	}


	if (left == 0)
	{
		int j;
		for (j = 0; j < src.cols; j++)
		{
			int colValue = GetColSum(src, j);//ͳ�����������ص��ܺ�
											 //cout <<i<<" th "<< colValue << endl;
			if (colValue<src.rows*255)//ɨ��ֱ�������ص��ܺʹ���0ʱ�����µ�ǰλ��left
			{
				left = j;
				break;
			}
		}

		//����ɨ��
		for (; j < src.cols; j++)
		{
			int colValue = GetColSum(src, j);//ͳ�����������ص��ܺ�
											 //cout << i << " th " << colValue << endl;
			if (colValue == src.rows * 255)//����ɨ��ֱ�������ص��ܺ͵���0ʱ�����µ�ǰλ��right
			{
				right = j;
				break;
			}
		}
	}
	int width = right - left;//�ָ�ͼƬ�Ŀ����Ϊright - left
	if (width < 48)
	{
		width = 48;
	}
	Rect rect(left, 0, width, src.rows);//����һ�����Σ������ֱ�Ϊ������߶�����X���ꡢY���꣬�ұߵײ���X���ꡢY���꣨���Ͻ�����Ϊ0��0��
	leftImg = src(rect).clone();
	Rect rectRight(right, 0, src.cols - right, src.rows);//�ָ��ʣ�µ�ԭͼ
	rightImg = src(rectRight).clone();
	CutTop(leftImg, leftImg);//�����и�
	return 0;
}



Mat trackbar(Mat& src)
{
	Mat src_color, labels, stats, centroids, reversal, dst;
	if (src.channels() == 3)
	{
		cvtColor(src, src, CV_BGR2GRAY);
	}
	reversal = reversalImage(src);
	cvtColor(src, src, CV_GRAY2BGR);
	int num = connectedComponentsWithStats(reversal, labels, stats, centroids, 4);
	vector<Vec3b> color(num + 1);
	color[0] = Vec3b(0, 0, 0);
	for (int m = 0; m < num; m++) {
		color[m] = Vec3b(0, 0, 0);
		if (stats.at<int>(m, CC_STAT_AREA) < 15000)
		{
			color[m] = Vec3b(255, 255, 255);
		}
	}
	src_color = Mat::zeros(src.size(), CV_8UC3);
	for (int y = 0; y < src_color.rows; y++)
	{
		for (int x = 0; x < src_color.cols; x++)
		{

			int label = labels.at<int>(y, x);//ע��labels��int�ͣ�����uchar.
			CV_Assert(0 <= label && label <= num);
			src_color.at<Vec3b>(y, x) = color[label];
		}
	}
	addWeighted(src, 1, src_color, 1, 1, dst);
	return dst;
}

Mat trackbar1(Mat& src)
{
	Mat src_color, labels, stats, centroids, reversal, dst;
	if (src.channels() == 3)
	{
		cvtColor(src, src, CV_BGR2GRAY);
	}
	adaptiveThreshold(src, src, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 1333, 1);
	reversal = reversalImage(src);
	int num = connectedComponentsWithStats(src, labels, stats, centroids, 4);
	vector<Vec3b> color(num + 1);
	color[0] = Vec3b(0, 0, 0);
	for (int m = 0; m < num; m++) {
		color[m] = Vec3b(0, 0, 0);
		if (stats.at<int>(m, CC_STAT_AREA) < 8000)
		{
			color[m] = Vec3b(255, 255, 255);
		}
	}
	src_color = Mat::zeros(src.size(), CV_8UC3);
	for (int y = 0; y < src_color.rows; y++)
	{
		for (int x = 0; x < src_color.cols; x++)
		{

			int label = labels.at<int>(y, x);//ע��labels��int�ͣ�����uchar.
			CV_Assert(0 <= label && label <= num);
			src_color.at<Vec3b>(y, x) = color[label];
		}
	}
	cvtColor(reversal, reversal, CV_GRAY2BGR);
	addWeighted(reversal, 1, src_color, 1, 1, dst);
	dst = reversalImage(dst);
	return dst;
}

Mat reversalImage(Mat& src)
{

	Mat img_result;
	uchar lutData[256];
	for (int i = 0; i < 256; i++)
	{
		lutData[i] = 255 - i;
	}
	Mat lut(1, 256, CV_8UC1, lutData);
	LUT(src, lut, img_result);

	return img_result;
}

vector<vector<Point2f>> FindCellBox1(const Mat& g_srcImage)
{
	Mat g_srcImage1, g_grayImage, CannyImg, g_srcImage2;
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	vector<vector<Point2f>>P;
	vector<Point2f> corners;//�ṩ��ʼ�ǵ������λ�ú;�ȷ�������λ��
	int maxcorners = 200;
	double qualityLevel = 0.3;  //�ǵ���ɽ��ܵ���С����ֵ
	double minDistance = 50;	//�ǵ�֮����С����
	int blockSize = 3;//���㵼������ؾ���ʱָ��������Χ
	double  k = 0.04; //Ȩ��ϵ��
	g_srcImage1 = g_srcImage.clone();
	g_srcImage2 = g_srcImage.clone();
	cvtColor(g_srcImage1, g_grayImage, COLOR_BGR2GRAY);
	adaptiveThreshold(g_grayImage, g_grayImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 2111, 1);
	g_grayImage = trackbar(g_grayImage);
	g_grayImage = trackbar1(g_grayImage);
	morphologyEx(g_grayImage, g_grayImage, MORPH_CLOSE, element, Point(-1, -1), 5);
	//resize(g_grayImage, g_grayImage, Size(0.1 * g_grayImage.cols, 0.1 * g_grayImage.rows));
	cvtColor(g_grayImage, g_grayImage, CV_BGR2GRAY);
	threshold(g_grayImage, g_grayImage, 1, 255, THRESH_BINARY + THRESH_OTSU);
	Canny(g_grayImage, CannyImg, 140, 250, 3);
	/*********************************************************************************************************/
	/* �ǵ��� */
	goodFeaturesToTrack(CannyImg, corners, maxcorners, qualityLevel, minDistance, Mat(), blockSize, false, k);
	for (int i = 0; i < corners.size(); i++)
	{
		circle(g_srcImage2, corners[i], 10, Scalar(0, 0, 255), -1, 8, 0);
	}

	for (int i = 0; i < corners.size(); i++)
	{
		vector<Point2f>p1;
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

	/*P[i][1]�����Ͻǣ�P[i][0]�����Ͻǣ�P[i][2]�����½ǣ�P[i][3]���½�*/
	for (int i = 0; i < P.size(); i++)
	{
		Point2f p1;
		for (int j = 0; j < corners.size(); j++)
		{	/* ���޸Ĳ��� -700  -650*/
			if (P[i][0].x - corners[j].x> -710 && P[i][0].x - corners[j].x < -640 && P[i][0].y - corners[j].y < -640 && P[i][0].y - corners[j].y > -700)
			{
				P[i].push_back(corners[j]);
			}
		}
	}

	for (int i = 0; i < P.size(); i++)
	{
		Point2f p1;
		for (int j = 0; j < corners.size(); j++)
		{	/* ���޸Ĳ��� -700  -650*/
			if (P[i][0].x - corners[j].x> -20 && P[i][0].x - corners[j].x < 20 && P[i][0].y - corners[j].y < -650 && P[i][0].y - corners[j].y > -700)
			{
				P[i].push_back(corners[j]);
			}
		}
	}

	vector<vector<Point2f>>P1;
	for (int i = 0; i < P.size(); i++) {

		if (P[i].size() == 4) {
			P1.push_back(P[i]);
		}

	}
	for (int i = 0; i < P1.size(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			circle(g_srcImage1, P1[i][j], 10, Scalar(0, 0, 255), -1, 8, 0);
		}
	}
	return P1;
}

