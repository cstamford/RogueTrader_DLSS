#include <Services/Platform/DXGI/_DXGI.hpp>
#include <Util.hpp>

using namespace DXGI;
using namespace DXGI::Internal;

class DXGIFactoryWrapper final : public IDXGIFactory6 {
public:
    explicit DXGIFactoryWrapper(IDXGIFactory* factory)
    {
        _factory = factory;
        _factory->QueryInterface(__uuidof(IDXGIFactory1), (void**)&_factory1);
        _factory->QueryInterface(__uuidof(IDXGIFactory2), (void**)&_factory2);
        _factory->QueryInterface(__uuidof(IDXGIFactory3), (void**)&_factory3);
        _factory->QueryInterface(__uuidof(IDXGIFactory4), (void**)&_factory4);
        _factory->QueryInterface(__uuidof(IDXGIFactory5), (void**)&_factory5);
        _factory->QueryInterface(__uuidof(IDXGIFactory6), (void**)&_factory6);

        if (_factory1) {
            const ULONG refs = _factory1->Release();
            assert(refs >= 1);
        }

        if (_factory2) {
            const ULONG refs = _factory2->Release();
            assert(refs >= 1);
        }

        if (_factory3) {
            const ULONG refs = _factory3->Release();
            assert(refs >= 1);
        }

        if (_factory4) {
            const ULONG refs = _factory4->Release();
            assert(refs >= 1);
        }

        if (_factory5) {
            const ULONG refs = _factory5->Release();
            assert(refs >= 1);
        }

        if (_factory6) {
            const ULONG refs = _factory6->Release();
            assert(refs >= 1);
        }
    }

    // IDXGIFactory6
    virtual HRESULT STDMETHODCALLTYPE EnumAdapterByGpuPreference(UINT Adapter, DXGI_GPU_PREFERENCE GpuPreference, REFIID riid, void** ppvAdapter) override;

    // IDXGIFactory5
    virtual HRESULT STDMETHODCALLTYPE CheckFeatureSupport(DXGI_FEATURE Feature, void* pFeatureSupportData, UINT FeatureSupportDataSize) override;

    // IDXGIFactory4
    virtual HRESULT STDMETHODCALLTYPE EnumAdapterByLuid(LUID AdapterLuid, REFIID riid, void** ppvAdapter) override;
    virtual HRESULT STDMETHODCALLTYPE EnumWarpAdapter(REFIID riid, void** ppvAdapter) override;

    // IDXGIFactory3
    virtual UINT STDMETHODCALLTYPE GetCreationFlags() override;

    // IDXGIFactory2
    virtual BOOL STDMETHODCALLTYPE IsWindowedStereoEnabled();
    virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForHwnd(IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) override;
    virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForCoreWindow(IUnknown* pDevice, IUnknown* pWindow, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) override;
    virtual HRESULT STDMETHODCALLTYPE GetSharedResourceAdapterLuid(HANDLE hResource, LUID* pLuid) override;
    virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusWindow(HWND WindowHandle, UINT wMsg, DWORD* pdwCookie) override;
    virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusEvent(HANDLE hEvent, DWORD* pdwCookie) override;
    virtual void STDMETHODCALLTYPE UnregisterStereoStatus(DWORD dwCookie) override;
    virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusWindow(HWND WindowHandle, UINT wMsg, DWORD* pdwCookie) override;
    virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusEvent(HANDLE hEvent, DWORD* pdwCookie) override;
    virtual void STDMETHODCALLTYPE UnregisterOcclusionStatus(DWORD dwCookie) override;
    virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForComposition(IUnknown* pDevice, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) override;

    // IDXGIFactory1
    virtual HRESULT STDMETHODCALLTYPE EnumAdapters1(UINT Adapter, IDXGIAdapter1** ppAdapter) override;
    virtual BOOL STDMETHODCALLTYPE IsCurrent() override;

    // IDXGIFactory
    virtual HRESULT STDMETHODCALLTYPE EnumAdapters(UINT Adapter, IDXGIAdapter** ppAdapter) override;
    virtual HRESULT STDMETHODCALLTYPE MakeWindowAssociation(HWND WindowHandle, UINT Flags) override;
    virtual HRESULT STDMETHODCALLTYPE GetWindowAssociation(HWND* pWindowHandle) override;
    virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain) override;
    virtual HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter** ppAdapter) override;

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
    IDXGIFactory* _factory;
    IDXGIFactory1* _factory1;
    IDXGIFactory2* _factory2;
    IDXGIFactory3* _factory3;
    IDXGIFactory4* _factory4;
    IDXGIFactory5* _factory5;
    IDXGIFactory6* _factory6;
};

IDXGIFactory* DXGI::Internal::Wrap(IDXGIFactory* factory)
{
    return new DXGIFactoryWrapper(factory);
}

HRESULT DXGIFactoryWrapper::EnumAdapterByGpuPreference(UINT Adapter, DXGI_GPU_PREFERENCE GpuPreference, REFIID riid, void** ppvAdapter)
{
    return _factory6->EnumAdapterByGpuPreference(Adapter, GpuPreference, riid, ppvAdapter);
}

HRESULT DXGIFactoryWrapper::CheckFeatureSupport(DXGI_FEATURE Feature, void* pFeatureSupportData, UINT FeatureSupportDataSize)
{
    return _factory5->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
}

HRESULT DXGIFactoryWrapper::EnumAdapterByLuid(LUID AdapterLuid, REFIID riid, void** ppvAdapter)
{
    return _factory4->EnumAdapterByLuid(AdapterLuid, riid, ppvAdapter);
}

HRESULT DXGIFactoryWrapper::EnumWarpAdapter(REFIID riid, void** ppvAdapter)
{
    return _factory4->EnumWarpAdapter(riid, ppvAdapter);
}

UINT DXGIFactoryWrapper::GetCreationFlags()
{
    return _factory3->GetCreationFlags();
}

BOOL DXGIFactoryWrapper::IsWindowedStereoEnabled()
{
    return _factory2->IsWindowedStereoEnabled();
}

HRESULT DXGIFactoryWrapper::CreateSwapChainForHwnd(IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
{
    HRESULT hr = _factory2->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
    if (ppSwapChain && SUCCEEDED(hr)) {
        *ppSwapChain = (IDXGISwapChain1*)Wrap(*ppSwapChain);
        for (const FnCreateSwapChain& fn : _callbacks.CreateSwapChain) {
            fn(*ppSwapChain, hWnd);
        }
    }
    return hr;
}

HRESULT DXGIFactoryWrapper::CreateSwapChainForCoreWindow(IUnknown* pDevice, IUnknown* pWindow, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
{
    HRESULT hr = _factory2->CreateSwapChainForCoreWindow(pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
    if (ppSwapChain && SUCCEEDED(hr)) {
        *ppSwapChain = (IDXGISwapChain1*)Wrap(*ppSwapChain);
        for (const FnCreateSwapChain& fn : _callbacks.CreateSwapChain) {
            fn(*ppSwapChain, nullptr);
        }
    }
    return hr;
}

HRESULT DXGIFactoryWrapper::GetSharedResourceAdapterLuid(HANDLE hResource, LUID* pLuid)
{
    return _factory2->GetSharedResourceAdapterLuid(hResource, pLuid);
}

HRESULT DXGIFactoryWrapper::RegisterStereoStatusWindow(HWND WindowHandle, UINT wMsg, DWORD* pdwCookie)
{
    return _factory2->RegisterStereoStatusWindow(WindowHandle, wMsg, pdwCookie);
}

HRESULT DXGIFactoryWrapper::RegisterStereoStatusEvent(HANDLE hEvent, DWORD* pdwCookie)
{
    return _factory2->RegisterStereoStatusEvent(hEvent, pdwCookie);
}

void DXGIFactoryWrapper::UnregisterStereoStatus(DWORD dwCookie)
{
    return _factory2->UnregisterStereoStatus(dwCookie);
}

HRESULT DXGIFactoryWrapper::RegisterOcclusionStatusWindow(HWND WindowHandle, UINT wMsg, DWORD* pdwCookie)
{
    return _factory2->RegisterOcclusionStatusWindow(WindowHandle, wMsg, pdwCookie);
}

HRESULT DXGIFactoryWrapper::RegisterOcclusionStatusEvent(HANDLE hEvent, DWORD* pdwCookie)
{
    return _factory2->RegisterOcclusionStatusEvent(hEvent, pdwCookie);
}

void DXGIFactoryWrapper::UnregisterOcclusionStatus(DWORD dwCookie)
{
    return _factory2->UnregisterOcclusionStatus(dwCookie);
}

HRESULT DXGIFactoryWrapper::CreateSwapChainForComposition(IUnknown* pDevice, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
{
    HRESULT hr = _factory2->CreateSwapChainForComposition(pDevice, pDesc, pRestrictToOutput, ppSwapChain);
    if (ppSwapChain && SUCCEEDED(hr)) {
        *ppSwapChain = (IDXGISwapChain1*)Wrap(*ppSwapChain);
        for (const FnCreateSwapChain& fn : _callbacks.CreateSwapChain) {
            fn(*ppSwapChain, nullptr);
        }
    }
    return hr;
}

HRESULT DXGIFactoryWrapper::EnumAdapters1(UINT Adapter, IDXGIAdapter1** ppAdapter)
{
    return _factory1->EnumAdapters1(Adapter, ppAdapter);
}

BOOL DXGIFactoryWrapper::IsCurrent()
{
    return _factory1->IsCurrent();
}

HRESULT DXGIFactoryWrapper::EnumAdapters(UINT Adapter, IDXGIAdapter** ppAdapter)
{
    return _factory->EnumAdapters(Adapter, ppAdapter);
}

HRESULT DXGIFactoryWrapper::MakeWindowAssociation(HWND WindowHandle, UINT Flags)
{
    return _factory->MakeWindowAssociation(WindowHandle, Flags);
}

HRESULT DXGIFactoryWrapper::GetWindowAssociation(HWND* pWindowHandle)
{
    return _factory->GetWindowAssociation(pWindowHandle);
}

HRESULT DXGIFactoryWrapper::CreateSwapChain(IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
    HRESULT hr = _factory->CreateSwapChain(pDevice, pDesc, ppSwapChain);
    if (ppSwapChain && SUCCEEDED(hr)) {
        *ppSwapChain = Wrap(*ppSwapChain);
        for (const FnCreateSwapChain& fn : _callbacks.CreateSwapChain) {
            fn(*ppSwapChain, pDesc->OutputWindow);
        }

    }
    return hr;
}

HRESULT DXGIFactoryWrapper::CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter** ppAdapter)
{
    return _factory->CreateSoftwareAdapter(Module, ppAdapter);
}

HRESULT DXGIFactoryWrapper::SetPrivateData(REFGUID Name, UINT DataSize, const void* pData)
{
    return _factory->SetPrivateData(Name, DataSize, pData);
}

HRESULT DXGIFactoryWrapper::SetPrivateDataInterface(REFGUID Name, const IUnknown* pUnknown)
{
    return _factory->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT DXGIFactoryWrapper::GetPrivateData(REFGUID Name, UINT* pDataSize, void* pData)
{
    return _factory->GetPrivateData(Name, pDataSize, pData);
}

HRESULT DXGIFactoryWrapper::GetParent(REFIID riid, void** ppParent)
{
    return _factory->GetParent(riid, ppParent);
}

HRESULT DXGIFactoryWrapper::QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppvObject)
{
    if (riid == __uuidof(IUnknown) ||
        riid == __uuidof(IDXGIFactory) && _factory ||
        riid == __uuidof(IDXGIFactory1) && _factory1 ||
        riid == __uuidof(IDXGIFactory2) && _factory2 ||
        riid == __uuidof(IDXGIFactory3) && _factory3 ||
        riid == __uuidof(IDXGIFactory4) && _factory4 ||
        riid == __uuidof(IDXGIFactory5) && _factory5 ||
        riid == __uuidof(IDXGIFactory6) && _factory6)
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }

    LOG_WARNING("DXGIFactoryWrapper::QueryInterface unknown: {}\n", ToString(riid));
    return _factory->QueryInterface(riid, ppvObject);
}

ULONG DXGIFactoryWrapper::AddRef()
{
    return _factory->AddRef();
}

ULONG DXGIFactoryWrapper::Release()
{
    const ULONG refCount = _factory->Release();
    if (refCount == 0) {
        delete this;
    }
    return refCount;
}
