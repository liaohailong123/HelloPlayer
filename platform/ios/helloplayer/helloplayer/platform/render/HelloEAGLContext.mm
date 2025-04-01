//
//  HelloEAGLContext.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/20.
//

#include "HelloEAGLContext.hpp"

HelloEAGLContext::HelloEAGLContext() : logger("HelloEAGLContext"), eaglContext(nullptr), eglPbufferLayer(nullptr), eaglLayers(), eaglTextures(),
                                    red(0.0),green(0.0),blue(0.0),alpha(1.0)
{
    logger.i("HelloEAGLContext::HelloEAGLContext(%p)",this);
}
HelloEAGLContext::~HelloEAGLContext()
{
    if (eaglContext) {
        EAGLContext* context = eaglContext;
        if ([EAGLContext currentContext] == context) {
            [EAGLContext setCurrentContext:nil];
        }
        eaglContext = nullptr;
    }
    logger.i("HelloEAGLContext::~HelloEAGLContext(%p)",this);
}

bool HelloEAGLContext::init(void *sharedContext)
{
    EAGLContext * context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (!context || ![EAGLContext setCurrentContext:context]) {
        logger.i("Failed to create or set EAGLContext");
        return false;
    }
    
    [EAGLContext setCurrentContext:context];
    eaglContext = context;

    return true;
}

bool HelloEAGLContext::setPbufferSurface(int width, int height)
{
    // 暂不支持
    return true;
}

bool HelloEAGLContext::usePbufferSurface()
{
    return true;
}

bool HelloEAGLContext::addSurface(void *surface, bool encode)
{
    // 在iOS上是 CAEAGLLayer
    CAEAGLLayer *layer = (__bridge CAEAGLLayer*)surface;
    
    if (!eaglContext) {
        logger.i("Init EAGLContext first");
        return false;
    }
    
    // 取出layer的内存地址，作为唯一标识
    uint64_t key = reinterpret_cast<uint64_t>(layer);
    
    auto find = eaglLayers.find(key);
    if (find != eaglLayers.end()) {
        logger.i("CAEAGLLayer [%ld] is already added", key);
        return true;
    }


    GLuint renderBuffer = 0;
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    // 渲染缓冲区绑定到 CAEAGLLayer，然后Layer通过View的addLayer函数加入视图层
    if (![eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer]) {
        logger.i("Failed to bind renderbufferStorage");
        return false;
    }
    
    GLuint frameBuffer = 0;
    // 创建帧缓冲和渲染缓冲
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    // 把FBO依附到渲染缓冲区上
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
    
    // 检查帧缓冲状态
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        logger.i("Framebuffer is not complete");

        return false;
    }
    
    // -------- 获取 Layer 的宽高 (以 pixel 计) --------
    CGSize layerSize = layer.bounds.size;
    CGFloat scale = layer.contentsScale; // Retina 屏幕 scale = 2 或 3
    int pixelWidth = (int)(layerSize.width * scale);
    int pixelHeight = (int)(layerSize.height * scale);
    
    logger.i("CAEAGLLayer [%ld] width=%d height=%d", key, pixelWidth, pixelHeight);
    
    std::shared_ptr<EAGLLayerCtx> ctx(new EAGLLayerCtx());
    ctx->key = key;
    ctx->frameBuffer = frameBuffer;
    ctx->renderBuffer = renderBuffer;
    ctx->width = pixelWidth;
    ctx->height = pixelHeight;
    
    eaglLayers.insert(std::pair<uint64_t, std::shared_ptr<EAGLLayerCtx>>(key,ctx));

    return true;
}

bool HelloEAGLContext::removeSurface(void *surface)
{
    // 在iOS上是 CAEAGLLayer
    CAEAGLLayer *layer = (__bridge CAEAGLLayer*)surface;
    
    // 取出layer的内存地址，作为唯一标识
    uint64_t key = reinterpret_cast<uint64_t>(layer);
    auto find = eaglLayers.find(key);
    if (find != eaglLayers.end()) {
        std::shared_ptr<EAGLLayerCtx> &ctx = (*find).second;
        
            if (ctx->renderBuffer) {
                glDeleteRenderbuffers(1, &ctx->renderBuffer);
                ctx->renderBuffer = 0;
            }
            if (ctx->frameBuffer) {
                glDeleteFramebuffers(1, &ctx->frameBuffer);
                ctx->frameBuffer = 0;
            }
        size_t count = eaglLayers.erase(key);
        if (count) {
            logger.i("removeSurface [%ld] success", key);
        } else{
            logger.i("removeSurface [%ld] error", key);
        }
    }

    return true;
}

int HelloEAGLContext::getSurfaceSize()
{
    return static_cast<int>(eaglLayers.size());
}

void HelloEAGLContext::getSurfaceKeys(std::vector<uint64_t> *keys)
{
    for (const auto &item: eaglLayers)
    {
        auto key = item.first;
        keys->push_back(key);
    }
}

int HelloEAGLContext::getSurfaceWidth(uint64_t key)
{
    auto find = eaglLayers.find(key);
    if (find != eaglLayers.end())
    {
        return (*find).second->width;
    }
    return 0;
}

int HelloEAGLContext::getSurfaceHeight(uint64_t key)
{
    auto find = eaglLayers.find(key);
    if (find != eaglLayers.end())
    {
        return (*find).second->height;
    }
    return 0;
}

void HelloEAGLContext::setClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    this->red = r;
    this->green = g;
    this->blue = b;
    this->alpha = a;
}

bool HelloEAGLContext::renderStart(uint64_t key)
{
    auto find = eaglLayers.find(key);
    if (find == eaglLayers.end()) {
        return false;
    }
    std::shared_ptr<EAGLLayerCtx> ctx = (*find).second;
    
    glBindFramebuffer(GL_FRAMEBUFFER, ctx->frameBuffer);
    
    // 每次绘制之前，需要清空缓冲区内容，不然会在上一次的基础上绘制，导致雪花屏
    glClearColor(red, green, blue, alpha);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    return true;
}

bool HelloEAGLContext::renderEnd(uint64_t key, int64_t ptsUs)
{
//    [eaglContext presentRenderbuffer:GL_RENDERBUFFER atTime:double(ptsUs)];
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    return true;
}

bool HelloEAGLContext::addOutputTexture(int textureId, int width, int height)
{
    auto find = eaglTextures.find(textureId);
    if (find == eaglTextures.end()) {
        std::shared_ptr<TextureCtx> ctx(new TextureCtx());
        ctx->fboId = 0;
        ctx->textureId = textureId;
        ctx->textureWidth = width;
        ctx->textureHeight = height;

        eaglTextures.insert(std::pair<uint64_t, std::shared_ptr<TextureCtx>>(textureId, ctx));
        logger.i("addOutputTexture [%d] success", textureId);
    } else {
        logger.i("outputTexture [%d] is already added", textureId);
    }
    return true;
}

void HelloEAGLContext::removeOutputTexture(int textureId)
{
    auto find = eaglTextures.find(textureId);
    if (find != eaglTextures.end()) {
        std::shared_ptr<TextureCtx> &ctx = (*find).second;
        if (ctx->fboId) {
            // 可能需要释放fbo资源
            const GLuint framebuffer = ctx->fboId;
            glDeleteFramebuffers(1, &framebuffer);
            ctx->fboId = 0;
            logger.i("removeOutputTexture delete fbo[%ld]", ctx->fboId);
        }

        size_t count = eaglTextures.erase(textureId);
        if (count) {
            logger.i("removeOutputTexture [%ld] success", textureId);
        } else {
            logger.i("removeOutputTexture [%ld] error", textureId);
        }
    } else {
        logger.i("removeOutputTexture [%ld] not found", textureId);
    }
}

std::shared_ptr<IGLContext::TextureCtx> HelloEAGLContext::getTextureCtx(int textureId){
    auto find = eaglTextures.find(textureId);
    if (find != eaglTextures.end()) {
        return (*find).second;
    }
    return std::make_shared<IGLContext::TextureCtx>();
}

bool HelloEAGLContext::getOutputTextures(std::vector<std::shared_ptr<TextureCtx>> &textures)
{
    for (const auto &item: eaglTextures) {
        std::shared_ptr<TextureCtx> ctx = item.second;
        textures.push_back(ctx);
    }
    return true;
}
