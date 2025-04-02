//
//  HelloMTLContext.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#ifndef HelloMTLContext_hpp
#define HelloMTLContext_hpp

#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include <stdio.h>
#include <vector>
#include <unordered_map>

#include "log/Logger.hpp"

class HelloMTLContext
{
public:
    typedef struct {
        /**
         * MetalLayer 的一个唯一标识
         */
        uint64_t key;
        /**
         * Metal组件,里面有CAMetalLayer
         */
        CAMetalLayer *metalLayer;
        /**
         * Metal渲染缓冲区
         */
        id <CAMetalDrawable> currentDrawable;
        /**
         * layer width
         */
        int width;
        
        /**
         * layer height
         */
        int height;
    } MetalLayerCtx;
public:
    HelloMTLContext(const char* tag = "HelloMTLContext");
    ~HelloMTLContext();
    
    bool init();
    
    bool addSurface(void *surface);
    
    bool removeSurface(void *surface);
    
    int getSurfaceSize();
    
    void getSurfaceKeys(std::vector<uint64_t>* keys);
    
    std::shared_ptr<MetalLayerCtx> getSurfaceByKey(uint64_t key);
    
    int getSurfaceWidth(uint64_t key);
    
    int getSurfaceHeight(uint64_t key);
    
    void setClearColor(float red, float green, float blue, float alpha);
    
    id<MTLCommandBuffer> renderStart(uint64_t key);
    
    bool renderEnd(uint64_t key);

private:
    id<MTLCommandBuffer> begin();
    
    void commit(id<MTLCommandBuffer> buffer, id<CAMetalDrawable> currentDrawable = nullptr);
    
public:
    Logger logger;
    
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
    id<MTLLibrary> shaderLib;
    MTLRenderPassDescriptor *renderPassDescriptor;
    
    std::unordered_map<uint64_t, id<MTLCommandBuffer>> commandBuffers;
    std::unordered_map<uint64_t, std::shared_ptr<MetalLayerCtx>> metalLayers;
    
    // clear color
    float red, green, blue, alpha;
};


#endif /* HelloMTLContext_hpp */
