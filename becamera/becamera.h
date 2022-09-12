#pragma once

#ifndef BECAMERA_H
#define BECAMERA_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

// BeStatusCode 响应状态码枚举
typedef enum BeStatusCode {
	SUCCESS = 0, // 成功
	ERR_PARAM = 1, // 参数异常
	ERR_CREATE_AV_FORMAT_CONTEXT = 2, // 创建FFMPEG的AV格式上下文失败
	ERR_SET_INPUT_FORMAT = 3, // 配置采集格式失败
	ERR_SET_FRAMERATE = 4, // 配置采集帧率失败
	ERR_SET_VIDEO_SIZE = 5, // 配置采集分辨率失败
	ERR_CAMERA_HANDLE_EMPTY = 6, // 相机句柄为空
	ERR_NOT_FOUND_VIDEO_DEVICE = 7, // 未找到视频输入设备
	ERR_OPEN_VIDEO_DEVICE = 8, // 打开视频输入设备失败
	ERR_FIND_STREAM_INFO = 9, // 查询设备流信息失败
	ERR_NOT_FOUND_VIDEO_STREAM_DEVICE = 10, // 未查询到支持视频流的设备
	ERR_CAMERA_STREAM_ALLOC = 11,// 相机流内存空间开辟失败
	ERR_GET_CAMERA_STREAM = 12, // 取流失败
} BeStatusCode;

// BeCameraHandle 相机句柄
typedef void* BeCameraHandle;

// BeCameraStream 相机流结构
typedef struct BeCameraStream {
	uint8_t* data; // 相机流
	size_t size; // 相机流大小
} BeCameraStream;

#ifdef __cplusplus
extern "C" {
#endif

	// BeInitCameraHandel 初始化相机句柄
	// @param cameraHandleAddress 相机句柄储存地址
	// @return 状态码
	BeStatusCode BeInitCameraHandel(BeCameraHandle* cameraHandleAddress);

	// BeUnInitCameraHandel 释放相机句柄
	// @param cameraHandleAddress 相机句柄储存地址
	// @return 状态码
	BeStatusCode BeUnInitCameraHandel(BeCameraHandle* cameraHandleAddress);

	// BeOpenCamera 打开相机
	// @param cameraHandle 相机句柄
	// @param index 需要打开第几个相机
	// @return 状态码
	BeStatusCode BeOpenCamera(BeCameraHandle cameraHandle, int32_t index);

	// BeGetBeCameraStream 获取相机流
	// @param cameraHandle 相机句柄
	// @param streamAddress 相机流储存地址
	// @return 状态码
	BeStatusCode BeGetBeCameraStream(BeCameraHandle cameraHandle, BeCameraStream** streamAddress);

	// BeFreeBeCameraStream 释放相机流
	// @param cameraHandle 相机句柄
	// @param streamAddress 相机流储存地址
	// @return 状态码
	BeStatusCode BeFreeBeCameraStream(BeCameraHandle cameraHandle, BeCameraStream** streamAddress);

#ifdef __cplusplus
};
#endif

#endif
