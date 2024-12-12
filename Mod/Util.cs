using Kingmaker.Settings;
using Kingmaker.Settings.Graphics;
using Owlcat.Runtime.Visual;
using Owlcat.Runtime.Visual.Waaagh;
using Owlcat.Runtime.Visual.Waaagh.Data;
using UnityEngine;
using UnityEngine.Experimental.Rendering;
using UnityEngine.Experimental.Rendering.RenderGraphModule;
using UnityEngine.Rendering;

namespace EnhancedGraphics;

public static class Util {
    public static WaaaghPipelineAsset RenderSettings => WaaaghPipeline.Asset;
    public static GraphicsSettingsController GraphicsSettings => SettingsController.Instance?.GraphicsSettingsController;

    public static TextureDesc CreateColorTargetDesc(string name, RenderTextureDescriptor descriptor, Vector2Int size) {
        TextureDesc desc = RenderingUtils.CreateTextureDesc(name, descriptor);
        desc.width = size.x;
        desc.height = size.y;
        desc.depthBufferBits = DepthBits.None;
        desc.filterMode = FilterMode.Bilinear;
        desc.wrapMode = TextureWrapMode.Clamp;
        desc.enableRandomWrite = true;
        return desc;
    }

    public static TextureDesc CreateDepthTargetDesc(string name, RenderTextureDescriptor descriptor, Vector2Int size) {
        TextureDesc desc = RenderingUtils.CreateTextureDesc(name, descriptor);
        desc.height = size.x;
        desc.width = size.y;
        desc.colorFormat = GraphicsFormat.D24_UNorm_S8_UInt;
        desc.depthBufferBits = DepthBits.Depth32;
        desc.filterMode = FilterMode.Point;
        return desc;
    }

    public static bool CanScaleCamera(CameraData data) =>
        data.CameraRenderTargetBufferType == CameraRenderTargetType.Scaled &&
        data.UpscalingFilter == ImageUpscalingFilter.FSR;

    public static bool CanApplyPipelineChanges(CameraData data) =>
        EnhancedGraphics.Upscaler != null &&
        CanScaleCamera(data);
}
