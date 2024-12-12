#include <Services/Platform/D3D11/_D3D11.hpp>

std::string D3D11::GetDebugName(ID3D11DeviceChild* child)
{
    static const GUID WKPDID_D3DDebugObjectName = { 0x429b8c22, 0x9188, 0x4b0c, 0x87, 0x42, 0xac, 0xb0, 0xbf, 0x85, 0xc2, 0x00 };

    if (UINT size; SUCCEEDED(child->GetPrivateData(WKPDID_D3DDebugObjectName, &size, nullptr)) && size > 0) {
        std::string name(size, '\0');
        if (SUCCEEDED(child->GetPrivateData(WKPDID_D3DDebugObjectName, &size, name.data()))) {
            return name;
        }
    }

    return std::string();
}

std::string D3D11::GetDebugName(D3D11_DSV_DIMENSION dsvDimensions)
{
    switch (dsvDimensions) {
    case D3D11_DSV_DIMENSION_UNKNOWN:
        return "D3D11_DSV_DIMENSION_UNKNOWN";
    case D3D11_DSV_DIMENSION_TEXTURE1D:
        return "D3D11_DSV_DIMENSION_TEXTURE1D";
    case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
        return "D3D11_DSV_DIMENSION_TEXTURE1DARRAY";
    case D3D11_DSV_DIMENSION_TEXTURE2D:
        return "D3D11_DSV_DIMENSION_TEXTURE2D";
    case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
        return "D3D11_DSV_DIMENSION_TEXTURE2DARRAY";
    case D3D11_DSV_DIMENSION_TEXTURE2DMS:
        return "D3D11_DSV_DIMENSION_TEXTURE2DMS";
    case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
        return "D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY";
    }

    return "<UNKNWON D3D11_DSV_DIMENSION>";
}


std::string D3D11::GetDebugName(D3D11_RTV_DIMENSION rtvDimensions)
{
    switch (rtvDimensions) {
    case D3D11_RTV_DIMENSION_UNKNOWN:
        return "D3D11_RTV_DIMENSION_UNKNOWN";
    case D3D11_RTV_DIMENSION_BUFFER:
        return "D3D11_RTV_DIMENSION_BUFFER";
    case D3D11_RTV_DIMENSION_TEXTURE1D:
        return "D3D11_RTV_DIMENSION_TEXTURE1D";
    case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
        return "D3D11_RTV_DIMENSION_TEXTURE1DARRAY";
    case D3D11_RTV_DIMENSION_TEXTURE2D:
        return "D3D11_RTV_DIMENSION_TEXTURE2D";
    case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
        return "D3D11_RTV_DIMENSION_TEXTURE2DARRAY";
    case D3D11_RTV_DIMENSION_TEXTURE2DMS:
        return "D3D11_RTV_DIMENSION_TEXTURE2DMS";
    case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
        return "D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY";
    case D3D11_RTV_DIMENSION_TEXTURE3D:
        return "D3D11_RTV_DIMENSION_TEXTURE3D";
    }

    return "<UNKNWON D3D11_RTV_DIMENSION>";
}
