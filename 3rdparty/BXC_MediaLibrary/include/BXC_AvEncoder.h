#ifndef BXC_AVENCODER_H
#define BXC_AVENCODER_H
#ifdef BXC_MEDIALIBRARY_EXPORTS
#define __DECLSPEC_INC __declspec(dllexport)
#else
#define __DECLSPEC_INC __declspec(dllimport)
#endif //!BXC_MEDIALIBRARY_EXPORTS
#include "BXC_AvFrame.h"

namespace BXC_MediaLibrary {

#ifdef __cplusplus
	extern "C" {
#endif

		/**
		 * 打开一个音视频编码器实例
		 *
		 * @param encoder 音视频编码器实例
		 * @param url 存储地址
		 * @return >= 0 on success, a negative code on failure
		 */
		int __DECLSPEC_INC BXC_AvEncoder_Open(BXC_AvEncoder* encoder, const char* url);

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
		* 添加一帧待编码帧
		*
		* @param encoder 音视频编码器实例
		* @param frame 待编码帧
		* @return >= 0 on success, a negative code on failure
		*/
		int __DECLSPEC_INC BXC_send_frame(BXC_AvEncoder* encoder, BXC_AvFrame* frame);
		
		/**
		* 获取一帧编码帧
		*
		* @param encoder 音视频编码器实例
		* @param packet 获取的编码帧（获取的编码帧类型，取决于参数中的type）
		* @return >= 0 on success, a negative code on failure
		*/
		int __DECLSPEC_INC BXC_receive_packet(BXC_AvEncoder* encoder, BXC_AvPacket* packet);


#ifdef __cplusplus
	}
#endif
}
#endif //BXC_AVENCODER_H
