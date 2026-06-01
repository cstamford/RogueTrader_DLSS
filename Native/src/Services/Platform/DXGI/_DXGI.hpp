#include <Services/Platform/DXGI.hpp>
#include <dxgi1_6.h>
#include <vector>

namespace DXGI::Internal {

extern struct Callbacks {
    std::vector<DXGI::FnCreateSwapChain> CreateSwapChain;
    std::vector<DXGI::FnPresent> Present_Before;
    std::vector<DXGI::FnPresent> Present_After;
    std::vector<DXGI::FnVoid> ResizeBuffers;
} _callbacks;

extern struct State {
    IDXGIAdapter4* BestAdapter;
} _dxgiState;

IDXGIFactory* Wrap(IDXGIFactory* factory);
IDXGISwapChain* Wrap(IDXGISwapChain* swapChain);

}
