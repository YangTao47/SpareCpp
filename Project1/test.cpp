#include <iostream>
#include <mutex>  // 对于 std::unique_lock
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

// 解释：下列输出在单核机器上生成。 thread1 开始时，它首次进入循环并调用 increment() ，
// 随后调用 get() 。然而，在它能打印返回值到 std::cout 前，调度器将 thread1 置于休眠
// 并唤醒 thread2 ，它显然有足够时间一次运行全部三个循环迭代。再回到 thread1 ，它仍在首个
// 循环迭代中，它最终打印其局部的计数器副本的值，即 1 到 std::cout ，再运行剩下二个循环。
// 多核机器上，没有线程被置于休眠，且输出更可能为递增顺序。