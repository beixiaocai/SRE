/*
 * Copyright (c) 2023 The SRE project authors. All Rights Reserved.
 *
 * This file is part of SRE(https://github.com/any12345com/SRE).
 * Windows平台捕获麦克风或声卡
 */

#ifndef WINDOWSCAPTUREAUDIO_H
#define WINDOWSCAPTUREAUDIO_H
#include <qglobal.h>

#ifdef Q_OS_WIN
class WindowsCaptureAudio
{
public:
    WindowsCaptureAudio();
};
#endif
#endif // WINDOWSCAPTUREAUDIO_H
