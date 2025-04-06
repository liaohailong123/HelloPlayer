//
//  MTLPipeline.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#include "MTLPipeline.hpp"


MTLPipeline::MTLPipeline(const char *tag): logger(tag), ctx(nullptr), texture(nil),
                moduleMat4(glm::identity<glm::mat4>()),rotation(0), hMirror(false),vMirror(false),
                renderTarget(nil),renderPassDescriptor(nil), pipelineState(nil)
{
    logger.i("MTLPipeline::MTLPipeline(%p)", this);
}

MTLPipeline::~MTLPipeline()
{
    logger.i("MTLPipeline::~MTLPipeline(%p)", this);
}


void MTLPipeline::initialized(const std::shared_ptr<HelloMTLContext> &_ctx)
{
    this->ctx = _ctx;
    if(!renderPassDescriptor)
    {
        renderPassDescriptor = [MTLRenderPassDescriptor new];
        renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1); // 默认擦拭颜色:黑色
    }
    
    NSError *error;
    if (!pipelineState)
    {

        // 生成着色器程序
        std::string vertex = getVertextFuncName();
        std::string fragment = getFragmentFuncName();
        NSString *vertexFuncName = [NSString stringWithUTF8String:vertex.c_str()];
        NSString *fragmentFuncName = [NSString stringWithUTF8String:fragment.c_str()];
        id <MTLFunction> vertexProgram = [ctx->shaderLib newFunctionWithName:vertexFuncName];
        if(!vertexProgram)
        {
            logger.e(">> ERROR: Couldn't load vertex function from default library");
            return;
        }
        id <MTLFunction> fragmentProgram = [ctx->shaderLib newFunctionWithName:fragmentFuncName];
        if(!fragmentProgram)
        {
            logger.e(">> ERROR: Couldn't load fragment function from default library");
            return;
        }
        
        // 创建渲染管线
        // Create a pipeline state descriptor to create a compiled pipeline state object
        MTLRenderPipelineDescriptor *pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];

        pipelineDescriptor.label                           = @"MTLPipeline";
        pipelineDescriptor.sampleCount                     = 1;
        pipelineDescriptor.vertexFunction                  = vertexProgram;
        pipelineDescriptor.fragmentFunction                = fragmentProgram;
        pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm; // 统一渲染格式都是BGRA

        
        pipelineState = [ctx->device newRenderPipelineStateWithDescriptor:pipelineDescriptor
                                                                 error:&error];
    }

    if(!pipelineState)
    {
        logger.e("ERROR: Failed aquiring pipeline state: %d", error);
        return;
    }
    
    logger.e("MTLPipeline::initialized() success");
    
    onPipelineCreated();
}

void MTLPipeline::onPipelineCreated()
{
    // do nothing... implement by child
}

void MTLPipeline::setTexture(const id<MTLTexture> &source)
{
    texture = nil;
    texture = source;
}

void MTLPipeline::setTextureData(const std::shared_ptr<MTLTextureData> &data)
{
    // do nothing... implement by child
}

void MTLPipeline::setRotation(float _rotation)
{
    this->rotation = _rotation;
}

void MTLPipeline::setMirror(bool _hMirror, bool _vMirror)
{
    this->hMirror = _hMirror;
    this->vMirror = _vMirror;
}

void MTLPipeline::setClearColor(float r, float g, float b, float a)
{
    if (renderPassDescriptor) {
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(r, g, b, a);
    } else {
        logger.i("MTLPipeline::setClearColor renderPassDescriptor is null?");
    }
}

void MTLPipeline::setRenderTarget(const id<MTLTexture> &texture)
{
    if (this->renderTarget != texture) {
        this->renderTarget = nil;
        this->renderTarget = texture;
        // 设置渲染目标
        renderPassDescriptor.colorAttachments[0].texture = nil;
        renderPassDescriptor.colorAttachments[0].texture = this->renderTarget;
    }
}

bool MTLPipeline::begin(const id<MTLCommandBuffer> &buffer)
{
    if (!buffer) {
        logger.e("MTLPipeline::begin command buffer is null?");
        return false;
    }
    if (!renderTarget)
    {
        logger.e("MTLPipeline::begin renderTarget is null?");
        return false;
    }

    if (!renderPassDescriptor)
    {
        logger.e("MTLPipeline::begin renderPassDescriptor is null?");
        return false;
    }
    if (encoder)
    {
        logger.e("MTLPipeline::begin encoder is already exist, call end() first");
        return false;
    }

    encoder = [buffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    if (!encoder) {
        logger.e("MTLPipeline::begin encoder is null?");
        return false;
    }
    
    // 设置视口
    MTLViewport viewport;
    viewport.originX = 0;
    viewport.originY = 0;
    viewport.width = renderTarget.width;
    viewport.height = renderTarget.height;
    viewport.znear = 0.0;
    viewport.zfar = 1.0;
    [encoder setViewport:viewport];
    
    return true;
}

bool MTLPipeline::end()
{
    if (encoder)
    {
        [encoder endEncoding];
        encoder = nil;
    }
    
    texture = nil;

    return true;
}
