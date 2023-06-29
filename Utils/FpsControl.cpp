#include "FpsControl.h"
#include <chrono>
#include <thread>
#include <iostream>

FpsControl::FpsControl(int fps):target_fps(fps){
    interval_duration = float(float(1000) / float(fps));
}
FpsControl::~FpsControl() {

}

void FpsControl::realTimeStart() {
    realtime_start = gettime();
}
void FpsControl::realTimeIncrease() {
	++realtime_totalCount;
}

void FpsControl::intervalStart() {
	interval_start = gettime();
}

void FpsControl::adjust() {

    //单帧间隔计算及其休眠计算
    interval_end = gettime();
    interval_extra = interval_duration - (interval_end - interval_start);

    if (interval_extra > 0) {
        if (total_interval_extra > 0) {
            if (total_interval_extra >= interval_extra) {
                total_interval_extra -= interval_extra;
            }
            else {
                interval_extra -= total_interval_extra;
                total_interval_extra = 0;
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_extra / 2));
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_extra / 2));
        }
    }
    else if (interval_extra < 0) {
        total_interval_extra += (-interval_extra);
    }

    //实时计算帧率
    realtime_duration = gettime() - realtime_start;

    if (realtime_duration > 0) {
        realtime_fps = float(realtime_totalCount * 1000) / realtime_duration;
        if (realtime_fps > target_fps) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_extra / 2));
        }
    }

}
float FpsControl::getRealTimeFps(){
    return realtime_fps;
}
int64_t FpsControl::gettime()// 获取毫秒级时间戳（13位）
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).
		count();

}

