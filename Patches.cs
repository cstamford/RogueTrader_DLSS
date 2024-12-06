using HarmonyLib;
using Owlcat.Runtime.Visual.Waaagh;
using Owlcat.Runtime.Visual.Waaagh.Passes;
using Owlcat.Runtime.Visual.Waaagh.Passes.Base;
using Owlcat.Runtime.Visual.Waaagh.Passes.PostProcess;
using Owlcat.Runtime.Visual.Waaagh.RendererFeatures.Highlighting.Passes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Emit;
using UnityEngine;
using UnityEngine.Experimental.Rendering.RenderGraphModule;
using UnityEngine.Rendering;

using static DLSS.NativeInterop;

namespace DLSS;

public static class CustomRenderState {
    public static UpscaleType UpscaleType = UpscaleType.Vanilla;
    public static float UpscaleRatio = 0.0f;
    public static TextureHandle UpscaleCameraColor;
}

[HarmonyPatch]
public static class PatchCustomUpscalePass {
    [HarmonyPostfix]
    [HarmonyPatch(typeof(WaaaghRenderer), nameof(WaaaghRenderer.Setup))]
    private static void WaaaghRenderer_Setup(WaaaghRenderer __instance, in RenderingData renderingData) {
        _upscalePass ??= new(RenderPassEvent.BeforeRenderingPostProcessing - 1, __instance.Settings.Shaders.FinalBlitShader);

        if (Util.CanApplyPipelineChanges(renderingData.CameraData)) {
            __instance.EnqueuePass(_upscalePass);

            // Ensure we have motion vectors (would otherwise be disabled due to TAA disabled).
            if (!__instance.m_ActiveRenderPassQueue.Any(x => x is CameraMotionVectorsPass)) {
                __instance.EnqueuePass(__instance.m_CameraMotionVectorsPass);
            }

            if (!__instance.m_ActiveRenderPassQueue.Any(x => x is ObjectMotionVectorsPass)) {
                __instance.EnqueuePass(__instance.m_ObjectMotionVectorsPass);
            }

            if (!__instance.m_ActiveRenderPassQueue.Any(x => x is CameraSetupPass)) {
                __instance.EnqueuePass(__instance.m_CameraSetupAfterTaa);
            }

            // Move highlighting pass to before upscale pass, preventing instability (and increasing quality).
            if (__instance.m_ActiveRenderPassQueue.FirstOrDefault(x => x is HighlighterPass) is HighlighterPass highlighter) {
                highlighter.RenderPassEvent = _upscalePass.RenderPassEvent - 1;
            }
        }
    }

    [HarmonyPostfix]
    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.CheckPostProcessForDepth))]
    private static void WaaaghPipeline_CheckPostProcessForDepth(in CameraData cameraData, ref bool __result) {
        __result = __result || Util.CanApplyPipelineChanges(cameraData);
    }

    [HarmonyPostfix]
    [HarmonyPatch(typeof(FinalBlitPass), nameof(FinalBlitPass.Setup))]
    private static void FinalBlitPass_Setup(ref FinalBlitPass.PassData data, ref RenderingData renderingData) {
        if (Util.CanApplyPipelineChanges(renderingData.CameraData) && data.IntermediateBlitType == FinalBlitPass.IntermediateBlitType.Easu) {
            data.IntermediateBlitType = FinalBlitPass.IntermediateBlitType.NearestNeighbour;
        }
    }

    private static UpscalePass _upscalePass;
}

[HarmonyPatch]
public static class PatchCameraColorBufferAccess {
    private static int _currentCameraIndex;
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
        if (!Util.CanApplyPipelineChanges(data.CameraData)) {
            pass.Execute(ref data);
            return;
        }

        // Cam0 (Background) -> [ work..., FinalBlitPass ]
        // Cam1 (Main) -> [ work..., UpscaleDispatchPass, postprocess, FinalBlitPass ]
        //                                   ^ 
        //       CameraColorScaled -> CameraColorUpscaled -> CameraColor
        // Cam2 (UI) -> [ work..., FinalBlitPass ]

        RenderGraphResources resources = data.CameraData.Renderer.RenderGraphResources;
        if (_upscaleDispatchCameraIdx != -1) { // Resources should draw from original CameraColor (now copied over).
            resources.CameraColorBuffer = resources.m_CameraNonScaledColorBuffer;
        } else { // Leave resources alone - they're already set up OK.
            Debug.Assert(
                (Util.CanScaleCamera(data.CameraData) && resources.CameraColorBuffer.handle == resources.m_CameraScaledColorBuffer.handle) ||
                (!Util.CanScaleCamera(data.CameraData) && resources.CameraColorBuffer.handle == resources.m_CameraNonScaledColorBuffer.handle)
            );
        }

        pass.Execute(ref data);

        if (pass is UpscalePass) {
            _upscaleDispatchCameraIdx = _currentCameraIndex;
        }
    }

    [HarmonyPrefix]
    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.RenderCameraStack))]
    private static void WaaaghPipeline_RenderCameraStack() {
        _currentCameraIndex = -1;
        _upscaleDispatchCameraIdx = -1;
    }

    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.InitializeCameraData))]
    private static class WaaaghPipeline_InitializeCameraData {
        [HarmonyPrefix]
        private static void Before(WaaaghPipeline __instance, int cameraIndex) {
            Debug.Assert(cameraIndex == 0);
            _currentCameraIndex = cameraIndex;
            _renderGraph = __instance.m_RenderGraph;
            WaaaghPipeline.Asset.RenderScale = Main.Settings.UpscaleRatio;
        }

        [HarmonyPostfix]
        private static void After(ref CameraData cameraData) {
            if (!Util.CanScaleCamera(cameraData)) {
                return;
            }

            int renderWidth = cameraData.ScaledCameraTargetViewportSize.x;
            int renderHeight = cameraData.ScaledCameraTargetViewportSize.y;
            int displayWidth = cameraData.NonScaledCameraTargetViewportSize.x;
            int displayHeight = cameraData.NonScaledCameraTargetViewportSize.y;

            bool needReapply = CustomRenderState.UpscaleType != Main.Settings.UpscaleType;
            needReapply |= CustomRenderState.UpscaleRatio != cameraData.RenderScale;

            if (needReapply) {
                CustomRenderState.UpscaleType = Main.Settings.UpscaleType;
                CustomRenderState.UpscaleRatio = cameraData.RenderScale;

                if (CustomRenderState.UpscaleType == UpscaleType.Dlss) {
                    DlssSetQualityMode(new() {
                        InputWidth = (uint)renderWidth,
                        InputHeight = (uint)renderHeight,
                        FinalWidth = (uint)displayWidth,
                        FinalHeight = (uint)displayHeight
                    });
                }
            }

            if (Util.CanApplyPipelineChanges(cameraData)) {
                cameraData.Antialiasing = AntialiasingMode.None;
                TextureDesc desc = Util.CreateColorTargetDesc("CameraColorUpscaled", cameraData.CameraTargetDescriptor, displayWidth, displayHeight);
                CustomRenderState.UpscaleCameraColor = EnsureUpscaleTexture(desc);
            }
        }

        private static TextureHandle EnsureUpscaleTexture(TextureDesc desc) {
            if (!_lastUpscaleTextureDesc.TryGetValue(desc.name, out (TextureHandle Handle, TextureDesc Desc) current)) {
                current = (_renderGraph.m_Resources.CreateSharedTexture(desc, explicitRelease: true), desc);
                _lastUpscaleTextureDesc[desc.name] = current;
            }

            if (current.Desc.width != desc.width || current.Desc.height != desc.height) {
                _renderGraph.m_Resources.RefreshSharedTextureDesc(current.Handle, desc);
                current = (current.Handle, desc);
                _lastUpscaleTextureDesc[desc.name] = current;
            }

            Debug.Assert(current.Handle.IsValid());
            Debug.Assert(current.Desc.width == desc.width);
            Debug.Assert(current.Desc.height == desc.height);

            return current.Handle;
        }

        private readonly static Dictionary<string, (TextureHandle, TextureDesc)> _lastUpscaleTextureDesc = [];
        private static RenderGraph _renderGraph;
    }

    [HarmonyPostfix]
    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.InitializeAdditionalCameraData))]
    private static void WaaaghPipeline_InitializeAdditionalCameraData(WaaaghAdditionalCameraData additionalCameraData, int cameraIndex) {
        Debug.Assert(cameraIndex > 0);
        _currentCameraIndex = cameraIndex;
    }
}

[HarmonyPatch]
public static class PatchFullResolutionPostProcessing {
    [HarmonyTranspiler]
    [HarmonyPatch(typeof(PostProcessPass), nameof(PostProcessPass.RecordRenderGraph))]
    private static IEnumerable<CodeInstruction> PostProcessPass_RecordRenderGraph(IEnumerable<CodeInstruction> instructions) {
        foreach (CodeInstruction inst in instructions) {
            if (inst.StoresField(AccessTools.Field(typeof(PostProcessPass), nameof(PostProcessPass.m_Desc)))) {
                yield return new(OpCodes.Ldarg_1);
                yield return new(OpCodes.Call, AccessTools.Method(typeof(PatchFullResolutionPostProcessing), nameof(ModifyPostProcessingDesc)));
            }

            yield return inst;
        }
    }

    private static TextureDesc ModifyPostProcessingDesc(TextureDesc current, in RenderingData renderingData) =>
        Util.CanApplyPipelineChanges(renderingData.CameraData) ? current with {
            width = renderingData.CameraData.NonScaledCameraTargetViewportSize.x,
            height = renderingData.CameraData.NonScaledCameraTargetViewportSize.y
        } : current;
}

[HarmonyPatch]
public static class PatchJitter {

    [HarmonyTranspiler]
    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.InitializeAdditionalCameraData))]
    private static IEnumerable<CodeInstruction> WaaaghPipeline_InitializeAdditionalCameraData(IEnumerable<CodeInstruction> instructions) {
        CodeInstruction mostRecentLdargS = null;

        foreach (CodeInstruction inst in instructions) {
            if (inst.opcode == OpCodes.Ldarg_S) {
                mostRecentLdargS = inst;
            }

            if (inst.Calls(AccessTools.Method(typeof(WaaaghCameraBuffer), nameof(WaaaghCameraBuffer.Update)))) {
                yield return mostRecentLdargS; // load cameraData
                yield return new CodeInstruction(OpCodes.Call, AccessTools.Method(typeof(PatchJitter), nameof(UpdateCameraBufferWithCameraData)));
            } else {
                yield return inst;
            }
        }
    }

    private static void UpdateCameraBufferWithCameraData(WaaaghCameraBuffer buffer, in CameraData cameraData) {
        buffer.Update();

        if (Util.CanApplyPipelineChanges(cameraData)) {
            int basePhaseCount = 8;
            int totalPhases = (int)(basePhaseCount * Math.Pow(1 / cameraData.RenderScale, 2));
            int index = (Time.frameCount % totalPhases) + 1;

            Vector2Int renderResolution = cameraData.ScaledCameraTargetViewportSize;

            buffer.m_Jitter = new(HaltonSequence.Get(index, 2) - 0.5f, HaltonSequence.Get(index, 3) - 0.5f);
            buffer.m_JitterUV = buffer.m_Jitter * new Vector2(2.0f / renderResolution.x, 2.0f / renderResolution.y);
            buffer.JitterMatrix = Matrix4x4.Translate(new Vector3(buffer.m_JitterUV.x, buffer.m_JitterUV.y, 0));
        }
    }
}
