#include <Services.hpp>
#include <Services/Platform/DXGI/_DXGI.hpp>
#include <Util.hpp>

using namespace DXGI::Internal;

typedef HRESULT (STDMETHODCALLTYPE* Present_t)(
    IDXGISwapChain* pSwapChain,
    UINT            SyncInterval,
    UINT            Flags);

typedef HRESULT (STDMETHODCALLTYPE* ResizeBuffers_t)(
    IDXGISwapChain* pSwapChain,
    UINT            BufferCount,
    UINT            Width,
    UINT            Height,
    DXGI_FORMAT     NewFormat,
    UINT            SwapChainFlags);

typedef HRESULT (STDMETHODCALLTYPE* Present1_t)(
    IDXGISwapChain1*               pSwapChain,
    UINT                           SyncInterval,
    UINT                           PresentFlags,
    const DXGI_PRESENT_PARAMETERS* pPresentParameters);

typedef HRESULT (STDMETHODCALLTYPE* CreateSwapChainForHwnd_t)(
    IDXGIFactory2*                         pFactory,
    IUnknown*                              pDevice,
    HWND                                   hWnd,
    const DXGI_SWAP_CHAIN_DESC1*           pDesc,
    const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
    IDXGIOutput*                           pRestrictToOutput,
    IDXGISwapChain1**                      ppSwapChain);

struct {
    decltype(&CreateDXGIFactory1) CreateFactory1;
    decltype(&CreateDXGIFactory2) CreateFactory2;
    Present_t Present;
    Present1_t Present1;
    ResizeBuffers_t ResizeBuffers;
    CreateSwapChainForHwnd_t CreateSwapChainForHwnd;
} _originals;

Callbacks DXGI::Internal::_callbacks;
State DXGI::Internal::_dxgiState;

IDXGIAdapter* DXGI::GetAdapter()
{
    if (_dxgiState.BestAdapter) {
        return _dxgiState.BestAdapter;
    }

    IDXGIFactory6* factory = nullptr;

    if (FAIL(CreateDXGIFactory1(__uuidof(IDXGIFactory6), (void**)(&factory)))) {
        return nullptr;
    }

    int i = 0;
    IDXGIAdapter4* adapter = nullptr;
    while (factory->EnumAdapterByGpuPreference(i++, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, __uuidof(IDXGIAdapter4), (void**)&adapter) != DXGI_ERROR_NOT_FOUND) {

        DXGI_ADAPTER_DESC3 desc = {};

        if (FAIL(adapter->GetDesc3(&desc))) {
            continue;
        }

        constexpr static UINT NVIDIA_VENDOR_ID = 0x10DE;
        if (desc.VendorId == NVIDIA_VENDOR_ID) {
            LOG_INFO("Found NVIDIA adapter: {} ({} GB VRAM)\n", FromWide(desc.Description), (float)desc.DedicatedVideoMemory / 1024 / 1024 / 1024);
            break;
        }
    }

    factory->Release();

    _dxgiState.BestAdapter = adapter;
    return _dxgiState.BestAdapter;
}

static HRESULT WINAPI Hook_CreateDXGIFactory2(UINT flags, REFIID riid, void** ppFactory)
{
    LOG_DEBUG("Hook_CreateDXGIFactory2\n");
    HRESULT hr = _originals.CreateFactory2(flags, riid, ppFactory);

    if (SUCCEEDED(hr)) {
        *ppFactory = Wrap((IDXGIFactory*)*ppFactory);
    }

    return hr;
}

static HRESULT WINAPI Hook_CreateDXGIFactory1(REFIID riid, void** ppFactory)
{
    LOG_DEBUG("Hook_CreateDXGIFactory1\n");
    HRESULT hr = _originals.CreateFactory1(riid, ppFactory);

    if (SUCCEEDED(hr)) {
        *ppFactory = Wrap((IDXGIFactory*)*ppFactory);
    }

    return hr;
}

void DXGI::On_CreateSwapChain(FnCreateSwapChain&& fn)
{
    _callbacks.CreateSwapChain.emplace_back(std::move(fn));
}

void DXGI::On_Present_Before(FnPresent&& fn)
{
    _callbacks.Present_Before.emplace_back(std::move(fn));
}

void DXGI::On_Present_After(FnPresent&& fn)
{
    _callbacks.Present_After.emplace_back(std::move(fn));
}

void DXGI::On_ResizeBuffers(FnVoid&& fn)
{
    _callbacks.ResizeBuffers.emplace_back(std::move(fn));
}

HRESULT STDMETHODCALLTYPE Hook_Present(
    IDXGISwapChain* pSwapChain,
    UINT            SyncInterval,
    UINT            Flags)
{
    for (const DXGI::FnPresent& fn : _callbacks.Present_Before) {
        fn(pSwapChain);
    }
    
    HRESULT res = _originals.Present(pSwapChain, SyncInterval, Flags);
    
    for (const DXGI::FnPresent& fn : _callbacks.Present_After) {
        fn(pSwapChain);
    }
    
    return res;
}

HRESULT STDMETHODCALLTYPE Hook_Present1(
    IDXGISwapChain1*               pSwapChain,
    UINT                           SyncInterval,
    UINT                           Flags,
    const DXGI_PRESENT_PARAMETERS* pPresentParameters)
{
    for (const DXGI::FnPresent& fn : _callbacks.Present_Before) {
        fn(pSwapChain);
    }
    
    HRESULT res = _originals.Present1(pSwapChain, SyncInterval, Flags, pPresentParameters);
    
    for (const DXGI::FnPresent& fn : _callbacks.Present_After) {
        fn(pSwapChain);
    }
    
    return res;
}

HRESULT STDMETHODCALLTYPE Hook_ResizeBuffers(
    IDXGISwapChain* pSwapChain,
    UINT            BufferCount,
    UINT            Width,
    UINT            Height,
    DXGI_FORMAT     NewFormat,
    UINT            SwapChainFlags)
{
    LOG_DEBUG("Hook_ResizeBuffers\n");
    HRESULT hr = _originals.ResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    for (const DXGI::FnVoid& fn : _callbacks.ResizeBuffers) {
        fn();
    }
    
    return hr;
}

void CheckStatus(MH_STATUS status)
{
    if (status != MH_OK
        && status != MH_ERROR_ALREADY_INITIALIZED
        && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG_ERROR("MH_CreateHook failed: {}\n", static_cast<int>(status));
    }
}

void RegisterSwapChainHooks(IDXGISwapChain1* swapChain)
{
    void** vtbl = *reinterpret_cast<void***>(swapChain);
    CheckStatus(MH_CreateHook(vtbl[8],  reinterpret_cast<void*>(&Hook_Present),       reinterpret_cast<void**>(&_originals.Present)));
    CheckStatus(MH_CreateHook(vtbl[22], reinterpret_cast<void*>(&Hook_Present1),      reinterpret_cast<void**>(&_originals.Present1)));
    CheckStatus(MH_CreateHook(vtbl[13], reinterpret_cast<void*>(&Hook_ResizeBuffers), reinterpret_cast<void**>(&_originals.ResizeBuffers)));
    MH_EnableHook(MH_ALL_HOOKS);
}

HRESULT STDMETHODCALLTYPE Hook_CreateSwapChainForHwnd(
    IDXGIFactory2*                         pFactory,
    IUnknown*                              pDevice,
    HWND                                   hWnd,
    const DXGI_SWAP_CHAIN_DESC1*           pDesc,
    const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
    IDXGIOutput*                           pRestrictToOutput,
    IDXGISwapChain1**                      ppSwapChain)
{
    LOG_DEBUG("Hook_CreateSwapChainForHwnd\n");
    HRESULT hr = _originals.CreateSwapChainForHwnd(pFactory, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
    RegisterSwapChainHooks(*ppSwapChain);
    
    for (const DXGI::FnCreateSwapChain& fn : _callbacks.CreateSwapChain) {
        fn(*ppSwapChain, hWnd);
    }
    
    return hr;
}

bool DXGI::RegisterHooks()
{
    CreateThread(nullptr, 0, [](LPVOID) -> DWORD {
        IDXGIFactory2* factory = nullptr;

        HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
        
        void** fvtbl = *reinterpret_cast<void***>(factory);
        CheckStatus(MH_CreateHook(fvtbl[15], reinterpret_cast<void*>(&Hook_CreateSwapChainForHwnd), reinterpret_cast<void**>(&_originals.CreateSwapChainForHwnd)));
        MH_EnableHook(MH_ALL_HOOKS);
        
        factory->Release();
        LOG_INFO("DXGI hooks registered\n");
        return 0;
    }, nullptr, 0, nullptr);
    
    
    return true;
}

static FARPROC GetHook(const char* fn)
{
    if (strcmp(fn, "CreateDXGIFactory1") == 0) {
        return (FARPROC)Hook_CreateDXGIFactory1;
    }

    if (strcmp(fn, "CreateDXGIFactory2") == 0) {
        return (FARPROC)Hook_CreateDXGIFactory2;
    }

    return nullptr;
}

static void SetHookOriginal(const char* fn, FARPROC original)
{
    if (strcmp(fn, "CreateDXGIFactory1") == 0) {
        _originals.CreateFactory1 = (decltype(_originals.CreateFactory1))original;
        return;
    }

    if (strcmp(fn, "CreateDXGIFactory2") == 0) {
        _originals.CreateFactory2 = (decltype(_originals.CreateFactory2))original;
        return;
    }
    
    assert(false);
}

REGISTER_SERVICE(
    .Name = "DXGI",
    .FnGetHook = GetHook,
    .FnSetHookOriginal = SetHookOriginal
);
