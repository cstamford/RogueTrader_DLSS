#pragma once

#include <compare>

namespace DLSS {

using RenderResource = void*;

enum EvaluationFlags {
    None            = 0,
    IsHDR           = 1 << 0,
    MVRenderRes     = 1 << 1,
    MVJittered      = 1 << 2,
    DepthInverted   = 1 << 3,
    Reserved_0      = 1 << 4,
    DoSharpening    = 1 << 5,
    AutoExposure    = 1 << 6,
    AlphaUpscaling  = 1 << 7,
};

struct EvaluationParams {
    RenderResource DepthIn;
    RenderResource MvecIn;
    float JitterX;
    float JitterY;
    float MVecScaleX;
    float MVecScaleY;
    bool Reset;
};

struct QualityMode {
    const char* Name;
    uint32_t InputWidth;
    uint32_t InputHeight;
    uint32_t FinalWidth;
    uint32_t FinalHeight;
};

}
