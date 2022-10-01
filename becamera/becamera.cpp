// becamera.cpp : 定义静态库的函数

#include <iostream>
#include <string>
#include <map>
#include "becamera/becamera.h"
extern "C"
{
#include "libavutil/ffversion.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
	// #include "libavfilter/avfilter.h"
	// #include "libavformat/avformat.h"
	// #include "libavutil/avutil.h"
	// #include "libavutil/avstring.h"
	// #include "libswresample/swresample.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
};
#define FMT_HEADER_ONLY
#include "fmt/core.h"

// 定义个平台的视频输入设备类型
#ifdef LINUX
#define BECAMERA_INPUT_FORMAT "v4l2"
#elif defined WINDOWS
#define BECAMERA_INPUT_FORMAT "dshow"
#else
#define BECAMERA_INPUT_FORMAT "v4l2"
#endif

// BECAMERA_VERSION BeCamera的版本信息
#define BECAMERA_VERSION "1.0.0-alpha1"

// 全局指定版本信息
const BeCameraVersion BECAMERA_GLOBAL_VERSION = BeCameraVersion{
	becamera : (char *)BECAMERA_VERSION,
	ffmpeg : (char *)FFMPEG_VERSION
};

// ImageFormat 图像格式枚举
enum ImageFormat
{
	MJPEG,
	RGB3,
};

// 图像格式MAP
const std::map<ImageFormat, std::string> IMG_FMT_MAP = {
	{ImageFormat::MJPEG, "mjpeg"},
	{ImageFormat::RGB3, "rgb3"},
};

// BeCameraHandleImp 相机句柄的真实实现结构
struct BeCameraHandleImp
{
	const std::string inputFormat = BECAMERA_INPUT_FORMAT; // 输入设备类型
	AVFormatContext *avFmtCtx = nullptr;				   // FFMPEG的AV格式上下文
	const AVInputFormat *avInFmt = nullptr;				   // FFMPEG的AV输入句柄
	AVCodecContext *inCodecCtx = nullptr;				   // 输入设备的解码器上下文
	AVDictionary *openCameraOpts = nullptr;				   // 相机打开时的参数
	BeCameraDeviceList *devices = nullptr;				   // 相机列表
	int currDeviceID = -1;								   // 当前使用的设备ID(id <= -1时表示设备未打开)
	ImageFormat currFormat = ImageFormat::MJPEG;		   // 当前使用的输出格式
	uint32_t currWidth = 0;								   // 当前使用的分辨率宽度
	uint32_t currHeight = 0;							   // 当前使用的分辨率高度
};

// 释放上一次缓存的相机列表
void globalCameraDevicesFree(BeCameraHandleImp *handle)
{
	// 检查句柄
	if (handle == nullptr)
	{
		return;
	}

	// 检查对象
	if (handle->devices == nullptr)
	{
		return;
	}

	// 不为空时执行释放
	if (handle->devices->list != nullptr)
	{
		// 释放每个元素中的指针
		BeCameraDevice *list = handle->devices->list;
		for (size_t i = 0; i < handle->devices->listSize; i++)
		{
			if (list[i].name != nullptr)
			{
				delete[] list[i].name;
				list[i].name = nullptr;
			}
			if (handle->devices->list[i].description != nullptr)
			{
				delete[] list[i].description;
				list[i].description = nullptr;
			}
		}

		// 长度清零
		handle->devices->listSize = 0;
		// 释放整个数组
		delete[] handle->devices->list;
		handle->devices->list = nullptr;
		// 释放整个对象
		delete handle->devices;
		handle->devices = nullptr;
	}
}

// 开辟新的相机缓存列表
void globalCameraDevicesMalloc(BeCameraHandleImp *handle, int size)
{
	// 释放上一次的缓存列表
	globalCameraDevicesFree(handle);
	// 创建新的相机列表对象
	handle->devices = new BeCameraDeviceList;
	// 创建新的缓存列表
	handle->devices->list = new BeCameraDevice[size];
}

// BeCameraGetVersion 获取BeCamera相关版本信息
// @return BeCamera相关版本信息
BeCameraVersion *BeCameraGetVersion()
{
	return (BeCameraVersion *)&BECAMERA_GLOBAL_VERSION;
}

// BeCameraInit 初始化BeCamera句柄
// @param[out] becameraHandleAddress BeCamera句柄储存地址
// @return 状态码
BeCameraStatusCode BeCameraInit(BeCameraHandle *becameraHandleAddress)
{
	// 入参是否正常
	if (becameraHandleAddress == nullptr)
	{
		return BeCameraStatusCode::ERR_PARAM;
	}

	// 初始化一个句柄，所有属性初始化为0值
	BeCameraHandleImp *handle = new BeCameraHandleImp;

	// 注册所有设备
	avdevice_register_all();

	// 初始化一个FFMPEG的AV格式上下文
	handle->avFmtCtx = avformat_alloc_context();
	if (handle->avFmtCtx == nullptr)
	{
		// 释放对象
		delete handle;
		// 返回错误
		return BeCameraStatusCode::ERR_CREATE_AV_FORMAT_CONTEXT;
	}

	// 查找所有视频输入设备
	handle->avInFmt = av_find_input_format(handle->inputFormat.c_str());
	if (handle->avInFmt == nullptr)
	{
		// 释放上下文
		avformat_free_context(handle->avFmtCtx);
		// 释放对象
		delete handle;
		// 返回错误
		return BeCameraStatusCode::ERR_FIND_VIDEO_INPUT_DEVICE;
	}

	// 赋值初始化完成的句柄
	*becameraHandleAddress = (BeCameraHandle)handle;

	// 返回成功
	return BeCameraStatusCode::SUCCESS;
}

// BeCameraUnInit 释放BeCamera句柄
// @param becameraHandleAddress BeCamera句柄储存地址
// @return 状态码
BeCameraStatusCode BeCameraUnInit(BeCameraHandle *becameraHandleAddress)
{
	// 入参是否正常
	if (becameraHandleAddress == nullptr)
	{
		return BeCameraStatusCode::ERR_PARAM;
	}
	if (*becameraHandleAddress == nullptr)
	{
		return BeCameraStatusCode::ERR_BECAMERA_HANDLE_EMPTY;
	}

	// 转换类型
	BeCameraHandleImp *handle = (BeCameraHandleImp *)(*becameraHandleAddress);

	// 释放上下文
	if (handle->avFmtCtx != nullptr)
	{
		// 关闭已打开的视频设备
		avformat_close_input(&handle->avFmtCtx);
		handle->currDeviceID = -1;
		// 释放FFMPEG的AV格式上下文
		avformat_free_context(handle->avFmtCtx);
		handle->avFmtCtx = nullptr;
	}

	// 判断是否需要关闭解码器
	if (handle->inCodecCtx != nullptr)
	{
		// 关闭解码器
		avcodec_close(handle->inCodecCtx);
		// 释放上下文
		avcodec_free_context(&handle->inCodecCtx);
		handle->inCodecCtx = nullptr;
	}

	// 释放打开相机时的参数
	if (handle->openCameraOpts != nullptr)
	{
		av_dict_free(&handle->openCameraOpts);
		handle->openCameraOpts = nullptr;
	}

	// 释放句柄
	delete handle;
	// 句柄储存地址值赋空
	*becameraHandleAddress = nullptr;

	// 释放完成
	return BeCameraStatusCode::SUCCESS;
}

// BeCameraGetDeviceList 获取相机列表
// @param becameraHandle BeCamera句柄
// @param[out] deviceList 相机列表储存地址
// @return 状态码
BeCameraStatusCode BeCameraGetDeviceList(BeCameraHandle becameraHandle, BeCameraDeviceList **devices)
{
	// 判断句柄
	if (becameraHandle == nullptr)
	{
		return BeCameraStatusCode::ERR_BECAMERA_HANDLE_EMPTY;
	}

	// 判断相机列表出参地址
	if (devices == nullptr)
	{
		return BeCameraStatusCode::ERR_PARAM;
	}

	// 转换句柄类型
	BeCameraHandleImp *handle = (BeCameraHandleImp *)becameraHandle;

	// 获取所有视频输入设备列表
	AVDeviceInfoList *deviceList;
	int ret = avdevice_list_input_sources(handle->avInFmt, nullptr, nullptr, &deviceList);
	if (ret < 0)
	{
		return BeCameraStatusCode::ERR_FIND_VIDEO_INPUT_DEVICE;
	}

	// 校验设备数量
	if (deviceList->nb_devices == 0)
	{
		return BeCameraStatusCode::ERR_NOT_FOUND_VIDEO_INPUT_DEVICE;
	}

	// 创建新的缓存
	globalCameraDevicesMalloc(handle, deviceList->nb_devices);

	// 遍历赋值设备列表
	BeCameraDeviceList *ds = handle->devices;
	for (size_t i = 0; i < deviceList->nb_devices; i++)
	{
		// 提取当前设备
		AVDeviceInfo *item = deviceList->devices[i];
		ds->list[i] = BeCameraDevice{
			id : (uint8_t)(i + 1),
			name : new char[strlen(item->device_name)],
			description : new char[strlen(item->device_description)]
		};
		strcpy(ds->list[i].name, item->device_name);
		strcpy(ds->list[i].description, item->device_description);
	}
	// 数量
	ds->listSize = deviceList->nb_devices;

	// 赋值到返回值参数上
	*devices = ds;

	// 清除查找源
	avdevice_free_list_devices(&deviceList);

	// 查询成功
	return BeCameraStatusCode::SUCCESS;
}

// BeCameraOpenDevice 打开相机
// @param becameraHandle BeCamera句柄
// @param cameraID 相机ID
// @return 状态码
BeCameraStatusCode BeCameraOpenDevice(BeCameraHandle becameraHandle, uint8_t cameraID)
{
	// 预定义状态码
	BeCameraStatusCode status = BeCameraStatusCode::SUCCESS;
	// 预定义内部参数
	BeCameraHandleImp *handle = nullptr;			// 句柄
	std::string deviceName = "";					// 预定义设备名称
	int ret = -1;									// FFmpeg调用结果
	int inVideoStreamIndex = -1;					// 设备视频流下标
	AVCodecParameters *inVideoCodecParam = nullptr; // 设备支持的解码器参数
	const AVCodec *inCodec = nullptr;				// 输入设备解码器

	// 判断句柄
	if (becameraHandle == nullptr)
	{
		status = BeCameraStatusCode::ERR_BECAMERA_HANDLE_EMPTY;
		goto ERROR;
	}

	// 转换句柄类型
	handle = (BeCameraHandleImp *)becameraHandle;

	// 查找ID对应的设备
	if (handle->devices == nullptr)
	{
		status = BeCameraStatusCode::ERR_NOT_FOUND_VIDEO_INPUT_DEVICE;
		goto ERROR;
	}

	for (size_t i = 0; i < handle->devices->listSize; i++)
	{
		if (handle->devices->list[i].id == cameraID)
		{
			deviceName = handle->devices->list[i].name;
			break;
		}
	}
	if (deviceName.empty())
	{
		status = BeCameraStatusCode::ERR_NOT_FOUND_VIDEO_INPUT_DEVICE;
		goto ERROR;
	}

	// 判断设备是否已打开
	if (handle->currDeviceID >= 0)
	{
		// 将使用中的设备ID移除
		handle->currDeviceID = -1;
		// 关闭已打开的视频设备
		avformat_close_input(&handle->avFmtCtx);
	}
	// 判断是否需要关闭解码器
	if (handle->inCodecCtx != nullptr)
	{
		// 关闭解码器
		avcodec_close(handle->inCodecCtx);
		// 释放上下文
		avcodec_free_context(&handle->inCodecCtx);
		handle->inCodecCtx = nullptr;
	}

	// fflags: 设置读取或者写出的格式的形式的标签
	//
	// 分为多种方式：
	// -flush_packets，ignidx，genpts，nofillin，noparse，igndts，discardcorrupt，
	// sortdts，keepside，fastseek，latm，nobuffer，bitexact
	//
	// 下面针对这些format读写方式进行一一详解；
	// flush_packets： 立即将packets数据刷新入文件中；
	// genpts: 输出是产生pts;
	// nofillin: 不填写可以精确计算缺失的值;
	// igndts: 忽略dts；
	// discardcorrupt： 丢弃损坏的帧；
	// sortdts： 尝试以dts的顺序为准输出；
	// fastseek： 快速seek，但是不够精确；
	// latm： 设置RTP MP4_LATM 生效；
	// nobuffer： 直接读取或者写出，不存buffer；
	// bitexact： 不写入随机或者不稳定的数据；
	av_dict_set(&handle->openCameraOpts, "fflags", "nobuffer", 0);
	// 强制低延时
	av_dict_set(&handle->openCameraOpts, "flags", "low_delay", 0);
	// 最大延时设置为0
	av_dict_set(&handle->openCameraOpts, "max_delay", "0", 0);
	// 允许丢帧
	av_dict_set_int(&handle->openCameraOpts, "framedrop", 1, 0);

	// 遍历图像格式列表,只要有一个能打开就OK
	ret = -1;
	for (auto item : IMG_FMT_MAP)
	{
		// 取出键值
		ImageFormat key = item.first;
		std::string val = item.second;
		// 图像格式赋值到全局
		handle->currFormat = key;
		// 配置采集格式
		av_dict_set(&handle->openCameraOpts, "input_format", val.c_str(), 0);
		// 配置帧率
		av_dict_set(&handle->openCameraOpts, "framerate", "30", 0);
		// 分辨率赋值到全局
		handle->currWidth = 1920;
		handle->currHeight = 1080;
		std::string wh = fmt::format("{0}*{1}", handle->currWidth, handle->currHeight);
		// 配置分辨率
		av_dict_set(&handle->openCameraOpts, "video_size", wh.c_str(), 0);

		// 打开指定的视频输入设备
		ret = avformat_open_input(&handle->avFmtCtx, deviceName.c_str(), handle->avInFmt, &handle->openCameraOpts);
		if (ret == 0)
		{
			// 打开成功,跳出
			break;
		}
	}
	if (ret != 0)
	{
		// 全部打开失败
		status = BeCameraStatusCode::ERR_OPEN_VIDEO_INPUT_DEVICE;
		goto ERROR;
	}

	// 赋值当前使用的设备ID
	handle->currDeviceID = (int)cameraID;

	// 查询设备流信息
	ret = avformat_find_stream_info(handle->avFmtCtx, NULL);
	if (ret < 0)
	{
		status = BeCameraStatusCode::ERR_FIND_DEVICE_STREAM_INFO;
		goto ERROR;
	}

	// 判断设备是否支持视频流
	inVideoStreamIndex = -1;
	for (size_t i = 0; i < handle->avFmtCtx->nb_streams; i++)
	{
		AVCodecParameters *codecpar = handle->avFmtCtx->streams[i]->codecpar;
		if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			inVideoStreamIndex = (int)i;
			break;
		}
	}
	if (inVideoStreamIndex == -1)
	{
		status = BeCameraStatusCode::ERR_NOT_SUPPORT_VIDEO_STREAM;
		goto ERROR;
	}

	// 查询设备支持的解码器
	inVideoCodecParam = handle->avFmtCtx->streams[inVideoStreamIndex]->codecpar;
	inCodec = avcodec_find_decoder(inVideoCodecParam->codec_id);
	if (inCodec == nullptr)
	{
		status = BeCameraStatusCode::ERR_NOT_FOUND_VIDEO_CODEC;
		goto ERROR;
	}
	// 初始化解码器空间
	handle->inCodecCtx = avcodec_alloc_context3(inCodec);
	if (handle->inCodecCtx == nullptr)
	{
		status = BeCameraStatusCode::ERR_NOT_FOUND_VIDEO_CODEC;
		goto ERROR;
	}
	// 初始化解码器参数
	if (avcodec_parameters_to_context(handle->inCodecCtx, inVideoCodecParam) < 0)
	{
		status = BeCameraStatusCode::ERR_NOT_FOUND_VIDEO_CODEC;
		goto ERROR;
	}
	// 打开解码器
	if (avcodec_open2(handle->inCodecCtx, inCodec, NULL) < 0)
	{
		status = BeCameraStatusCode::ERR_NOT_FOUND_VIDEO_CODEC;
		goto ERROR;
	}

	// 视频流设备打开成功
	return BeCameraStatusCode::SUCCESS;

ERROR:
	// 判断设备是否已打开
	if (handle->currDeviceID >= 0)
	{
		// 将使用中的设备ID移除
		handle->currDeviceID = -1;
		// 关闭已打开的视频设备
		avformat_close_input(&handle->avFmtCtx);
	}
	// 判断是否需要关闭解码器
	if (handle->inCodecCtx != nullptr)
	{
		// 关闭解码器
		avcodec_close(handle->inCodecCtx);
		// 释放上下文
		avcodec_free_context(&handle->inCodecCtx);
		handle->inCodecCtx = nullptr;
	}
	// 返回错误码
	return status;
}

// BeCameraGetStream 获取相机流
// @param becameraHandle 相机句柄
// @param[out] streamAddress 相机流储存地址
// @return 状态码
BeCameraStatusCode BeCameraGetStream(BeCameraHandle becameraHandle, BeCameraStream **streamAddress)
{
	// 判断句柄
	if (becameraHandle == nullptr)
	{
		return BeCameraStatusCode::ERR_BECAMERA_HANDLE_EMPTY;
	}

	// 判断流储存地址
	if (streamAddress == nullptr)
	{
		return BeCameraStatusCode::ERR_PARAM;
	}

	// 转换句柄类型
	BeCameraHandleImp *handle = (BeCameraHandleImp *)becameraHandle;

	// 判断设备是否已打开
	if (handle->currDeviceID < 0)
	{
		return BeCameraStatusCode::ERR_NOT_OPEN_VIDEO_INPUT_DEVICE;
	}

	// 初始化视频流接收空间
	AVPacket *pkt = av_packet_alloc();
	if (pkt == nullptr)
	{
		return BeCameraStatusCode::ERR_VIDEO_STREAM_ALLOC;
	}

	// 读取一帧流
	int ret = av_read_frame(handle->avFmtCtx, pkt);
	if (ret != 0)
	{
		return BeCameraStatusCode::ERR_GET_CAMERA_STREAM;
	}

	// 拷贝流
	BeCameraStream *cameraStream = new BeCameraStream{
		data : new uint8_t[pkt->size],
		dataSize : (size_t)pkt->size
	};
	memcpy(cameraStream->data, pkt->data, (size_t)(pkt->size * sizeof(uint8_t)));
	// 释放接收空间
	av_packet_free(&pkt);

	// 当前不是MJPG流时需要转换
	switch (handle->currFormat)
	{
	case ImageFormat::RGB3: // 将RGB3转换为MJPG
		// 赋值到响应
		*streamAddress = cameraStream;
		break;

	default:
		// 赋值到响应
		*streamAddress = cameraStream;
		break;
	}

	// 取流成功
	return BeCameraStatusCode::SUCCESS;
}

// BeCameraFreeStream 释放相机流
// @param streamAddress 相机流储存地址
// @return 状态码
BeCameraStatusCode BeCameraFreeStream(BeCameraStream **streamAddress)
{
	// 判断流储存地址
	if (streamAddress == nullptr)
	{
		return BeCameraStatusCode::ERR_PARAM;
	}

	// 提取储存的流结构指针
	BeCameraStream *cameraStream = *streamAddress;

	// 判断是否需要释放
	if (cameraStream == nullptr || cameraStream->data == nullptr)
	{
		return BeCameraStatusCode::ERR_PARAM;
	}

	// 释放字节流
	cameraStream->dataSize = 0;
	delete[] cameraStream->data;
	// 释放对象
	delete cameraStream;
	// 置空指针
	*streamAddress = nullptr;

	// 释放成功
	return BeCameraStatusCode::SUCCESS;
}
