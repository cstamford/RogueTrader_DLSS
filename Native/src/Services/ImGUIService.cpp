#include <d3d11_4.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <algorithm>

#include <Services.hpp>
#include <Services/Platform/D3D11.hpp>
#include <Services/Platform/DXGI.hpp>
#include <Util.hpp>

using FnWndProc = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);

struct {
    HWND Window;
    ID3D11RenderTargetView* Rtv;
} _state;

struct {
    FnWndProc WndProc;
} _originals;

static void CreateInspectorWindow(IDXGISwapChain*, void*);
static void UpdateInspectorWindow_StartFrame();
static void UpdateInspectorWindow_EndFrame(IDXGISwapChain*);

static LRESULT WINAPI Hook_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return true;
    }

    return _originals.WndProc(hWnd, msg, wParam, lParam);
}

static void CreateInspectorWindow(IDXGISwapChain* sc, void* hwnd)
{
    if (_state.Window) {
        LOG_WARNING("The inspector window was already initialized when a new device was created.\n");
        return;
    }

    _state.Window = (HWND)hwnd;

    _originals.WndProc = (FnWndProc)GetWindowLongPtr(_state.Window, GWLP_WNDPROC);
    SetWindowLongPtr(_state.Window, GWLP_WNDPROC, (LONG_PTR)Hook_WndProc);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    DISCARD((BOOL)ImGui_ImplWin32_Init(hwnd));
    DISCARD((BOOL)ImGui_ImplDX11_Init(D3D11::GetDevice(), D3D11::GetImmediateContext()));

    UpdateInspectorWindow_StartFrame();
}

static void UpdateInspectorWindow_StartFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

static void UpdateInspectorWindow_EndFrame(IDXGISwapChain* swapChain)
{
    assert(swapChain);

    if (!_state.Rtv) {
        ID3D11Texture2D* backBuffer = nullptr;
        swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        assert(backBuffer);

        DXGI_SWAP_CHAIN_DESC desc;
        swapChain->GetDesc(&desc);

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {
            .Format = desc.BufferDesc.Format,
            .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D
        };

        DISCARD(D3D11::GetDevice()->CreateRenderTargetView(backBuffer, &rtvDesc, &_state.Rtv));
        backBuffer->Release();
    }

    assert(_state.Rtv);
    D3D11::GetImmediateContext()->OMSetRenderTargets(1, &_state.Rtv, nullptr);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    UpdateInspectorWindow_StartFrame();
}

static bool Initialize()
{
    DXGI::On_CreateSwapChain(CreateInspectorWindow);
    DXGI::On_Present_Before(UpdateInspectorWindow_EndFrame);

    return true;
}

REGISTER_SERVICE(
    .Name = "ImGUI",
    .FnInitialize = Initialize);
