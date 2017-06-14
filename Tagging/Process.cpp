#include "Process.h"

// 未命名名字空间，防止外部使用
namespace {
	namespace fs = std::experimental::filesystem;

	void mouseCallBack(int mouse_event, int x, int y, int, void *param)
	{
		if (mouse_event == cv::EVENT_LBUTTONDOWN) {
			cv::Point *pPoint = static_cast<cv::Point *>(param);
			pPoint->x = x;
			pPoint->y = y;
		}
	}

	cv::Point getPoint(const std::string& window_name)
	{
		cv::Point point(-1, -1);
		cv::setMouseCallback(window_name, mouseCallBack, 
			static_cast<void *>(&point));
		while (true)
		{
			if (cv::waitKey(10) >= 0)
			{
			}
			if (point.x > 0)
				break;
		}
		return point;
	}
}

std::map<std::string, int> HandlePictures::state_map = {
	{ "WRITE", 'W' },
	{ "MANU", 'V' },
	{ "BACK", 'B' },
	{ "QUIT", 'Q' },
	{ "EXIT", 'E' }
};

std::map<std::string, int> HandlePictures::manual_map = {
	{ "CHOOSE", 'W' },
	{ "SKIP", ' ' }
};

bool HandlePictures::changeKeysOfStates(const std::vector<int> &keys)
{
	if (keys.size() < state_map.size())
		return false;
	for (const auto ele : keys)
		if (ele < 0 || ele > 255)
			return false;
	state_map["WRITE"] = keys[0];
	state_map["MANU"] = keys[1];
	state_map["BACK"] = keys[2];
	state_map["QUIT"] = keys[3];
	state_map["EXIT"] = keys[4];

	return true;
}

bool HandlePictures::changeKeysOfManual(const std::vector<int> &keys)
{
	if (keys.size() < manual_map.size())
		return false;
	for (const auto ele : keys)
		if (ele < 0 || ele > 255)
			return false;
	manual_map["CHOOSE"] = keys[0];
	manual_map["SKIP"] = keys[1];

	return true;
}

bool HandlePictures::changeKeysOfModes(const std::vector<int> &keys)
{
	return Recs::setInputKey(keys);
}

HandlePictures & HandlePictures::GetInstance(
	const std::string & _picture_path,
	const std::string & _save_path,
	const std::string & _txt_path)
{
	static HandlePictures instance(
		_picture_path,
		_save_path,
		(_txt_path == "" ? _save_path : _txt_path));
	return instance;
}

HandlePictures::HandlePictures(
	const std::string & _picture_path,
	const std::string & _save_path,
	const std::string & _txt_path)
	: picture_path(_picture_path),
	save_path(_save_path),
	txt_path(_txt_path),
	begin(0),
	end(0)
{
}

bool HandlePictures::Init()
{
	std::string input;

	int in_num;
	do {
		try {
			std::cout << "Input the index of the first picture to handle with: ";
			std::cin >> input;
			in_num = std::stoi(input);
		}
		// 若转换错误，则重新输入
		catch (std::exception)
		{
			continue;
		}
		// 若转换成功，则退出
		break;
	} while (true);

	begin = in_num;

	for (const auto &file : fs::directory_iterator(picture_path))
		++end;

	if (end < begin)
		return false;
	return true;
}

bool HandlePictures::writeResult()
{
	std::ofstream fout(save_path + "\\label.txt");
	if (!fout)
	{
		std::cout << "Save error. Please examine the path." << std::endl;
		return false;
	}

	for (const auto &ele : pictureInfos)
		ele.write(fout) << std::endl;

	return true;
}

bool HandlePictures::keyIsValid(int key)
{
	for (const auto &ele : state_map)
	{
		if (ele.second == key)
			return true;
	}
	return false;
}

void HandlePictures::Handle()
{
	if (!Init())
	{
		std::cout << "Input error, please examine the number you input." << std::endl;
		return;
	}

	for (int start = begin; start <= end; ++start)
	{
		std::string name = std::to_string(start) + ".jpg";
		std::string full_name = picture_path + "\\" + name;
		
		if (!fs::exists(full_name))
		{
			std::cout << full_name << " not exists." << std::endl;
			return;
		}
		// 为单张图片创建 PictureProcess 实例
		PictureProcess picture_process(full_name, name, *this);
		std::pair<int, cv::Rect> result = picture_process.begin();

		// 若得到结果，则保存
		if (result.first == state_map["MANU"]
			|| result.first == state_map["WRITE"])
		{
			pictureInfos.push_back({ name, result.second });
		}
		// 若退回，则如下处理
		else if (result.first == state_map["BACK"])
		{
			if (start == begin)
			{
				--start;
				continue;
			}

			std::string pre_path =
				save_path + "\\" + std::to_string(--start) + "+.jpg";
			if (fs::exists(pre_path))
				fs::remove(pre_path);
			if (pictureInfos.size() > 0 && pictureInfos.back().name == name)
				pictureInfos.pop_back();
			--start;
		}
		// 若退出，则进行下一循环
		else if (result.first == state_map["QUIT"])
			;
		// 若退出或出错，则函数结束
		else if (result.first == state_map["EXIT"] || result.first == -1)
		{
			writeResult();
			return;
		}
	}
}

// 处理函数
std::pair<int, cv::Rect> HandlePictures::PictureProcess::begin()
{
	cv::Mat picture = cv::imread(full_path);
	int key;
	cv::Rect rect = { -1, -1, 0, 0 };

	// 若读取错误
	if (!picture.data)
	{
		std::cout << "Read " << full_path << " failed." << std::endl;
		return { -1, rect };
	}

	cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
	Recs recs(picture);

	std::string save_name(name);
	auto insert = save_name.find('.');
	save_name.insert(insert, 1, '+');

	do {
		recs.draw(name);
		key = cv::waitKey(0);
		
		if (key >= 'a' && key <= 'z')
			key += 'A' - 'a';

		recs.set(key);

		if (key == state_map["MANU"])
		{
			rect = recs.choose(name);
			savePicture(picture, rect, pictures.save_path + "\\" +  save_name);
		}
		else if (key == state_map["WRITE"])
		{
			rect = recs.findRect(
				getPoint(name),
				manual_map["CHOOSE"],
				manual_map["SKIP"]
			);
			// 没有选中矩形，则将 key 改为无效值以再次循环
			if (rect == cv::Rect())
				key = -1;
			else
			{
				savePicture(picture, rect, pictures.save_path + "\\" + save_name);
			}
		}
	} while (!HandlePictures::keyIsValid(key));

	cv::destroyWindow(name);

	return std::make_pair(key, rect);
}

void HandlePictures::PictureProcess::savePicture(
	const cv::Mat & picture,
	cv::Rect rect,
	const std::string & name)
{
	cv::Mat part_picture(picture, rect);
	cv::imwrite(name, part_picture);
}
