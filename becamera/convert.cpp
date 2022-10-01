// // convert.cpp : 定义图像转换

// #include <stdint.h>
// extern "C"
// {
// #include "libswscale/swscale.h"
// #include <libavutil/opt.h>
// #include "libavutil/pixfmt.h"
// };

// // rgb3ToMjpg RGB3格式转换为MJPG格式
// uint8_t *rgb3ToMjpg(uint8_t *rgb3Bytes)
// {
//     // 为SwsContext结构体分配内存
//     SwsContext *swsCtx = sws_alloc_context();
//     av_opt_set_int(swsCtx, "sws_flags", SWS_BICUBIC | SWS_PRINT_INFO, 0);
//     av_opt_set_int(swsCtx, "srcw", 1920, 0);
//     av_opt_set_int(swsCtx, "srch", 1080, 0);
//     av_opt_set_int(swsCtx, "src_format", AVPixelFormat::AV_PIX_FMT_RGB4, 0);
//     // 0: 代表输出的YUV的取值范围遵循“mpeg”标准
//     // 1: 代表输出的YUV的取值范围遵循“jpeg”标准
//     av_opt_set_int(swsCtx, "src_range", 1, 0);
//     av_opt_set_int(swsCtx, "dstw", 1920, 0);
//     av_opt_set_int(swsCtx, "dsth", 1080, 0);
//     av_opt_set_int(swsCtx, "dst_format", AVPixelFormat::AV_PIX_FMT_YUVA420P, 0);
//     // 0: 代表输出的YUV的取值范围遵循“mpeg”标准
//     // 1: 代表输出的YUV的取值范围遵循“jpeg”标准
//     av_opt_set_int(swsCtx, "dst_range", 0, 0);
//     // 初始化结构体
//     sws_init_context(swsCtx, NULL, NULL);
//     return nullptr;
// }