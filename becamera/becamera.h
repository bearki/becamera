#pragma once

#ifndef BECAMERA_H
#define BECAMERA_H

#include <stddef.h>
#include <stdint.h>

// BeCameraVersion BeCamera相关版本信息
typedef struct BeCameraVersion
{
	char *becamera; // BeCamera版本信息
	char *ffmpeg;	// FFmpeg版本信息
} BeCameraVersion;

// BeCameraStatusCode 响应状态码枚举
typedef enum BeCameraStatusCode
{
	SUCCESS = 0,						  // 成功
	ERR_PARAM = 1,						  // 参数异常
	ERR_CREATE_AV_FORMAT_CONTEXT = 2,	  // 创建FFMPEG的AV格式上下文失败
	ERR_FIND_VIDEO_INPUT_DEVICE = 3,	  // 查询视频输入设备失败
	ERR_BECAMERA_HANDLE_EMPTY = 4,		  // BeCamera句柄为空
	ERR_NOT_FOUND_VIDEO_INPUT_DEVICE = 5, // 未找到视频输入设备
	ERR_FIND_DEVICE_STREAM_INFO = 6,	  // 查询设备支持的流信息失败
	ERR_NOT_SUPPORT_VIDEO_STREAM = 7,	  // 设备不支持视频流
	ERR_OPEN_VIDEO_INPUT_DEVICE = 8,	  // 打开视频输入设备失败
	ERR_NOT_FOUND_VIDEO_CODEC = 9,		  // 未查询到视频解码器
	ERR_NOT_OPEN_VIDEO_INPUT_DEVICE = 10, // 视频输入设备未打开
	ERR_VIDEO_STREAM_ALLOC = 11,		  // 视频流内存空间开辟失败
	ERR_GET_CAMERA_STREAM = 12,			  // 取流失败
} BeCameraStatusCode;

// BeCameraHandle BeCamera句柄
typedef void *BeCameraHandle;

// BeCameraDevice 相机设备结构
typedef struct BeCameraDevice
{
	uint8_t id;		   // 相机ID
	char *name;		   // 相机名称
	char *description; // 设备位置
} BeCameraDevice;

// BeCameraDeviceList 相机列表
typedef struct BeCameraDeviceList
{
	BeCameraDevice *list; // 相机列表
	size_t listSize;	  // 相机数量
} BeCameraDeviceList;

// BeCameraStream 相机流结构
typedef struct BeCameraStream
{
	uint8_t *data;	 // 相机流
	size_t dataSize; // 相机流大小
} BeCameraStream;

#ifdef __cplusplus
extern "C"
{
#endif

	// BeCameraGetVersion 获取BeCamera相关版本信息
	// @return BeCamera相关版本信息
	BeCameraVersion *BeCameraGetVersion();

	// BeCameraInit 初始化BeCamera句柄
	// @param[out] becameraHandleAddress BeCamera句柄储存地址
	// @return 状态码
	BeCameraStatusCode BeCameraInit(BeCameraHandle *becameraHandleAddress);

	// BeCameraUnInit 释放BeCamera句柄
	// @param becameraHandleAddress BeCamera句柄储存地址
	// @return 状态码
	BeCameraStatusCode BeCameraUnInit(BeCameraHandle *becameraHandleAddress);

	// BeCameraGetDeviceList 获取相机列表
	// @param becameraHandle BeCamera句柄
	// @param[out] devices 相机列表储存地址
	// @return 状态码
	BeCameraStatusCode BeCameraGetDeviceList(BeCameraHandle becameraHandle, BeCameraDeviceList **devices);

	// BeCameraOpenDevice 打开相机
	// @param becameraHandle BeCamera句柄
	// @param cameraID 相机ID
	// @return 状态码
	BeCameraStatusCode BeCameraOpenDevice(BeCameraHandle becameraHandle, uint8_t cameraID);

	// BeCameraGetStream 获取相机流
	// @param becameraHandle BeCamera句柄
	// @param[out] streamAddress 相机流储存地址
	// @return 状态码
	BeCameraStatusCode BeCameraGetStream(BeCameraHandle becameraHandle, BeCameraStream **streamAddress);

	// BeCameraFreeStream 释放相机流
	// @param streamAddress 相机流储存地址
	// @return 状态码
	BeCameraStatusCode BeCameraFreeStream(BeCameraStream **streamAddress);

#ifdef __cplusplus
};
#endif

#endif
