using HarmonyLib;
using System;
using System.Reflection;
using UnityEngine;
using UnityModManagerNet;

// TODO:
// 1. Cloaks do not have presence in motion vectors - heavy ghosting behind them.

namespace DLSS;

public enum UpscaleType {
    Vanilla,
    Dlss,
    Fsr,
    XeSS
}

public class ModSettings : UnityModManager.ModSettings {
    public UpscaleType UpscaleType = UpscaleType.Dlss;
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

        _harmony ??= new Harmony(modEntry.Info.Id);
        _harmony.PatchAll(Assembly.GetExecutingAssembly());

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

