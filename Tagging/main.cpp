#include <string>
#include "tagPicture.h"
#include "Process.h"


// 路径名
const std::string readPath = "E:\\testpicture";
const std::string savePath = "E:\\test";
const std::string txtPath = "E:\\test";

// 自定义绘制矩形的属性
const cv::Scalar color = { 255, 255, 255 };
const int thickness = 1;
const int linetype = 8;

std::vector<std::vector<cv::Point>> func(cv::Mat &picture)
{
	return { { {100, 100}, {200, 200} } };
}

int main()
{
	// 测试添加函数和丢弃函数
	Recs::addFunc(func, 'I');
	Recs::popFunc(func);

	if (HandlePictures::changeKeysOfStates({ 'W', 'V', 'B', 'Q', 'E' })
		&& HandlePictures::changeKeysOfManual({ 'W', ' ' })
		&& HandlePictures::changeKeysOfModes({ 'A', 'S', 'D', 'F', 'G', 'H' }))
	{
		HandlePictures &handle = HandlePictures::GetInstance(
			readPath, savePath
		);
		handle.Handle();
	}

	return EXIT_SUCCESS;
}