#pragma once

#include <string>

struct IDXGIAdapter;
struct IDXGIFactory2;
struct IDXGISwapChain;

enum DXGI_FORMAT;

namespace DXGI {

IDXGIAdapter* GetAdapter();

std::string GetDebugName(DXGI_FORMAT);

using FnCreateSwapChain = void (*)(IDXGISwapChain* swapChain, void* hwnd);
using FnPresent = void (*)(IDXGISwapChain* swapChain);

void On_CreateSwapChain(FnCreateSwapChain&& fn);
void On_Present_Before(FnPresent&& fn);
void On_Present_After(FnPresent&& fn);

}
