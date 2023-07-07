#ifndef BXC_AUDIORECORDER_H
#define BXC_AUDIORECORDER_H
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
		struct BXC_AudioRecorder {
		public:
			BXC_AudioRecorder() = delete;
			BXC_AudioRecorder(const char* capture) {
				this->capture = capture;
			}
			~BXC_AudioRecorder() {
			}
		public:
			int id = -1;
			const char* capture;//音频采样设备,默认声卡:SOUNDCARD, 默认麦克风:MICROPHONE

			int nb_channels;//音频采样声道数（常见值：2,1）
			int nb_bits_sample;//一个音频采样点的比特数（常见值：16,8,24）
			int sample_rate;//音频采样率（常见值：44100,48000）
			int nb_samples;//一帧音频的采样点数量（常见值：1024,1056,1152）

		};

		/**
		 * 打开一个音频采样实例
		 *
		 * @param recorder 音频采样实例
		 * @return >= 0 on success, a negative code on failure
		 */
		int __DECLSPEC_INC BXC_AudioRecorder_Open(BXC_AudioRecorder * recorder);

		/**
		 * 关闭一个音频采样实例
		 *
		 * @param recorder 音频采样实例
		 * @return >= 0 on success, a negative code on failure
		 */
		int __DECLSPEC_INC BXC_AudioRecorder_Close(BXC_AudioRecorder* recorder);

		/**
		 * 获取音频采样数据
		 *
		 * @param recorder  音频采样实例
		 * @param buffer    采样得到的音频字节流
		 * @param size      采样得到的音频字节流长度
		 * @param timestamp 采样时间戳
		 * @return >= 0 on success, a negative code on failure
		 */
		int __DECLSPEC_INC BXC_get_sample(BXC_AudioRecorder* recorder,unsigned char*& buffer, int& size, int64_t& timestamp);//获取采样数据
		
		/**
		* 获取音频采样时使用的时间戳
		*
		* @return 时间戳（单位与BXC_get_sample获得的音频时间戳单位一致）
		*/
		int64_t __DECLSPEC_INC BXC_audio_timestamp();

#ifdef __cplusplus
	}
#endif
}
#endif //BXC_AUDIORECORDER_H
