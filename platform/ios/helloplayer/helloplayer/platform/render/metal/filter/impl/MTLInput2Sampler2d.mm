//
//  MTLInput2Sampler2d.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/4/1.
//

#include "MTLInput2Sampler2d.hpp"


MTLInput2Sampler2d::MTLInput2Sampler2d(const std::shared_ptr<HelloMTLContext> &ctx, AVPixelFormat format): IMTLFilterNode("MTLInput2Sampler2d",ctx), format(AV_PIX_FMT_NONE)
{
    updateAVPixelFormat(format);
    logger.i("MTLInput2Sampler2d::MTLInput2Sampler2d(format[%d],%p)", format, this);
}

MTLInput2Sampler2d::~MTLInput2Sampler2d()
{
    logger.i("MTLInput2Sampler2d::~MTLInput2Sampler2d(%p)", this);
}

void MTLInput2Sampler2d::updateAVPixelFormat(AVPixelFormat f)
{
    if (f == format) return;
    this->format = f;

    switch (format)
    {
        case AV_PIX_FMT_NV12:
            // only NV12
            pipeline = std::make_shared<NV12Pipeline>();
            break;
        case AV_PIX_FMT_YUV422P:
        case AV_PIX_FMT_YUVJ422P:
        case AV_PIX_FMT_YUV420P:
        case AV_PIX_FMT_YUVJ420P:
            // I420P + YUV422P
            pipeline = std::make_shared<I420Pipeline>();
            break;
        default:
            format = AV_PIX_FMT_YUV420P;
            pipeline = std::make_shared<I420Pipeline>();
            logger.i("color format is not support default use to YUV420P");
            break;
    }

    // 初始化着色器
    if (pipeline)
    {
        pipeline->initialized(ctx);
    }
}

AVPixelFormat MTLInput2Sampler2d::getAVPixelFormat() noexcept
{
    return format;
}

void MTLInput2Sampler2d::onProcess(std::shared_ptr<MTLFilterPacket> packet)
{
    int textureWidth = packet->textureWidth;
    int textureHeight = packet->textureHeight;
    
    id<MTLTexture> target = pipeline->getRenderTarget();
    
    if (target == nil || target.width != textureWidth || target.height != textureHeight)
    {
        target = PipelineUtil::genrateTexure2D(ctx, textureWidth, textureHeight);
        // 将内容渲染到 2d纹理上 类似OpenGLES上的fbo纹理
        pipeline->setRenderTarget(target);
    }
    
    if (!target) {
        logger.e("MTLInput2Sampler2d::onProcess target is nil?");
        return;
    }
    
    if (packet->textureData && packet->commandBuffer) {
        // 渲染管线开始绘制,内部创建 command encoder
        pipeline->begin(packet->commandBuffer);
        
        // YUV内容设置
        pipeline->setTextureData(packet->textureData);
        
        // 绘制
        pipeline->draw(glm::value_ptr(packet->prjMat4));
        
        // 渲染管线结束绘制,结束 command encoder
        pipeline->end();
    }
    
    // 本次渲染的结果,作为下一个过滤器的资源
    packet->texture = target;

}
