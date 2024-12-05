using HarmonyLib;
using System;
using System.Reflection;
using UnityEngine;
using UnityEngine.Experimental.Rendering.RenderGraphModule;
using UnityModManagerNet;

using static DLSS.NativeInterop;

namespace DLSS;

public static class CustomRenderState {
    public static UpscaleConfiguration UpscaleConfig;
    public static TextureHandle UpscaleCameraColor;
    public static int NextConfigChangeFrameIdx = 0;
}

public class ModSettings : UnityModManager.ModSettings {
    public UpscaleType UpscaleType = UpscaleType.None;
    public float UpscaleRatio = 0.33f;
}

#if DEBUG
[EnableReloading]
#endif
public static class Main {
    public static ModSettings Settings;

    public static bool Load(UnityModManager.ModEntry modEntry) {
        Settings = ModSettings.Load<ModSettings>(modEntry);

#if DEBUG
        modEntry.OnUnload = OnUnload;
#endif
        modEntry.OnGUI = OnGUI;
        modEntry.OnSaveGUI = OnSaveGUI;

        try {
            _harmony = new Harmony(modEntry.Info.Id);
            _harmony.PatchAll(Assembly.GetExecutingAssembly());
        } catch (Exception ex) {
            Debug.LogError(ex);
        }
        return true;
    }

    public static void OnGUI(UnityModManager.ModEntry modEntry) {
        GUILayout.BeginVertical();

        GUILayout.BeginHorizontal();
        GUILayout.Label("Upscale Type");
        Settings.UpscaleType = (UpscaleType)GUILayout.SelectionGrid((int)Settings.UpscaleType, Enum.GetNames(typeof(UpscaleType)), 1);
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal();
        GUILayout.Label("Upscale Ratio");
        Settings.UpscaleRatio = GUILayout.HorizontalSlider(Settings.UpscaleRatio, 0.33f, 1.0f);
        GUILayout.EndHorizontal();

        GUILayout.EndVertical();
    }

    private static void OnSaveGUI(UnityModManager.ModEntry modEntry) {
        Settings.Save(modEntry);
    }

#if DEBUG
    public static bool OnUnload(UnityModManager.ModEntry modEntry) {
        _harmony.UnpatchAll(modEntry.Info.Id);
        return true;
    }
#endif


    private static Harmony _harmony;
}

public enum UpscaleType {
    None,
    Dlss,
    Fsr,
    XeSS
}

public interface IUpscaleConfiguration {
    public UpscaleType UpscaleType {
        get;
    }

    public Vector2Int RenderResolution {
        get;
    }

    public Vector2Int DisplayResolution {
        get;
    }

    public float UpscaleRatio {
        get;
    }
}

public abstract record class UpscaleConfiguration(
    UpscaleType UpscaleType,
    Vector2Int RenderResolution,
    Vector2Int DisplayResolution
) : IUpscaleConfiguration {
    public float UpscaleRatio => (float)RenderResolution.x / DisplayResolution.x;
}

public record class VanillaUpscaleConfiguration(
    Vector2Int RenderResolution,
    Vector2Int DisplayResolution
) : UpscaleConfiguration(UpscaleType.None, RenderResolution, DisplayResolution);

public record class DlssUpscaleConfiguration(
    Vector2Int RenderResolution,
    Vector2Int DisplayResolution
) : UpscaleConfiguration(UpscaleType.Dlss, RenderResolution, DisplayResolution) {
    public DlssQualityMode Mode => new() {
        Name = IntPtr.Zero,
        InputWidth = (uint)RenderResolution.x,
        InputHeight = (uint)RenderResolution.y,
        FinalWidth = (uint)DisplayResolution.x,
        FinalHeight = (uint)DisplayResolution.y
    };
}
