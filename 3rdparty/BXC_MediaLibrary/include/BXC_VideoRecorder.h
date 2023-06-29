#ifndef BXC_VIDEORECORDER_H
#define BXC_VIDEORECORDER_H
#include <string>

#ifdef BXC_MEDIALIBRARY_EXPORTS
#define __DECLSPEC_INC __declspec(dllexport)
#else
#define __DECLSPEC_INC __declspec(dllimport)
#endif //!BXC_MEDIALIBRARY_EXPORTS

namespace BXC_MediaLibrary {

#ifdef __cplusplus
	extern "C" {
#endif
		struct BXC_VideoRecorder {
			int id;
			const char* capture;//视频采样设备,WindowsDXGI: DXGI, WindowsGDI: GDI,或者其他摄像头的具体名称
			int width;			//视频采样宽度
			int height;			//视频采样高度
			int idx;			//视频采样源为屏幕时，具体的屏幕序号

			const char* pixelFormat;//视频帧像素格式:BGRA,RGB
			int factWidth; //接口获取的视频采样设备宽度
			int factHeight;//接口获取的视频采样设备宽度
		};

		/**
		 * 打开一个视频采样实例
		 *
		 * @param recorder 视频采样实例
		 * @return >= 0 on success, a negative code on failure
		 */
		int __DECLSPEC_INC BXC_VideoRecorder_Open(BXC_VideoRecorder*& recorder);
		
		/**
		 * 关闭一个视频采样实例
		 *
		 * @param recorder 视频采样实例
		 * @return >= 0 on success, a negative code on failure
		 */
		int __DECLSPEC_INC BXC_VideoRecorder_Close(BXC_VideoRecorder* recorder);

		/**
		 * 采样一帧视频帧数据
		 *
		 * @param recorder  视频采样实例
		 * @param buffer    采样得到的一帧视频帧字节流
		 * @param size      采样得到的一帧视频帧字节流长度
		 * @param timestamp 采样时间戳
		 * @return >= 0 on success, a negative code on failure
		 */
		int __DECLSPEC_INC BXC_get_frame(BXC_VideoRecorder* recorder,unsigned char*& buffer, int& size, int64_t& timestamp);

#ifdef __cplusplus
	}
#endif
}
#endif //BXC_VIDEORECORDER_H
