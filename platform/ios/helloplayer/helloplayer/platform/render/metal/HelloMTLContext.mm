//
//  HelloMTLContext.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#include "HelloMTLContext.hpp"

HelloMTLContext::HelloMTLContext(const char *tag): logger(tag), metalLayers(), renderPassDescriptor(nil), red(0), green(0), blue(0), alpha(1.0)
{
    this->device = MTLCreateSystemDefaultDevice();
    this->commandQueue = [this->device newCommandQueue];
    
    // 手动找出 default.metallib 加载出 id<MTLLibrary>
    NSString *frameworksPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"Frameworks"];
    NSBundle *frameworkBundle = [NSBundle bundleWithPath:[frameworksPath stringByAppendingPathComponent:@"helloplayer.framework"]];
    NSString *metalLibPath = [frameworkBundle pathForResource:@"default" ofType:@"metallib"];
    
    NSError *error = nil;
    this->shaderLib = [this->device newLibraryWithFile:metalLibPath error:&error];
    if (!this->shaderLib) {
        logger.e("Failed to load Metal default library: %s", error.localizedDescription.UTF8String);
    } else {
        logger.i("✅ Successfully loaded Metal library!");
    }
    logger.i("HelloMTLContext::HelloMTLContext(%p)", this);
}

HelloMTLContext::~HelloMTLContext()
{
    renderPassDescriptor = nil;
    logger.i("HelloMTLContext::~HelloMTLContext(%p)", this);
}

bool HelloMTLContext::init()
{
    // 创建 render pass descriptor, 主要用来擦拭 color buffer
    renderPassDescriptor = [MTLRenderPassDescriptor new];
    renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1); // 默认擦拭颜色:黑色
    
    return true;
}

bool HelloMTLContext::addSurface(void *surface)
{
    CAMetalLayer *layer = (__bridge CAMetalLayer*)surface;
    CGSize size = layer.frame.size;
    int width = size.width;
    int height = size.height;
    
    uint64_t key = reinterpret_cast<uint64_t>(surface);
    std::shared_ptr<MetalLayerCtx> ctx(new MetalLayerCtx());
    ctx->key = key;
    ctx->metalLayer = layer;
    ctx->commandBuffer = nil;
    ctx->width = width;
    ctx->height = height;
    
    auto find = metalLayers.find(key);
    if (find == metalLayers.end()) {
        metalLayers[key] = ctx;
    } else {
        logger.i("addSurface [%llu] w*h[%d,%d]", key, width, height);
    }
    
    return true;
}

bool HelloMTLContext::removeSurface(void *surface)
{
    uint64_t key = reinterpret_cast<uint64_t>(surface);
    logger.i("removeSurface [%llu]", key);
    auto find = metalLayers.find(key);
    if (find != metalLayers.end())
    {
        if (find->second->commandBuffer) {
            [find->second->commandBuffer commit];
            find->second->commandBuffer = nil;
        }
        metalLayers.erase(find);
    }
    
    return true;
}

int HelloMTLContext::getSurfaceSize()
{
    return static_cast<int>(metalLayers.size());
}

void HelloMTLContext::getSurfaceKeys(std::vector<uint64_t>* keys)
{
    for (const auto &item: metalLayers)
    {
        auto key = item.first;
        keys->push_back(key);
    }
}

std::shared_ptr<HelloMTLContext::MetalLayerCtx> HelloMTLContext::getSurfaceByKey(uint64_t key)
{
    auto find = metalLayers.find(key);
    if (find != metalLayers.end())
    {
        return (*find).second;
    }
    return std::make_shared<HelloMTLContext::MetalLayerCtx>();
}

int HelloMTLContext::getSurfaceWidth(uint64_t key)
{
    auto find = metalLayers.find(key);
    if (find != metalLayers.end())
    {
        return (*find).second->width;
    }
    return 0;
}

int HelloMTLContext::getSurfaceHeight(uint64_t key)
{
    auto find = metalLayers.find(key);
    if (find != metalLayers.end())
    {
        return (*find).second->height;
    }
    return 0;
}

void HelloMTLContext::setClearColor(float r, float g, float b, float a)
{
    this->red = r;
    this->green = g;
    this->blue = b;
    this->alpha = a;
}

bool HelloMTLContext::renderStart(uint64_t key)
{
    auto findLayer = metalLayers.find(key);
    if (findLayer == metalLayers.end()) return false;

    const std::shared_ptr<MetalLayerCtx> &ctx = findLayer->second;
    if (ctx->commandBuffer)
    {
        logger.e("commandBuffer is already exist");
        return false;
    }

    ctx->currentDrawable = ctx->metalLayer.nextDrawable;
    if (!ctx->currentDrawable)
    {
        logger.e("No drawable from CAMetalLayer!");
        return false;
    }
    
    ctx->commandBuffer = [commandQueue commandBuffer];
    if (!ctx->commandBuffer)
    {
        return false;
    }
    ctx->commandBuffer.label = @"HelloMTLCommandBuffer";
    
    // 设置 renderPassDescriptor
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(red, green, blue, alpha);
    renderPassDescriptor.colorAttachments[0].texture = ctx->currentDrawable.texture;

    // 清屏
    id<MTLRenderCommandEncoder> encoder = [ctx->commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    [encoder endEncoding];

    return true;
}

bool HelloMTLContext::renderEnd(uint64_t key)
{
    auto findLayer = metalLayers.find(key);
    if (findLayer == metalLayers.end()) return false;

    std::shared_ptr<MetalLayerCtx> ctx = findLayer->second;
    
    if (ctx->commandBuffer)
    {
        [ctx->commandBuffer presentDrawable:ctx->currentDrawable];
        [ctx->commandBuffer commit];
    }
    ctx->currentDrawable = nil;
    ctx->commandBuffer = nil;

    return true;
}
