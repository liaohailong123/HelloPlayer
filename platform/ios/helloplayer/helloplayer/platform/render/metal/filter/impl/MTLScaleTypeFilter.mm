//
//  MTLScaleTypeFilter.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/4/1.
//

#include "MTLScaleTypeFilter.hpp"


MTLScaleTypeFilter::MTLScaleTypeFilter(const std::shared_ptr<HelloMTLContext> &ctx): IMTLFilterNode("MTLScaleTypeFilter",ctx)
{
    pipeline = std::make_shared<ScaleTypePipeline>();
    pipeline->initialized(ctx);
    pipeline->setScaleType(ScaleTypePipeline::ScaleType::FitCenter);
    logger.i("MTLScaleTypeFilter::MTLScaleTypeFilter(%p)", this);
}
MTLScaleTypeFilter::~MTLScaleTypeFilter()
{
    logger.i("MTLScaleTypeFilter::~MTLScaleTypeFilter(%p)", this);
}

void MTLScaleTypeFilter::onProcess(std::shared_ptr<MTLFilterPacket> packet)
{
    if (!packet->texture) {
        logger.e("MTLScaleTypeFilter::onProcess input texture is nil?");
        return;
    }
    
    int textureWidth = packet->width;
    int textureHeight = packet->height;
    
    if (pipeline->renderTarget == nil
        || pipeline->renderTarget.width != textureWidth
        || pipeline->renderTarget.height != textureHeight)
    {
        const id<MTLTexture> &target = PipelineUtil::generateTexture2D(ctx, textureWidth, textureHeight);
        // 将内容渲染到 2d纹理上 类似OpenGLES上的fbo纹理
        pipeline->setRenderTarget(target);
    }
    
    auto layer = ctx->getSurfaceByKey(packet->key);
    
    if (layer->commandBuffer) {
        // 渲染管线开始绘制,内部创建 command encoder
        if(pipeline->begin(layer->commandBuffer))
        {
            std::shared_ptr<ScaleTypePipeline::Rect> src(new ScaleTypePipeline::Rect());
            std::shared_ptr<ScaleTypePipeline::Rect> dst(new ScaleTypePipeline::Rect());

            // 原点在左下,Y轴向上,X轴向右,视口坐标系
            src->left = 0;
            src->top = packet->textureHeight; // 资源
            src->right = packet->textureWidth;
            src->bottom = 0;

            dst->left = 0;
            dst->top = packet->height;
            dst->right = packet->width;
            dst->bottom = 0;
            
            // 设置缩放区域 FIXME: 仅input参数变化,才调用该函数
            pipeline->setRect(src, dst);

            pipeline->setTexture(packet->texture);
            
            // 绘制
            pipeline->draw(glm::value_ptr(packet->prjMat4));
            
            // 渲染管线结束绘制,结束 command encoder
            pipeline->end();
        }

    }
    
    // 本次渲染的结果,作为下一个过滤器的资源
    packet->texture = pipeline->renderTarget;
}
