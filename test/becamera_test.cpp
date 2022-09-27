// becamera_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <chrono>
#include "becamera/becamera.h"

#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "fmt/chrono.h"

using namespace std::chrono;

// 获取毫秒时间戳
auto timeStampMill()
{
	return system_clock::now();
}

int main()
{
	// 获取版本信息
	BeCameraVersion *v = BeCameraGetVersion();
	if (v == nullptr)
	{
		fmt::print("BeCameraGetVersion Failed, version info is nullptr\n");
		return -1;
	}
	fmt::print("BeCamera Version: {}\n", v->becamera);
	fmt::print("FFmpeg Version: {}\n", v->ffmpeg);

	// 初始化句柄
	BeCameraHandle handle;
	BeCameraStatusCode status = BeCameraInit(&handle);
	if (status != BeCameraStatusCode::SUCCESS)
	{
		fmt::print("InitCameraHandel Failed: {}\n", (int)status);
		return -1;
	}

	// 获取相机列表
	BeCameraDeviceList *devices;
	status = BeCameraGetDeviceList(handle, &devices);
	if (status != BeCameraStatusCode::SUCCESS)
	{
		fmt::print("BeCameraGetDeviceList Failed: {}\n", (int)status);
		return -1;
	}
	if (devices == nullptr)
	{
		fmt::print("BeCameraGetDeviceList Failed, devices is nullptr\n");
		return -1;
	}

	uint8_t id = 2;
	for (size_t i = 0; i < devices->listSize; i++)
	{
		BeCameraDevice device = devices->list[i];
		fmt::print("[ID: {}, Name: \"{}\", Desc: \"{}\"]\n", device.id, device.name, device.description);
	}

	// 打开相机
	status = BeCameraOpenDevice(handle, id);
	if (status != BeCameraStatusCode::SUCCESS)
	{
		fmt::print("OpenCamera Failed: {}\n", (int)status);
		return -1;
	}

	// 开始计时
	fmt::print("取流开始\n");
	auto timeStampStart = timeStampMill();

	// 取1000张
	for (size_t i = 0; i < 1000; i++)
	{
		auto timeStampStartTmp = timeStampMill();

		// 取一张图像流
		BeCameraStream *stream;
		status = BeCameraGetStream(handle, &stream);
		if (status != BeCameraStatusCode::SUCCESS)
		{
			fmt::print("GetCameraStream Failed: {}\n", (int)status);
			return -1;
		}

		auto timeStampEndTmp = timeStampMill();
		fmt::print("第{1}张图耗时：{0}\n", duration_cast<milliseconds>(timeStampEndTmp - timeStampStartTmp), i);

		// 格式化字符串
		std::string fname;
		// 不足3位的时候，自动在数字前面加0，比如数字1补完变成001
		fname = fmt::format("data/test_{:04}.jpg", i);
		fmt::print("{}, size:{}\n", fname, stream->dataSize);

		// 保存到文件
		std::ofstream outfile;
		outfile.open(fname, std::ios::out | std::ios::binary);
		outfile.write((char *)stream->data, stream->dataSize);
		outfile.flush();
		outfile.close();

		// 释放流
		status = BeCameraFreeStream(&stream);
		if (status != BeCameraStatusCode::SUCCESS)
		{
			fmt::print("BeCameraFreeStream Failed: {}\n", (int)status);
			return -1;
		}

		// 限制取流频率
		// Sleep(1000 / 60);
	}

	// 结束计时
	auto timeStampEnd = timeStampMill();
	fmt::print("所有取流结束，累计耗时：{}\n", timeStampEnd - timeStampStart);

	// 释放句柄，关闭相机
	status = BeCameraUnInit(&handle);
	if (status != BeCameraStatusCode::SUCCESS)
	{
		fmt::print("BeCameraUnInit Failed: {}\n", (int)status);
		return -1;
	}

	return 0;
}
