#include <Services/Platform/D3D11.hpp>
#include <d3d11_4.h>

namespace D3D11::Internal { 

extern struct Callbacks {
    std::vector<FnSetRenderTargets> SetRenderTargets;
} _callbacks;

extern struct State {
    ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;
} _state;

ID3D11Device* Wrap(ID3D11Device* device);
ID3D11DeviceContext* Wrap(ID3D11DeviceContext* deviceContext);

}
