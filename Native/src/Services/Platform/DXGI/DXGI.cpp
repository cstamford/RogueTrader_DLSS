#include <Services.hpp>
#include <Services/Platform/DXGI/_DXGI.hpp>
#include <Util.hpp>

using namespace DXGI::Internal;

struct {
    decltype(&CreateDXGIFactory1) CreateFactory1;
    decltype(&CreateDXGIFactory2) CreateFactory2;
} _originals;

Callbacks DXGI::Internal::_callbacks;
State DXGI::Internal::_state;

IDXGIAdapter* DXGI::GetAdapter()
{
    if (_state.BestAdapter) {
        return _state.BestAdapter;
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

    _state.BestAdapter = adapter;
    return _state.BestAdapter;
}

static HRESULT WINAPI Hook_CreateDXGIFactory2(UINT flags, REFIID riid, void** ppFactory)
{
    HRESULT hr = _originals.CreateFactory2(flags, riid, ppFactory);

    if (SUCCEEDED(hr)) {
        *ppFactory = Wrap((IDXGIFactory*)*ppFactory);
    }

    return hr;
}

static HRESULT WINAPI Hook_CreateDXGIFactory1(REFIID riid, void** ppFactory)
{
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
