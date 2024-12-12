using HarmonyLib;
using Owlcat.Runtime.Visual.Waaagh;
using Owlcat.Runtime.Visual.Waaagh.Passes;
using Owlcat.Runtime.Visual.Waaagh.Passes.Base;
using Owlcat.Runtime.Visual.Waaagh.Passes.PostProcess;
using Owlcat.Runtime.Visual.Waaagh.RendererFeatures.Highlighting.Passes;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Emit;
using UnityEngine;
using UnityEngine.Experimental.Rendering.RenderGraphModule;

namespace EnhancedGraphics.Game;

[HarmonyPatch]
public static class PatchCustomUpscalePass {
    [HarmonyPostfix]
    [HarmonyPatch(typeof(WaaaghRenderer), nameof(WaaaghRenderer.Setup))]
    private static void WaaaghRenderer_Setup(WaaaghRenderer __instance, in RenderingData renderingData) {
        _upscalePass ??= new(RenderPassEvent.BeforeRenderingPostProcessing - 20, __instance.Settings.Shaders.FinalBlitShader);

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

            if (EnhancedGraphics.DebugSkipPostProcessing) {
                __instance.m_ActiveRenderPassQueue.RemoveAll(x => x is PostProcessPass);
            }
        }
    }

    [HarmonyPostfix]
    [HarmonyPatch(typeof(RenderGraphResources), nameof(RenderGraphResources.ImportCameraData))]
    private static void RenderGraphResources_ImportCameraData(RenderGraphResources __instance, ref CameraData cameraData) {
        if (Util.CanApplyPipelineChanges(cameraData) && cameraData.RenderScale == 1) {
            __instance.CameraColorBuffer = __instance.m_CameraNonScaledColorBuffer;
            __instance.CameraDepthBuffer = __instance.m_CameraNonScaledDepthBuffer;
        }
    }

    [HarmonyPostfix]
    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.CheckPostProcessForDepth))]
    private static void WaaaghPipeline_CheckPostProcessForDepth(in CameraData cameraData, ref bool __result) {
        __result |= Util.CanApplyPipelineChanges(cameraData);
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
public static class PatchUpscaleResolution {
    [HarmonyPrefix]
    [HarmonyPatch(typeof(CameraData), nameof(CameraData.NonScaledCameraTargetViewportSize), MethodType.Getter)]
    private static bool CameraData__NonScaledCameraTargetViewportSize(in CameraData __instance, ref Vector2Int __result) {
        if (Util.CanApplyPipelineChanges(__instance)) {
            Vector2 display = EnhancedGraphics.Preset.DisplayResolution;
            __result = new((int)display.x, (int)display.y);
            return false;
        }

        return true;
    }

    [HarmonyPrefix]
    [HarmonyPatch(typeof(CameraData), nameof(CameraData.ScaledCameraTargetViewportSize), MethodType.Getter)]
    private static bool CameraData__ScaledCameraTargetViewportSize(in CameraData __instance, ref Vector2Int __result) {
        if (Util.CanApplyPipelineChanges(__instance)) {
            Vector2 render = EnhancedGraphics.Preset.RenderResolution;
            __result = new((int)render.x, (int)render.y);
            return false;
        }

        return true;
    }

    [HarmonyPostfix]
    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.InitializeCameraData))]
    private static void WaaaghPipeline_InitializeCameraData(RenderGraph ___m_RenderGraph, ref CameraData cameraData) {
        if (Util.CanApplyPipelineChanges(cameraData)) {
            cameraData.Antialiasing = AntialiasingMode.None;

            TextureDesc desc = Util.CreateColorTargetDesc("CameraColorUpscaled", cameraData.CameraTargetDescriptor, cameraData.NonScaledCameraTargetViewportSize);
            if (!EnhancedGraphics.CameraColorUpscaled.IsValid()) {
                EnhancedGraphics.CameraColorUpscaled = ___m_RenderGraph.CreateSharedTexture(desc, explicitRelease: true);
            }

            TextureDesc currentDesc = ___m_RenderGraph.GetTextureDesc(EnhancedGraphics.CameraColorUpscaled);
            if (desc.width != currentDesc.width || desc.height != currentDesc.height) {
                ___m_RenderGraph.RefreshSharedTextureDesc(EnhancedGraphics.CameraColorUpscaled, desc);
            }
        }
    }
}

[HarmonyPatch]
public static class PatchCameraColorBufferAccess {
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
#if false
        EnhancedGraphics.DebugPrint($"{data.CameraData.Camera.name} {pass.Name} _dispatchedUpscale={_dispatchedUpscale}");
#endif

        RenderGraphResources resources = data.CameraData.Renderer.RenderGraphResources;
        resources.CameraColorBuffer = _dispatchedUpscale ? resources.m_CameraNonScaledColorBuffer : resources.CameraColorBuffer;
        pass.Execute(ref data);
        _dispatchedUpscale |= pass is UpscalePass;
    }

    [HarmonyPrefix]
    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.RenderCameraStack))]
    private static void WaaaghPipeline_RenderCameraStack() {
        _dispatchedUpscale = false;
    }

    private static bool _dispatchedUpscale;
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
public static class PatchMipBias {
    [HarmonyPostfix]
    [HarmonyPatch(typeof(SetCameraShaderVariablesPass), nameof(SetCameraShaderVariablesPass.Setup))]
    public static void SetCameraShaderVariablesPass_Setup(SetCameraShaderVariablesPassData data, ref RenderingData renderingData) {
        if (Util.CanApplyPipelineChanges(renderingData.CameraData)) {
            float mipBias = Mathf.Log(renderingData.CameraData.ScaledCameraTargetViewportSize.x / renderingData.CameraData.NonScaledCameraTargetViewportSize.x, 2) - 1.0f;
            data.GlobalMipBias = new Vector2(mipBias, Mathf.Pow(2.0f, mipBias));
        }
    }
}

[HarmonyPatch]
public static class PatchUpscalingThreshold {
    [HarmonyTranspiler]
    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.InitializeAdditionalCameraData))]
    private static IEnumerable<CodeInstruction> WaaaghPipeline_InitializeAdditionalCameraData(IEnumerable<CodeInstruction> instructions) {
        List<CodeInstruction> insts = [.. instructions];

        for (int i = 0; i < insts.Count; ++i) {
            // find: 
            //  ldarg.s cameraData
            //  ldfld float32 Owlcat.Runtime.Visual.Waaagh.CameraData::RenderScale
            //  ldc.r4 1
            //  call bool [UnityEngine.CoreModule]UnityEngine.Mathf::Approximately(float32, float32)
            //
            // replace with:
            //  call bool EnhancedGraphics.PatchUpscalingThreshold::IsUpscalingThreshold(in Owlcat.Runtime.Visual.Waaagh.CameraData)

            if (insts[i].Calls(AccessTools.Method(typeof(Mathf), nameof(Mathf.Approximately))) &&
                insts[i - 1].opcode == OpCodes.Ldc_R4 && (float)insts[i - 1].operand == 1.0f &&
                insts[i - 2].LoadsField(AccessTools.Field(typeof(CameraData), nameof(CameraData.RenderScale)))) {
                int injectCallIdx = i - 2;
                insts[injectCallIdx] = new CodeInstruction(OpCodes.Call, AccessTools.Method(typeof(PatchUpscalingThreshold), nameof(IsUpscalingThreshold)));
                insts.RemoveRange(injectCallIdx + 1, 2);
                i = injectCallIdx + 1;
            }
        }

        return insts;
    }

    private static bool IsUpscalingThreshold(in CameraData cameraData) {
        return EnhancedGraphics.Upscaler == null && Mathf.Approximately(cameraData.RenderScale, 1.0f);
    }

    [HarmonyTranspiler]
    [HarmonyPatch(typeof(WaaaghPipeline), nameof(WaaaghPipeline.InitializeStackedCameraData))]
    private static IEnumerable<CodeInstruction> WaaaghPipeline_InitializeStackedCameraData(IEnumerable<CodeInstruction> instructions) {
        List<CodeInstruction> insts = [.. instructions];

        for (int i = 0; i < insts.Count; ++i) {
            // before: Mathf.Abs(1f - Asset.RenderScale) > 0.05f
            // after: Mathf.Abs(1f - Asset.RenderScale) >= UpscalingThreshold
            if (insts[i].opcode == OpCodes.Ble_Un_S &&
                insts[i - 1].opcode == OpCodes.Ldc_R4 &&
                (float)insts[i - 1].operand == 0.05f
            ) {
                insts[i].opcode = OpCodes.Blt_Un_S;
                insts[i - 1] = new CodeInstruction(OpCodes.Call, AccessTools.Method(typeof(PatchUpscalingThreshold), nameof(GetUpscalingThreshold)));
            }

            // before: cameraData.RenderScale < 1.0
            // after: cameraData.RenderScale <= 1.0
            if (insts[i].opcode == OpCodes.Blt_S && insts[i - 1].opcode == OpCodes.Ldc_R8 && (double)insts[i - 1].operand == 1.0) {
                insts[i].opcode = OpCodes.Ble_S;
            }
        }

        return insts;
    }

    private static float GetUpscalingThreshold() => EnhancedGraphics.Upscaler == null ? 0.05f : 0.0f;
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
            Vector2 displayResolution = cameraData.NonScaledCameraTargetViewportSize;
            Vector2Int renderResolution = cameraData.ScaledCameraTargetViewportSize;

            Jitter jitter = new(displayResolution, renderResolution, Time.frameCount);

            buffer.m_Jitter = jitter.Offset;
            buffer.m_JitterUV = jitter.OffsetUV;
            buffer.JitterMatrix = jitter.Matrix;
        }
    }

    private readonly record struct Jitter(
        Vector2 Offset,
        Vector2 OffsetUV,
        Matrix4x4 Matrix
    ) {
        public Jitter() : this(Vector2.zero, Vector2.zero, Matrix4x4.identity) { }
        public Jitter(Vector2 displayResolution, Vector2 renderResolution, int frameIdx) : this() {
            float scale = Scale(displayResolution, renderResolution);
            int phaseCount = PhaseCount(scale);
            int currentPhase = frameIdx % phaseCount;

            float x = Halton(currentPhase + 1, 2) - 0.5f;
            float y = Halton(currentPhase + 1, 3) - 0.5f;

            Debug.Assert(x >= -0.5f && x <= 0.5f);
            Debug.Assert(y >= -0.5f && y <= 0.5f);

            Offset = new Vector2(x, y) * EnhancedGraphics.JitterScale;
            OffsetUV = Offset * 2 / renderResolution;
            Matrix = Matrix4x4.Translate(OffsetUV);
        }

        private static float Scale(Vector2 displayResolution, Vector2 renderResolution) => displayResolution.x / renderResolution.x;
        private static int PhaseCount(float scale) => (int)(8 * Mathf.Pow(scale, 2));

        private static float Halton(int index, int basis) {
            float result = 0;
            float fraction = 1.0f;

            while (index > 0) {
                fraction /= basis;
                result += fraction * (index % basis);
                index /= basis;
            }

            return result;
        }
    }
}
