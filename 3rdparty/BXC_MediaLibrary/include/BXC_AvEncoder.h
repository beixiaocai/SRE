#ifndef BXC_AVENCODER_H
#define BXC_AVENCODER_H
#ifdef BXC_MEDIALIBRARY_EXPORTS
#define __DECLSPEC_INC __declspec(dllexport)
#else
#define __DECLSPEC_INC __declspec(dllimport)
#endif //!BXC_MEDIALIBRARY_EXPORTS
#include <string>
namespace BXC_MediaLibrary {

#ifdef __cplusplus
	extern "C" {
#endif
		struct BXC_AvEncoder {
			int id;

			bool hasVideo = false;//是否编码视频流
			const char* videoCodecName = "";//视频编码名称
			const char* pixelFormat = "";//视频帧像素格式 BGRA,RGB
			int width = 0;//视频宽
			int height = 0;//视频高
			int fps = 0;// 视频编码帧率
			int videoBitrate = 0;//视频编码码率

			bool hasAudio = false;//是否编码音频流
			const char* audioCodecName = "";//视频编码名称
			int audioBitrate = 0;//音频编码码率

		};


		/**
		 * 打开一个音视频编码器实例
		 *
		 * @param encoder 音视频编码器实例
		 * @param url 存储地址
		 * @return >= 0 on success, a negative code on failure
		 */
		int __DECLSPEC_INC BXC_AvEncoder_Open(BXC_AvEncoder*& encoder, const char* url);

		/**
		 * 关闭一个音视频编码器实例
		 *
		 * @param encoder 音视频编码器实例
		 * @return >= 0 on success, a negative code on failure
		 */
		int __DECLSPEC_INC BXC_AvEncoder_Close(BXC_AvEncoder* encoder);

		/**
		* 获取当前时间戳（microseconds.）
		*
		* @return 当前时间戳
		*/
		int64_t __DECLSPEC_INC BXC_gettime();

		/**
		* 添加一帧待编码视频帧
		*
		* @param encoder 音视频编码器实例
		* @param size 视频帧字节流长度
		* @param data 视频帧字节流
		* @param timestamp 视频帧捕获时间戳（microseconds）
		* @param width 视频帧捕获宽度
		* @param height 视频帧捕获高度
		* @param count 视频帧捕获序号
		* @return >= 0 on success, a negative code on failure
		*/
		int __DECLSPEC_INC BXC_send_video_frame(BXC_AvEncoder* encoder, int size, uint8_t* data,int64_t timestamp, int width, int height, int64_t count);
		
		/**
		* 添加一帧待编码音频帧
		*
		* @param encoder 音视频编码实例，由BXC_AvEncoder_New创建
		* @param size 音频帧字节流长度
		* @param data 音频帧字节流
		* @param timestamp 音频帧捕获时间戳（microseconds）
		* @param count 音频帧捕获序号
		* @return >= 0 on success, a negative code on failure
		*/
		int __DECLSPEC_INC BXC_send_audio_frame(BXC_AvEncoder* encoder, int size, uint8_t* data, int64_t timestamp, int64_t count);

#ifdef __cplusplus
	}
#endif
}
#endif //BXC_AVENCODER_H
