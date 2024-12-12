#include <Services/Platform/DXGI/_DXGI.hpp>
#include <Util.hpp>

using namespace DXGI;
using namespace DXGI::Internal;

class DXGISwapChainWrapper final : public IDXGISwapChain4 {
public:
    explicit DXGISwapChainWrapper(IDXGISwapChain* swapChain)
    {
        _swapChain = swapChain;
        _swapChain->QueryInterface(__uuidof(IDXGISwapChain1), (void**)&_swapChain1);
        _swapChain->QueryInterface(__uuidof(IDXGISwapChain2), (void**)&_swapChain2);
        _swapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&_swapChain3);
        _swapChain->QueryInterface(__uuidof(IDXGISwapChain4), (void**)&_swapChain4);

        if (_swapChain1) {
            const ULONG refs = _swapChain1->Release();
            assert(refs >= 1);
        }

        if (_swapChain2) {
            const ULONG refs = _swapChain2->Release();
            assert(refs >= 1);
        }

        if (_swapChain3) {
            const ULONG refs = _swapChain3->Release();
            assert(refs >= 1);
        }

        if (_swapChain4) {
            const ULONG refs = _swapChain4->Release();
            assert(refs >= 1);
        }
    }

    // IDXGISwapChain4
    virtual HRESULT STDMETHODCALLTYPE SetHDRMetaData(DXGI_HDR_METADATA_TYPE Type, UINT Size, void* pMetaData) override;

    // IDXGISwapChain3
    virtual UINT STDMETHODCALLTYPE GetCurrentBackBufferIndex() override;
    virtual HRESULT STDMETHODCALLTYPE CheckColorSpaceSupport(DXGI_COLOR_SPACE_TYPE ColorSpace, UINT* pColorSpaceSupport) override;
    virtual HRESULT STDMETHODCALLTYPE SetColorSpace1(DXGI_COLOR_SPACE_TYPE ColorSpace) override;
    virtual HRESULT STDMETHODCALLTYPE ResizeBuffers1(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT Format, UINT SwapChainFlags, const UINT* pCreationNodeMask, IUnknown* const* ppPresentQueue) override;

    // IDXGISwapChain2
    virtual HRESULT STDMETHODCALLTYPE SetSourceSize(UINT Width, UINT Height) override;
    virtual HRESULT STDMETHODCALLTYPE GetSourceSize(UINT* pWidth, UINT* pHeight) override;
    virtual HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(UINT MaxLatency) override;
    virtual HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(UINT* pMaxLatency) override;
    virtual HANDLE STDMETHODCALLTYPE GetFrameLatencyWaitableObject() override;
    virtual HRESULT STDMETHODCALLTYPE SetMatrixTransform(const DXGI_MATRIX_3X2_F* pMatrix) override;
    virtual HRESULT STDMETHODCALLTYPE GetMatrixTransform(DXGI_MATRIX_3X2_F* pMatrix) override;

    // IDXGISwapChain1
    virtual HRESULT STDMETHODCALLTYPE GetDesc1(DXGI_SWAP_CHAIN_DESC1* pDesc) override;
    virtual HRESULT STDMETHODCALLTYPE GetFullscreenDesc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pDesc) override;
    virtual HRESULT STDMETHODCALLTYPE GetHwnd(HWND* pHwnd) override;
    virtual HRESULT STDMETHODCALLTYPE GetCoreWindow(REFIID refiid, void** ppUnk) override;
    virtual HRESULT STDMETHODCALLTYPE Present1(UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters) override;
    virtual BOOL STDMETHODCALLTYPE IsTemporaryMonoSupported() override;
    virtual HRESULT STDMETHODCALLTYPE GetRestrictToOutput(IDXGIOutput** ppRestrictToOutput) override;
    virtual HRESULT STDMETHODCALLTYPE SetBackgroundColor(const DXGI_RGBA* pColor) override;
    virtual HRESULT STDMETHODCALLTYPE GetBackgroundColor(DXGI_RGBA* pColor) override;
    virtual HRESULT STDMETHODCALLTYPE SetRotation(DXGI_MODE_ROTATION Rotation) override;
    virtual HRESULT STDMETHODCALLTYPE GetRotation(DXGI_MODE_ROTATION* pRotation) override;

    // IDXGISwapChain
    virtual HRESULT STDMETHODCALLTYPE Present(UINT SyncInterval, UINT Flags) override;
    virtual HRESULT STDMETHODCALLTYPE GetBuffer(UINT Buffer, REFIID riid, void** ppSurface) override;
    virtual HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL Fullscreen, IDXGIOutput* pTarget) override;
    virtual HRESULT STDMETHODCALLTYPE GetFullscreenState(BOOL* pFullscreen, IDXGIOutput** ppTarget) override;
    virtual HRESULT STDMETHODCALLTYPE GetDesc(DXGI_SWAP_CHAIN_DESC* pDesc) override;
    virtual HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) override;
    virtual HRESULT STDMETHODCALLTYPE ResizeTarget(const DXGI_MODE_DESC* pNewTargetParameters) override;
    virtual HRESULT STDMETHODCALLTYPE GetContainingOutput(IDXGIOutput** ppOutput) override;
    virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(DXGI_FRAME_STATISTICS* pStats) override;
    virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT* pLastPresentCount) override;

    // IDXGIDeviceSubObject
    virtual HRESULT STDMETHODCALLTYPE GetDevice(REFIID riid, void** ppDevice) override;

    // IDXGIObject
    virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID Name, UINT DataSize, const void* pData) override;
    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID Name, const IUnknown* pUnknown) override;
    virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID Name, UINT* pDataSize, void* pData) override;
    virtual HRESULT STDMETHODCALLTYPE GetParent(REFIID riid, void** ppParent) override;

    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppvObject) override;
    virtual ULONG STDMETHODCALLTYPE AddRef() override;
    virtual ULONG STDMETHODCALLTYPE Release() override;

private:
    IDXGISwapChain* _swapChain;
    IDXGISwapChain1* _swapChain1;
    IDXGISwapChain2* _swapChain2;
    IDXGISwapChain3* _swapChain3;
    IDXGISwapChain4* _swapChain4;
};

IDXGISwapChain* DXGI::Internal::Wrap(IDXGISwapChain* SwapChain)
{
    return new DXGISwapChainWrapper(SwapChain);
}

HRESULT DXGISwapChainWrapper::SetHDRMetaData(DXGI_HDR_METADATA_TYPE Type, UINT Size, void* pMetaData)
{
    return _swapChain4->SetHDRMetaData(Type, Size, pMetaData);
}

UINT DXGISwapChainWrapper::GetCurrentBackBufferIndex()
{
    return _swapChain3->GetCurrentBackBufferIndex();
}

HRESULT DXGISwapChainWrapper::CheckColorSpaceSupport(DXGI_COLOR_SPACE_TYPE ColorSpace, UINT* pColorSpaceSupport)
{
    return _swapChain3->CheckColorSpaceSupport(ColorSpace, pColorSpaceSupport);
}

HRESULT DXGISwapChainWrapper::SetColorSpace1(DXGI_COLOR_SPACE_TYPE ColorSpace)
{
    return _swapChain3->SetColorSpace1(ColorSpace);
}

HRESULT DXGISwapChainWrapper::ResizeBuffers1(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT Format, UINT SwapChainFlags, const UINT* pCreationNodeMask, IUnknown* const* ppPresentQueue)
{
    return _swapChain3->ResizeBuffers1(BufferCount, Width, Height, Format, SwapChainFlags, pCreationNodeMask, ppPresentQueue);
}

HRESULT DXGISwapChainWrapper::SetSourceSize(UINT Width, UINT Height)
{
    return _swapChain2->SetSourceSize(Width, Height);
}

HRESULT DXGISwapChainWrapper::GetSourceSize(UINT* pWidth, UINT* pHeight)
{
    return _swapChain2->GetSourceSize(pWidth, pHeight);
}

HRESULT DXGISwapChainWrapper::SetMaximumFrameLatency(UINT MaxLatency)
{
    return _swapChain2->SetMaximumFrameLatency(MaxLatency);
}

HRESULT DXGISwapChainWrapper::GetMaximumFrameLatency(UINT* pMaxLatency)
{
    return _swapChain2->GetMaximumFrameLatency(pMaxLatency);
}

HANDLE DXGISwapChainWrapper::GetFrameLatencyWaitableObject()
{
    return _swapChain2->GetFrameLatencyWaitableObject();
}

HRESULT DXGISwapChainWrapper::SetMatrixTransform(const DXGI_MATRIX_3X2_F* pMatrix)
{
    return _swapChain2->SetMatrixTransform(pMatrix);
}

HRESULT DXGISwapChainWrapper::GetMatrixTransform(DXGI_MATRIX_3X2_F* pMatrix)
{
    return _swapChain2->GetMatrixTransform(pMatrix);
}

HRESULT DXGISwapChainWrapper::GetDesc1(DXGI_SWAP_CHAIN_DESC1* pDesc)
{
    return _swapChain1->GetDesc1(pDesc);
}

HRESULT DXGISwapChainWrapper::GetFullscreenDesc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pDesc)
{
    return _swapChain1->GetFullscreenDesc(pDesc);
}

HRESULT DXGISwapChainWrapper::GetHwnd(HWND* pHwnd)
{
    return _swapChain1->GetHwnd(pHwnd);
}

HRESULT DXGISwapChainWrapper::GetCoreWindow(REFIID refiid, void** ppUnk)
{
    return _swapChain1->GetCoreWindow(refiid, ppUnk);
}

HRESULT DXGISwapChainWrapper::Present1(UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters)
{
    for (const FnPresent& fn : _callbacks.Present_Before) {
        fn(this);
    }

    HRESULT hr = _swapChain1->Present1(SyncInterval, PresentFlags, pPresentParameters);

    for (const FnPresent& fn : _callbacks.Present_After) {
        fn(this);
    }

    return hr;
}

BOOL DXGISwapChainWrapper::IsTemporaryMonoSupported()
{
    return _swapChain1->IsTemporaryMonoSupported();
}

HRESULT DXGISwapChainWrapper::GetRestrictToOutput(IDXGIOutput** ppRestrictToOutput)
{
    return _swapChain1->GetRestrictToOutput(ppRestrictToOutput);
}

HRESULT DXGISwapChainWrapper::SetBackgroundColor(const DXGI_RGBA* pColor)
{
    return _swapChain1->SetBackgroundColor(pColor);
}

HRESULT DXGISwapChainWrapper::GetBackgroundColor(DXGI_RGBA* pColor)
{
    return _swapChain1->GetBackgroundColor(pColor);
}

HRESULT DXGISwapChainWrapper::SetRotation(DXGI_MODE_ROTATION Rotation)
{
    return _swapChain1->SetRotation(Rotation);
}

HRESULT DXGISwapChainWrapper::GetRotation(DXGI_MODE_ROTATION* pRotation)
{
    return _swapChain1->GetRotation(pRotation);
}

HRESULT DXGISwapChainWrapper::Present(UINT SyncInterval, UINT Flags)
{
    for (const FnPresent& fn : _callbacks.Present_Before) {
        fn(this);
    }

    HRESULT hr = _swapChain->Present(SyncInterval, Flags);

    for (const FnPresent& fn : _callbacks.Present_After) {
        fn(this);
    }

    return hr;
}

HRESULT DXGISwapChainWrapper::GetBuffer(UINT Buffer, REFIID riid, void** ppSurface)
{
    return _swapChain->GetBuffer(Buffer, riid, ppSurface);
}

HRESULT DXGISwapChainWrapper::SetFullscreenState(BOOL Fullscreen, IDXGIOutput* pTarget)
{
    return _swapChain->SetFullscreenState(Fullscreen, pTarget);
}

HRESULT DXGISwapChainWrapper::GetFullscreenState(BOOL* pFullscreen, IDXGIOutput** ppTarget)
{
    return _swapChain->GetFullscreenState(pFullscreen, ppTarget);
}

HRESULT DXGISwapChainWrapper::GetDesc(DXGI_SWAP_CHAIN_DESC* pDesc)
{
    return _swapChain->GetDesc(pDesc);
}

HRESULT DXGISwapChainWrapper::ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    return _swapChain->ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

HRESULT DXGISwapChainWrapper::ResizeTarget(const DXGI_MODE_DESC* pNewTargetParameters)
{
    return _swapChain->ResizeTarget(pNewTargetParameters);
}

HRESULT DXGISwapChainWrapper::GetContainingOutput(IDXGIOutput** ppOutput)
{
    return _swapChain->GetContainingOutput(ppOutput);
}

HRESULT DXGISwapChainWrapper::GetFrameStatistics(DXGI_FRAME_STATISTICS* pStats)
{
    return _swapChain->GetFrameStatistics(pStats);
}

HRESULT DXGISwapChainWrapper::GetLastPresentCount(UINT* pLastPresentCount)
{
    return _swapChain->GetLastPresentCount(pLastPresentCount);
}

HRESULT DXGISwapChainWrapper::GetDevice(REFIID riid, void** ppDevice)
{
    return _swapChain->GetDevice(riid, ppDevice);
}

HRESULT DXGISwapChainWrapper::SetPrivateData(REFGUID Name, UINT DataSize, const void* pData)
{
    return _swapChain->SetPrivateData(Name, DataSize, pData);
}

HRESULT DXGISwapChainWrapper::SetPrivateDataInterface(REFGUID Name, const IUnknown* pUnknown)
{
    return _swapChain->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT DXGISwapChainWrapper::GetPrivateData(REFGUID Name, UINT* pDataSize, void* pData)
{
    return _swapChain->GetPrivateData(Name, pDataSize, pData);
}

HRESULT DXGISwapChainWrapper::GetParent(REFIID riid, void** ppParent)
{
    return _swapChain->GetParent(riid, ppParent);
}

HRESULT DXGISwapChainWrapper::QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppvObject)
{
    if (riid == __uuidof(IUnknown) ||
        riid == __uuidof(IDXGISwapChain) && _swapChain ||
        riid == __uuidof(IDXGISwapChain1) && _swapChain1 ||
        riid == __uuidof(IDXGISwapChain2) && _swapChain2 ||
        riid == __uuidof(IDXGISwapChain3) && _swapChain3 ||
        riid == __uuidof(IDXGISwapChain4) && _swapChain4)
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }

    LOG_WARNING("DXGISwapChainWrapper::QueryInterface unknown: {}\n", ToString(riid));
    return _swapChain->QueryInterface(riid, ppvObject);
}

ULONG DXGISwapChainWrapper::AddRef()
{
    return _swapChain->AddRef();
}

ULONG DXGISwapChainWrapper::Release()
{
    const ULONG refCount = _swapChain->Release();
    if (refCount == 0) {
        delete this;
    }
    return refCount;
}
