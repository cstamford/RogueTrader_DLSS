using System;
using UnityEngine;

namespace EnhancedGraphics.Upscalers;

public enum UpscaleType {
    Vanilla,
    Dlss,
    Fsr,
    XeSS
}

[Flags]
public enum UpscaleFlags {
    None = 0,
    HDR = 1 << 0,
    MVRenderRes = 1 << 1,
    MVJitter = 1 << 2,
    DepthInverted = 1 << 3,
    AutoExposure = 1 << 4,
    AlphaUpscaling = 1 << 5,
};

public record struct UpscalePreset(string Name, Vector2 RenderResolution, Vector2 DisplayResolution) {
    public readonly float Ratio => Mathf.Max(RenderResolution.x / DisplayResolution.x, RenderResolution.y / DisplayResolution.y);
}

public record struct UpscaleOptionalParams(IntPtr Depth, IntPtr Mvec, Vector2 Jitter, Vector2 MvecScale, bool Reset);

public interface IUpscaler {
    UpscalePreset[] AvailablePresets {
        get;
    }
    bool SetPreset(UpscalePreset preset, UpscaleFlags flags);
    bool Evaluate(IntPtr colorIn, IntPtr colorOut, float sharpness, UpscaleOptionalParams param);
}
