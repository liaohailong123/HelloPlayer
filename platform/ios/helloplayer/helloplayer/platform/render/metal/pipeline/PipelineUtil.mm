//
//  PipelineUtil.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/31.
//

#include "PipelineUtil.hpp"


id<MTLTexture> PipelineUtil::generateTexture2D(std::shared_ptr<HelloMTLContext> ctx, int width, int height,
                                           MTLPixelFormat pixelFormat, int rotation)
{
    int fbo_w;
    int fbo_h;
    // 创建滤镜入口纹理fbo
    if (rotation == -1) {
        // 不处理旋转
        fbo_w = width;
        fbo_h = height;
    } else {
        bool portrait = rotation % 180 != 0;
        if (portrait) {
            // 竖屏模式
            fbo_w = width > height ? height : width; // 宽度取小值
            fbo_h = width > height ? width : height; // 高度取大值
        } else {
            // 横屏模式
            fbo_w = width > height ? width : height; // 宽度取大值
            fbo_h = width > height ? height : width; // 高度取小值
        }
    }

    
    // Set up a texture for rendering to and sampling from
    MTLTextureDescriptor *texDescriptor = [MTLTextureDescriptor new];
    texDescriptor.textureType = MTLTextureType2D;
    texDescriptor.width = fbo_w;
    texDescriptor.height = fbo_h;
    texDescriptor.pixelFormat = pixelFormat;
    texDescriptor.usage = MTLTextureUsageRenderTarget |
                          MTLTextureUsageShaderRead;

    return [ctx->device newTextureWithDescriptor:texDescriptor];
}
