using System;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;

namespace EnhancedGraphics.Upscalers;

public class DlssUpscaler : IUpscaler {
    public UpscalePreset[] AvailablePresets {
        get {
            Vector2 displayResolution = new(Screen.width, Screen.height);
            if (_availablePresets == null || displayResolution != _availablePresetsResolution) {
                _availablePresets = [.. GetAvailablePresets(displayResolution).OrderByDescending(x => x.Ratio)];
                _availablePresetsResolution = displayResolution;
            }
            return _availablePresets;
        }
    }

    public unsafe bool SetPreset(UpscalePreset preset, UpscaleFlags flags) {
        if (_setQualityMode == null) {
            return false;
        }

        QualityMode mode = new() {
            Name = Marshal.StringToHGlobalAnsi(preset.Name),
            InputWidth = (uint)preset.RenderResolution.x,
            InputHeight = (uint)preset.RenderResolution.y,
            FinalWidth = (uint)preset.DisplayResolution.x,
            FinalHeight = (uint)preset.DisplayResolution.y
        };

        EvaluationFlags evalFlags = 0;

        if (flags.HasFlag(UpscaleFlags.HDR)) {
            evalFlags |= EvaluationFlags.IsHDR;
        }

        if (flags.HasFlag(UpscaleFlags.MVRenderRes)) {
            evalFlags |= EvaluationFlags.MVRenderRes;
        }

        if (flags.HasFlag(UpscaleFlags.MVJitter)) {
            evalFlags |= EvaluationFlags.MVJittered;
        }

        if (flags.HasFlag(UpscaleFlags.DepthInverted)) {
            evalFlags |= EvaluationFlags.DepthInverted;
        }

        if (flags.HasFlag(UpscaleFlags.AutoExposure)) {
            evalFlags |= EvaluationFlags.AutoExposure;
        }

        if (flags.HasFlag(UpscaleFlags.AlphaUpscaling)) {
            evalFlags |= EvaluationFlags.AlphaUpscaling;
        }

        _setQualityMode(&mode, evalFlags);
        Marshal.FreeHGlobal(mode.Name);

        return true;
    }

    public unsafe bool Evaluate(IntPtr colorIn, IntPtr colorOut, float sharpness, UpscaleOptionalParams param) {
        if (_evaluate == null) {
            return false;
        }

        EvaluationParams evalParams = new() {
            DepthIn = param.Depth,
            MvecIn = param.Mvec,
            JitterX = param.Jitter.x,
            JitterY = param.Jitter.y,
            MVecScaleX = param.MvecScale.x,
            MVecScaleY = param.MvecScale.y,
            Reset = param.Reset
        };

        _evaluate(colorIn, colorOut, sharpness, &evalParams);

        return true;
    }

    private UpscalePreset[] _availablePresets;
    private Vector2 _availablePresetsResolution;

    private unsafe UpscalePreset[] GetAvailablePresets(Vector2 displayResolution) {
        if (_getQualityModes == null) {
            return [];
        }

        int numQualityModes = _getQualityModes((uint)displayResolution.x, (uint)displayResolution.y, null);
        UpscalePreset[] ret = new UpscalePreset[numQualityModes];

        if (numQualityModes > 0) {
            QualityMode* qualityModes = stackalloc QualityMode[numQualityModes];
            int numQualityModesWithDetails = _getQualityModes((uint)displayResolution.x, (uint)displayResolution.y, qualityModes);
            Debug.Assert(numQualityModes == numQualityModesWithDetails);

            for (int i = 0; i < numQualityModes; i++) {
                ret[i] = new(
                    Name: Marshal.PtrToStringAnsi(qualityModes[i].Name),
                    RenderResolution: new(qualityModes[i].InputWidth, qualityModes[i].InputHeight),
                    DisplayResolution: new(qualityModes[i].FinalWidth, qualityModes[i].FinalHeight)
                );
            }
        }

        return ret;
    }

    static unsafe DlssUpscaler() {
        try {
            _getQualityModes = Marshal.GetDelegateForFunctionPointer<FnGetQualityModes>(GetProcAddress(IntPtr.Zero, "DLSS_GetQualityModes"));
            _setQualityMode = Marshal.GetDelegateForFunctionPointer<FnSetQualityMode>(GetProcAddress(IntPtr.Zero, "DLSS_SetQualityMode"));
            _evaluate = Marshal.GetDelegateForFunctionPointer<FnEvaluate>(GetProcAddress(IntPtr.Zero, "DLSS_Evaluate"));
        } catch (Exception ex) {
            Debug.LogWarning($"Failed to acquire native addresses for DLSS - activating DLSS won't upscale correctly - {ex}");
        }
    }

    [Flags]
    private enum EvaluationFlags {
        None = 0,
        IsHDR = 1 << 0,
        MVRenderRes = 1 << 1,
        MVJittered = 1 << 2,
        DepthInverted = 1 << 3,
        Reserved_0 = 1 << 4,
        DoSharpening = 1 << 5,
        AutoExposure = 1 << 6,
        AlphaUpscaling = 1 << 7,
    };

    [StructLayout(LayoutKind.Sequential)]
    private struct EvaluationParams {
        public IntPtr DepthIn;
        public IntPtr MvecIn;
        public float JitterX;
        public float JitterY;
        public float MVecScaleX;
        public float MVecScaleY;
        public bool Reset;
    };

    [StructLayout(LayoutKind.Sequential)]
    private struct QualityMode {
        public IntPtr Name;
        public uint InputWidth;
        public uint InputHeight;
        public uint FinalWidth;
        public uint FinalHeight;
    };

    private unsafe delegate int FnGetQualityModes(uint finalWidth, uint finalHeight, QualityMode* outQualityModes);
    private static readonly FnGetQualityModes _getQualityModes;

    private unsafe delegate void FnSetQualityMode(QualityMode* qualityMode, EvaluationFlags flags);
    private static readonly FnSetQualityMode _setQualityMode;

    private unsafe delegate void FnEvaluate(IntPtr colorIn, IntPtr colorOut, float sharpness, EvaluationParams* param);
    private static readonly FnEvaluate _evaluate;

    [DllImport("kernel32.dll", CharSet = CharSet.Ansi, SetLastError = true)]
    private static extern IntPtr GetProcAddress(IntPtr hModule, string procName);
}
