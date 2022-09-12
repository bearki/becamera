// becamera_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
//#include <windows.h>
extern "C"
{
#include "becamera.h"
}

//using namespace becamera;
using namespace std;
using namespace std::chrono;

// 获取毫秒时间戳
long long timeStampMill() {
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

int main()
{
	// 初始化句柄
	CameraHandle handle;
	StatusCode status = InitCameraHandel(&handle);
	if (status != StatusCode::SUCCESS)
	{
		cerr << "InitCameraHandel Failed: " << status << endl;
		return 0;
	}

	// 打开相机
	status = OpenCamera(handle, 0);
	if (status != StatusCode::SUCCESS)
	{
		cerr << "OpenCamera Failed: " << status << endl;
		return 0;
	}

	// 开始计时
	cout << "取流开始" << endl;
	auto timeStampStart = timeStampMill();

	// 取1000张
	for (size_t i = 0; i < 1000; i++)
	{
		auto timeStampStartTmp = timeStampMill();

		// 取一张图像流
		CameraStream* stream;
		status = GetCameraStream(handle, &stream);
		if (status != StatusCode::SUCCESS)
		{
			cerr << "GetCameraStream Failed: " << status << endl;
			return 0;
		}

		auto timeStampEndTmp = timeStampMill();
		cout << "第" << i << "张图耗时：" << timeStampEndTmp - timeStampStartTmp << "ms" << endl;

		// 格式化字符串
		stringstream fmt;
		// 不足3位的时候，自动在数字前面加0，比如数字1补完变成001
		fmt << "data/test_" << setw(4) << setfill('0') << i << ".jpg";

		// 保存到文件
		ofstream outfile;
		outfile.open(fmt.str(), ios::out | ios::trunc | ios::binary);
		outfile.write((char*)stream->data, stream->size);
		outfile.flush();
		outfile.close();

		// 释放流
		status = FreeCameraStream(handle, &stream);
		if (status != StatusCode::SUCCESS)
		{
			cerr << "FreeCameraStream Failed: " << status << endl;
			return 0;
		}

		// 限制取流频率
		//Sleep(1000 / 40);
	}

	// 结束计时
	auto timeStampEnd = timeStampMill();
	cout << "所有取流结束，累计耗时：" << timeStampEnd - timeStampStart << "ms" << endl;

	// 释放句柄，关闭相机
	status = UnInitCameraHandel(&handle);
	if (status != StatusCode::SUCCESS)
	{
		cerr << "FreeCameraStream Failed: " << status << endl;
		return 0;
	}

	cout << "Operator Success" << endl;
	return 0;
}
