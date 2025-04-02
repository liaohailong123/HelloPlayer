//
//  MTLTexture2Layer.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/4/1.
//

#include "MTLTexture2Layer.hpp"

MTLTexture2Layer::MTLTexture2Layer(const std::shared_ptr<HelloMTLContext> &ctx): IMTLFilterNode("MTLTexture2Layer",ctx)
{
    pipeline = std::make_shared<Texture2DPipeline>();
    pipeline->initialized(ctx);
    logger.i("MTLTexture2Layer::MTLTexture2Layer(%p)", this);
}

MTLTexture2Layer::~MTLTexture2Layer()
{
    logger.i("MTLTexture2Layer::~MTLTexture2Layer(%p)", this);
}

void MTLTexture2Layer::onProcess(std::shared_ptr<MTLFilterPacket> packet)
{
//    int64_t startMs = TimeUtil::getCurrentTimeMs();
    if (!packet->texture) {
        logger.e("MTLTexture2Layer::onProcess input texture is nil?");
        return;
    }
    
    // 本次渲染需要上屏幕显示
    std::shared_ptr<HelloMTLContext::MetalLayerCtx> layerCtx = ctx->getSurfaceByKey(packet->key);
    if (!layerCtx || !layerCtx->metalLayer) {
        logger.i("MTLTexture2Layer::onProcess cannot found layerCtx?");
        return;
    }
    
    id<CAMetalDrawable> drawable = layerCtx->currentDrawable;
    if (!drawable) {
        logger.i("MTLTexture2Layer::onProcess drawable is nil?");
        return;
    }
    
    id<MTLTexture> target = drawable.texture;
    if (!target) {
        logger.i("MTLTexture2Layer::onProcess drawable.texture is nil?");
        return;
    }

    // 配置CAMetalLayer的纹理,接下来绘制内容就能上屏了
    pipeline->setRenderTarget(target);
    
    // 渲染管线开始绘制,内部创建 command encoder
    pipeline->begin(packet->commandBuffer);
    
    // 设置资源纹理,将这个纹理内容,按照fitXY(平铺)的方式渲染
    std::vector<id<MTLTexture>> textures = { packet->texture };
    pipeline->setTexture(textures);
    
    // 绘制
    pipeline->draw(glm::value_ptr(packet->prjMat4));
    
    // 渲染管线结束绘制,结束 command encoder
    pipeline->end();
    
//    int64_t costMs = TimeUtil::getCurrentTimeMs() - startMs;
//    logger.i("MTLTexture2Layer::onProcess cost[%d]ms", costMs);
    
}
