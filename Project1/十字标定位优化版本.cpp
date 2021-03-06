#include<iostream>
#include<opencv2\opencv.hpp>
#include<chrono>
#include"line2Dup.h"
using namespace std::chrono;
using namespace std;
using namespace cv;
static std::string prefix = "test/";
class LocateCrossMark
{
public:
	Point locateCrossMark(const Mat& Image)
	{
		Point2f p;
		vector<vector<Point>>contours;
		Mat gray_image;
		cvtColor(Image, gray_image, CV_BGR2GRAY);
		threshold(gray_image, gray_image, 1, 255, THRESH_OTSU + THRESH_BINARY);
		findContours(gray_image, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
		p = operateRectangular(Image, contours);
		return p;
	}

	void train()
	{
		trainTemplate();
	}

	Point2f test(Mat& Image)
	{
		Point2f P;
		P = matchTemplate(Image);
		return P;
	}

private:
	Point2f operateRectangular(const Mat& Image, const vector<vector<Point>>contours)
	{
		Mat box;
		Point2f p1;
		double h_minVal = 0, h_maxVal = 0;
		double wminVal = 0, wmaxVal = 0;
		float circle_radius = 0;
		int contours_size = (int)contours.size();
		for (int i = 0; i < contours_size; i++)
		{
			RotatedRect r = minAreaRect(Mat(contours[i]));
			boxPoints(r, box);
			minMaxIdx(box.col(0), &h_minVal, &h_maxVal);
			minMaxIdx(box.col(1), &wminVal, &wmaxVal);
			double height = h_maxVal - h_minVal;
			double width = wmaxVal - wminVal;
			if ((float)width / height >= 0.9 && (float)width / height <= 1.2 && (float)width * height >= 40000 && (float)width * height <= 50000)
			{
				minEnclosingCircle(box, p1, circle_radius);
				circle(Image, p1, (int)circle_radius, Scalar(0, 0, 255));
			}
		}
		return p1;
	}

	void trainTemplate(bool use_rot = true)
	{
		line2Dup::Detector detector(128, { 4, 8 });
		Mat img = imread(prefix + "train.jpg");
		assert(!img.empty() && "check your img path");
		resize(img, img, Size(205, 205));
		Mat mask = Mat(img.size(), CV_8UC1, { 255 });

		shape_based_matching::shapeInfo_producer shapes(img, mask);
		shapes.angle_range = { 0, 360 };
		shapes.angle_step = 2;

		shapes.scale_range = { 1 }; // support just one
		shapes.produce_infos();
		vector<shape_based_matching::shapeInfo_producer::Info> infos_have_templ;
		string class_id = "test";

		bool is_first = true;

		// for other scales you want to re-extract points: 
		// set shapes.scale_range then produce_infos; set is_first = false;

		int first_id = 0;
		float first_angle = 0;
		for (auto& info : shapes.infos) {
			Mat to_show = shapes.src_of(info);

			std::cout << "\ninfo.angle: " << info.angle << std::endl;
			int templ_id;

			if (is_first) {
				templ_id = detector.addTemplate(shapes.src_of(info), class_id, shapes.mask_of(info));
				first_id = templ_id;
				first_angle = info.angle;

				if (use_rot) is_first = false;
			}
			else {
				templ_id = detector.addTemplate_rotate(class_id, first_id,
					info.angle - first_angle,
					{ shapes.src.cols / 2.0f, shapes.src.rows / 2.0f });
			}

			auto templ = detector.getTemplates("test", templ_id);
			for (int i = 0; i<templ[0].features.size(); i++) {
				auto feat = templ[0].features[i];
				cv::circle(to_show, { feat.x + templ[0].tl_x, feat.y + templ[0].tl_y }, 3, { 0, 0, 255 }, -1);
			}

			if (templ_id != -1) 
			{
				infos_have_templ.push_back(info);
			}
		}
		detector.writeClasses(prefix + "%s_templ.yaml");
		shapes.save_infos(infos_have_templ, prefix + "test_info.yaml");
	}
	
	Point2f matchTemplate(Mat &test_img)
	{
		line2Dup::Detector detector(128, { 4, 8 });
		vector<std::string> ids;
		Point2f P;
		ids.push_back("test");
		detector.readClasses(ids, prefix + "%s_templ.yaml");

		// angle & scale are saved here, fetched by match id
		auto infos = shape_based_matching::shapeInfo_producer::load_infos(prefix + "test_info.yaml");

		Mat Img = test_img.clone();
		assert(Img.isContinuous());

		auto matches = detector.match(Img, 90, ids);

		if (Img.channels() == 1) cvtColor(Img, Img, CV_GRAY2BGR);
		//cout << "matches.size(): " << matches.size() << std::endl;
		size_t top5 = 1;
		if (top5>matches.size()) top5 = matches.size();
		for (size_t i = 0; i<top5; i++) 
		{
			auto match = matches[i];
			auto templ = detector.getTemplates("test",match.template_id);

			float train_Img_half_width = 205 / 2.0f;
			float train_Img_half_height = 205 / 2.0f;
			// center x,y of train_Img in test Img
			float x = match.x - templ[0].tl_x + train_Img_half_width;
			float y = match.y - templ[0].tl_y + train_Img_half_height;
			P.x = x;
			P.y = y;

			//circle(Img, { (int)x, (int)y }, 1, { 255,0,0 }, -1);
			//drawRect(Img, match, infos, x, y);
		}
		return P;
	}

	void drawRect(Mat& src, line2Dup::Match match, vector<shape_based_matching::shapeInfo_producer::Info> infos, float x, float y)
	{
		float r_scaled = 205 / 2.0f * infos[match.template_id].scale;
		putText(src, to_string(int(round(match.similarity))),
			Point(match.x + r_scaled - 10, match.y - 3), FONT_HERSHEY_PLAIN, 2, Scalar(176, 48, 96));

		RotatedRect rotatedRectangle({ x, y }, { 2 * r_scaled, 2 * r_scaled }, -infos[match.template_id].angle);
		Point2f vertices[4];
		rotatedRectangle.points(vertices);
		for (int i = 0; i<4; i++) 
		{
			int next = (i + 1 == 4) ? 0 : (i + 1);
			line(src, vertices[i], vertices[next], Scalar(176,48,96), 2);
		}
	}
};

int main()
{
	LocateCrossMark locate;
	Mat image = imread("??????\\???\\2.bmp");
	Point2f p1;
	auto start_time = high_resolution_clock::now();


	p1 = locate.test(image);
	cout << p1 << endl;
	time_point<high_resolution_clock> end_time = high_resolution_clock::now();
	milliseconds processing_time = duration_cast<milliseconds>(end_time - start_time) / 10;
	cout << processing_time.count() << endl;
	system("pause");
	return 0;
}
