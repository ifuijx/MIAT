#pragma once

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <string>
#include <vector>

#ifndef _TAGPICTURE_H_
#define _TAGPICTURE_H_

/*
 * 根据 state 状态获取一系列待选矩形
 */
class Recs {
public:
	// DEFAULT 表示创建矩形组实例时，默认生成矩形组
	// NONE    表示创建矩形组实例时，不生成矩形组
	enum { DEFAULT, NONE };
public:
	Recs(cv::Mat _picture, int mode = DEFAULT);		// 构造函数
	void set(int key);								// 设置模式
	cv::Rect choose(std::string windowname) const;	// 手动选框
	void draw(std::string windowname) const;		// 显示矩形
	cv::Rect findRect(cv::Point, int write = 'W', int skip = ' ');	// 根据选中点挑出待选矩形

public:
	// 自定义矩形属性
	static void setColor(cv::Scalar _color) { color = _color; }
	static void setThickness(int _thickness) { thickness = _thickness; }
	static void setLinetype(int _linetype) { linetype = _linetype; }
	static bool setInputKey(std::vector<int> input);
	static std::size_t getFuncMount();

public:
	// 挑选矩形的最小面积
	static int area;

private:
	// 显示矩形的属性
	static cv::Scalar color;
	static int thickness;
	static int linetype;


public:
	using Rectangles = std::vector<std::vector<cv::Point>>;
	using RectanglesFunc = Rectangles(*)(cv::Mat &picture);

private:
	static std::vector<RectanglesFunc> funcVec;
	static std::vector<int> funcKey;

public:
	static void addFunc(RectanglesFunc func, int key);
	static void popFunc(RectanglesFunc func);

private:
	// 根据矩形最小面积筛选矩形
	void filterRect(const std::vector<std::vector<cv::Point>>& origin,
		std::vector<cv::Rect>& dest);

	cv::Mat picture;
	std::vector<cv::Rect> vrect;
};

#endif
