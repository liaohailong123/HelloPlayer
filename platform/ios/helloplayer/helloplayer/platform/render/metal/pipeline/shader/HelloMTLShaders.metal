//
//  HelloMTLShaders.metal
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#include <metal_stdlib>
using namespace metal;

#include "HelloMTLShaderTypes.h"


#pragma mark -

#pragma mark Shaders for pipeline used texture from renderable texture when rendering to the drawable.

// Vertex shader outputs and fragment shader inputs for texturing pipeline.
struct TexturePipelineRasterizerData
{
    float4 position [[position]];
    float2 texcoord;
    vector_int2 mirror;
};

// Vertex shader which adjusts positions by an aspect ratio and passes texture
// coordinates through to the rasterizer.
vertex TexturePipelineRasterizerData
textureVertexShader(const uint vertexID [[ vertex_id ]],
                    const device AAPLTextureVertex *vertices [[ buffer(AAPLVertexInputIndexVertices) ]],
                    constant AAPLObjectParams &object [[ buffer(AAPLVertexInputIndexObjParams) ]])
{
    TexturePipelineRasterizerData out;

    // 计算顶点位置
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.x = vertices[vertexID].position.x;
    out.position.y = vertices[vertexID].position.y;
    out.position = object.mvpMat4 * out.position;

    // 传递 UV 坐标
    out.texcoord = vertices[vertexID].texcoord;

    // 传递 mirror 信息到 fragment shader
    out.mirror = object.mirror;

    return out;
}
// Fragment shader that samples a texture and outputs the sampled color.
fragment float4 textureFragmentShader(TexturePipelineRasterizerData in      [[stage_in]],
                                      texture2d<float>              texture [[texture(AAPLTextureInputIndexColor0)]])
{
    sampler simpleSampler (mag_filter::linear, min_filter::linear);

    // 计算最终的 UV 坐标
    float2 texcoord = in.texcoord;

    // 水平镜像 (mirror.x = 1 时，左右翻转)
    if (in.mirror.x == 1) {
        texcoord.x = 1.0 - texcoord.x;
    }

    // 垂直镜像 (mirror.y = 1 时，上下翻转)
    if (in.mirror.y == 1) {
        texcoord.y = 1.0 - texcoord.y;
    }

    // Sample data from the texture.
    float4 colorSample = texture.sample(simpleSampler, texcoord);

    // Return the color sample as the final color.
    return colorSample;
}


#pragma mark -

#pragma mark draw YUV I420
fragment float4 i420FragmentShader(TexturePipelineRasterizerData in      [[stage_in]],
                                   texture2d<float>              textureY [[texture(AAPLTextureInputIndexColor0)]],
                                   texture2d<float>              textureU [[texture(AAPLTextureInputIndexColor1)]],
                                   texture2d<float>              textureV [[texture(AAPLTextureInputIndexColor2)]])
{
    sampler simpleSampler (mag_filter::linear, min_filter::linear);

    // 计算最终的 UV 坐标
    float2 texcoord = in.texcoord;

    // 水平镜像 (mirror.x = 1 时，左右翻转)
    if (in.mirror.x == 1) {
        texcoord.x = 1.0 - texcoord.x;
    }

    // 垂直镜像 (mirror.y = 1 时，上下翻转)
    if (in.mirror.y == 1) {
        texcoord.y = 1.0 - texcoord.y;
    }
    
    // Sample data from the texture.
    float y = textureY.sample(simpleSampler, texcoord).r;
    float u = textureU.sample(simpleSampler, texcoord).r - 0.5;
    float v = textureV.sample(simpleSampler, texcoord).r - 0.5;
    
    float r = y + 1.402 * v;
    float g = y - 0.344 * u - 0.714 * v;
    float b = y + 1.772 * u;
    

    return float4(b,g,r,1.0);
}


#pragma mark -

#pragma mark draw YUV NV12
fragment float4 nv12FragmentShader(TexturePipelineRasterizerData in      [[stage_in]],
                                   texture2d<float>              textureY [[texture(AAPLTextureInputIndexColor0)]],
                                   texture2d<float>              textureUV [[texture(AAPLTextureInputIndexColor1)]])
{
    sampler simpleSampler (mag_filter::linear, min_filter::linear);

    // 计算最终的 UV 坐标
    float2 texcoord = in.texcoord;

    // 水平镜像 (mirror.x = 1 时，左右翻转)
    if (in.mirror.x == 1) {
        texcoord.x = 1.0 - texcoord.x;
    }

    // 垂直镜像 (mirror.y = 1 时，上下翻转)
    if (in.mirror.y == 1) {
        texcoord.y = 1.0 - texcoord.y;
    }
    
    // Sample data from the texture.
    float y = textureY.sample(simpleSampler, texcoord).r;
    float2 uv = textureUV.sample(simpleSampler, texcoord).rg;
    
    float u = uv.r - 0.5;
    float v = uv.g - 0.5;
    
    float r = y + 1.402 * v;
    float g = y - 0.344 * u - 0.714 * v;
    float b = y + 1.772 * u;
    

    return float4(b,g,r,1.0);
}
