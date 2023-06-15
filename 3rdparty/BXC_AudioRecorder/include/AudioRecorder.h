#ifndef BXC_AUDIORECORDER_H
#define BXC_AUDIORECORDER_H
#include <string>
#include <vector>

#ifdef BXC_AUDIORECORDER_EXPORTS
#define __DECLSPEC_INC __declspec(dllexport)
#else
#define __DECLSPEC_INC __declspec(dllimport)
#endif // !BXC_AUDIORECORDER_EXPORTS

namespace BXC_AudioRecorderLibrary {

#ifdef __cplusplus
	extern "C" {
#endif
		enum BXC_RecordDeviceType{
			BXC_SOUNDCARD = 10, //声卡（扬声器）
			BXC_MICROPHONE    //麦克风
		};

		bool __DECLSPEC_INC BXC_AudioRecorder_New(BXC_RecordDeviceType deviceType);
		void __DECLSPEC_INC BXC_AudioRecorder_Destory();
		bool __DECLSPEC_INC BXC_AudioRecorder_reInit();

		bool __DECLSPEC_INC BXC_get_sample(unsigned char*& buffer, int& size);//获取采样数据
		int __DECLSPEC_INC BXC_get_nb_channels();   //获取采样声道数（常见值：2,1）
		int __DECLSPEC_INC BXC_get_nb_bits_sample();//获取采样时每一个采样点的存储比特数（常见值：16,8,24）
		int __DECLSPEC_INC BXC_get_sample_rate();   //获取采样率（常见值：44100,48000）
		int __DECLSPEC_INC BXC_get_nb_samples();    //获取一帧音频的采样点数量（常见值：1024,1056,1152）

#ifdef __cplusplus
	}
#endif
}
#endif //BXC_AUDIORECORDER_H
