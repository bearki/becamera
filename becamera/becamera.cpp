// becamera.cpp : 定义静态库的函数。
//

#include "becamera.h"
#include <string>
extern "C"
{
#include "libavutil/ffversion.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#include "libpostproc/postprocess.h"
#include "libavutil/imgutils.h"
};

using namespace std;


// BeCameraHandleImp 相机句柄的真实实现结构
struct BeCameraHandleImp {
	string ffmpegVersion = ""; // FFMPEG的版本信息
	AVFormatContext* avFmtCtx = nullptr;// FFMPEG的AV格式上下文
	AVDictionary* openCameraOpts = nullptr; // 相机打开时的参数
};

// BeInitCameraHandel 初始化相机句柄
// @param cameraHandleAddress 相机句柄储存地址
// @return 状态码
BeStatusCode BeInitCameraHandel(BeCameraHandle* cameraHandleAddress) {
	// 入参是否正常
	if (cameraHandleAddress == nullptr) {
		return BeStatusCode::ERR_PARAM;
	}

	// 初始化一个句柄，所有属性初始化为0值
	BeCameraHandleImp* handle = new BeCameraHandleImp;
	// 赋值FFMPEG版本信息
	handle->ffmpegVersion = FFMPEG_VERSION;

	// 注册所有设备
	avdevice_register_all();

	// 初始化一个FFMPEG的AV格式上下文
	handle->avFmtCtx = avformat_alloc_context();
	if (handle->avFmtCtx == nullptr) {
		return BeStatusCode::ERR_CREATE_AV_FORMAT_CONTEXT;
	}

	// 初始化打开相机时的参数
	// 配置采集格式
	int ret = av_dict_set(&handle->openCameraOpts, "input_format", "mjpeg", 0);
	if (ret < 0)
	{
		return BeStatusCode::ERR_SET_INPUT_FORMAT;
	}
	// 配置帧率
	ret = av_dict_set(&handle->openCameraOpts, "framerate", "30", 0);
	if (ret < 0)
	{
		return BeStatusCode::ERR_SET_FRAMERATE;
	}
	// 配置分辨率
	ret = av_dict_set(&handle->openCameraOpts, "video_size", "1920*1080", 0);
	if (ret < 0)
	{
		return BeStatusCode::ERR_SET_VIDEO_SIZE;
	}

	// 赋值初始化完成的句柄
	*cameraHandleAddress = (BeCameraHandle)handle;

	// 返回成功
	return BeStatusCode::SUCCESS;
}

// BeUnInitCameraHandel 释放相机句柄
// @param cameraHandleAddress 相机句柄储存地址
// @return 状态码
BeStatusCode BeUnInitCameraHandel(BeCameraHandle* cameraHandleAddress)
{
	// 入参是否正常
	if (cameraHandleAddress == nullptr) {
		return BeStatusCode::ERR_CAMERA_HANDLE_EMPTY;
	}
	if (*cameraHandleAddress == nullptr) {
		return BeStatusCode::ERR_CAMERA_HANDLE_EMPTY;
	}

	// 转换类型
	BeCameraHandleImp* handle = (BeCameraHandleImp*)(*cameraHandleAddress);

	// 释放打开相机时的参数
	if (handle->openCameraOpts != nullptr) {
		av_dict_free(&handle->openCameraOpts);
		handle->openCameraOpts = nullptr;
	}

	// 释放上下文
	if (handle->avFmtCtx != nullptr) {
		// 关闭已打开的视频设备
		avformat_close_input(&handle->avFmtCtx);
		// 释放FFMPEG的AV格式上下文
		avformat_free_context(handle->avFmtCtx);
		handle->avFmtCtx = nullptr;
	}

	// 句柄储存地址值赋空
	*cameraHandleAddress = nullptr;

	// 释放完成
	return BeStatusCode::SUCCESS;
}

// BeOpenCamera 打开相机
// @param cameraHandle 相机句柄
// @param index 需要打开第几个相机
// @return 状态码
BeStatusCode BeOpenCamera(BeCameraHandle cameraHandle, int32_t index)
{
	// 判断句柄
	if (cameraHandle == nullptr)
	{
		return BeStatusCode::ERR_CAMERA_HANDLE_EMPTY;
	}

	// 转换句柄类型
	BeCameraHandleImp* handle = (BeCameraHandleImp*)cameraHandle;

	// 关闭已打开的视频设备
	avformat_close_input(&handle->avFmtCtx);

	// 查找视频输入设备
	const AVInputFormat* ifmt = av_find_input_format("vfwcap");
	if (ifmt == nullptr) {
		return BeStatusCode::ERR_NOT_FOUND_VIDEO_DEVICE;
	}

	// 打开指定的视频输入设备
	int ret = avformat_open_input(&handle->avFmtCtx, (char*)index, ifmt, &handle->openCameraOpts);
	if (ret != 0) {
		return BeStatusCode::ERR_OPEN_VIDEO_DEVICE;
	}

	// 查询设备流信息
	ret = avformat_find_stream_info(handle->avFmtCtx, NULL);
	if (ret < 0) {
		// 关闭已打开的视频设备
		avformat_close_input(&handle->avFmtCtx);
		return BeStatusCode::ERR_FIND_STREAM_INFO;
	}

	// 判断设备是否支持视频流
	size_t inVideoStreamIndex = -1;
	for (size_t i = 0; i < handle->avFmtCtx->nb_streams; i++) {
		if (handle->avFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			inVideoStreamIndex = i;
			break;
		}
	}
	if (inVideoStreamIndex == -1) {
		// 关闭已打开的视频设备
		avformat_close_input(&handle->avFmtCtx);
		return BeStatusCode::ERR_NOT_FOUND_VIDEO_STREAM_DEVICE;
	}

	//// 查询设备支持的解码器
	//AVCodecParameters* inVideoCodecPara = handle->avFmtCtx->streams[inVideoStreamIndex]->codecpar;
	//const AVCodec* inCodec = avcodec_find_decoder(inVideoCodecPara->codec_id);
	//if (!inCodec) {
	//	cout << "cannot find valid video decoder" << endl;
	//	return -1;
	//}
	//// 初始化解码器空间
	//AVCodecContext* inCodecCtx = avcodec_alloc_context3(inCodec);
	//if (!inCodecCtx) {
	//	cout << "cannot alloc valid decode codec context" << endl;
	//	return -1;
	//}
	//// 初始化解码器参数
	//if (avcodec_parameters_to_context(inCodecCtx, inVideoCodecPara) < 0) {
	//	cout << "cannot initialize parameters" << endl;
	//	return -1;
	//}
	//// 打开解码器
	//if (avcodec_open2(inCodecCtx, inCodec, NULL) < 0) {
	//	cout << "cannot open codec" << endl;
	//	return -1;
	//}

	// 视频流设备打开成功
	return BeStatusCode::SUCCESS;
}

// BeGetBeCameraStream 获取相机流
// @param cameraHandle 相机句柄
// @param streamAddress 相机流储存地址
// @return 状态码
BeStatusCode BeGetBeCameraStream(BeCameraHandle cameraHandle, BeCameraStream** streamAddress) {
	// 判断句柄
	if (cameraHandle == nullptr)
	{
		return BeStatusCode::ERR_CAMERA_HANDLE_EMPTY;
	}

	// 判断流储存地址
	if (streamAddress == nullptr) {
		return BeStatusCode::ERR_PARAM;
	}

	// 转换句柄类型
	BeCameraHandleImp* handle = (BeCameraHandleImp*)cameraHandle;

	// 初始化流接收空间
	AVPacket* pkt = av_packet_alloc();
	if (pkt == nullptr) {
		return BeStatusCode::ERR_CAMERA_STREAM_ALLOC;
	}

	// 读取一帧流
	int ret = av_read_frame(handle->avFmtCtx, pkt);
	if (ret != 0) {
		return BeStatusCode::ERR_GET_CAMERA_STREAM;
	}

	// 拷贝流
	BeCameraStream* cameraStream = new BeCameraStream{ 0 };
	cameraStream->data = new uint8_t[pkt->size];
	cameraStream->size = pkt->size;
	/*for (size_t i = 0; i < pkt->size; i++)
	{
		cameraStream->data[i] = *(pkt->data + i * sizeof(uint8_t));
	}*/
	memcpy(cameraStream->data, pkt->data, pkt->size * sizeof(uint8_t));

	// 赋值到响应
	*streamAddress = cameraStream;

	// 释放接收空间
	av_packet_free(&pkt);

	// 取流成功
	return BeStatusCode::SUCCESS;
}

// BeFreeBeCameraStream 释放相机流
// @param cameraHandle 相机句柄
// @param streamAddress 相机流储存地址
// @return 状态码
BeStatusCode BeFreeBeCameraStream(BeCameraHandle cameraHandle, BeCameraStream** streamAddress) {
	// 判断句柄
	if (cameraHandle == nullptr)
	{
		return BeStatusCode::ERR_CAMERA_HANDLE_EMPTY;
	}

	// 判断流储存地址
	if (streamAddress == nullptr) {
		return BeStatusCode::ERR_PARAM;
	}

	// 提取储存的流结构指针
	BeCameraStream* cameraStream = *streamAddress;

	// 判断是否需要释放
	if (cameraStream == nullptr) {
		return BeStatusCode::ERR_PARAM;
	}

	// 释放字节流
	delete[]cameraStream->data;
	// 置空指针
	*streamAddress = nullptr;

	// 释放成功
	return BeStatusCode::SUCCESS;
}
