#include <Services/Platform/D3D11/_D3D11.hpp>
#include <Util.hpp>

using namespace D3D11;
using namespace D3D11::Internal;

class D3D11DeviceWrapper final : public ID3D11Device5 {
public:
    explicit D3D11DeviceWrapper(ID3D11Device* device)
    {
        _device = device;
        _device->QueryInterface(__uuidof(ID3D11Device1), (void**)&_device1);
        _device->QueryInterface(__uuidof(ID3D11Device2), (void**)&_device2);
        _device->QueryInterface(__uuidof(ID3D11Device3), (void**)&_device3);
        _device->QueryInterface(__uuidof(ID3D11Device4), (void**)&_device4);
        _device->QueryInterface(__uuidof(ID3D11Device5), (void**)&_device5);

        if (_device1) {
            const ULONG refs = _device1->Release();
            assert(refs >= 1);
        }

        if (_device2) {
            const ULONG refs = _device2->Release();
            assert(refs >= 1);
        }

        if (_device3) {
            const ULONG refs = _device3->Release();
            assert(refs >= 1);
        }

        if (_device4) {
            const ULONG refs = _device4->Release();
            assert(refs >= 1);
        }

        if (_device5) {
            const ULONG refs = _device5->Release();
            assert(refs >= 1);
        }
    }

    // ID3D11Device5
    virtual HRESULT STDMETHODCALLTYPE OpenSharedFence(HANDLE hFence, REFIID ReturnedInterface, void** ppFence) override;
    virtual HRESULT STDMETHODCALLTYPE CreateFence(UINT64 InitialValue, D3D11_FENCE_FLAG Flags, REFIID ReturnedInterface, void** ppFence) override;

    // ID3D11Device4
    virtual HRESULT STDMETHODCALLTYPE RegisterDeviceRemovedEvent(HANDLE hEvent, DWORD* pdwCookie) override;
    virtual void STDMETHODCALLTYPE UnregisterDeviceRemoved(DWORD dwCookie) override;

    // ID3D11Device3
    virtual HRESULT STDMETHODCALLTYPE CreateTexture2D1(const D3D11_TEXTURE2D_DESC1* pDesc1, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D1** ppTexture2D) override;
    virtual HRESULT STDMETHODCALLTYPE CreateTexture3D1(const D3D11_TEXTURE3D_DESC1* pDesc1, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture3D1** ppTexture3D) override;
    virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState2(const D3D11_RASTERIZER_DESC2* pRasterizerDesc, ID3D11RasterizerState2** ppRasterizerState) override;
    virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView1(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC1* pDesc1, ID3D11ShaderResourceView1** ppSRView1) override;
    virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView1(ID3D11Resource* pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC1* pDesc1, ID3D11UnorderedAccessView1** ppUAView1) override;
    virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView1(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC1* pDesc1, ID3D11RenderTargetView1** ppRTView1) override;
    virtual HRESULT STDMETHODCALLTYPE CreateQuery1(const D3D11_QUERY_DESC1* pQueryDesc1, ID3D11Query1** ppQuery1) override;
    virtual void STDMETHODCALLTYPE GetImmediateContext3(ID3D11DeviceContext3** ppImmediateContext) override;
    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext3(UINT ContextFlags, ID3D11DeviceContext3** ppDeferredContext) override;
    virtual void STDMETHODCALLTYPE WriteToSubresource(ID3D11Resource* pDstResource, UINT DstSubresource, const D3D11_BOX* pDstBox, const void* pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch) override;
    virtual void STDMETHODCALLTYPE ReadFromSubresource(void* pDstData, UINT DstRowPitch, UINT DstDepthPitch, ID3D11Resource* pSrcResource, UINT SrcSubresource, const D3D11_BOX* pSrcBox) override;

    // ID3D11Device2
    virtual void STDMETHODCALLTYPE GetImmediateContext2(ID3D11DeviceContext2** ppImmediateContext) override;
    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext2(UINT ContextFlags, ID3D11DeviceContext2** ppDeferredContext) override;
    virtual void STDMETHODCALLTYPE GetResourceTiling(ID3D11Resource* pTiledResource, UINT* pNumTilesForEntireResource, D3D11_PACKED_MIP_DESC* pPackedMipDesc, D3D11_TILE_SHAPE* pStandardTileShapeForNonPackedMips, UINT* pNumSubresourceTilings, UINT FirstSubresourceTilingToGet, D3D11_SUBRESOURCE_TILING* pSubresourceTilingsForNonPackedMips) override;
    virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels1(DXGI_FORMAT Format, UINT SampleCount, UINT Flags, UINT* pNumQualityLevels) override;

    // ID3D11Device1
    virtual void STDMETHODCALLTYPE GetImmediateContext1(ID3D11DeviceContext1** ppImmediateContext) override;
    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext1(UINT ContextFlags, ID3D11DeviceContext1** ppDeferredContext) override;
    virtual HRESULT STDMETHODCALLTYPE CreateBlendState1(const D3D11_BLEND_DESC1* pBlendStateDesc, ID3D11BlendState1** ppBlendState) override;
    virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState1(const D3D11_RASTERIZER_DESC1* pRasterizerDesc, ID3D11RasterizerState1** ppRasterizerState) override;
    virtual HRESULT STDMETHODCALLTYPE CreateDeviceContextState(UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, REFIID EmulatedInterface, D3D_FEATURE_LEVEL* pChosenFeatureLevel, ID3DDeviceContextState** ppContextState) override;
    virtual HRESULT STDMETHODCALLTYPE OpenSharedResource1(HANDLE hResource, REFIID returnedInterface, void** ppResource) override;
    virtual HRESULT STDMETHODCALLTYPE OpenSharedResourceByName(LPCWSTR lpName, DWORD dwDesiredAccess, REFIID returnedInterface, void** ppResource) override;

    // ID3D11Device
    virtual HRESULT STDMETHODCALLTYPE CreateBuffer(const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer) override;
    virtual HRESULT STDMETHODCALLTYPE CreateTexture1D(const D3D11_TEXTURE1D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture1D** ppTexture1D) override;
    virtual HRESULT STDMETHODCALLTYPE CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D) override;
    virtual HRESULT STDMETHODCALLTYPE CreateTexture3D(const D3D11_TEXTURE3D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture3D** ppTexture3D) override;
    virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppSRView) override;
    virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView(ID3D11Resource* pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc, ID3D11UnorderedAccessView** ppUAView) override;
    virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTView) override;
    virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilView(ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc, ID3D11DepthStencilView** ppDepthStencilView) override;
    virtual HRESULT STDMETHODCALLTYPE CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout** ppInputLayout) override;
    virtual HRESULT STDMETHODCALLTYPE CreateVertexShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11VertexShader** ppVertexShader) override;
    virtual HRESULT STDMETHODCALLTYPE CreateGeometryShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11GeometryShader** ppGeometryShader) override;
    virtual HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput(const void* pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY* pSODeclaration, UINT NumEntries, const UINT* pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage* pClassLinkage, ID3D11GeometryShader** ppGeometryShader) override;
    virtual HRESULT STDMETHODCALLTYPE CreatePixelShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11PixelShader** ppPixelShader) override;
    virtual HRESULT STDMETHODCALLTYPE CreateHullShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11HullShader** ppHullShader) override;
    virtual HRESULT STDMETHODCALLTYPE CreateDomainShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11DomainShader** ppDomainShader) override;
    virtual HRESULT STDMETHODCALLTYPE CreateComputeShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11ComputeShader** ppComputeShader) override;
    virtual HRESULT STDMETHODCALLTYPE CreateClassLinkage(ID3D11ClassLinkage** ppLinkage) override;
    virtual HRESULT STDMETHODCALLTYPE CreateBlendState(const D3D11_BLEND_DESC* pBlendStateDesc, ID3D11BlendState** ppBlendState) override;
    virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* pDepthStencilDesc, ID3D11DepthStencilState** ppDepthStencilState) override;
    virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState(const D3D11_RASTERIZER_DESC* pRasterizerDesc, ID3D11RasterizerState** ppRasterizerState) override;
    virtual HRESULT STDMETHODCALLTYPE CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc, ID3D11SamplerState** ppSamplerState) override;
    virtual HRESULT STDMETHODCALLTYPE CreateQuery(const D3D11_QUERY_DESC* pQueryDesc, ID3D11Query** ppQuery) override;
    virtual HRESULT STDMETHODCALLTYPE CreatePredicate(const D3D11_QUERY_DESC* pPredicateDesc, ID3D11Predicate** ppPredicate) override;
    virtual HRESULT STDMETHODCALLTYPE CreateCounter(const D3D11_COUNTER_DESC* pCounterDesc, ID3D11Counter** ppCounter) override;
    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext(UINT ContextFlags, ID3D11DeviceContext** ppDeferredContext) override;
    virtual HRESULT STDMETHODCALLTYPE OpenSharedResource(HANDLE hResource, REFIID ReturnedInterface, void** ppResource) override;
    virtual HRESULT STDMETHODCALLTYPE CheckFormatSupport(DXGI_FORMAT Format, UINT* pFormatSupport) override;
    virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels(DXGI_FORMAT Format, UINT SampleCount, UINT* pNumQualityLevels) override;
    virtual void STDMETHODCALLTYPE CheckCounterInfo(D3D11_COUNTER_INFO* pCounterInfo) override;
    virtual HRESULT STDMETHODCALLTYPE CheckCounter(const D3D11_COUNTER_DESC* pDesc, D3D11_COUNTER_TYPE* pType, UINT* pActiveCounters, LPSTR szName, UINT* pNameLength, LPSTR szUnits, UINT* pUnitsLength, LPSTR szDescription, UINT* pDescriptionLength) override;
    virtual HRESULT STDMETHODCALLTYPE CheckFeatureSupport(D3D11_FEATURE Feature, void* pFeatureSupportData, UINT FeatureSupportDataSize) override;
    virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID guid, UINT* pDataSize, void* pData) override;
    virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID guid, UINT DataSize, const void* pData) override;
    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID guid, const IUnknown* pData) override;
    virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel() override;
    virtual UINT STDMETHODCALLTYPE GetCreationFlags() override;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason() override;
    virtual void STDMETHODCALLTYPE GetImmediateContext(ID3D11DeviceContext** ppImmediateContext) override;
    virtual HRESULT STDMETHODCALLTYPE SetExceptionMode(UINT RaiseFlags) override;
    virtual UINT STDMETHODCALLTYPE GetExceptionMode() override;

    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppvObject) override;
    virtual ULONG STDMETHODCALLTYPE AddRef() override;
    virtual ULONG STDMETHODCALLTYPE Release() override;

private:
    ID3D11Device* _device;
    ID3D11Device1* _device1;
    ID3D11Device2* _device2;
    ID3D11Device3* _device3;
    ID3D11Device4* _device4;
    ID3D11Device5* _device5;
};

ID3D11Device* D3D11::Internal::Wrap(ID3D11Device* device) {
    return new D3D11DeviceWrapper(device);
}

HRESULT D3D11DeviceWrapper::OpenSharedFence(HANDLE hFence, REFIID ReturnedInterface, void** ppFence)
{
    return _device5->OpenSharedFence(hFence, ReturnedInterface, ppFence);
}

HRESULT D3D11DeviceWrapper::CreateFence(UINT64 InitialValue, D3D11_FENCE_FLAG Flags, REFIID ReturnedInterface, void** ppFence)
{
    return _device5->CreateFence(InitialValue, Flags, ReturnedInterface, ppFence);
}

HRESULT D3D11DeviceWrapper::RegisterDeviceRemovedEvent(HANDLE hEvent, DWORD* pdwCookie)
{
    return _device4->RegisterDeviceRemovedEvent(hEvent, pdwCookie);
}

void D3D11DeviceWrapper::UnregisterDeviceRemoved(DWORD dwCookie)
{
    return _device4->UnregisterDeviceRemoved(dwCookie);
}

HRESULT D3D11DeviceWrapper::CreateTexture2D1(const D3D11_TEXTURE2D_DESC1* pDesc1, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D1** ppTexture2D)
{
    return _device3->CreateTexture2D1(pDesc1, pInitialData, ppTexture2D);
}

HRESULT D3D11DeviceWrapper::CreateTexture3D1(const D3D11_TEXTURE3D_DESC1* pDesc1, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture3D1** ppTexture3D)
{
    return _device3->CreateTexture3D1(pDesc1, pInitialData, ppTexture3D);
}

HRESULT D3D11DeviceWrapper::CreateRasterizerState2(const D3D11_RASTERIZER_DESC2* pRasterizerDesc, ID3D11RasterizerState2** ppRasterizerState)
{
    return _device3->CreateRasterizerState2(pRasterizerDesc, ppRasterizerState);
}

HRESULT D3D11DeviceWrapper::CreateShaderResourceView1(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC1* pDesc1, ID3D11ShaderResourceView1** ppSRView1)
{
    return _device3->CreateShaderResourceView1(pResource, pDesc1, ppSRView1);
}

HRESULT D3D11DeviceWrapper::CreateUnorderedAccessView1(ID3D11Resource* pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC1* pDesc1, ID3D11UnorderedAccessView1** ppUAView1)
{
    return _device3->CreateUnorderedAccessView1(pResource, pDesc1, ppUAView1);
}

HRESULT D3D11DeviceWrapper::CreateRenderTargetView1(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC1* pDesc1, ID3D11RenderTargetView1** ppRTView1)
{
    return _device3->CreateRenderTargetView1(pResource, pDesc1, ppRTView1);
}

HRESULT D3D11DeviceWrapper::CreateQuery1(const D3D11_QUERY_DESC1* pQueryDesc1, ID3D11Query1** ppQuery1)
{
    return _device3->CreateQuery1(pQueryDesc1, ppQuery1);
}

void D3D11DeviceWrapper::GetImmediateContext3(ID3D11DeviceContext3** ppImmediateContext)
{
    _device3->GetImmediateContext3(ppImmediateContext);

    if (*ppImmediateContext && !_state.DeviceContext) {
        *ppImmediateContext = (ID3D11DeviceContext3*)D3D11::Internal::Wrap(*ppImmediateContext);
        _state.DeviceContext = *ppImmediateContext;
    }
}

HRESULT D3D11DeviceWrapper::CreateDeferredContext3(UINT ContextFlags, ID3D11DeviceContext3** ppDeferredContext)
{
    return _device3->CreateDeferredContext3(ContextFlags, ppDeferredContext);
}

void D3D11DeviceWrapper::WriteToSubresource(ID3D11Resource* pDstResource, UINT DstSubresource, const D3D11_BOX* pDstBox, const void* pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch)
{
    return _device3->WriteToSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
}

void D3D11DeviceWrapper::ReadFromSubresource(void* pDstData, UINT DstRowPitch, UINT DstDepthPitch, ID3D11Resource* pSrcResource, UINT SrcSubresource, const D3D11_BOX* pSrcBox)
{
    return _device3->ReadFromSubresource(pDstData, DstRowPitch, DstDepthPitch, pSrcResource, SrcSubresource, pSrcBox);
}

void D3D11DeviceWrapper::GetImmediateContext2(ID3D11DeviceContext2** ppImmediateContext)
{
    _device2->GetImmediateContext2(ppImmediateContext);

    if (*ppImmediateContext && !_state.DeviceContext) {
        *ppImmediateContext = (ID3D11DeviceContext3*)D3D11::Internal::Wrap(*ppImmediateContext);
        _state.DeviceContext = *ppImmediateContext;
    }
}

HRESULT D3D11DeviceWrapper::CreateDeferredContext2(UINT ContextFlags, ID3D11DeviceContext2** ppDeferredContext)
{
    return _device2->CreateDeferredContext2(ContextFlags, ppDeferredContext);
}

void D3D11DeviceWrapper::GetResourceTiling(ID3D11Resource* pTiledResource, UINT* pNumTilesForEntireResource, D3D11_PACKED_MIP_DESC* pPackedMipDesc, D3D11_TILE_SHAPE* pStandardTileShapeForNonPackedMips, UINT* pNumSubresourceTilings, UINT FirstSubresourceTilingToGet, D3D11_SUBRESOURCE_TILING* pSubresourceTilingsForNonPackedMips)
{
    return _device2->GetResourceTiling(pTiledResource, pNumTilesForEntireResource, pPackedMipDesc, pStandardTileShapeForNonPackedMips, pNumSubresourceTilings, FirstSubresourceTilingToGet, pSubresourceTilingsForNonPackedMips);
}

HRESULT D3D11DeviceWrapper::CheckMultisampleQualityLevels1(DXGI_FORMAT Format, UINT SampleCount, UINT Flags, UINT* pNumQualityLevels)
{
    return _device2->CheckMultisampleQualityLevels1(Format, SampleCount, Flags, pNumQualityLevels);
}

void D3D11DeviceWrapper::GetImmediateContext1(ID3D11DeviceContext1** ppImmediateContext)
{
    _device1->GetImmediateContext1(ppImmediateContext);

    if (*ppImmediateContext && !_state.DeviceContext) {
        *ppImmediateContext = (ID3D11DeviceContext3*)D3D11::Internal::Wrap(*ppImmediateContext);
        _state.DeviceContext = *ppImmediateContext;
    }
}

HRESULT D3D11DeviceWrapper::CreateDeferredContext1(UINT ContextFlags, ID3D11DeviceContext1** ppDeferredContext)
{
    return _device1->CreateDeferredContext1(ContextFlags, ppDeferredContext);
}

HRESULT D3D11DeviceWrapper::CreateBlendState1(const D3D11_BLEND_DESC1* pBlendStateDesc, ID3D11BlendState1** ppBlendState)
{
    return _device1->CreateBlendState1(pBlendStateDesc, ppBlendState);
}

HRESULT D3D11DeviceWrapper::CreateRasterizerState1(const D3D11_RASTERIZER_DESC1* pRasterizerDesc, ID3D11RasterizerState1** ppRasterizerState)
{
    return _device1->CreateRasterizerState1(pRasterizerDesc, ppRasterizerState);
}

HRESULT D3D11DeviceWrapper::CreateDeviceContextState(UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, REFIID EmulatedInterface, D3D_FEATURE_LEVEL* pChosenFeatureLevel, ID3DDeviceContextState** ppContextState)
{
    return _device1->CreateDeviceContextState(Flags, pFeatureLevels, FeatureLevels, SDKVersion, EmulatedInterface, pChosenFeatureLevel, ppContextState);
}

HRESULT D3D11DeviceWrapper::OpenSharedResource1(HANDLE hResource, REFIID returnedInterface, void** ppResource)
{
    return _device1->OpenSharedResource1(hResource, returnedInterface, ppResource);
}

HRESULT D3D11DeviceWrapper::OpenSharedResourceByName(LPCWSTR lpName, DWORD dwDesiredAccess, REFIID returnedInterface, void** ppResource)
{
    return _device1->OpenSharedResourceByName(lpName, dwDesiredAccess, returnedInterface, ppResource);
}

HRESULT D3D11DeviceWrapper::CreateBuffer(const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer)
{
    return _device->CreateBuffer(pDesc, pInitialData, ppBuffer);
}

HRESULT D3D11DeviceWrapper::CreateTexture1D(const D3D11_TEXTURE1D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture1D** ppTexture1D)
{
    return _device->CreateTexture1D(pDesc, pInitialData, ppTexture1D);
}

HRESULT D3D11DeviceWrapper::CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D)
{
    return _device->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
}

HRESULT D3D11DeviceWrapper::CreateTexture3D(const D3D11_TEXTURE3D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture3D** ppTexture3D)
{
    return _device->CreateTexture3D(pDesc, pInitialData, ppTexture3D);
}

HRESULT D3D11DeviceWrapper::CreateShaderResourceView(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppSRView)
{
    return _device->CreateShaderResourceView(pResource, pDesc, ppSRView);
}

HRESULT D3D11DeviceWrapper::CreateUnorderedAccessView(ID3D11Resource* pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc, ID3D11UnorderedAccessView** ppUAView)
{
    return _device->CreateUnorderedAccessView(pResource, pDesc, ppUAView);
}

HRESULT D3D11DeviceWrapper::CreateRenderTargetView(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTView)
{
    return _device->CreateRenderTargetView(pResource, pDesc, ppRTView);
}

HRESULT D3D11DeviceWrapper::CreateDepthStencilView(ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc, ID3D11DepthStencilView** ppDepthStencilView)
{
    return _device->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
}

HRESULT D3D11DeviceWrapper::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout** ppInputLayout)
{
    return _device->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
}

HRESULT D3D11DeviceWrapper::CreateVertexShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11VertexShader** ppVertexShader)
{
    return _device->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
}

HRESULT D3D11DeviceWrapper::CreateGeometryShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11GeometryShader** ppGeometryShader)
{
    return _device->CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);
}

HRESULT D3D11DeviceWrapper::CreateGeometryShaderWithStreamOutput(const void* pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY* pSODeclaration, UINT NumEntries, const UINT* pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage* pClassLinkage, ID3D11GeometryShader** ppGeometryShader)
{
    return _device->CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);
}

HRESULT D3D11DeviceWrapper::CreatePixelShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11PixelShader** ppPixelShader)
{
    return _device->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
}

HRESULT D3D11DeviceWrapper::CreateHullShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11HullShader** ppHullShader)
{
    return _device->CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);
}

HRESULT D3D11DeviceWrapper::CreateDomainShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11DomainShader** ppDomainShader)
{
    return _device->CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);
}

HRESULT D3D11DeviceWrapper::CreateComputeShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11ComputeShader** ppComputeShader)
{
    return _device->CreateComputeShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);
}

HRESULT D3D11DeviceWrapper::CreateClassLinkage(ID3D11ClassLinkage** ppLinkage)
{
    return _device->CreateClassLinkage(ppLinkage);
}

HRESULT D3D11DeviceWrapper::CreateBlendState(const D3D11_BLEND_DESC* pBlendStateDesc, ID3D11BlendState** ppBlendState)
{
    return _device->CreateBlendState(pBlendStateDesc, ppBlendState);
}

HRESULT D3D11DeviceWrapper::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* pDepthStencilDesc, ID3D11DepthStencilState** ppDepthStencilState)
{
    return _device->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
}

HRESULT D3D11DeviceWrapper::CreateRasterizerState(const D3D11_RASTERIZER_DESC* pRasterizerDesc, ID3D11RasterizerState** ppRasterizerState)
{
    return _device->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
}

HRESULT D3D11DeviceWrapper::CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc, ID3D11SamplerState** ppSamplerState)
{
    return _device->CreateSamplerState(pSamplerDesc, ppSamplerState);
}

HRESULT D3D11DeviceWrapper::CreateQuery(const D3D11_QUERY_DESC* pQueryDesc, ID3D11Query** ppQuery)
{
    return _device->CreateQuery(pQueryDesc, ppQuery);
}

HRESULT D3D11DeviceWrapper::CreatePredicate(const D3D11_QUERY_DESC* pPredicateDesc, ID3D11Predicate** ppPredicate)
{
    return _device->CreatePredicate(pPredicateDesc, ppPredicate);
}

HRESULT D3D11DeviceWrapper::CreateCounter(const D3D11_COUNTER_DESC* pCounterDesc, ID3D11Counter** ppCounter)
{
    return _device->CreateCounter(pCounterDesc, ppCounter);
}

HRESULT D3D11DeviceWrapper::CreateDeferredContext(UINT ContextFlags, ID3D11DeviceContext** ppDeferredContext)
{
    return _device->CreateDeferredContext(ContextFlags, ppDeferredContext);
}

HRESULT D3D11DeviceWrapper::OpenSharedResource(HANDLE hResource, REFIID ReturnedInterface, void** ppResource)
{
    return _device->OpenSharedResource(hResource, ReturnedInterface, ppResource);
}

HRESULT D3D11DeviceWrapper::CheckFormatSupport(DXGI_FORMAT Format, UINT* pFormatSupport)
{
    return _device->CheckFormatSupport(Format, pFormatSupport);
}

HRESULT D3D11DeviceWrapper::CheckMultisampleQualityLevels(DXGI_FORMAT Format, UINT SampleCount, UINT* pNumQualityLevels)
{
    return _device->CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
}

void D3D11DeviceWrapper::CheckCounterInfo(D3D11_COUNTER_INFO* pCounterInfo)
{
    return _device->CheckCounterInfo(pCounterInfo);
}

HRESULT D3D11DeviceWrapper::CheckCounter(const D3D11_COUNTER_DESC* pDesc, D3D11_COUNTER_TYPE* pType, UINT* pActiveCounters, LPSTR szName, UINT* pNameLength, LPSTR szUnits, UINT* pUnitsLength, LPSTR szDescription, UINT* pDescriptionLength)
{
    return _device->CheckCounter(pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);
}

HRESULT D3D11DeviceWrapper::CheckFeatureSupport(D3D11_FEATURE Feature, void* pFeatureSupportData, UINT FeatureSupportDataSize)
{
    return _device->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
}

HRESULT D3D11DeviceWrapper::GetPrivateData(REFGUID guid, UINT* pDataSize, void* pData)
{
    return _device->GetPrivateData(guid, pDataSize, pData);
}

HRESULT D3D11DeviceWrapper::SetPrivateData(REFGUID guid, UINT DataSize, const void* pData)
{
    return _device->SetPrivateData(guid, DataSize, pData);
}

HRESULT D3D11DeviceWrapper::SetPrivateDataInterface(REFGUID guid, const IUnknown* pData)
{
    return _device->SetPrivateDataInterface(guid, pData);
}

D3D_FEATURE_LEVEL D3D11DeviceWrapper::GetFeatureLevel()
{
    return _device->GetFeatureLevel();
}

UINT D3D11DeviceWrapper::GetCreationFlags()
{
    return _device->GetCreationFlags();
}

HRESULT D3D11DeviceWrapper::GetDeviceRemovedReason()
{
    return _device->GetDeviceRemovedReason();
}

void D3D11DeviceWrapper::GetImmediateContext(ID3D11DeviceContext** ppImmediateContext)
{
    _device->GetImmediateContext(ppImmediateContext);

    if (*ppImmediateContext && !_state.DeviceContext) {
        *ppImmediateContext = (ID3D11DeviceContext3*)D3D11::Internal::Wrap(*ppImmediateContext);
        _state.DeviceContext = *ppImmediateContext;
    }
}

HRESULT D3D11DeviceWrapper::SetExceptionMode(UINT RaiseFlags)
{
    return _device->SetExceptionMode(RaiseFlags);
}

UINT D3D11DeviceWrapper::GetExceptionMode()
{
    return _device->GetExceptionMode();
}

HRESULT D3D11DeviceWrapper::QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppvObject)
{
    if (riid == __uuidof(IUnknown) ||
        riid == __uuidof(ID3D11Device) && _device ||
        riid == __uuidof(ID3D11Device1) && _device1 ||
        riid == __uuidof(ID3D11Device2) && _device2 ||
        riid == __uuidof(ID3D11Device3) && _device3 ||
        riid == __uuidof(ID3D11Device4) && _device4 ||
        riid == __uuidof(ID3D11Device5) && _device5)
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }

    static const IID _uuid_d3d12device = IIDFromString("{189819f1-1db6-4b57-be54-1821339b85f7}");

    if (riid != __uuidof(IDXGIDevice) && 
        riid != __uuidof(IDXGIDevice1) &&
        riid != __uuidof(IDXGIDevice2) &&
        riid != __uuidof(IDXGIDevice3) &&
        riid != __uuidof(IDXGIDevice4) &&
        riid != __uuidof(ID3D11Multithread) &&
        riid != _uuid_d3d12device)
    {
        LOG_WARNING("D3D11DeviceWrapper::QueryInterface unknown: {}\n", ToString(riid));
    }

    return _device->QueryInterface(riid, ppvObject);
}

ULONG D3D11DeviceWrapper::AddRef()
{
    return _device->AddRef();
}

ULONG D3D11DeviceWrapper::Release()
{
    const ULONG refCount = _device->Release();
    if (refCount == 0) {
        delete this;
    }
    return refCount;
}
