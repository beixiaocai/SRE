#include"Duplication.h"

Duplication::Duplication():m_DeskDupl(nullptr),
m_AcquiredDesktopImage(nullptr),
m_OutputNumber(0),
m_Device(nullptr)
{
}

Duplication::~Duplication()
{
    if (m_DeskDupl)
    {
        m_DeskDupl->Release();
        m_DeskDupl = nullptr;
    }

    if (m_AcquiredDesktopImage)
    {
        m_AcquiredDesktopImage->Release();
        m_AcquiredDesktopImage = nullptr;
    }

    if (m_Device)
    {
        m_Device->Release();
        m_Device = nullptr;
    }
}

bool Duplication::GetFrame(int timeout)
{
    IDXGIResource* DesktopResource = nullptr;
    DXGI_OUTDUPL_FRAME_INFO FrameInfo;

    // Get new frame
    HRESULT hr = m_DeskDupl->AcquireNextFrame(timeout, &FrameInfo, &DesktopResource);
    if (hr == DXGI_ERROR_WAIT_TIMEOUT)
    {
        return false;
    }

    if (FAILED(hr))
    {
        return false;
    }

    // If still holding old frame, destroy it
    if (m_AcquiredDesktopImage)
    {
        m_AcquiredDesktopImage->Release();
        m_AcquiredDesktopImage = nullptr;
    }

    // QI for IDXGIResource
    hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&m_AcquiredDesktopImage));
    DesktopResource->Release();
    DesktopResource = nullptr;
    if (FAILED(hr))
    {
        return true;
    }
    return 1;
}

bool Duplication::copyFrameDataToBuffer(uint8_t* buffer,int bufferSize,int& width,int& height)
{
    HRESULT hr;
    ID3D11DeviceContext* context;
    m_Device->GetImmediateContext(&context);

    D3D11_TEXTURE2D_DESC desc;
    m_AcquiredDesktopImage->GetDesc(&desc);

    ID3D11Texture2D* destImage = nullptr;
    D3D11_TEXTURE2D_DESC desc2;
    desc2.Width = desc.Width;
    desc2.Height = desc.Height;
    desc2.MipLevels = desc.MipLevels;
    desc2.ArraySize = desc.ArraySize;
    desc2.Format = desc.Format;
    desc2.SampleDesc = desc.SampleDesc;
    desc2.Usage = D3D11_USAGE_STAGING;
    desc2.BindFlags = 0;
    desc2.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc2.MiscFlags = 0;
    //不能直接使用m_AquiredDesktopImage的desc描述信息,需要拷贝一个
    hr = m_Device->CreateTexture2D(&desc2, nullptr, &destImage);
    if (FAILED(hr)) {
        return false;
    }

    context->CopyResource(destImage, m_AcquiredDesktopImage);//源Texture2D和目的Texture2D需要有相同的多重采样计数和质量时（就是一些属性相同）
    //所以目的纹理对象应该初始化设置，使用m_AcquiredDesktopImage的描述信息来创建纹理（代码是前几行）

    D3D11_MAPPED_SUBRESOURCE mapRes;
    UINT subresource = D3D11CalcSubresource(0, 0, 0);
    hr = context->Map(destImage, subresource, D3D11_MAP_READ, 0, &mapRes);
    if (FAILED(hr)) {
        printf("%x\n", hr);
        return false;
    }
    width = desc2.Width;
    height = desc2.Height;

    /*
    UINT lBmpRowPitch = desc2.Width * 4;
    UINT lRowPitch = std::min<UINT>(lBmpRowPitch, mapRes.RowPitch);
    long totalSize = lRowPitch*desc2.Height;

    *buffer = new UCHAR[totalSize];
    BYTE* sptr = reinterpret_cast<BYTE*>(mapRes.pData);
    BYTE* dptr = *buffer;

    memcpy_s(dptr, totalSize, sptr, totalSize);
    */

    memcpy_s(buffer, bufferSize, mapRes.pData, bufferSize);

    context->Unmap(destImage, subresource);
    destImage->Release();
    return true;
}

bool Duplication::DoneWithFrame()
{
    HRESULT hr = m_DeskDupl->ReleaseFrame();
    if (FAILED(hr))
    {
        return false;
    }

    if (m_AcquiredDesktopImage)
    {
        m_AcquiredDesktopImage->Release();
        m_AcquiredDesktopImage = nullptr;
    }

    return true;
}

bool Duplication::InitDevice()
{
    HRESULT hr = S_OK;

    // Driver types supported
    D3D_DRIVER_TYPE DriverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

    // Feature levels supported
    D3D_FEATURE_LEVEL FeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1
    };
    UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);

    D3D_FEATURE_LEVEL FeatureLevel;

    // Create device
    for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
    {
        hr = D3D11CreateDevice(nullptr, DriverTypes[DriverTypeIndex], nullptr, 0, FeatureLevels, NumFeatureLevels,
            D3D11_SDK_VERSION, &m_Device, &FeatureLevel, nullptr);
        if (SUCCEEDED(hr))
        {
            // Device creation success, no need to loop anymore
            return true;
        }
    }

    return false;
}

//init duplication
bool Duplication::InitDupl(UINT Output)
{


    m_OutputNumber = Output;
    // Take a reference on the device
    m_Device->AddRef();

    // Get DXGI device
    IDXGIDevice* DxgiDevice = nullptr;
    HRESULT hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
    if (FAILED(hr))
    {
        return false;
    }

    // Get DXGI adapter
    IDXGIAdapter* DxgiAdapter = nullptr;
    hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
    DxgiDevice->Release();
    DxgiDevice = nullptr;
    if (FAILED(hr))
    {
        return false;
    }

    // Get output
    IDXGIOutput* DxgiOutput = nullptr;
    hr = DxgiAdapter->EnumOutputs(Output, &DxgiOutput);
    DxgiAdapter->Release();
    DxgiAdapter = nullptr;
    if (FAILED(hr))
    {
        return false;
    }

    DxgiOutput->GetDesc(&m_OutputDesc);

    // QI for Output 1
    IDXGIOutput1* DxgiOutput1 = nullptr;
    hr = DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1));
    DxgiOutput->Release();
    DxgiOutput = nullptr;
    if (FAILED(hr))
    {
        return false;
    }

    // Create desktop duplication
    hr = DxgiOutput1->DuplicateOutput(m_Device, &m_DeskDupl);
    DxgiOutput1->Release();
    DxgiOutput1 = nullptr;
    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
        {
            MessageBoxW(nullptr, L"There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again.", L"Error", MB_OK);
            return false;
        }
        return false;
    }

    return true;
}
