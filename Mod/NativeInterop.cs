using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace EnhancedGraphics;
public class NativeInterop {
    public static IntPtr GetFunction(string functionName) => GetProcAddress(IntPtr.Zero, functionName);

    public static void DebugPrint(string str) {
        if (_debugPrintFromManaged != null) {
            IntPtr ptr = Marshal.StringToHGlobalAnsi(str);
            _debugPrintFromManaged(ptr);
            Marshal.FreeHGlobal(ptr);
        }
    }

    static unsafe NativeInterop() {
        try {
            _debugPrintFromManaged = Marshal.GetDelegateForFunctionPointer<FnDebugPrintFromManaged>(GetFunction("DebugPrintFromManaged"));
        } catch {
            Debug.LogWarning($"Failed to load native function DebugPrintFromManaged. This won't break anything, just no debug log output.");
        }
    }

    private unsafe delegate void FnDebugPrintFromManaged(IntPtr str);
    private static readonly FnDebugPrintFromManaged _debugPrintFromManaged;

    [DllImport("kernel32.dll", CharSet = CharSet.Ansi, SetLastError = true)]
    private static extern IntPtr GetProcAddress(IntPtr hModule, string procName);
}
