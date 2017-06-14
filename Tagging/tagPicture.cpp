#include "tagPicture.h"

// 初始化静态变量
int Recs::area = 200;

cv::Scalar Recs::color = { 0, 255, 0 };
int Recs::thickness = 1;
int Recs::linetype = 8;

namespace {

	Recs::Rectangles modeA(cv::Mat & picture)
	{
		cv::Mat ROIPicture;
		cv::cvtColor(picture, ROIPicture, cv::COLOR_RGB2GRAY);
		cv::Mat binaryPicture;
		cv::threshold(ROIPicture, binaryPicture, 127, 255, 0);
		std::vector<std::vector<cv::Point>> rects;
		cv::findContours(binaryPicture, rects, 1, 2);

		return rects;
	}

	Recs::Rectangles modeB(cv::Mat & picture)
	{
		cv::Mat ROIPicture;
		cv::Canny(picture, ROIPicture, 100, 200);
		std::vector<std::vector<cv::Point>> rects;
		cv::findContours(ROIPicture, rects, 1, 2);

		return rects;
	}

	Recs::Rectangles modeC(cv::Mat & picture)
	{
		cv::Mat ROIPicture;
		cv::cvtColor(picture, ROIPicture, cv::COLOR_RGB2HSV);
		std::vector<cv::Mat> hsv;
		cv::split(ROIPicture, hsv);
		cv::Mat s;
		cv::threshold(hsv.at(1), s, 120, 255, 0);
		std::vector<std::vector<cv::Point>> rects;
		cv::findContours(s, rects, 1, 2);

		return rects;
	}

	Recs::Rectangles modeD(cv::Mat & picture)
	{
		std::vector<cv::Mat> bgr;
		cv::split(picture, bgr);
		cv::Mat r;
		cv::threshold(bgr.at(2), r, 120, 255, 0);
		std::vector<std::vector<cv::Point>> rects;
		cv::findContours(r, rects, 1, 2);

		return rects;
	}

	Recs::Rectangles modeE(cv::Mat & picture)
	{
		std::vector<cv::Mat> bgr;
		cv::split(picture, bgr);
		cv::Mat g;
		cv::threshold(bgr.at(1), g, 120, 255, 0);
		std::vector<std::vector<cv::Point>> rects;
		cv::findContours(g, rects, 1, 2);

		return rects;
	}

	Recs::Rectangles modeF(cv::Mat & picture)
	{
		std::vector<cv::Mat> bgr;
		cv::split(picture, bgr);
		cv::Mat b;
		cv::threshold(bgr.at(0), b, 120, 255, 0);
		std::vector<std::vector<cv::Point>> rects;
		cv::findContours(b, rects, 1, 2);

		return rects;
	}
}

std::vector<Recs::RectanglesFunc> Recs::funcVec = { 
	modeA, modeB, modeC, modeD, modeE, modeF
};

std::vector<int> Recs::funcKey = {
	'A', 'B', 'C', 'D', 'E', 'F'
};

// 工具函数
namespace {
	// 鼠标相应函数传入结构体
	struct BindRect {
		cv::Point prePoint;
		cv::Point nextPoint;
		bool begin = false;
		bool end = false;
	};

	// 得出框选矩形的工具函数
	void getValue(int& min, int& length, int a, int b) {
		min = a;
		length = b - a;
		if (a > b) {
			min = b;
			length = a - b;
		}
	}

	// 根据两点得出框选矩形
	inline void getRect(cv::Rect& rect, cv::Point& pre, cv::Point& next) {
		getValue(rect.x, rect.width, pre.x, next.x);
		getValue(rect.y, rect.height, pre.y, next.y);
	}

	void mouseDraw(int mouseEvent, int x, int y, int, void *param)
	{
		BindRect *data = static_cast<BindRect*>(param);
		if (mouseEvent == cv::EVENT_LBUTTONDOWN) {
			data->begin = true;
			data->prePoint.x = x;
			data->prePoint.y = y;
		}
		if (mouseEvent == cv::EVENT_MOUSEMOVE && data->begin) {
			data->nextPoint.x = x;
			data->nextPoint.y = y;
		}
		if (mouseEvent == cv::EVENT_LBUTTONUP) {
			data->nextPoint.x = x;
			data->nextPoint.y = y;
			data->end = true;
		}
	}
}

Recs::Recs(cv::Mat _picture, int mode) :
	picture(_picture),
	vrect(std::vector<cv::Rect>())
{
	if (mode == DEFAULT)
	{
		set(funcKey[0]);
	}
}

void Recs::set(int key)
{
	for (std::size_t i = 0; i < funcKey.size(); ++i) {
		if (key == funcKey[i] && i < funcVec.size()) {
			cv::Mat pictureCopy = picture.clone();
			vrect.clear();
			filterRect(funcVec[i](pictureCopy), vrect);
			return;
		}
	}
}

cv::Rect Recs::choose(std::string windowname) const
{
	cv::imshow(windowname, picture);

	BindRect result, *pResult;
	pResult = &result;

	cv::Rect returnRect;

	cv::setMouseCallback(windowname, mouseDraw, (void*)pResult);
	while (true) {
		if (cv::waitKey(10))
		{}
		if (result.begin) {
			cv::Mat showPicture = picture.clone();

			getRect(returnRect, result.prePoint, result.nextPoint);

			cv::rectangle(showPicture, returnRect, color, thickness, linetype);
			cv::imshow(windowname, showPicture);
		}
		if (result.end)
			break;
	}

	cv::Mat showPicture = picture.clone();
	cv::rectangle(showPicture, returnRect, color, thickness, linetype);
	cv::imshow(windowname, showPicture);
	cv::waitKey(1000);

	cv::destroyWindow(windowname);
	return returnRect;
}

void Recs::draw(std::string windowname) const
{
	cv::Mat showPicture = picture.clone();
	for (const cv::Rect& rect : vrect)
	{
		cv::rectangle(showPicture, rect, color, thickness, linetype);
	}
	cv::imshow(windowname, showPicture);
}

cv::Rect Recs::findRect(cv::Point point, int write, int skip)
{
	cv::Mat ROIPicture;
	cv::namedWindow("ROI");

	for (const cv::Rect& rect : vrect)
	{
		if (point.x < rect.x || point.y < rect.y
			|| point.x > rect.x + rect.width
			|| point.y > rect.y + rect.width)
			continue;
		ROIPicture = picture(rect);
		cv::imshow("ROI", ROIPicture);

		int key = 0;
		do {
			key = cv::waitKey(0);
			if (key >= 'a' && key <= 'z')
				key += 'A' - 'a';
		} while (key != skip && key != write);

		if (key == skip)
			continue;
		else if (key == write) {
			cv::destroyWindow("ROI");
			return rect;
		}
	}
	cv::destroyWindow("ROI");
	return cv::Rect();
}

bool Recs::setInputKey(std::vector<int> input)
{
	if (input.size() == funcVec.size())
	{
		funcKey = std::move(input);
		return true;
	}
	return false;
}

std::size_t Recs::getFuncMount()
{
	return funcVec.size();
}

void Recs::addFunc(RectanglesFunc func, int key)
{
	funcVec.push_back(func);
	funcKey.push_back(key);
}

void Recs::popFunc(RectanglesFunc func)
{
	for (std::size_t i = 0; i < funcVec.size(); ++i)
	{
		if (funcVec[i] == func)
		{
			std::swap(funcVec[i], funcVec.back());
			std::swap(funcKey[i], funcKey.back());

			funcVec.pop_back();
			funcKey.pop_back();
		}
	}
}

void Recs::filterRect(
	const std::vector<std::vector<cv::Point>>& origin,
	std::vector<cv::Rect>& dest)
{
	cv::Rect rect;
	for (const auto& rec : origin) {
		rect = cv::boundingRect(rec);
		if (rect.area() > 200)
			dest.push_back(rect);
	}
}
