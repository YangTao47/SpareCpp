#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<iostream>

using namespace std;
using namespace cv;

//��һ���Ҷ�ͼ���ֱ��ͼͼ�񣬷��ص���ֱ��ͼͼ��
Mat getHistograph(const Mat grayImage);

int main(int argc, char* argv[])
{
	//����ͼ��
	Mat image;
	image = imread("C:/Users/hzxuxin/Downloads/�궨1/ƫ��/1.bmp", IMREAD_COLOR);
	//�ж��Ƿ�Ϊ��
	if (image.empty())
	{
		cerr << "" << endl;
		return -1;
	}
	//����Ҷ�ͼ��ת�ɻҶ�ͼ
	Mat grayImage;
	cvtColor(image, grayImage, COLOR_BGR2GRAY);
	//double x=compareHist(hist,hist,/*CV_COMP_CORREL��CV_COMP_INTERSECT*/CV_COMP_BHATTACHARYYA);
	//ֱ��ͼͼ��
	Mat hist = getHistograph(grayImage);
	return 0;
}


Mat getHistograph(const Mat grayImage)
{
	//������ֱ��ͼ��ͨ����Ŀ����0��ʼ����
	int channels[] = { 0 };
	//����ֱ��ͼ����ÿһά�ϵĴ�С������Ҷ�ͼֱ��ͼ�ĺ�������ͼ��ĻҶ�ֵ����һά��bin�ĸ���
	//���ֱ��ͼͼ�������bin����Ϊx��������bin����Ϊy����channels[]={1,2}��ֱ��ͼӦ��Ϊ��ά�ģ�Z����ÿ��bin��ͳ�Ƶ���Ŀ
	const int histSize[] = { 256 };
	//ÿһάbin�ı仯��Χ
	float range[] = { 0,256 };

	//����bin�ı仯��Χ��������channelsӦ�ø�channelsһ��
	const float* ranges[] = { range };

	//����ֱ��ͼ�����������ֱ��ͼ����
	Mat hist;
	//opencv�м���ֱ��ͼ�ĺ�����hist��СΪ256*1��ÿ�д洢��ͳ�Ƶĸ��ж�Ӧ�ĻҶ�ֵ�ĸ���
	calcHist(&grayImage, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);//cv����cvCalcHist

																					 //�ҳ�ֱ��ͼͳ�Ƶĸ��������ֵ��������Ϊֱ��ͼ������ĸ�
	double maxValue = 0;
	//�Ҿ����������Сֵ����Ӧ�����ĺ���
	minMaxLoc(hist, 0, &maxValue, 0, 0);
	//���ֵȡ��
	int rows = cvRound(maxValue);
	//����ֱ��ͼͼ��ֱ��ͼ������ĸ���Ϊ����������Ϊ256(�Ҷ�ֵ�ĸ���)
	//��Ϊ��ֱ��ͼ��ͼ�������Ժڰ���ɫΪ���֣���ɫΪֱ��ͼ��ͼ��
	Mat histImage = Mat::zeros(rows, 256, CV_8UC1);

	//ֱ��ͼͼ���ʾ
	for (int i = 0; i<256; i++)
	{
		//ȡÿ��bin����Ŀ
		int temp = (int)(hist.at<float>(i, 0));
		//���bin��ĿΪ0����˵��ͼ����û�иûҶ�ֵ��������Ϊ��ɫ
		//���ͼ�����иûҶ�ֵ���򽫸��ж�Ӧ������������Ϊ��ɫ
		if (temp)
		{
			//����ͼ�������������Ͻ�Ϊԭ�㣬����Ҫ���б任��ʹֱ��ͼͼ�������½�Ϊ����ԭ��
			histImage.col(i).rowRange(Range(rows - temp, rows)) = 255;
		}
	}
	//����ֱ��ͼͼ���и߿��ܸܺߣ���˽���ͼ�����Ҫ���ж�Ӧ��������ʹֱ��ͼͼ���ֱ��
	Mat resizeImage;
	resize(histImage, resizeImage, Size(256, 256));
	return resizeImage;
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

