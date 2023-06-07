/*
 * Copyright (c) 2023 The SRE project authors. All Rights Reserved.
 *
 * This file is part of SRE(https://github.com/any12345com/SRE).
 * Windows平台通过DXGI的方式捕获屏幕
 * Qt跨平台宏 https://zhuanlan.zhihu.com/p/619580373
 */

#ifndef WINDOWSDXGIDUPLICATION_H
#define WINDOWSDXGIDUPLICATION_H
#include <qglobal.h>
#ifdef Q_OS_WIN
#include <d3d11.h>
#include <dxgi1_2.h>
#include <sal.h>
#include <new>
#include <DirectXMath.h>
#include <memory>   //std::unique_ptr
#include <algorithm>//std::min
#include <windows.h>

//#pragma comment(lib,"d3d11.lib")
class WindowsDXGIDuplication{

public:
    WindowsDXGIDuplication();
    ~WindowsDXGIDuplication();
public:
    bool InitDevice();
    bool InitDuplication(UINT Output);

    bool getFrame(int timeout=100); //millisecond
    bool copyFrameToBuffer(uint8_t* buffer,int bufferSize,int& width,int& height);
	bool DoneWithFrame();

private:
    ID3D11Device* m_Device;
	IDXGIOutputDuplication* m_DeskDupl;
	ID3D11Texture2D* m_AcquiredDesktopImage;
	UINT m_OutputNumber;
	DXGI_OUTPUT_DESC m_OutputDesc;

};
#endif
#endif // WINDOWSDXGIDUPLICATION_H
