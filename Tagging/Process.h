#pragma once

#include <opencv2\highgui\highgui.hpp>
#include <string>
#include <map>
#include <utility>
#include <fstream>
#include <filesystem>
#include "tagPicture.h"

#ifndef _PROCESS_H_
#define _PROCESS_H_

class PictureProcess;

class HandlePictures {
public:
	// 获取唯一类实例的静态函数
	static HandlePictures& GetInstance(
		const std::string & _picture_path,
		const std::string & _save_path,
		const std::string & _txt_path = ""
	);
	// 处理函数
	void Handle();

public:
	// 更改键位的函数
	static bool changeKeysOfStates(const std::vector<int>&);	// 更改各种状态的键位，如退出等
	static bool changeKeysOfManual(const std::vector<int>&);	// 更改手动选框时的键位
	static bool changeKeysOfModes(const std::vector<int>&);		// 更改选择矩形组模式的键位，包装了 Recs 中的函数

private:
	// 检查按键是否为状态键位
	static bool keyIsValid(int key);

private:
	// 私有构造函数，防止生成多个实例
	HandlePictures(
		const std::string & _picture_path,
		const std::string & _save_path,
		const std::string & _txt_path);

	// 构造函数中使用的初始化函数
	bool Init();
	// 写入结果
	bool writeResult();

private:
	// 处理单张图片的嵌套类
	class PictureProcess {
	public:
		PictureProcess(
			const std::string& _full_path,
			const std::string& _name,
			const HandlePictures &_pictures)
			: full_path(_full_path),
			name(_name),
			pictures(_pictures) {}

		std::pair<int, cv::Rect> begin();
		inline void savePicture(
			const cv::Mat &picture,
			cv::Rect rect,
			const std::string &name);

	private:
		std::string full_path;		// 完整路径的图片名
		std::string name;		// 图片名称，有后缀
		const HandlePictures &pictures;
	};

	// 保存图片信息的结构体
	struct PictureInfo {
		PictureInfo(std::string _name, cv::Rect _rect)
			: name(_name), rect(_rect) {}
		std::ostream& write(std::ostream& out) const
		{
			out << name << " " << rect.x << " " << rect.y << " "
				<< rect.width << " " << rect.height;
			return out;
		}
	
		std::string name;
		cv::Rect rect;
	};

private:
	int begin;		// 起始图片序号
	int end;		// 终止序号，为最后一张图片
	std::string picture_path;
	std::string save_path;
	std::string txt_path;
	std::vector<PictureInfo> pictureInfos;	// 保存信息

private:
	static std::map<std::string, int> state_map;
	static std::map<std::string, int> manual_map;
};

#endif // !_PROCESS_H_
