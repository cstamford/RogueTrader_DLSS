using EnhancedGraphics.Upscalers;
using Owlcat.Runtime.Visual.Waaagh;
using Owlcat.Runtime.Visual.Waaagh.Passes;
using Owlcat.Runtime.Visual.Waaagh.Passes.PostProcess;
using UnityEngine;
using UnityEngine.Assertions;
using UnityEngine.Experimental.Rendering.RenderGraphModule;
using UnityEngine.Rendering;

namespace EnhancedGraphics.Game;

public class UpscalePass(RenderPassEvent evt, Shader backupBlitShader) : ScriptableRenderPass<UpscalePass.PassData>(evt) {
    public override string Name => nameof(UpscalePass);

    public class PassData : PassDataBase {
        public IUpscaler InputUpscaler;
        public Vector2 InputRenderResolution;
        public Vector2 InputDisplayResolution;

        public TextureHandle InputColor;
        public TextureHandle InputDepth;
        public TextureHandle InputMvec;
        public TextureHandle InputOutputIntermediate;
        public TextureHandle OutputColor;

        public Vector2 InputJitter;
        public Vector2 InputMvecScale;
    }

    public override void Setup(RenderGraphBuilder builder, PassData data, ref RenderingData renderingData) {
        data.InputUpscaler = EnhancedGraphics.Upscaler;
        data.InputRenderResolution = renderingData.CameraData.ScaledCameraTargetViewportSize;
        data.InputDisplayResolution = renderingData.CameraData.NonScaledCameraTargetViewportSize;

        TextureHandle inputColorHandle = builder.ReadTexture(data.Resources.CameraColorBuffer);
        TextureHandle inputDepthHandle = builder.UseDepthBuffer(data.Resources.CameraDepthBuffer, DepthAccess.Read);
        TextureHandle inputMvecHandle = builder.ReadTexture(data.Resources.CameraMotionVectorsRT);
        TextureHandle inputOutputIntermediateHandle = builder.ReadWriteTexture(EnhancedGraphics.CameraColorUpscaled);
        TextureHandle outputColorHandle = builder.WriteTexture(data.Resources.CameraResolveColorBuffer);

        Assert.IsTrue(inputColorHandle.IsValid());
        Assert.IsTrue(inputOutputIntermediateHandle.IsValid());
        Assert.IsTrue(outputColorHandle.IsValid());

        data.InputColor = inputColorHandle;
        data.InputDepth = inputDepthHandle;
        data.InputMvec = inputMvecHandle;
        data.InputOutputIntermediate = inputOutputIntermediateHandle;
        data.OutputColor = outputColorHandle;

        data.InputJitter = renderingData.CameraData.CameraBuffer.Jitter * EnhancedGraphics.JitterScaleToUpscale;
        data.InputMvecScale = data.InputRenderResolution * EnhancedGraphics.MvecScaleToUpscale;
    }

    public override void Render(PassData data, RenderGraphContext context) {
        RenderTexture inputColorRt = data.InputColor;
        RenderTexture inputDepthRt = data.InputDepth;
        RenderTexture inputMvecRt = data.InputMvec;
        RenderTexture inputOutputRt = data.InputOutputIntermediate;
        RenderTexture outputRt = data.OutputColor;

        // How this works:
        //
        // 1. We are immediately calling DlssEvaluate, which, on C++ side, will 'queue' our evaluate operation.
        //    It needs to be queued because command buffer is actually being played back on a different thread at a later time.
        // 2. On C++ side, we hook OMSetRenderTargets.
        //    Once we see data.InputOutputIntermediate has been set as the render target, we know it's time to execute the upscale.
        // 3. We do an extra blit here (from intermediate -> outputcolor) to force Render Graph to play nicely with us.
        //    If we don't, we'll see Render Graph optimizing render targets such that our pass is never executed/ignored.
        //    There might be a better way to do this (you can see my attempts with IncrementUpdateCount) to save a bit of perf.

        UpscaleOptionalParams param = new() {
            Depth = inputDepthRt.GetNativeDepthBufferPtr(),
            Mvec = inputMvecRt.GetNativeTexturePtr(),
            Jitter = data.InputJitter,
            MvecScale = data.InputMvecScale,
            Reset = false
        };

        bool evaluated = data.InputUpscaler.Evaluate(
            inputColorRt.GetNativeTexturePtr(),
            inputOutputRt.GetNativeTexturePtr(),
            0.0f,
            param
        );

        context.cmd.SetRenderTarget(inputOutputRt);
        context.cmd.IncrementUpdateCount(inputOutputRt);

        if (!evaluated) {
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

        context.cmd.Blit(inputOutputRt, outputRt);
    }

    private readonly Material _backupBlitMaterial = CoreUtils.CreateEngineMaterial(backupBlitShader);
}
