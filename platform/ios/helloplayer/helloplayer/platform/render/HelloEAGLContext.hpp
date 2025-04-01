//
//  HelloEAGLContext.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/20.
//

#ifndef HelloEAGLContext_hpp
#define HelloEAGLContext_hpp



#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>  // 用于 CAEAGLLayer

#include <stdio.h>
#include <string>
#include <unordered_map>
#include <memory>

#include "log/Logger.hpp"
#include "render/IGLContext.hpp"
#include "render/program/GLSLUtil.hpp"


class HelloEAGLContext : public IGLContext {
public:
    typedef struct {
        /**
        CAEGALLayer的唯一标识
         */
        uint64_t key;
        /**
         fbo id
         */
        GLuint frameBuffer;
        /**
         render buffer id
         */
        GLuint renderBuffer;
        
        int width;
        int height;
    } EAGLLayerCtx;
public:
    HelloEAGLContext();
    ~HelloEAGLContext() override;
    
    bool init(void *sharedContext) override;
    
    bool setPbufferSurface(int width, int height) override;

    bool usePbufferSurface() override;

    bool addSurface(void *surface, bool encode) override;

    bool removeSurface(void *surface) override;

    int getSurfaceSize() override;

    void getSurfaceKeys(std::vector<uint64_t> *keys) override;

    int getSurfaceWidth(uint64_t key) override;

    int getSurfaceHeight(uint64_t key) override;

    void setClearColor(float red, float green, float blue, float alpha) override;

    bool renderStart(uint64_t key) override;

    bool renderEnd(uint64_t key, int64_t ptsUs) override;

    bool addOutputTexture(int textureId, int width, int height) override;

    void removeOutputTexture(int textureId) override;

    std::shared_ptr<IGLContext::TextureCtx> getTextureCtx(int textureId);
    
    bool getOutputTextures(std::vector<std::shared_ptr<TextureCtx>> &textures) override;

private:
    Logger logger;
    
    EAGLContext * eaglContext;
    CAEAGLLayer *eglPbufferLayer;

    std::unordered_map<uint64_t, std::shared_ptr<EAGLLayerCtx>> eaglLayers;
    
    /**
     * 待渲染的纹理
     */
    std::unordered_map<uint64_t, std::shared_ptr<TextureCtx>> eaglTextures;
    
    // clear color
    GLfloat red, green, blue, alpha;
};


#endif /* HelloEAGLContext_hpp */
