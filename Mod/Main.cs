using EnhancedGraphics.Upscalers;
using HarmonyLib;
using Owlcat.Runtime.Visual.Waaagh;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using UnityEngine;
using UnityEngine.Experimental.Rendering.RenderGraphModule;
using UnityModManagerNet;

namespace EnhancedGraphics;

public class EGSettings : UnityModManager.ModSettings {
    public string SelectedUpscaler = "Vanilla";

    public SerializableDictionary<string, string> SelectedPreset = [];
    public SerializableDictionary<string, UpscalePreset> CustomPreset = [];

    public Vector2 MvecScaleToUpscale = -Vector2.one;
    public Vector2 JitterScale = Vector2.one * 2;
    public Vector2 JitterScaleToUpscale = Vector2.one;
    public UpscaleFlags Flags = UpscaleFlags.HDR | UpscaleFlags.MVRenderRes | UpscaleFlags.DepthInverted;
    public float GlobalMipBiasOffset = 0.0f;

    public bool DebugSkipPostProcessing = false;

    public override void Save(UnityModManager.ModEntry modEntry) {
        Save(this, modEntry);
    }
}

#if DEBUG
[EnableReloading]
#endif
public static class EnhancedGraphics {
    public static TextureHandle CameraColorUpscaled {
        get => _cameraColorUpscaled;
        set => _cameraColorUpscaled = value;
    }

    public static Vector2 GlobalMipBias {
        get => _globalMipBias;
        set => _globalMipBias = value;
    }

    public static float GlobalMipBiasOffset => _settings.GlobalMipBiasOffset;

    public static IUpscaler Upscaler => _settings.SelectedUpscaler switch {
        nameof(DlssUpscaler) => _dlss,
        _ => null
    };

    public static UpscalePreset Preset => GetSelectedPreset(_settings.SelectedUpscaler);
    public static IEnumerable<UpscalePreset> Presets => (Upscaler?.AvailablePresets ?? [])
        .Append(_settings.CustomPreset.TryGetValue(_settings.SelectedUpscaler, out UpscalePreset customPreset) ? customPreset : GetCustomPreset(1));

    public static Vector2 MvecScaleToUpscale => _settings.MvecScaleToUpscale;
    public static Vector2 JitterScale => _settings.JitterScale;
    public static Vector2 JitterScaleToUpscale => _settings.JitterScaleToUpscale;

    public static bool DebugSkipPostProcessing => _settings.DebugSkipPostProcessing;

    public static bool Load(UnityModManager.ModEntry modEntry) {
        _settings = EGSettings.Load<EGSettings>(modEntry);

#if DEBUG
        modEntry.OnUnload = OnUnload;
#endif

        modEntry.OnGUI = OnGUI;
        modEntry.OnSaveGUI = OnSaveGUI;
        modEntry.OnUpdate = OnUpdate;
        modEntry.OnLateUpdate = OnLateUpdate;

        _dlss = new DlssUpscaler();
        _harmony = new Harmony(modEntry.Info.Id);
        _harmony.PatchAll(Assembly.GetExecutingAssembly());

        return true;
    }

    [Conditional("DEBUG")]
    public static void DebugPrint(string str) {
        NativeInterop.DebugPrint($"{str}\n");
    }

    private static void OnGUI(UnityModManager.ModEntry modEntry) {
        GUILayout.BeginVertical();

        GUILayout.BeginHorizontal();
        GUILayout.Label("Upscaler Type");
        List<string> upscalerTypeOptions = ["Vanilla", nameof(DlssUpscaler)];
        int currentUpscalerIdx = upscalerTypeOptions.IndexOf(_settings.SelectedUpscaler);
        int upscalerIdx = GUILayout.SelectionGrid(currentUpscalerIdx, [.. upscalerTypeOptions], 1);
        _settings.SelectedUpscaler = upscalerTypeOptions[upscalerIdx];
        GUILayout.EndHorizontal();

        if (Upscaler != null) {
            GUILayout.BeginHorizontal();
            GUILayout.Label("Upscaler Preset");
            List<string> upscalerPresetOptions = [.. Presets.Select(p => p.Name)];
            List<string> upscalerPresetLabels = [.. Presets.Select(p =>
                $"{p.Name.Replace("NVSDK_NGX_PerfQuality_Value_", "")} " +
                $"{p.RenderResolution.x}x{p.RenderResolution.y} " +
                $"({Math.Round(p.Ratio, 3):f3})")
            ];
            int currentPresetIdx = upscalerPresetOptions.IndexOf(Preset.Name);
            int presetIdx = GUILayout.SelectionGrid(currentPresetIdx, [.. upscalerPresetLabels], 1);
            _settings.SelectedPreset[_settings.SelectedUpscaler] = upscalerPresetOptions[presetIdx];
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();

            GUILayout.BeginVertical();
            GUILayout.Label("Width");
            string newWidthAsString = GUILayout.TextField(GUILayout.TextField(((int)Preset.RenderResolution.x).ToString()));
            GUILayout.EndVertical();

            if (!int.TryParse(newWidthAsString, out int newWidth)) {
                newWidth = (int)Preset.RenderResolution.x;
            }

            newWidth = Math.Max(Math.Min(newWidth, Screen.width), (int)(Screen.width * WaaaghPipeline.MinRenderScale));

            GUILayout.BeginVertical();
            GUILayout.Label("Height");
            string newHeightAsString = GUILayout.TextField(GUILayout.TextField(((int)Preset.RenderResolution.y).ToString()));
            GUILayout.EndVertical();


            if (!int.TryParse(newHeightAsString, out int newHeight)) {
                newHeight = (int)Preset.RenderResolution.y;
            }

            newHeight = Math.Max(Math.Min(newHeight, Screen.height), (int)(Screen.height * WaaaghPipeline.MinRenderScale));

            GUILayout.BeginVertical();
            GUILayout.Label("Ratio");
            string newRatioAsString = GUILayout.TextField(GUILayout.TextField(Preset.Ratio.ToString()));
            GUILayout.EndVertical();

            GUILayout.EndHorizontal();

            if (!float.TryParse(newRatioAsString, out float newRatio)) {
                newRatio = Preset.Ratio;
            }

            newRatio = Mathf.Clamp(newRatio, WaaaghPipeline.MinRenderScale, 1.0f);

            GUILayout.BeginHorizontal();
            float newRatioSlider = GUILayout.HorizontalSlider(Preset.Ratio, WaaaghPipeline.MinRenderScale, 1.0f);
            GUILayout.EndHorizontal();

            UpscalePreset preset = Preset;

            if (preset.RenderResolution.x != newWidth) {
                float aspect = preset.RenderResolution.x / preset.RenderResolution.y;
                preset = GetCustomPreset(newWidth, (int)(newWidth / aspect));
            } else if (preset.RenderResolution.y != newHeight) {
                float aspect = preset.RenderResolution.x / preset.RenderResolution.y;
                preset = GetCustomPreset((int)(newHeight * aspect), newHeight);
            } else if (!Mathf.Approximately(preset.Ratio, newRatio)) {
                preset = GetCustomPreset(newRatio);
            } else if (!Mathf.Approximately(preset.Ratio, newRatioSlider)) {
                preset = GetCustomPreset(newRatioSlider);
            }

            if (preset.Name == "Custom") {
                _settings.CustomPreset[_settings.SelectedUpscaler] = preset;
                _settings.SelectedPreset[_settings.SelectedUpscaler] = preset.Name;
            }

            GUILayout.BeginHorizontal();
            GUILayout.Label("Mvec Scale To Upscale");
            GUILayout.Label($"X: {_settings.MvecScaleToUpscale.x:F2}");
            _settings.MvecScaleToUpscale.x = GUILayout.HorizontalSlider(_settings.MvecScaleToUpscale.x, -5.0f, 5.0f);
            GUILayout.Label($"Y: {_settings.MvecScaleToUpscale.y:F2}");
            _settings.MvecScaleToUpscale.y = GUILayout.HorizontalSlider(_settings.MvecScaleToUpscale.y, -5.0f, 5.0f);
            if (GUILayout.Button("Reset")) {
                _settings.MvecScaleToUpscale = -Vector2.one;
            }
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            GUILayout.Label("Jitter Scale");
            GUILayout.Label($"X: {_settings.JitterScale.x:F2}");
            _settings.JitterScale.x = GUILayout.HorizontalSlider(_settings.JitterScale.x, -5.0f, 5.0f);
            GUILayout.Label($"Y: {_settings.JitterScale.y:F2}");
            _settings.JitterScale.y = GUILayout.HorizontalSlider(_settings.JitterScale.y, -5.0f, 5.0f);
            if (GUILayout.Button("Reset")) {
                _settings.JitterScale = Vector2.one * 2;
            }
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            GUILayout.Label("Jitter Scale To Upscale");
            GUILayout.Label($"X: {_settings.JitterScaleToUpscale.x:F2}");
            _settings.JitterScaleToUpscale.x = GUILayout.HorizontalSlider(_settings.JitterScaleToUpscale.x, -5.0f, 5.0f);
            GUILayout.Label($"Y: {_settings.JitterScaleToUpscale.y:F2}");
            _settings.JitterScaleToUpscale.y = GUILayout.HorizontalSlider(_settings.JitterScaleToUpscale.y, -5.0f, 5.0f);
            if (GUILayout.Button("Reset")) {
                _settings.JitterScaleToUpscale = Vector2.one;
            }
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            GUILayout.Label("Flags");
            List<string> flagOptions = [.. Enum.GetNames(typeof(UpscaleFlags))];

            foreach (string flag in Enum.GetNames(typeof(UpscaleFlags)).Skip(1)) {
                UpscaleFlags flagValue = (UpscaleFlags)Enum.Parse(typeof(UpscaleFlags), flag);
                bool flagEnabled = _settings.Flags.HasFlag(flagValue);
                bool newFlagEnabled = GUILayout.Toggle(flagEnabled, flag);
                if (newFlagEnabled != flagEnabled) {
                    if (newFlagEnabled) {
                        _settings.Flags |= flagValue;
                    } else {
                        _settings.Flags &= ~flagValue;
                    }
                }
            }

            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            GUILayout.Label($"Global Mip Bias Offset: {_settings.GlobalMipBiasOffset:F2} (resolved: {GlobalMipBias.x:F2})");
            _settings.GlobalMipBiasOffset = GUILayout.HorizontalSlider(_settings.GlobalMipBiasOffset, -5.0f, 5.0f);
            if (GUILayout.Button("Reset")) {
                _settings.GlobalMipBiasOffset = 0;
            }
            GUILayout.EndHorizontal();
        }

        GUILayout.BeginHorizontal();
        GUILayout.Label("Debug Skip Post Processing");
        _settings.DebugSkipPostProcessing = GUILayout.Toggle(_settings.DebugSkipPostProcessing, "");
        GUILayout.EndHorizontal();

        GUILayout.EndVertical();
    }

    private static void OnSaveGUI(UnityModManager.ModEntry modEntry) {
        _settings.Save(modEntry);
    }

    private static void OnUpdate(UnityModManager.ModEntry modEntry, float dt) {
        if (Upscaler == null) {
            if (_upscalerLastFrame != null) {
                Util.GraphicsSettings.ApplyGraphicsSettings();
            }

            return;
        }

        Upscaler.SetPreset(Preset, _settings.Flags);

        if (Util.RenderSettings != null) {
            Util.RenderSettings.RenderScale = Preset.Ratio;
        }
    }

    private static void OnLateUpdate(UnityModManager.ModEntry modEntry, float dt) {
        _upscalerLastFrame = Upscaler;
    }

#if DEBUG
    public static bool OnUnload(UnityModManager.ModEntry modEntry) {
        _harmony.UnpatchAll(modEntry.Info.Id);
        return true;
    }
#endif

    private static EGSettings _settings;
    private static DlssUpscaler _dlss;
    private static Harmony _harmony;
    private static IUpscaler _upscalerLastFrame;
    private static Vector2 _globalMipBias;

    [SaveOnReload]
    private static TextureHandle _cameraColorUpscaled;

    private static UpscalePreset GetSelectedPreset(string upscaler) {
        if (upscaler != null) {
            UpscalePreset preset = default;
            if (_settings.SelectedPreset.TryGetValue(upscaler, out string presetName)) {
                preset = Presets.FirstOrDefault(p => p.Name == presetName);
            }
            return preset == default ? Presets.First() : preset;
        }

        return GetVanillaPreset();
    }

    private static UpscalePreset GetVanillaPreset() => new("Vanilla", new(Screen.width, Screen.height), new(Screen.width, Screen.height));

    private static UpscalePreset GetCustomPreset(float ratio) {
        Vector2 exactMatch = new(Screen.width * ratio, Screen.height * ratio);
        Vector2 roundedDown = new(Mathf.Floor(Screen.width * ratio), Mathf.Floor(Screen.height * ratio));
        Vector2 roundedUp = new(Mathf.Ceil(Screen.width * ratio), Mathf.Ceil(Screen.height * ratio));
        Vector2[] resolutions = [roundedDown, new(roundedDown.x, roundedUp.y), new(roundedUp.x, roundedDown.y), roundedUp];

        Vector2 displayResolution = new(Screen.width, Screen.height);
        Vector2 renderResolution = resolutions.OrderBy(r => Mathf.Abs(r.x / r.y - Screen.width / (float)Screen.height)).First();

        return new("Custom", renderResolution, displayResolution);
    }

    private static UpscalePreset GetCustomPreset(float width, float height) {
        Vector2 displayResolution = new(Screen.width, Screen.height);
        Vector2 renderResolution = new(width, height);

        return new("Custom", renderResolution, displayResolution);
    }
}
