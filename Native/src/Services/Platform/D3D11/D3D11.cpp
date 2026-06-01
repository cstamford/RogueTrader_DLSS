#include <Services.hpp>
#include <Services/Platform/D3D11/_D3D11.hpp>
#include <Util.hpp>

#include <d3d11on12.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;


using namespace D3D11::Internal;

struct {
    decltype(&D3D11CreateDevice) CreateDevice;
    decltype(&D3D11CreateDeviceAndSwapChain) CreateDeviceAndSwapChain;
    decltype(&D3D11On12CreateDevice) Create11on12Device;
} _originals;

Callbacks D3D11::Internal::_callbacks;
State D3D11::Internal::_d3d11State;

ID3D11Device* D3D11::GetDevice()
{
    return _d3d11State.Device;
}

ID3D11DeviceContext* D3D11::GetImmediateContext()
{
    return _d3d11State.DeviceContext;
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

bool IsD3D11On12(ID3D11Device* device) {
    ComPtr<ID3D11On12Device> on12;
    return SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&on12)));
}

static bool d3d11On12Creating = false;

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
    
    if (_d3d11State.Device != nullptr)
    {
        return result;
    }
    
    if (SUCCEEDED(result) && !IsD3D11On12(*ppDevice)) {
        if (ppDevice) {
            *ppDevice = D3D11::Internal::Wrap(*ppDevice);
            _d3d11State.Device = _d3d11State.Device ? _d3d11State.Device : *ppDevice;
        }

        if (ppImmediateContext) {
            *ppImmediateContext = D3D11::Internal::Wrap(*ppImmediateContext);
            _d3d11State.DeviceContext = _d3d11State.DeviceContext ? _d3d11State.DeviceContext : *ppImmediateContext;
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
    
    if (_d3d11State.Device != nullptr)
    {
        return result;
    }

    if (SUCCEEDED(result) && !IsD3D11On12(*ppDevice)) {
        if (ppDevice) {
            *ppDevice = D3D11::Internal::Wrap(*ppDevice);
            _d3d11State.Device = _d3d11State.Device ? _d3d11State.Device : *ppDevice;
        }

        if (ppImmediateContext) {
            *ppImmediateContext = D3D11::Internal::Wrap(*ppImmediateContext);
            _d3d11State.DeviceContext = _d3d11State.DeviceContext ? _d3d11State.DeviceContext : *ppImmediateContext;
        }
    }

    return result;
}

HRESULT WINAPI Hook_D3D11On12CreateDevice(
    _In_ IUnknown* pDevice,
    UINT Flags,
    _In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    _In_reads_opt_(NumQueues) IUnknown* CONST* ppCommandQueues,
    UINT NumQueues,
    UINT NodeMask,
    _COM_Outptr_opt_ ID3D11Device** ppDevice,
    _COM_Outptr_opt_ ID3D11DeviceContext** ppImmediateContext,
    _Out_opt_ D3D_FEATURE_LEVEL* pChosenFeatureLevel)
{
    d3d11On12Creating = true;
    HRESULT result = _originals.Create11on12Device(
        pDevice,
        Flags,
        pFeatureLevels,
        FeatureLevels,
        ppCommandQueues,
        NumQueues,
        NodeMask,
        ppDevice,
        ppImmediateContext,
        pChosenFeatureLevel
    );
    d3d11On12Creating = false;
    
    return result;
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
    
    if (strcmp(fn, "D3D11On12CreateDevice") == 0) {
        _originals.Create11on12Device = (decltype(_originals.Create11on12Device))original;
        return;
    }

    assert(false);
}

REGISTER_SERVICE(
    .Name = "D3D11",
    .FnSetHookOriginal = SetHookOriginal,
    .DirectHooks = { 
        {L"d3d11.dll", "D3D11CreateDevice", reinterpret_cast<void*>(&Hook_D3D11CreateDevice)},
        {L"d3d11.dll", "D3D11CreateDeviceAndSwapChain", reinterpret_cast<void*>(&Hook_D3D11CreateDeviceAndSwapChain)},
        {L"d3d11.dll", "D3D11On12CreateDevice", reinterpret_cast<void*>(&Hook_D3D11On12CreateDevice)}
        }
);
