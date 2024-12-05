using Owlcat.Runtime.Visual.Waaagh;
using Owlcat.Runtime.Visual.Waaagh.Passes;
using System;
using UnityEngine;
using UnityEngine.Experimental.Rendering.RenderGraphModule;

using static DLSS.NativeInterop;

namespace DLSS;

public class UpscalePass(RenderPassEvent evt) : ScriptableRenderPass<UpscalePass.PassData>(evt) {
    public override string Name => nameof(UpscalePass);

    public class PassData : PassDataBase {
        public TextureHandle InputColor;
        public TextureHandle InputDepth;
        public TextureHandle InputMvec;
        public Vector2 InputJitter;
        public Vector2 InputMvecScale;
        public TextureHandle InputOutputIntermediate;
        public TextureHandle OutputColor;
    }

    public override void Setup(RenderGraphBuilder builder, PassData data, ref RenderingData renderingData) {
        _resources = renderingData.RenderGraph.m_Resources;
        _cameraBuffer = renderingData.CameraData.CameraBuffer;

        data.InputColor = builder.ReadTexture(data.Resources.m_CameraScaledColorBuffer);
        data.InputDepth = builder.UseDepthBuffer(data.Resources.m_CameraScaledDepthBuffer, DepthAccess.Read);

        if (data.Resources.CameraMotionVectorsRT.IsValid()) {
            data.InputMvec = builder.ReadTexture(data.Resources.CameraMotionVectorsRT);
        }

        data.InputJitter = _cameraBuffer.Jitter;
        data.InputMvecScale = -CustomRenderState.UpscaleConfig.RenderResolution;
        data.InputOutputIntermediate = builder.ReadWriteTexture(CustomRenderState.UpscaleCameraColor);
        data.OutputColor = builder.WriteTexture(data.Resources.m_CameraNonScaledColorBuffer);
    }

    public override void Render(PassData data, RenderGraphContext context) {
        RenderTexture inputColorRt = data.InputColor;
        RenderTexture inputDepthRt = data.InputDepth;
        RenderTexture inputMvecRt = data.InputMvec;
        RenderTexture inputOutputRt = data.InputOutputIntermediate;

        IntPtr inputColorRtPtr = inputColorRt.GetNativeTexturePtr();
        IntPtr inputDepthRtPtr = inputDepthRt.GetNativeDepthBufferPtr();
        IntPtr inputMvecRtPtr = inputMvecRt.GetNativeTexturePtr();
        IntPtr inputOutputRtPtr = inputOutputRt.GetNativeTexturePtr();

        Debug.Assert(inputColorRtPtr != null);
        Debug.Assert(inputDepthRtPtr != null);
        Debug.Assert(inputMvecRtPtr != null);
        Debug.Assert(inputOutputRtPtr != null);

        DlssEvaluate(inputColorRtPtr, inputOutputRtPtr, new() {
            DepthIn = inputDepthRtPtr,
            MvecIn = inputMvecRtPtr,
            JitterX = data.InputJitter.x,
            JitterY = data.InputJitter.y,
            MVecScaleX = data.InputMvecScale.x,
            MVecScaleY = data.InputMvecScale.y,
            Reset = false
        });

        context.cmd.SetRenderTarget(inputOutputRt);
        context.cmd.IncrementUpdateCount(inputOutputRt);
        context.cmd.Blit(inputOutputRt, data.OutputColor);
    }

    private RenderGraphResourceRegistry _resources;
    private WaaaghCameraBuffer _cameraBuffer;
}