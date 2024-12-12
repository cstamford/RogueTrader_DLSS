#include <Services/Platform/D3D11/_D3D11.hpp>
#include <Util.hpp>

using namespace D3D11;
using namespace D3D11::Internal;

class D3D11DeviceContextWrapper final : public ID3D11DeviceContext4 {
public:
    explicit D3D11DeviceContextWrapper(ID3D11DeviceContext* device)
    {
        _deviceContext = device;
        _deviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&_deviceContext1);
        _deviceContext->QueryInterface(__uuidof(ID3D11DeviceContext2), (void**)&_deviceContext2);
        _deviceContext->QueryInterface(__uuidof(ID3D11DeviceContext3), (void**)&_deviceContext3);
        _deviceContext->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)&_deviceContext4);

        if (_deviceContext1) {
            const ULONG refs = _deviceContext1->Release();
            assert(refs >= 1);
        }

        if (_deviceContext2) {
            const ULONG refs = _deviceContext2->Release();
            assert(refs >= 1);
        }

        if (_deviceContext3) {
            const ULONG refs = _deviceContext3->Release();
            assert(refs >= 1);
        }

        if (_deviceContext4) {
            const ULONG refs = _deviceContext4->Release();
            assert(refs >= 1);
        }
    }

    // ID3D11DeviceContext4
    virtual HRESULT STDMETHODCALLTYPE Signal(ID3D11Fence* pFence, UINT64 Value) override;
    virtual HRESULT STDMETHODCALLTYPE Wait(ID3D11Fence* pFence, UINT64 Value) override;

    // ID3D11DeviceContext3
    virtual void STDMETHODCALLTYPE Flush1(D3D11_CONTEXT_TYPE ContextType, HANDLE hEvent) override;
    virtual void STDMETHODCALLTYPE SetHardwareProtectionState(BOOL HwProtectionEnable) override;
    virtual void STDMETHODCALLTYPE GetHardwareProtectionState(BOOL* pHwProtectionEnable) override;

    // ID3D11DeviceContext2
    virtual HRESULT STDMETHODCALLTYPE UpdateTileMappings(ID3D11Resource* pTiledResource, UINT NumTiledResourceRegions, const D3D11_TILED_RESOURCE_COORDINATE* pTiledResourceRegionStartCoordinates, const D3D11_TILE_REGION_SIZE* pTiledResourceRegionSizes, ID3D11Buffer* pTilePool, UINT NumRanges, const UINT* pRangeFlags, const UINT* pTilePoolStartOffsets, const UINT* pRangeTileCounts, UINT Flags) override;
    virtual HRESULT STDMETHODCALLTYPE CopyTileMappings(ID3D11Resource* pDestTiledResource, const D3D11_TILED_RESOURCE_COORDINATE* pDestRegionStartCoordinate, ID3D11Resource* pSourceTiledResource, const D3D11_TILED_RESOURCE_COORDINATE* pSourceRegionStartCoordinate, const D3D11_TILE_REGION_SIZE* pTileRegionSize, UINT Flags) override;
    virtual void STDMETHODCALLTYPE CopyTiles(ID3D11Resource* pTiledResource, const D3D11_TILED_RESOURCE_COORDINATE* pTileRegionStartCoordinate, const D3D11_TILE_REGION_SIZE* pTileRegionSize, ID3D11Buffer* pBuffer, UINT64 BufferStartOffsetInBytes, UINT Flags) override;
    virtual void STDMETHODCALLTYPE UpdateTiles(ID3D11Resource* pDestTiledResource, const D3D11_TILED_RESOURCE_COORDINATE* pDestTileRegionStartCoordinate, const D3D11_TILE_REGION_SIZE* pDestTileRegionSize, const void* pSourceTileData, UINT Flags) override;
    virtual HRESULT STDMETHODCALLTYPE ResizeTilePool(ID3D11Buffer* pTilePool, UINT64 NewSizeInBytes) override;
    virtual void STDMETHODCALLTYPE TiledResourceBarrier(ID3D11DeviceChild* pTiledResourceOrViewAccessBeforeBarrier, ID3D11DeviceChild* pTiledResourceOrViewAccessAfterBarrier) override;
    virtual BOOL STDMETHODCALLTYPE IsAnnotationEnabled() override;
    virtual void STDMETHODCALLTYPE SetMarkerInt(LPCWSTR pLabel, INT Data) override;
    virtual void STDMETHODCALLTYPE BeginEventInt(LPCWSTR pLabel, INT Data) override;
    virtual void STDMETHODCALLTYPE EndEvent() override;

    // ID3D11DeviceContext1
    virtual void STDMETHODCALLTYPE CopySubresourceRegion1(ID3D11Resource* pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource* pSrcResource, UINT SrcSubresource, const D3D11_BOX* pSrcBox, UINT CopyFlags) override;
    virtual void STDMETHODCALLTYPE UpdateSubresource1(ID3D11Resource* pDstResource, UINT DstSubresource, const D3D11_BOX* pDstBox, const void* pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch, UINT CopyFlags) override;
    virtual void STDMETHODCALLTYPE DiscardResource(ID3D11Resource* pResource) override;
    virtual void STDMETHODCALLTYPE DiscardView(ID3D11View* pResourceView) override;
    virtual void STDMETHODCALLTYPE VSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE HSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE DSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE GSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE PSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE CSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE VSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE HSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE DSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE GSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE PSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE CSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants) override;
    virtual void STDMETHODCALLTYPE SwapDeviceContextState(ID3DDeviceContextState *pState, ID3DDeviceContextState **ppPreviousState) override;
    virtual void STDMETHODCALLTYPE ClearView(ID3D11View* pView, const FLOAT Color[4], const D3D11_RECT* pRect, UINT NumRects) override;
    virtual void STDMETHODCALLTYPE DiscardView1(ID3D11View* pResourceView, const D3D11_RECT* pRects, UINT NumRects) override;


    // ID3D11DeviceContext
    virtual void STDMETHODCALLTYPE VSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE PSSetShader(ID3D11PixelShader* pPixelShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override;
    virtual void STDMETHODCALLTYPE PSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override;
    virtual void STDMETHODCALLTYPE VSSetShader(ID3D11VertexShader* pVertexShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override;
    virtual void STDMETHODCALLTYPE DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation) override;
    virtual void STDMETHODCALLTYPE Draw(UINT VertexCount, UINT StartVertexLocation) override;
    virtual HRESULT STDMETHODCALLTYPE Map(ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource) override;
    virtual void STDMETHODCALLTYPE Unmap(ID3D11Resource* pResource, UINT Subresource) override;
    virtual void STDMETHODCALLTYPE PSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE IASetInputLayout(ID3D11InputLayout* pInputLayout) override;
    virtual void STDMETHODCALLTYPE IASetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets) override;
    virtual void STDMETHODCALLTYPE IASetIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset) override;
    virtual void STDMETHODCALLTYPE DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) override;
    virtual void STDMETHODCALLTYPE DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) override;
    virtual void STDMETHODCALLTYPE GSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE GSSetShader(ID3D11GeometryShader* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override;
    virtual void STDMETHODCALLTYPE IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology) override;
    virtual void STDMETHODCALLTYPE VSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE VSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override;
    virtual void STDMETHODCALLTYPE Begin(ID3D11Asynchronous* pAsync) override;
    virtual void STDMETHODCALLTYPE End(ID3D11Asynchronous* pAsync) override;
    virtual HRESULT STDMETHODCALLTYPE GetData(ID3D11Asynchronous* pAsync, void* pData, UINT DataSize, UINT GetDataFlags) override;
    virtual void STDMETHODCALLTYPE SetPredication(ID3D11Predicate* pPredicate, BOOL PredicateValue) override;
    virtual void STDMETHODCALLTYPE GSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE GSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override;
    virtual void STDMETHODCALLTYPE OMSetRenderTargets(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView) override;
    virtual void STDMETHODCALLTYPE OMSetRenderTargetsAndUnorderedAccessViews(UINT NumRTVs, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts) override;
    virtual void STDMETHODCALLTYPE OMSetBlendState(ID3D11BlendState* pBlendState, const FLOAT BlendFactor[4], UINT SampleMask) override;
    virtual void STDMETHODCALLTYPE OMSetDepthStencilState(ID3D11DepthStencilState* pDepthStencilState, UINT StencilRef) override;
    virtual void STDMETHODCALLTYPE SOSetTargets(UINT NumBuffers, ID3D11Buffer* const* ppSOTargets, const UINT* pOffsets) override;
    virtual void STDMETHODCALLTYPE DrawAuto() override;
    virtual void STDMETHODCALLTYPE DrawIndexedInstancedIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs) override;
    virtual void STDMETHODCALLTYPE DrawInstancedIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs) override;
    virtual void STDMETHODCALLTYPE Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ) override;
    virtual void STDMETHODCALLTYPE DispatchIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs) override;
    virtual void STDMETHODCALLTYPE RSSetState(ID3D11RasterizerState* pRasterizerState) override;
    virtual void STDMETHODCALLTYPE RSSetViewports(UINT NumViewports, const D3D11_VIEWPORT* pViewports) override;
    virtual void STDMETHODCALLTYPE RSSetScissorRects(UINT NumRects, const D3D11_RECT* pRects) override;
    virtual void STDMETHODCALLTYPE CopySubresourceRegion(ID3D11Resource* pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource* pSrcResource, UINT SrcSubresource, const D3D11_BOX* pSrcBox) override;
    virtual void STDMETHODCALLTYPE CopyResource(ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource) override;
    virtual void STDMETHODCALLTYPE UpdateSubresource(ID3D11Resource* pDstResource, UINT DstSubresource, const D3D11_BOX* pDstBox, const void* pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch) override;
    virtual void STDMETHODCALLTYPE CopyStructureCount(ID3D11Buffer* pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView* pSrcView) override;
    virtual void STDMETHODCALLTYPE ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4]) override;
    virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView* pUnorderedAccessView, const UINT Values[4]) override;
    virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView* pUnorderedAccessView, const FLOAT Values[4]) override;
    virtual void STDMETHODCALLTYPE ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil) override;
    virtual void STDMETHODCALLTYPE GenerateMips(ID3D11ShaderResourceView* pShaderResourceView) override;
    virtual void STDMETHODCALLTYPE SetResourceMinLOD(ID3D11Resource* pResource, FLOAT MinLOD) override;
    virtual FLOAT STDMETHODCALLTYPE GetResourceMinLOD(ID3D11Resource* pResource) override;
    virtual void STDMETHODCALLTYPE ResolveSubresource(ID3D11Resource* pDstResource, UINT DstSubresource, ID3D11Resource* pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format) override;
    virtual void STDMETHODCALLTYPE ExecuteCommandList(ID3D11CommandList* pCommandList, BOOL RestoreContextState) override;
    virtual void STDMETHODCALLTYPE HSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE HSSetShader(ID3D11HullShader* pHullShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override;
    virtual void STDMETHODCALLTYPE HSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override;
    virtual void STDMETHODCALLTYPE HSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE DSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE DSSetShader(ID3D11DomainShader* pDomainShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override;
    virtual void STDMETHODCALLTYPE DSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override;
    virtual void STDMETHODCALLTYPE DSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE CSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE CSSetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts) override;
    virtual void STDMETHODCALLTYPE CSSetShader(ID3D11ComputeShader* pComputeShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) override;
    virtual void STDMETHODCALLTYPE CSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) override;
    virtual void STDMETHODCALLTYPE CSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE VSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE PSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE PSGetShader(ID3D11PixelShader** ppPixelShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
    virtual void STDMETHODCALLTYPE PSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
    virtual void STDMETHODCALLTYPE VSGetShader(ID3D11VertexShader** ppVertexShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
    virtual void STDMETHODCALLTYPE PSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE IAGetInputLayout(ID3D11InputLayout** ppInputLayout) override;
    virtual void STDMETHODCALLTYPE IAGetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppVertexBuffers, UINT* pStrides, UINT* pOffsets) override;
    virtual void STDMETHODCALLTYPE IAGetIndexBuffer(ID3D11Buffer** pIndexBuffer, DXGI_FORMAT* Format, UINT* Offset) override;
    virtual void STDMETHODCALLTYPE GSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE GSGetShader(ID3D11GeometryShader** ppGeometryShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
    virtual void STDMETHODCALLTYPE IAGetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY* pTopology) override;
    virtual void STDMETHODCALLTYPE VSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE VSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
    virtual void STDMETHODCALLTYPE GetPredication(ID3D11Predicate** ppPredicate, BOOL* pPredicateValue) override;
    virtual void STDMETHODCALLTYPE GSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE GSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
    virtual void STDMETHODCALLTYPE OMGetRenderTargets(UINT NumViews, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView) override;
    virtual void STDMETHODCALLTYPE OMGetRenderTargetsAndUnorderedAccessViews(UINT NumRTVs, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews) override;
    virtual void STDMETHODCALLTYPE OMGetBlendState(ID3D11BlendState** ppBlendState, FLOAT BlendFactor[4], UINT* pSampleMask) override;
    virtual void STDMETHODCALLTYPE OMGetDepthStencilState(ID3D11DepthStencilState** ppDepthStencilState, UINT* pStencilRef) override;
    virtual void STDMETHODCALLTYPE SOGetTargets(UINT NumBuffers, ID3D11Buffer** ppSOTargets) override;
    virtual void STDMETHODCALLTYPE RSGetState(ID3D11RasterizerState** ppRasterizerState) override;
    virtual void STDMETHODCALLTYPE RSGetViewports(UINT* NumViewports, D3D11_VIEWPORT* pViewports) override;
    virtual void STDMETHODCALLTYPE RSGetScissorRects(UINT* NumRects, D3D11_RECT* pRects) override;
    virtual void STDMETHODCALLTYPE HSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE HSGetShader(ID3D11HullShader** ppHullShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
    virtual void STDMETHODCALLTYPE HSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
    virtual void STDMETHODCALLTYPE HSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE DSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE DSGetShader(ID3D11DomainShader** ppDomainShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
    virtual void STDMETHODCALLTYPE DSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
    virtual void STDMETHODCALLTYPE DSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE CSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) override;
    virtual void STDMETHODCALLTYPE CSGetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews) override;
    virtual void STDMETHODCALLTYPE CSGetShader(ID3D11ComputeShader** ppComputeShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances) override;
    virtual void STDMETHODCALLTYPE CSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers) override;
    virtual void STDMETHODCALLTYPE CSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers) override;
    virtual void STDMETHODCALLTYPE ClearState() override;
    virtual void STDMETHODCALLTYPE Flush() override;
    virtual D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE GetType() override;
    virtual UINT STDMETHODCALLTYPE GetContextFlags() override;
    virtual HRESULT STDMETHODCALLTYPE FinishCommandList(BOOL RestoreDeferredContextState, ID3D11CommandList** ppCommandList) override;

    // ID3D11DeviceChild
    virtual void STDMETHODCALLTYPE GetDevice(ID3D11Device** ppDevice) override;
    virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID guid, UINT* pDataSize, void* pData) override;
    virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID guid, UINT DataSize, const void* pData) override;
    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID guid, const IUnknown* pData) override;

    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppvObject) override;
    virtual ULONG STDMETHODCALLTYPE AddRef() override;
    virtual ULONG STDMETHODCALLTYPE Release() override;

private:
    ID3D11DeviceContext* _deviceContext;
    ID3D11DeviceContext1* _deviceContext1;
    ID3D11DeviceContext2* _deviceContext2;
    ID3D11DeviceContext3* _deviceContext3;
    ID3D11DeviceContext4* _deviceContext4;
};

ID3D11DeviceContext* D3D11::Internal::Wrap(ID3D11DeviceContext* deviceContext)
{
    return new D3D11DeviceContextWrapper(deviceContext);
}

HRESULT D3D11DeviceContextWrapper::Signal(ID3D11Fence* pFence, UINT64 Value)
{
    return _deviceContext4->Signal(pFence, Value);
}

HRESULT D3D11DeviceContextWrapper::Wait(ID3D11Fence* pFence, UINT64 Value)
{
    return _deviceContext4->Wait(pFence, Value);
}

void D3D11DeviceContextWrapper::Flush1(D3D11_CONTEXT_TYPE ContextType, HANDLE hEvent)
{
    return _deviceContext3->Flush1(ContextType, hEvent);
}

void D3D11DeviceContextWrapper::SetHardwareProtectionState(BOOL HwProtectionEnable)
{
    return _deviceContext3->SetHardwareProtectionState(HwProtectionEnable);
}

void D3D11DeviceContextWrapper::GetHardwareProtectionState(BOOL* pHwProtectionEnable)
{
    return _deviceContext3->GetHardwareProtectionState(pHwProtectionEnable);
}

HRESULT D3D11DeviceContextWrapper::UpdateTileMappings(ID3D11Resource* pTiledResource, UINT NumTiledResourceRegions, const D3D11_TILED_RESOURCE_COORDINATE* pTiledResourceRegionStartCoordinates, const D3D11_TILE_REGION_SIZE* pTiledResourceRegionSizes, ID3D11Buffer* pTilePool, UINT NumRanges, const UINT* pRangeFlags, const UINT* pTilePoolStartOffsets, const UINT* pRangeTileCounts, UINT Flags)
{
    return _deviceContext2->UpdateTileMappings(pTiledResource, NumTiledResourceRegions, pTiledResourceRegionStartCoordinates, pTiledResourceRegionSizes, pTilePool, NumRanges, pRangeFlags, pTilePoolStartOffsets, pRangeTileCounts, Flags);
}

HRESULT D3D11DeviceContextWrapper::CopyTileMappings(ID3D11Resource* pDestTiledResource, const D3D11_TILED_RESOURCE_COORDINATE* pDestRegionStartCoordinate, ID3D11Resource* pSourceTiledResource, const D3D11_TILED_RESOURCE_COORDINATE* pSourceRegionStartCoordinate, const D3D11_TILE_REGION_SIZE* pTileRegionSize, UINT Flags)
{
    return _deviceContext2->CopyTileMappings(pDestTiledResource, pDestRegionStartCoordinate, pSourceTiledResource, pSourceRegionStartCoordinate, pTileRegionSize, Flags);
}

void D3D11DeviceContextWrapper::CopyTiles(ID3D11Resource* pTiledResource, const D3D11_TILED_RESOURCE_COORDINATE* pTileRegionStartCoordinate, const D3D11_TILE_REGION_SIZE* pTileRegionSize, ID3D11Buffer* pBuffer, UINT64 BufferStartOffsetInBytes, UINT Flags)
{
    return _deviceContext2->CopyTiles(pTiledResource, pTileRegionStartCoordinate, pTileRegionSize, pBuffer, BufferStartOffsetInBytes, Flags);
}

void D3D11DeviceContextWrapper::UpdateTiles(ID3D11Resource* pDestTiledResource, const D3D11_TILED_RESOURCE_COORDINATE* pDestTileRegionStartCoordinate, const D3D11_TILE_REGION_SIZE* pDestTileRegionSize, const void* pSourceTileData, UINT Flags)
{
    return _deviceContext2->UpdateTiles(pDestTiledResource, pDestTileRegionStartCoordinate, pDestTileRegionSize, pSourceTileData, Flags);
}

HRESULT D3D11DeviceContextWrapper::ResizeTilePool(ID3D11Buffer* pTilePool, UINT64 NewSizeInBytes)
{
    return _deviceContext2->ResizeTilePool(pTilePool, NewSizeInBytes);
}

void D3D11DeviceContextWrapper::TiledResourceBarrier(ID3D11DeviceChild* pTiledResourceOrViewAccessBeforeBarrier, ID3D11DeviceChild* pTiledResourceOrViewAccessAfterBarrier)
{
    return _deviceContext2->TiledResourceBarrier(pTiledResourceOrViewAccessBeforeBarrier, pTiledResourceOrViewAccessAfterBarrier);
}

BOOL D3D11DeviceContextWrapper::IsAnnotationEnabled()
{
    return _deviceContext2->IsAnnotationEnabled();
}

void D3D11DeviceContextWrapper::SetMarkerInt(LPCWSTR pLabel, INT Data)
{
    return _deviceContext2->SetMarkerInt(pLabel, Data);
}

void D3D11DeviceContextWrapper::BeginEventInt(LPCWSTR pLabel, INT Data)
{
    return _deviceContext2->BeginEventInt(pLabel, Data);
}

void D3D11DeviceContextWrapper::EndEvent()
{
    return _deviceContext2->EndEvent();
}

void D3D11DeviceContextWrapper::CopySubresourceRegion1(ID3D11Resource* pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource* pSrcResource, UINT SrcSubresource, const D3D11_BOX* pSrcBox, UINT CopyFlags)
{
    return _deviceContext1->CopySubresourceRegion1(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox, CopyFlags);
}

void D3D11DeviceContextWrapper::UpdateSubresource1(ID3D11Resource* pDstResource, UINT DstSubresource, const D3D11_BOX* pDstBox, const void* pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch, UINT CopyFlags)
{
    return _deviceContext1->UpdateSubresource1(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch, CopyFlags);
}

void D3D11DeviceContextWrapper::DiscardResource(ID3D11Resource* pResource)
{
    return _deviceContext1->DiscardResource(pResource);
}

void D3D11DeviceContextWrapper::DiscardView(ID3D11View* pResourceView)
{
    return _deviceContext1->DiscardView(pResourceView);
}

void D3D11DeviceContextWrapper::VSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants)
{
    return _deviceContext1->VSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::HSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants)
{
    return _deviceContext1->HSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::DSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants)
{
    return _deviceContext1->DSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::GSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants)
{
    return _deviceContext1->GSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::PSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants)
{
    return _deviceContext1->PSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::CSSetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers, const UINT* pFirstConstant, const UINT* pNumConstants)
{
    return _deviceContext1->CSSetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::VSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants)
{
    return _deviceContext1->VSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::HSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants)
{
    return _deviceContext1->HSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::DSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants)
{
    return _deviceContext1->DSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::GSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants)
{
    return _deviceContext1->GSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::PSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants)
{
    return _deviceContext1->PSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::CSGetConstantBuffers1(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers, UINT* pFirstConstant, UINT* pNumConstants)
{
    return _deviceContext1->CSGetConstantBuffers1(StartSlot, NumBuffers, ppConstantBuffers, pFirstConstant, pNumConstants);
}

void D3D11DeviceContextWrapper::SwapDeviceContextState(ID3DDeviceContextState* pState, ID3DDeviceContextState** ppPreviousState)
{
    return _deviceContext1->SwapDeviceContextState(pState, ppPreviousState);
}

void D3D11DeviceContextWrapper::ClearView(ID3D11View* pView, const FLOAT Color[4], const D3D11_RECT* pRect, UINT NumRects)
{
    return _deviceContext1->ClearView(pView, Color, pRect, NumRects);
}

void D3D11DeviceContextWrapper::DiscardView1(ID3D11View* pResourceView, const D3D11_RECT* pRects, UINT NumRects)
{
    return _deviceContext1->DiscardView1(pResourceView, pRects, NumRects);
}

void D3D11DeviceContextWrapper::VSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
{
    return _deviceContext->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    return _deviceContext->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::PSSetShader(ID3D11PixelShader* pPixelShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances)
{
    return _deviceContext->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
}

void D3D11DeviceContextWrapper::PSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
{
    return _deviceContext->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::VSSetShader(ID3D11VertexShader* pVertexShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances)
{
    return _deviceContext->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
}

void D3D11DeviceContextWrapper::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
    return _deviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

void D3D11DeviceContextWrapper::Draw(UINT VertexCount, UINT StartVertexLocation)
{
    return _deviceContext->Draw(VertexCount, StartVertexLocation);
}

HRESULT D3D11DeviceContextWrapper::Map(ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource)
{
    return _deviceContext->Map(pResource, Subresource, MapType, MapFlags, pMappedResource);
}

void D3D11DeviceContextWrapper::Unmap(ID3D11Resource* pResource, UINT Subresource)
{
    return _deviceContext->Unmap(pResource, Subresource);
}

void D3D11DeviceContextWrapper::PSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
{
    return _deviceContext->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::IASetInputLayout(ID3D11InputLayout* pInputLayout)
{
    return _deviceContext->IASetInputLayout(pInputLayout);
}

void D3D11DeviceContextWrapper::IASetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets)
{
    return _deviceContext->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void D3D11DeviceContextWrapper::IASetIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
{
    return _deviceContext->IASetIndexBuffer(pIndexBuffer, Format, Offset);
}

void D3D11DeviceContextWrapper::DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
{
    return _deviceContext->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

void D3D11DeviceContextWrapper::DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
{
    return _deviceContext->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void D3D11DeviceContextWrapper::GSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
{
    return _deviceContext->GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::GSSetShader(ID3D11GeometryShader* pShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances)
{
    return _deviceContext->GSSetShader(pShader, ppClassInstances, NumClassInstances);
}

void D3D11DeviceContextWrapper::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology)
{
    return _deviceContext->IASetPrimitiveTopology(Topology);
}

void D3D11DeviceContextWrapper::VSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    return _deviceContext->VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::VSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
{
    return _deviceContext->VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::Begin(ID3D11Asynchronous* pAsync)
{
    return _deviceContext->Begin(pAsync);
}

void D3D11DeviceContextWrapper::End(ID3D11Asynchronous* pAsync)
{
    return _deviceContext->End(pAsync);
}

HRESULT D3D11DeviceContextWrapper::GetData(ID3D11Asynchronous* pAsync, void* pData, UINT DataSize, UINT GetDataFlags)
{
    return _deviceContext->GetData(pAsync, pData, DataSize, GetDataFlags);
}

void D3D11DeviceContextWrapper::SetPredication(ID3D11Predicate* pPredicate, BOOL PredicateValue)
{
    return _deviceContext->SetPredication(pPredicate, PredicateValue);
}

void D3D11DeviceContextWrapper::GSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    return _deviceContext->GSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::GSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
{
    return _deviceContext->GSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::OMSetRenderTargets(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView)
{
    _deviceContext->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
    
    std::vector<ID3D11RenderTargetView*> rtvs(ppRenderTargetViews, ppRenderTargetViews + NumViews);

    for (const FnSetRenderTargets& fn : _callbacks.SetRenderTargets) {
        fn(rtvs, pDepthStencilView);
    }
}

void D3D11DeviceContextWrapper::OMSetRenderTargetsAndUnorderedAccessViews(UINT NumRTVs, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts)
{
    return _deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, pDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
}

void D3D11DeviceContextWrapper::OMSetBlendState(ID3D11BlendState* pBlendState, const FLOAT BlendFactor[4], UINT SampleMask)
{
    return _deviceContext->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
}

void D3D11DeviceContextWrapper::OMSetDepthStencilState(ID3D11DepthStencilState* pDepthStencilState, UINT StencilRef)
{
    return _deviceContext->OMSetDepthStencilState(pDepthStencilState, StencilRef);
}

void D3D11DeviceContextWrapper::SOSetTargets(UINT NumBuffers, ID3D11Buffer* const* ppSOTargets, const UINT* pOffsets)
{
    return _deviceContext->SOSetTargets(NumBuffers, ppSOTargets, pOffsets);
}

void D3D11DeviceContextWrapper::DrawAuto()
{
    return _deviceContext->DrawAuto();
}

void D3D11DeviceContextWrapper::DrawIndexedInstancedIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs)
{
    return _deviceContext->DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}

void D3D11DeviceContextWrapper::DrawInstancedIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs)
{
    return _deviceContext->DrawInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}

void D3D11DeviceContextWrapper::Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
{
    return _deviceContext->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void D3D11DeviceContextWrapper::DispatchIndirect(ID3D11Buffer* pBufferForArgs, UINT AlignedByteOffsetForArgs)
{
    return _deviceContext->DispatchIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}

void D3D11DeviceContextWrapper::RSSetState(ID3D11RasterizerState* pRasterizerState)
{
    return _deviceContext->RSSetState(pRasterizerState);
}

void D3D11DeviceContextWrapper::RSSetViewports(UINT NumViewports, const D3D11_VIEWPORT* pViewports)
{
    return _deviceContext->RSSetViewports(NumViewports, pViewports);
}

void D3D11DeviceContextWrapper::RSSetScissorRects(UINT NumRects, const D3D11_RECT* pRects)
{
    return _deviceContext->RSSetScissorRects(NumRects, pRects);
}

void D3D11DeviceContextWrapper::CopySubresourceRegion(ID3D11Resource* pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource* pSrcResource, UINT SrcSubresource, const D3D11_BOX* pSrcBox)
{
    return _deviceContext->CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
}

void D3D11DeviceContextWrapper::CopyResource(ID3D11Resource* pDstResource, ID3D11Resource* pSrcResource)
{
    return _deviceContext->CopyResource(pDstResource, pSrcResource);
}

void D3D11DeviceContextWrapper::UpdateSubresource(ID3D11Resource* pDstResource, UINT DstSubresource, const D3D11_BOX* pDstBox, const void* pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch)
{
    return _deviceContext->UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
}

void D3D11DeviceContextWrapper::CopyStructureCount(ID3D11Buffer* pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView* pSrcView)
{
    return _deviceContext->CopyStructureCount(pDstBuffer, DstAlignedByteOffset, pSrcView);
}

void D3D11DeviceContextWrapper::ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4])
{
    return _deviceContext->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
}

void D3D11DeviceContextWrapper::ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView* pUnorderedAccessView, const UINT Values[4])
{
    return _deviceContext->ClearUnorderedAccessViewUint(pUnorderedAccessView, Values);
}

void D3D11DeviceContextWrapper::ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView* pUnorderedAccessView, const FLOAT Values[4])
{
    return _deviceContext->ClearUnorderedAccessViewFloat(pUnorderedAccessView, Values);
}

void D3D11DeviceContextWrapper::ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil)
{
    return _deviceContext->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
}

void D3D11DeviceContextWrapper::GenerateMips(ID3D11ShaderResourceView* pShaderResourceView)
{
    return _deviceContext->GenerateMips(pShaderResourceView);
}

void D3D11DeviceContextWrapper::SetResourceMinLOD(ID3D11Resource* pResource, FLOAT MinLOD)
{
    return _deviceContext->SetResourceMinLOD(pResource, MinLOD);
}

FLOAT D3D11DeviceContextWrapper::GetResourceMinLOD(ID3D11Resource* pResource)
{
    return _deviceContext->GetResourceMinLOD(pResource);
}

void D3D11DeviceContextWrapper::ResolveSubresource(ID3D11Resource* pDstResource, UINT DstSubresource, ID3D11Resource* pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format)
{
    return _deviceContext->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
}

void D3D11DeviceContextWrapper::ExecuteCommandList(ID3D11CommandList* pCommandList, BOOL RestoreContextState)
{
    return _deviceContext->ExecuteCommandList(pCommandList, RestoreContextState);
}

void D3D11DeviceContextWrapper::HSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    return _deviceContext->HSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::HSSetShader(ID3D11HullShader* pHullShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances)
{
    return _deviceContext->HSSetShader(pHullShader, ppClassInstances, NumClassInstances);
}

void D3D11DeviceContextWrapper::HSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
{
    return _deviceContext->HSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::HSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
{
    return _deviceContext->HSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::DSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    return _deviceContext->DSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::DSSetShader(ID3D11DomainShader* pDomainShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances)
{
    return _deviceContext->DSSetShader(pDomainShader, ppClassInstances, NumClassInstances);
}

void D3D11DeviceContextWrapper::DSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
{
    return _deviceContext->DSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::DSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
{
    return _deviceContext->DSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::CSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
    return _deviceContext->CSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::CSSetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts)
{
    return _deviceContext->CSSetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
}

void D3D11DeviceContextWrapper::CSSetShader(ID3D11ComputeShader* pComputeShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances)
{
    return _deviceContext->CSSetShader(pComputeShader, ppClassInstances, NumClassInstances);
}

void D3D11DeviceContextWrapper::CSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
{
    return _deviceContext->CSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::CSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers)
{
    return _deviceContext->CSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::VSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
    return _deviceContext->VSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::PSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
    return _deviceContext->PSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::PSGetShader(ID3D11PixelShader** ppPixelShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
    return _deviceContext->PSGetShader(ppPixelShader, ppClassInstances, pNumClassInstances);
}

void D3D11DeviceContextWrapper::PSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
    return _deviceContext->PSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::VSGetShader(ID3D11VertexShader** ppVertexShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
    return _deviceContext->VSGetShader(ppVertexShader, ppClassInstances, pNumClassInstances);
}

void D3D11DeviceContextWrapper::PSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
    return _deviceContext->PSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::IAGetInputLayout(ID3D11InputLayout** ppInputLayout)
{
    return _deviceContext->IAGetInputLayout(ppInputLayout);
}

void D3D11DeviceContextWrapper::IAGetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppVertexBuffers, UINT* pStrides, UINT* pOffsets)
{
    return _deviceContext->IAGetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void D3D11DeviceContextWrapper::IAGetIndexBuffer(ID3D11Buffer** pIndexBuffer, DXGI_FORMAT* Format, UINT* Offset)
{
    return _deviceContext->IAGetIndexBuffer(pIndexBuffer, Format, Offset);
}

void D3D11DeviceContextWrapper::GSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
    return _deviceContext->GSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::GSGetShader(ID3D11GeometryShader** ppGeometryShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
    return _deviceContext->GSGetShader(ppGeometryShader, ppClassInstances, pNumClassInstances);
}

void D3D11DeviceContextWrapper::IAGetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY* pTopology)
{
    return _deviceContext->IAGetPrimitiveTopology(pTopology);
}

void D3D11DeviceContextWrapper::VSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
    return _deviceContext->VSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::VSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
    return _deviceContext->VSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::GetPredication(ID3D11Predicate** ppPredicate, BOOL* pPredicateValue)
{
    return _deviceContext->GetPredication(ppPredicate, pPredicateValue);
}

void D3D11DeviceContextWrapper::GSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
    return _deviceContext->GSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::GSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
    return _deviceContext->GSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::OMGetRenderTargets(UINT NumViews, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView)
{
    return _deviceContext->OMGetRenderTargets(NumViews, ppRenderTargetViews, ppDepthStencilView);
}

void D3D11DeviceContextWrapper::OMGetRenderTargetsAndUnorderedAccessViews(UINT NumRTVs, ID3D11RenderTargetView** ppRenderTargetViews, ID3D11DepthStencilView** ppDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews)
{
    return _deviceContext->OMGetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, ppDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews);
}

void D3D11DeviceContextWrapper::OMGetBlendState(ID3D11BlendState** ppBlendState, FLOAT BlendFactor[4], UINT* pSampleMask)
{
    return _deviceContext->OMGetBlendState(ppBlendState, BlendFactor, pSampleMask);
}

void D3D11DeviceContextWrapper::OMGetDepthStencilState(ID3D11DepthStencilState** ppDepthStencilState, UINT* pStencilRef)
{
    return _deviceContext->OMGetDepthStencilState(ppDepthStencilState, pStencilRef);
}

void D3D11DeviceContextWrapper::SOGetTargets(UINT NumBuffers, ID3D11Buffer** ppSOTargets)
{
    return _deviceContext->SOGetTargets(NumBuffers, ppSOTargets);
}

void D3D11DeviceContextWrapper::RSGetState(ID3D11RasterizerState** ppRasterizerState)
{
    return _deviceContext->RSGetState(ppRasterizerState);
}

void D3D11DeviceContextWrapper::RSGetViewports(UINT* NumViewports, D3D11_VIEWPORT* pViewports)
{
    return _deviceContext->RSGetViewports(NumViewports, pViewports);
}

void D3D11DeviceContextWrapper::RSGetScissorRects(UINT* NumRects, D3D11_RECT* pRects)
{
    return _deviceContext->RSGetScissorRects(NumRects, pRects);
}

void D3D11DeviceContextWrapper::HSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
    return _deviceContext->HSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::HSGetShader(ID3D11HullShader** ppHullShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
    return _deviceContext->HSGetShader(ppHullShader, ppClassInstances, pNumClassInstances);
}

void D3D11DeviceContextWrapper::HSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
    return _deviceContext->HSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::HSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
    return _deviceContext->HSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::DSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
    return _deviceContext->DSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::DSGetShader(ID3D11DomainShader** ppDomainShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
    return _deviceContext->DSGetShader(ppDomainShader, ppClassInstances, pNumClassInstances);
}

void D3D11DeviceContextWrapper::DSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
    return _deviceContext->DSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::DSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
    return _deviceContext->DSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::CSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
    return _deviceContext->CSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void D3D11DeviceContextWrapper::CSGetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews)
{
    return _deviceContext->CSGetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews);
}

void D3D11DeviceContextWrapper::CSGetShader(ID3D11ComputeShader** ppComputeShader, ID3D11ClassInstance** ppClassInstances, UINT* pNumClassInstances)
{
    return _deviceContext->CSGetShader(ppComputeShader, ppClassInstances, pNumClassInstances);
}

void D3D11DeviceContextWrapper::CSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState** ppSamplers)
{
    return _deviceContext->CSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void D3D11DeviceContextWrapper::CSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppConstantBuffers)
{
    return _deviceContext->CSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void D3D11DeviceContextWrapper::ClearState()
{
    return _deviceContext->ClearState();
}

void D3D11DeviceContextWrapper::Flush()
{
    return _deviceContext->Flush();
}

D3D11_DEVICE_CONTEXT_TYPE D3D11DeviceContextWrapper::GetType()
{
    return _deviceContext->GetType();
}

UINT D3D11DeviceContextWrapper::GetContextFlags()
{
    return _deviceContext->GetContextFlags();
}

HRESULT D3D11DeviceContextWrapper::FinishCommandList(BOOL RestoreDeferredContextState, ID3D11CommandList** ppCommandList)
{
    return _deviceContext->FinishCommandList(RestoreDeferredContextState, ppCommandList);
}

void D3D11DeviceContextWrapper::GetDevice(ID3D11Device** ppDevice)
{
    return _deviceContext->GetDevice(ppDevice);
}

HRESULT D3D11DeviceContextWrapper::GetPrivateData(REFGUID guid, UINT* pDataSize, void* pData)
{
    return _deviceContext->GetPrivateData(guid, pDataSize, pData);
}

HRESULT D3D11DeviceContextWrapper::SetPrivateData(REFGUID guid, UINT DataSize, const void* pData)
{
    return _deviceContext->SetPrivateData(guid, DataSize, pData);
}

HRESULT D3D11DeviceContextWrapper::SetPrivateDataInterface(REFGUID guid, const IUnknown* pData)
{
    return _deviceContext->SetPrivateDataInterface(guid, pData);
}

HRESULT D3D11DeviceContextWrapper::QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppvObject)
{
    if (riid == __uuidof(IUnknown) ||
        riid == __uuidof(ID3D11DeviceContext) && _deviceContext ||
        riid == __uuidof(ID3D11DeviceContext1) && _deviceContext1 ||
        riid == __uuidof(ID3D11DeviceContext2) && _deviceContext2 ||
        riid == __uuidof(ID3D11DeviceContext3) && _deviceContext3 ||
        riid == __uuidof(ID3D11DeviceContext4) && _deviceContext4)
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }

    static const IID _uuid_nvsdk = IIDFromString("{BC09D320-CA95-452A-B7B2-5C94A4F07D07}");

    if (riid != __uuidof(ID3DUserDefinedAnnotation) &&
        riid != _uuid_nvsdk)
    {
        LOG_WARNING("D3D11DeviceContextWrapper::QueryInterface unknown: {}\n", ToString(riid));
    }

    return _deviceContext->QueryInterface(riid, ppvObject);
}

ULONG D3D11DeviceContextWrapper::AddRef()
{
    return _deviceContext->AddRef();
}

ULONG D3D11DeviceContextWrapper::Release()
{
    const ULONG refCount = _deviceContext->Release();
    if (refCount == 0) {
        delete this;
    }
    return refCount;
}
