//
//  MTLPipeline.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#ifndef MTLPipeline_hpp
#define MTLPipeline_hpp


#import <Metal/Metal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>



#include "PipelineUtil.hpp"
#include "log/Logger.hpp" // 日志输出
#include "util/TimeUtil.hpp"
#include "entity/MTLTextureData.hpp"



// 渲染管线 基类
class MTLPipeline
{
public:
    MTLPipeline(const char *tag);
    virtual ~MTLPipeline();
    
    /**
     初始化渲染管线
     */
    virtual void initialized(const std::shared_ptr<HelloMTLContext> &ctx);
    
    virtual std::string getVertextFuncName() = 0;
    
    virtual std::string getFragmentFuncName() = 0;
    
    virtual void onPipelineCreated();
    
    virtual void setTexture(const std::vector<id<MTLTexture>> &textures);
    
    virtual void setTextureData(const std::shared_ptr<MTLTextureData> &data);
    
    virtual void setRotation(float rotation);
    
    virtual void setMirror(bool hMirror, bool vMirror);
    
    virtual void setClearColor(float r, float g, float b, float a);
    
    virtual void setRenderTarget(id<MTLTexture> texture);
    
    virtual id<MTLTexture> getRenderTarget();
    
    virtual bool begin(id<MTLCommandBuffer> buffer);
    
    virtual bool draw(float projectMat[4*4]) = 0;
    
    virtual bool end();
    
protected:
    Logger logger;
    
    std::shared_ptr<HelloMTLContext> ctx;
    
    glm::mat4 moduleMat4; // 模型矩阵(单位矩阵)
    float rotation; // 旋转角度
    bool hMirror; // 是否水平镜像
    bool vMirror; // 是否垂直镜像
    id<MTLTexture> renderTarget; // 渲染目标(2d纹理)
    
    MTLRenderPassDescriptor *renderPassDescriptor;
    
    id <MTLRenderPipelineState> pipelineState; // 渲染管线
    id<MTLRenderCommandEncoder> encoder; // 渲染指令编码,需要在begin时创建好
};


#endif /* MTLPipeline_hpp */
