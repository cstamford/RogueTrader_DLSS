using Owlcat.Runtime.Visual.Waaagh;
using Owlcat.Runtime.Visual.Waaagh.Passes;
using Owlcat.Runtime.Visual.Waaagh.Passes.PostProcess;
using UnityEngine;
using UnityEngine.Experimental.Rendering.RenderGraphModule;
using UnityEngine.Rendering;

using static DLSS.NativeInterop;

namespace DLSS;

public class UpscalePass(RenderPassEvent evt, Shader backupBlitShader) : ScriptableRenderPass<UpscalePass.PassData>(evt) {
    public override string Name => nameof(UpscalePass);

    public class PassData : PassDataBase {
        public TextureHandle InputColor;
        public TextureHandle InputDepth;
        public TextureHandle InputMvec;
        public TextureHandle InputOutputIntermediate;
        public TextureHandle OutputColor;

        public Vector2 InputJitter;
        public Vector2 InputRenderResolution;
        public Vector2 InputDisplayResolution;
    }

    public override void Setup(RenderGraphBuilder builder, PassData data, ref RenderingData renderingData) {
        _cameraBuffer = renderingData.CameraData.CameraBuffer;
        _resources = renderingData.RenderGraph.m_Resources;

        TextureHandle inputColorHandle = builder.ReadTexture(data.Resources.m_CameraScaledColorBuffer);
        TextureHandle inputDepthHandle = data.Resources.m_CameraScaledDepthBuffer.IsValid() ? builder.UseDepthBuffer(data.Resources.m_CameraScaledDepthBuffer, DepthAccess.Read) : TextureHandle.nullHandle;
        TextureHandle inputMvecHandle = data.Resources.CameraMotionVectorsRT.IsValid() ? builder.ReadTexture(data.Resources.CameraMotionVectorsRT) : TextureHandle.nullHandle;
        TextureHandle inputOutputIntermediateHandle = builder.ReadWriteTexture(CustomRenderState.UpscaleCameraColor);
        TextureHandle outputColorHandle = builder.WriteTexture(data.Resources.m_CameraNonScaledColorBuffer);

        Debug.Assert(inputColorHandle.IsValid());
        Debug.Assert(inputOutputIntermediateHandle.IsValid());
        Debug.Assert(outputColorHandle.IsValid());

        data.InputColor = inputColorHandle;
        data.InputDepth = inputDepthHandle;
        data.InputMvec = inputMvecHandle;
        data.InputOutputIntermediate = inputOutputIntermediateHandle;
        data.OutputColor = outputColorHandle;

        data.InputJitter = _cameraBuffer.Jitter;
        data.InputRenderResolution = renderingData.CameraData.ScaledCameraTargetViewportSize;
        data.InputDisplayResolution = renderingData.CameraData.NonScaledCameraTargetViewportSize;

        Debug.Assert(data.InputRenderResolution != data.InputDisplayResolution, "We're in upscale pass but with no upscaling to do?");
    }

    public override void Render(PassData data, RenderGraphContext context) {
        RenderTexture inputColorRt = data.InputColor;
        RenderTexture inputDepthRt = data.InputDepth;
        RenderTexture inputMvecRt = data.InputMvec;
        RenderTexture inputOutputRt = data.InputOutputIntermediate;

        // How this works:
        //
        // 1. We are immediately calling DlssEvaluate, which, on C++ side, will 'queue' our evaluate operation.
        //    It needs to be queued because command buffer is actually being played back on a different thread at a later time.
        // 2. On C++ side, we hook OMSetRenderTargets.
        //    Once we see data.InputOutputIntermediate has been set as the render target, we know it's time to execute the upscale.
        // 3. We do an extra blit here (from intermediate -> outputcolor) to force Render Graph to play nicely with us.
        //    If we don't, we'll see Render Graph optimizing render targets such that our pass is never executed/ignored.
        //    There might be a better way to do this (you can see my attempts with IncrementUpdateCount) to save a bit of perf.
        // 4. Per NVIDIA, we do a nearest-neighbour upscale of the depth buffer for use by the now-full-resolution post-processing.

        bool evaluated = DlssEvaluate(inputColorRt.GetNativeTexturePtr(), inputOutputRt.GetNativeTexturePtr(), new() {
            DepthIn = inputDepthRt.GetNativeDepthBufferPtr(),
            MvecIn = inputMvecRt.GetNativeTexturePtr(),
            JitterX = data.InputJitter.x,
            JitterY = data.InputJitter.y,
            MVecScaleX = -data.InputRenderResolution.x,
            MVecScaleY = -data.InputRenderResolution.y,
            Reset = false
        });

        context.cmd.SetRenderTarget(inputOutputRt);
        context.cmd.IncrementUpdateCount(inputOutputRt);

        if (!evaluated) { // fall back to doing a bilinear blit
            FinalBlitter.Blit(
                context.cmd,
                _backupBlitMaterial,
                data.InputColor,
                data.InputOutputIntermediate,
                ColorSpace.Gamma,
                ColorSpace.Gamma,
                new(Vector2.zero, data.InputDisplayResolution),
                FinalBlitter.SamplerType.Bilinear
            );
        }

        context.cmd.Blit(inputOutputRt, data.OutputColor);
    }

    private readonly Material _backupBlitMaterial = CoreUtils.CreateEngineMaterial(backupBlitShader);
    private WaaaghCameraBuffer _cameraBuffer;
    private RenderGraphResourceRegistry _resources;
}
