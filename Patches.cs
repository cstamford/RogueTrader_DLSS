using HarmonyLib;
using Kingmaker.Visual;
using Owlcat.Runtime.Visual.Waaagh;
using Owlcat.Runtime.Visual.Waaagh.Passes;
using Owlcat.Runtime.Visual.Waaagh.Passes.PostProcess;
using System;
using System.Collections.Generic;
using System.Reflection.Emit;
using UnityEngine;
using UnityEngine.Experimental.Rendering.RenderGraphModule;
using UnityEngine.Rendering;

using static DLSS.NativeInterop;

namespace DLSS;

[HarmonyPatch]
public static class PatchCustomUpscalePass {
    [HarmonyPostfix]
    [HarmonyPatch(typeof(WaaaghRenderer), nameof(WaaaghRenderer.Setup))]
    private static void WaaaghRenderer_Setup(WaaaghRenderer __instance, ScriptableRenderContext context, in RenderingData renderingData) {
        List<CameraStackManager.CameraInfo> camInfo = [];
        CameraStackManager.Instance.GetStack(camInfo);

        Camera ourCamera = renderingData.CameraData.Camera;
        int camIdx = camInfo.FindIndex(x => x.camera == ourCamera);

        if (camIdx == -1 ||
            (camInfo[camIdx].cameraStackType & CameraStackManager.CameraStackType.Main) == 0 ||
            !camInfo[camIdx].additionalCameraData.AllowRenderScaling) {
            return;
        }

        _upscalePass ??= new(RenderPassEvent.BeforeRenderingPostProcessing - 1);

        if (CustomRenderState.UpscaleConfig.UpscaleType != UpscaleType.None) {
            __instance.EnqueuePass(_upscalePass);
        }
    }

    [HarmonyPostfix]
    [HarmonyPatch(typeof(FinalBlitPass), nameof(FinalBlitPass.Setup))]
    private static void FinalBlitPass_Setup(ref FinalBlitPass.PassData data) {
        if (CustomRenderState.UpscaleConfig.UpscaleType != UpscaleType.None && data.IntermediateBlitType == FinalBlitPass.IntermediateBlitType.Easu) {
            data.IntermediateBlitType = FinalBlitPass.IntermediateBlitType.NearestNeighbour;
        }
    }

    private static UpscalePass _upscalePass;
}

[HarmonyPatch]
public static class PatchCameraColorBufferAccess {
    private static int _currentCameraIndex;
    private static bool _currentCameraShouldBeScaled;
    private static int _upscaleDispatchCameraIdx;

    [HarmonyTranspiler]
    [HarmonyPatch(typeof(ScriptableRenderer), nameof(ScriptableRenderer.Execute))]
    private static IEnumerable<CodeInstruction> ScriptableRenderer_Execute(IEnumerable<CodeInstruction> instructions) {
        foreach (CodeInstruction inst in instructions) {
            if (inst.Calls(AccessTools.Method(typeof(ScriptableRenderPass), nameof(ScriptableRenderPass.Execute)))) {
                yield return new CodeInstruction(OpCodes.Call, AccessTools.Method(typeof(PatchCameraColorBufferAccess), nameof(ScriptableRenderer_Execute_Impl)));
            } else {
                yield return inst;
            }
        }
    }

    private static void ScriptableRenderer_Execute_Impl(ScriptableRenderPass pass, ref RenderingData data) {
        if (CustomRenderState.UpscaleConfig.UpscaleType == UpscaleType.None) {
            pass.Execute(ref data);
            return;
        }

        // Cam0 (Background) -> [ work..., FinalBlitPass ]
        // Cam1 (Main) -> [ work..., UpscaleDispatchPass, postprocess, FinalBlitPass ]
        //                                   ^ 
        //       CameraColorScaled -> CameraColorUpscaled -> CameraColor
        // Cam2 (UI) -> [ work..., FinalBlitPass ]

        RenderGraphResources resources = data.CameraData.Renderer.RenderGraphResources;
        bool haveRunUpscaleDispatch = _upscaleDispatchCameraIdx != -1;

        if (haveRunUpscaleDispatch) { // Resources should draw from original CameraColor (now copied over).
            resources.CameraColorBuffer = resources.m_CameraNonScaledColorBuffer;
        } else { // Leave resources alone - they're already set up OK.
            Debug.Assert(
                (_currentCameraShouldBeScaled && resources.CameraColorBuffer.handle == resources.m_CameraScaledColorBuffer.handle) ||
                (!_currentCameraShouldBeScaled && resources.CameraColorBuffer.handle == resources.m_CameraNonScaledColorBuffer.handle)
            );
        }

        pass.Execute(ref data);

        if (pass is UpscalePass) {
            _upscaleDispatchCameraIdx = _currentCameraIndex;
        }
    }

    [HarmonyPrefix]
    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.RenderCameraStack))]
    // Start-of-frame reset (back to scaled buffer).
    private static void WaaaghPipeline_RenderCameraStack(WaaaghPipeline __instance) {
        _currentCameraIndex = -1;
        _currentCameraShouldBeScaled = false;
        _upscaleDispatchCameraIdx = -1;
    }

    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.InitializeCameraData))]
    // First camera only.
    private static class WaaaghPipeline_InitializeCameraData {
        [HarmonyPrefix]
        private static void Before(Camera camera, WaaaghAdditionalCameraData additionalCameraData, int cameraIndex) {
            Debug.Assert(cameraIndex == 0);
            _currentCameraIndex = cameraIndex;
            _currentCameraShouldBeScaled = additionalCameraData.AllowRenderScaling;

            if (Time.frameCount >= CustomRenderState.NextConfigChangeFrameIdx && (
                CustomRenderState.UpscaleConfig == null ||
                CustomRenderState.UpscaleConfig.UpscaleType != Main.Settings.UpscaleType ||
                !Mathf.Approximately(CustomRenderState.UpscaleConfig.UpscaleRatio, Main.Settings.UpscaleRatio)
            )) {
                Vector2Int displayResolution = new((int)camera.pixelRect.width, (int)camera.pixelRect.height);
                Vector2Int renderResolution = new((int)(displayResolution.x * Main.Settings.UpscaleRatio), (int)(displayResolution.y * Main.Settings.UpscaleRatio));

                if (Main.Settings.UpscaleType != UpscaleType.None && Main.Settings.UpscaleType != UpscaleType.Dlss) {
                    Debug.LogError($"Unsupported upscale type {Main.Settings.UpscaleType}, reverting to {UpscaleType.None}");
                    Main.Settings.UpscaleType = UpscaleType.None;
                }

                switch (Main.Settings.UpscaleType) {
                    case UpscaleType.None:
                        CustomRenderState.UpscaleConfig = new VanillaUpscaleConfiguration(renderResolution, displayResolution);
                        break;
                    case UpscaleType.Dlss:
                        DlssUpscaleConfiguration dlss = new(renderResolution, displayResolution);
                        DlssSetQualityMode(dlss.Mode);
                        CustomRenderState.UpscaleConfig = dlss;
                        break;
                    default:
                        throw new();
                }

                WaaaghPipeline.Asset.RenderScale = CustomRenderState.UpscaleConfig.UpscaleRatio;
                CustomRenderState.NextConfigChangeFrameIdx = Time.frameCount + 5;
            }
        }

        [HarmonyPostfix]
        private static void After(in CameraData cameraData, RenderGraph ___m_RenderGraph) {
            TextureDesc desc = Util.CreateColorTargetDesc("CameraColorUpscaled", cameraData.CameraTargetDescriptor, CustomRenderState.UpscaleConfig.DisplayResolution.x, CustomRenderState.UpscaleConfig.DisplayResolution.y);

            if (!CustomRenderState.UpscaleCameraColor.IsValid()) {
                CustomRenderState.UpscaleCameraColor = ___m_RenderGraph.m_Resources.CreateSharedTexture(desc, explicitRelease: true);
                _lastUpscaleTextureDesc = desc;
            }

            if (_lastUpscaleTextureDesc.width != desc.width || _lastUpscaleTextureDesc.height != desc.height) {
                ___m_RenderGraph.m_Resources.RefreshSharedTextureDesc(CustomRenderState.UpscaleCameraColor, desc);
                _lastUpscaleTextureDesc = desc;

            }
        }

        private static TextureDesc _lastUpscaleTextureDesc;
    }

    [HarmonyPostfix]
    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.InitializeAdditionalCameraData))]
    // All cameras beyond first.
    private static void WaaaghPipeline_InitializeAdditionalCameraData(WaaaghAdditionalCameraData additionalCameraData, int cameraIndex) {
        Debug.Assert(cameraIndex > 0);
        _currentCameraIndex = cameraIndex;
        _currentCameraShouldBeScaled = additionalCameraData.AllowRenderScaling;
    }
}

[HarmonyPatch]
public static class PatchFullResolutionPostProcessing {
    [HarmonyTranspiler]
    [HarmonyPatch(typeof(PostProcessPass), nameof(PostProcessPass.RecordRenderGraph))]
    private static IEnumerable<CodeInstruction> PostProcessPass_RecordRenderGraph(IEnumerable<CodeInstruction> instructions) {
        foreach (CodeInstruction inst in instructions) {
            if (inst.StoresField(AccessTools.Field(typeof(PostProcessPass), nameof(PostProcessPass.m_Desc)))) {
                yield return new(OpCodes.Call, AccessTools.Method(typeof(PatchFullResolutionPostProcessing), nameof(ModifyPostProcessingDesc)));
            }

            yield return inst;
        }
    }

    private static TextureDesc ModifyPostProcessingDesc(TextureDesc current) =>
        CustomRenderState.UpscaleConfig.UpscaleType == UpscaleType.None
            ? current
            : current with {
                width = CustomRenderState.UpscaleConfig.DisplayResolution.x,
                height = CustomRenderState.UpscaleConfig.DisplayResolution.y
            };
}

[HarmonyPatch]
public static class PatchJitter {
    [HarmonyPrefix]
    [HarmonyPatch(typeof(WaaaghCameraBuffer), nameof(WaaaghCameraBuffer.UpdateJitterMatrix))]
    private static bool WaaaghCameraBuffer_UpdateJitterMatrix(WaaaghCameraBuffer __instance) {
        if (CustomRenderState.UpscaleConfig == null || CustomRenderState.UpscaleConfig.UpscaleType == UpscaleType.None) {
            return true;
        }

        __instance.m_Jitter = Vector2.zero;
        __instance.m_JitterUV = Vector2.zero;
        __instance.JitterMatrix = Matrix4x4.identity;

        if (__instance.Camera.GetWaaaghAdditionalCameraData().AllowRenderScaling) {
            Vector2Int renderResolution = CustomRenderState.UpscaleConfig.RenderResolution;
            float upscaleRatio = 1.0f / CustomRenderState.UpscaleConfig.UpscaleRatio;

            int basePhaseCount = 8;
            int totalPhases = (int)(basePhaseCount * Math.Pow(upscaleRatio, 2));
            int index = (Time.frameCount % totalPhases) + 1;

            __instance.m_Jitter = new(HaltonSequence.Get(index, 2) - 0.5f, HaltonSequence.Get(index, 3) - 0.5f);
            __instance.m_JitterUV = __instance.m_Jitter * new Vector2(2.0f / renderResolution.x, 2.0f / renderResolution.y);
            __instance.JitterMatrix = Matrix4x4.Translate(new Vector3(__instance.m_JitterUV.x, __instance.m_JitterUV.y, 0));
        }

        return false;
    }
}
