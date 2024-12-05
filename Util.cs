using Owlcat.Runtime.Visual;
using UnityEngine;
using UnityEngine.Experimental.Rendering;
using UnityEngine.Experimental.Rendering.RenderGraphModule;
using UnityEngine.Rendering;

namespace DLSS;

public static class Util {
    public static TextureDesc CreateColorTargetDesc(string name, RenderTextureDescriptor descriptor, int width, int height) =>
        CreateColorTargetDesc(name, descriptor, (uint)width, (uint)height);
    public static TextureDesc CreateColorTargetDesc(string name, RenderTextureDescriptor descriptor, uint width, uint height) {
        TextureDesc desc = RenderingUtils.CreateTextureDesc(name, descriptor);
        desc.width = (int)width;
        desc.height = (int)height;
        desc.depthBufferBits = DepthBits.None;
        desc.filterMode = FilterMode.Bilinear;
        desc.wrapMode = TextureWrapMode.Clamp;
        desc.enableRandomWrite = true;
        return desc;
    }

    public static TextureDesc CreateDepthTargetDesc(string name, RenderTextureDescriptor descriptor, int width, int height) =>
        CreateDepthTargetDesc(name, descriptor, (uint)width, (uint)height);
    public static TextureDesc CreateDepthTargetDesc(string name, RenderTextureDescriptor descriptor, uint width, uint height) {
        TextureDesc desc = RenderingUtils.CreateTextureDesc(name, descriptor);
        desc.width = (int)width;
        desc.height = (int)height;
        desc.colorFormat = GraphicsFormat.D24_UNorm_S8_UInt;
        desc.depthBufferBits = DepthBits.Depth32;
        desc.filterMode = FilterMode.Point;
        return desc;
    }
}
