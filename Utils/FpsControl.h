#ifndef FPSCONTROL_H
#define FPSCONTROL_H
#include <string>

class FpsControl
{
public:
    FpsControl() = delete;
    FpsControl(int fps);
    ~FpsControl();
public:
    void realTimeStart();
    void realTimeIncrease();
    void intervalStart();
    void adjust();
    float getRealTimeFps();

private:
    int64_t gettime();
private:
    int target_fps;//目标 fps

    int64_t interval_start = 0;
    int64_t interval_end = 0;
    float   interval_duration = 0;//单帧间隔时长（单位毫秒）
    int     interval_extra = 0;//单帧额外需要休眠的时间（单位毫秒）
    int     total_interval_extra = 0;//积累滞后的补偿休眠时长（单位毫秒）

    int64_t realtime_start = 0;//计算实时帧率的开始时间戳
    int64_t realtime_totalCount = 0;//开始计算实时帧率的总数量
    int64_t realtime_duration = 0;//开始计算实时帧率至今总耗时
    float   realtime_fps = 0.0;//实时帧率


};

#endif //FPSCONTROL_H
