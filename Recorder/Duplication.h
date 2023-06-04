#pragma once

#include <windows.h>
#include <d3d11.h>
//#pragma comment(lib,"d3d11.lib")
#include <dxgi1_2.h>
#include <sal.h>
#include <new>
#include <DirectXMath.h>
#include <iostream>
#include <memory>   //std::unique_ptr
#include <algorithm>//std::min

class Duplication
{
public:
	Duplication();
	~Duplication();
	bool GetFrame(int timeout=100); //millisecond
    bool copyFrameDataToBuffer(uint8_t* buffer,int bufferSize,int& width,int& height);
	bool DoneWithFrame();
	bool InitDevice();
	bool InitDupl(UINT Output);
	//void saveFrameToBmp(PCWSTR FileName);
	//static void saveTextureToBmp(PCWSTR FileName, ID3D11Texture2D* Texture);

private:
	// vars
	IDXGIOutputDuplication* m_DeskDupl;
	ID3D11Texture2D* m_AcquiredDesktopImage;
	UINT m_OutputNumber;
	DXGI_OUTPUT_DESC m_OutputDesc;
	ID3D11Device* m_Device;
};
