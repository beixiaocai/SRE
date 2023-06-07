/*
 * Copyright (c) 2023 The SRE project authors. All Rights Reserved.
 *
 * This file is part of SRE(https://github.com/any12345com/SRE).
 * 基于FFmpeg实现的跨平台捕获设备（摄像头，麦克风）
 *
 * FFmpeg查看支持捕获设备的命令行：ffmpeg -hide_banner -f dshow -list_devices true -i dummy
 */

#ifndef FFMPEGCAPTURE_H
#define FFMPEGCAPTURE_H

#include <QObject>
extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/buffersink.h>
    #include <libavfilter/buffersrc.h>
    #include <libavutil/opt.h>
    #include <libavutil/pixdesc.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
}

class FFmpegCapture: public QObject
{
    Q_OBJECT
public:
    FFmpegCapture(QObject *parent);
    ~FFmpegCapture();
public:
    int init();

};

#endif // FFMPEGCAPTURE_H
