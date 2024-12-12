#include <Services.hpp>
#include <Services/Platform/D3D11/_D3D11.hpp>
#include <Util.hpp>



using namespace D3D11::Internal;

struct {
    decltype(&D3D11CreateDevice) CreateDevice;
    decltype(&D3D11CreateDeviceAndSwapChain) CreateDeviceAndSwapChain;
} _originals;

Callbacks D3D11::Internal::_callbacks;
State D3D11::Internal::_state;

ID3D11Device* D3D11::GetDevice()
{
    return _state.Device;
}

ID3D11DeviceContext* D3D11::GetImmediateContext()
{
    return _state.DeviceContext;
}

ID3D11Resource* D3D11::GetResource(ID3D11View* view)
{
    ID3D11Resource* res;
    view->GetResource(&res);
    return res;
}

void D3D11::On_SetRenderTargets(FnSetRenderTargets&& fn)
{
    _callbacks.SetRenderTargets.emplace_back(std::move(fn));
}

static HRESULT WINAPI Hook_D3D11CreateDevice(
    _In_opt_ IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE driverType,
    HMODULE software,
    UINT flags,
    _In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT featureLevels,
    UINT sdkVersion,
    _COM_Outptr_opt_ ID3D11Device** ppDevice,
    _Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
    _COM_Outptr_opt_ ID3D11DeviceContext** ppImmediateContext)
{
    HRESULT result = _originals.CreateDevice(
        pAdapter,
        driverType,
        software,
        flags,
        pFeatureLevels,
        featureLevels,
        sdkVersion,
        ppDevice,
        pFeatureLevel,
        ppImmediateContext
    );

    if (SUCCEEDED(result)) {
        if (ppDevice) {
            *ppDevice = D3D11::Internal::Wrap(*ppDevice);
            _state.Device = _state.Device ? _state.Device : *ppDevice;
        }

        if (ppImmediateContext) {
            *ppImmediateContext = D3D11::Internal::Wrap(*ppImmediateContext);
            _state.DeviceContext = _state.DeviceContext ? _state.DeviceContext : *ppImmediateContext;
        }
    }

    return result;
}

HRESULT WINAPI Hook_D3D11CreateDeviceAndSwapChain(
    _In_opt_ IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE driverType,
    HMODULE software,
    UINT flags,
    _In_reads_opt_(featureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT featureLevels,
    UINT sdkVersion,
    _In_opt_ CONST DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    _COM_Outptr_opt_ IDXGISwapChain** ppSwapChain,
    _COM_Outptr_opt_ ID3D11Device** ppDevice,
    _Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
    _COM_Outptr_opt_ ID3D11DeviceContext** ppImmediateContext)
{
    HRESULT result = _originals.CreateDeviceAndSwapChain(
        pAdapter,
        driverType,
        software,
        flags,
        pFeatureLevels,
        featureLevels,
        sdkVersion,
        pSwapChainDesc,
        ppSwapChain,
        ppDevice,
        pFeatureLevel,
        ppImmediateContext
    );

    if (SUCCEEDED(result)) {
        if (ppDevice) {
            *ppDevice = D3D11::Internal::Wrap(*ppDevice);
            _state.Device = _state.Device ? _state.Device : *ppDevice;
        }

        if (ppImmediateContext) {
            *ppImmediateContext = D3D11::Internal::Wrap(*ppImmediateContext);
            _state.DeviceContext = _state.DeviceContext ? _state.DeviceContext : *ppImmediateContext;
        }
    }

    return result;
}

static FARPROC GetHook(const char* fn)
{
    if (strcmp(fn, "D3D11CreateDevice") == 0) {
        return (FARPROC)Hook_D3D11CreateDevice;
    }

    if (strcmp(fn, "D3D11CreateDeviceAndSwapChain") == 0) {
        return (FARPROC)Hook_D3D11CreateDeviceAndSwapChain;
    }

    return nullptr;
}

static void SetHookOriginal(const char* fn, FARPROC original)
{
    if (strcmp(fn, "D3D11CreateDevice") == 0) {
        _originals.CreateDevice = (decltype(_originals.CreateDevice))original;
        return;
    }
    
    if (strcmp(fn, "D3D11CreateDeviceAndSwapChain") == 0) {
        _originals.CreateDeviceAndSwapChain = (decltype(_originals.CreateDeviceAndSwapChain))original;
        return;
    }

    assert(false);
}

REGISTER_SERVICE(
    .Name = "D3D11",
    .FnGetHook = GetHook,
    .FnSetHookOriginal = SetHookOriginal
);
