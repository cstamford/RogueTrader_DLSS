using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace DLSS;

public static class NativeInterop {
    [StructLayout(LayoutKind.Sequential)]
    public struct DlssQualityMode {
        public IntPtr Name;
        public uint InputWidth;
        public uint InputHeight;
        public uint FinalWidth;
        public uint FinalHeight;
    };


    [StructLayout(LayoutKind.Sequential)]
    public struct DlssDispatchParams {
        public IntPtr DepthIn;
        public IntPtr MvecIn;
        public float JitterX;
        public float JitterY;
        public float MVecScaleX;
        public float MVecScaleY;
        public bool Reset;
    };

    public static unsafe DlssQualityMode[] DlssGetQualityModes(Vector2Int displayResolution) {
        int numQualityModes = _dlssGetQualityModes((uint)displayResolution.x, (uint)displayResolution.y, null);
        DlssQualityMode[] ret = new DlssQualityMode[numQualityModes];

        if (numQualityModes > 0) {
            DlssQualityMode* qualityModes = stackalloc DlssQualityMode[numQualityModes];
            int numQualityModesWithDetails = _dlssGetQualityModes((uint)displayResolution.x, (uint)displayResolution.y, qualityModes);
            Debug.Assert(numQualityModes == numQualityModesWithDetails);

            for (int i = 0; i < numQualityModes; i++) {
                ret[i] = qualityModes[i];
            }
        }

        return ret;
    }

    public static unsafe void DlssSetQualityMode(DlssQualityMode qualityMode) {
        _dlssSetQualityMode(&qualityMode);
    }

    public static unsafe void DlssEvaluate(IntPtr colorIn, IntPtr colorOut, DlssDispatchParams param) {
        _dlssEvaluate(colorIn, colorOut, &param);
    }

    static unsafe NativeInterop() {
        try {
            _dlssGetQualityModes = Marshal.GetDelegateForFunctionPointer<FnGetQualityModes>(GetProcAddress(IntPtr.Zero, "DLSS_GetQualityModes"));
            _dlssSetQualityMode = Marshal.GetDelegateForFunctionPointer<FnSetQualityMode>(GetProcAddress(IntPtr.Zero, "DLSS_SetQualityMode"));
            _dlssEvaluate = Marshal.GetDelegateForFunctionPointer<FnEvaluate>(GetProcAddress(IntPtr.Zero, "DLSS_Evaluate"));
        } catch (Exception ex) {
            Debug.LogWarning($"Failed to acquire native addresses - {ex}");
        }
    }


    [DllImport("kernel32.dll", CharSet = CharSet.Ansi, SetLastError = true)]
    private static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

    private unsafe delegate int FnGetQualityModes(uint finalWidth, uint finalHeight, DlssQualityMode* outQualityModes);
    private static readonly FnGetQualityModes _dlssGetQualityModes;

    private unsafe delegate void FnSetQualityMode(DlssQualityMode* qualityMode);
    private static readonly FnSetQualityMode _dlssSetQualityMode;

    private unsafe delegate void FnEvaluate(IntPtr colorIn, IntPtr colorOut, DlssDispatchParams* param);
    private static readonly FnEvaluate _dlssEvaluate;
}
