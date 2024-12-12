#include <d3d11_4.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include <Services.hpp>
#include <Services/Debug/Inspector.hpp>
#include <Services/Platform/D3D11.hpp>
#include <Services/Platform/DXGI.hpp>
#include <Util.hpp>

using FnWndProc = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);

struct FrameRenderTargetInfo {
    int RefCount;
    int FrameRefCount;
    int LastSeenFrame;

    struct {
        std::string Name;
        std::string Format;
        std::string ViewDimension;
    } RenderTargetInfo;

    struct {
        std::string Name;
        std::string Format;
        int Width;
        int Height;
        int Depth;
    } TextureInfo;

    ID3D11ShaderResourceView* Srv;
};

struct {
    int Frame = 0;
    std::unordered_map<ID3D11View*, FrameRenderTargetInfo> FrameRenderTargets;
    std::unordered_set<std::string> PinnedResources;

    struct {
        bool Flip = true;
        std::string PinnedLabel;
    } ImGuiState;
} _state;


void Inspector::PinResourceForFrame(ID3D11Resource* resource)
{
    _state.PinnedResources.insert(D3D11::GetDebugName(resource));
}

static void UpdateInspectorWindow(IDXGISwapChain*)
{
    ID3D11ShaderResourceView* srvPreview = nullptr;

    if (ImGui::Begin("Inspector")) {
        ImGui::Text("Frame: %d", _state.Frame);
        ImGui::Separator();

        ImGui::Checkbox("Flip", &_state.ImGuiState.Flip);
        ImGui::Separator();

        std::vector<std::pair<ID3D11View*, FrameRenderTargetInfo>> sortedRts(_state.FrameRenderTargets.begin(), _state.FrameRenderTargets.end());
        std::sort(sortedRts.begin(), sortedRts.end(), [](const auto& lhs, const auto& rhs) {
            const bool lhsPinned = _state.PinnedResources.contains(lhs.second.TextureInfo.Name);
            const bool rhsPinned = _state.PinnedResources.contains(rhs.second.TextureInfo.Name);

            if (lhsPinned != rhsPinned) {
                return lhsPinned > rhsPinned;
            }

            if (lhs.second.RefCount != rhs.second.RefCount) {
                return lhs.second.RefCount > rhs.second.RefCount;
            }

            if (lhs.second.FrameRefCount != rhs.second.FrameRefCount) {
                return lhs.second.FrameRefCount > rhs.second.FrameRefCount;
            }

            return lhs.first > rhs.first;
        });

        for (const auto& [rt, info] : sortedRts) {
            const std::string labelPtr = fmt::format("{}", fmt::ptr(rt));
            const std::string rtInfo = fmt::format("[{} refs] {} -> {}, {}", info.FrameRefCount, info.RenderTargetInfo.Name, info.RenderTargetInfo.Format, info.RenderTargetInfo.ViewDimension);
            const std::string texInfo = fmt::format("\t{} -> {}, {}x{}x{}", info.TextureInfo.Name, info.TextureInfo.Format, info.TextureInfo.Width, info.TextureInfo.Height, info.TextureInfo.Depth);

            ImGui::Text(rtInfo.c_str());
            ImGui::Text(texInfo.c_str());

            const float aspect = (float)info.TextureInfo.Width / info.TextureInfo.Height;
            const ImVec2 windowSize = ImGui::GetWindowSize();
            const ImVec2 previewSize = ImVec2(windowSize.x, (windowSize.x / aspect));

            if (info.Srv) {
                if (ImGui::ImageButton(labelPtr.c_str(), (ImTextureID)info.Srv, previewSize)) {
                    _state.ImGuiState.PinnedLabel = _state.ImGuiState.PinnedLabel != labelPtr ? labelPtr : std::string();
                }

                srvPreview = ImGui::IsItemHovered() || _state.ImGuiState.PinnedLabel == labelPtr ? info.Srv : srvPreview;
            }
        }
    }

    ImGui::End();

    if (srvPreview) {
        const ImVec2 min = ImVec2(0, 0);
        const ImVec2 max = ImGui::GetIO().DisplaySize;
        const ImVec2 uv0 = _state.ImGuiState.Flip ? ImVec2(0, 1) : ImVec2(0, 0);
        const ImVec2 uv1 = _state.ImGuiState.Flip ? ImVec2(1, 0) : ImVec2(1, 1);

        ImGui::GetBackgroundDrawList()->AddRectFilled(min, max, IM_COL32(0, 0, 0, 255));
        ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)srvPreview, min, max, uv0, uv1);
    }

    std::vector<ID3D11View*> renderTargetsToClear;

    for (auto& [rt, info] : _state.FrameRenderTargets) {
        info.FrameRefCount = 0;
        if (_state.Frame - info.LastSeenFrame >= 10) {
            renderTargetsToClear.push_back(rt);
        }
    }

    for (ID3D11View* rt : renderTargetsToClear) {
        FrameRenderTargetInfo info = std::move(_state.FrameRenderTargets[rt]);
        _state.FrameRenderTargets.erase(rt);

        if (info.Srv) {
            info.Srv->Release();
        }

        rt->Release();
    }

    _state.PinnedResources.clear();

    ++_state.Frame;
}

static void OnSetRenderTarget(ID3D11View* rt)
{
    auto [it, inserted] = _state.FrameRenderTargets.try_emplace(rt, FrameRenderTargetInfo {});
    FrameRenderTargetInfo& info = it->second;

    if (inserted) {
        rt->AddRef();
    }

    info.RenderTargetInfo.Name = D3D11::GetDebugName(rt);

    if (info.RenderTargetInfo.Name.empty()) {
        info.RenderTargetInfo.Name = fmt::format("{}", fmt::ptr(rt));
    }

    DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN;

    if (ID3D11RenderTargetView * rtv; SUCCEEDED(rt->QueryInterface(__uuidof(ID3D11RenderTargetView), (void**)&rtv))) {
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
        rtv->GetDesc(&rtvDesc);
        rtv->Release();

        info.RenderTargetInfo.Format = DXGI::GetDebugName(rtvDesc.Format);
        info.RenderTargetInfo.ViewDimension = D3D11::GetDebugName(rtvDesc.ViewDimension);
        srvFormat = rtvDesc.Format;
    } else if (ID3D11DepthStencilView * dsv; SUCCEEDED(rt->QueryInterface(__uuidof(ID3D11DepthStencilView), (void**)&dsv))) {
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        dsv->GetDesc(&dsvDesc);
        dsv->Release();

        info.RenderTargetInfo.Format = DXGI::GetDebugName(dsvDesc.Format);
        info.RenderTargetInfo.ViewDimension = D3D11::GetDebugName(dsvDesc.ViewDimension);
        srvFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
    }

    ID3D11Resource* res;
    rt->GetResource(&res);

    info.TextureInfo.Name = D3D11::GetDebugName(res);

    if (info.TextureInfo.Name.empty()) {
        info.TextureInfo.Name = fmt::format("{}", fmt::ptr(res));
    }

    ID3D11Texture2D* tex2D = nullptr;

    if (SUCCEEDED(res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&tex2D))) {
        D3D11_TEXTURE2D_DESC desc = {};
        tex2D->GetDesc(&desc);
        tex2D->Release();

        info.TextureInfo.Width = desc.Width;
        info.TextureInfo.Height = desc.Height;
        info.TextureInfo.Depth = desc.ArraySize;
        info.TextureInfo.Format = DXGI::GetDebugName(desc.Format);
    }

    if (!info.Srv && tex2D) {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {
            .Format = srvFormat,
            .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
            .Texture2D = { .MostDetailedMip = 0, .MipLevels = 1 }
        };

        D3D11::GetDevice()->CreateShaderResourceView(res, &srvDesc, &info.Srv);
    }

    res->Release();

    ++info.RefCount;
    ++info.FrameRefCount;
    info.LastSeenFrame = _state.Frame;
}

static bool Initialize()
{
    DXGI::On_Present_After(UpdateInspectorWindow);
    
    D3D11::On_SetRenderTargets([](const std::vector<ID3D11RenderTargetView*>& rtvs, ID3D11DepthStencilView* dsv) {
        for (ID3D11RenderTargetView* rtv : rtvs) {
            if (rtv) {
                OnSetRenderTarget(rtv);
            }
        }

        if (dsv) {
            OnSetRenderTarget(dsv);
        }
    });

    return true;
}

REGISTER_SERVICE(
    .Name = "Inspector",
    .Flags = ServiceFlags::DebugOnly,
    .FnInitialize = Initialize
);
