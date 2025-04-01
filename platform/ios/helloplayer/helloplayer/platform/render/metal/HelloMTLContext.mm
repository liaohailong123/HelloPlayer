//
//  HelloMTLContext.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#include "HelloMTLContext.hpp"

HelloMTLContext::HelloMTLContext(const char *tag): logger(tag), metalLayers(), renderPassDescriptor(nil), commandBuffers(), red(0), green(0), blue(0), alpha(1.0)
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

bool HelloMTLContext::addSurface(MTKView *surface)
{
    CGSize size = surface.frame.size;
    int width = size.width;
    int height = size.height;
    
    uint64_t key = reinterpret_cast<uint64_t>(surface);
    std::shared_ptr<MetalLayerCtx> ctx(new MetalLayerCtx());
    ctx->key = key;
    ctx->metalLayer = surface;
    ctx->width = width;
    ctx->height = height;
    surface.device = device;
    
    auto find = metalLayers.find(key);
    if (find == metalLayers.end()) {
        metalLayers[key] = ctx;
    } else {
        logger.i("addSurface [%llu] w*h[%d,%d]", key, width, height);
    }
    
    return true;
}

bool HelloMTLContext::removeSurface(MTKView *surface)
{
    uint64_t key = reinterpret_cast<uint64_t>(surface);
    logger.i("removeSurface [%llu]", key);
    metalLayers.erase(key);
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

id<MTLCommandBuffer> HelloMTLContext::renderStart(uint64_t key)
{
    // 查一下是否已经存在
    auto find = commandBuffers.find(key);
    if (find != commandBuffers.end()) {
        id<MTLCommandBuffer> commandBuffer = find->second;
        if (commandBuffer) {
            logger.e("commandBuffer is already exist");
            return commandBuffer;
        }
    }

    auto findLayer = metalLayers.find(key);
    if (findLayer == metalLayers.end())return nil;
    
    MTKView *metalLayer = findLayer->second->metalLayer;
    
    // 首先清除一次 color buffer
    if (renderPassDescriptor) {
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(red, green, blue, alpha);
        renderPassDescriptor.colorAttachments[0].texture = metalLayer.currentDrawable.texture;
    } else {
        logger.i("MTLPipeline::setClearColor renderPassDescriptor is null?");
        return nil;
    }
    
    id<MTLCommandBuffer> commandBuffer = begin();
    commandBuffers[key] = commandBuffer;
    
    // 创建一个 encoder 然后直接 结束, 达到 clear color 的效果
    id <MTLRenderCommandEncoder> encoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    [encoder endEncoding];
    
    return commandBuffer;
}

bool HelloMTLContext::renderEnd(uint64_t key)
{
    // 查一下是否已经存在
    auto find = commandBuffers.find(key);
    if (find == commandBuffers.end()) {
        logger.e("Cannot render end cause commandBuffer is nullptr?");
        return false;
    }
    id<MTLCommandBuffer> commandBuffer = find->second;
    commandBuffers.erase(find);

    auto findLayer = metalLayers.find(key);
    if (findLayer == metalLayers.end())return false;
    
    MTKView *metalLayer = findLayer->second->metalLayer;
    if (metalLayer) {
        commit(commandBuffer, metalLayer.currentDrawable);
    }
    commandBuffer = nil;
    
    return true;
}


id<MTLCommandBuffer> HelloMTLContext::begin()
{
    return [this->commandQueue commandBuffer];
}

void HelloMTLContext::commit(id<MTLCommandBuffer> buffer, id<CAMetalDrawable> currentDrawable)
{
    if (currentDrawable) {
        [buffer presentDrawable:currentDrawable];
    }
    [buffer commit];
}
