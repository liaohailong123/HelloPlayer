//
// Created by liaohailong on 2024/6/13.
//

#include "HelloEGLContext.hpp"


HelloEGLContext::HelloEGLContext() : logger("HelloEGLContext"), eglDisplay(nullptr),
                                     eglConfig(nullptr), eglContext(nullptr), eglPbufferSurface(nullptr),
                                     eglSurfaces(), eglTextures(), red(0), green(0), blue(0), alpha(1.0)
{
    logger.i("HelloEGLContext::HelloEGLContext()");
}

HelloEGLContext::~HelloEGLContext()
{
    // 释放EGL环境资源
    // 移除操作参考：Android平台的GLSurfaceView.java
    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    for (const auto &pair: eglSurfaces)
    {
        std::shared_ptr<EGLSurfaceCtx> eglSurfaceCtx = pair.second;
        eglDestroySurface(eglDisplay, eglSurfaceCtx->eglSurface);
    }
    if (!eglDestroyContext(eglDisplay, eglContext))
    {
        EGLint error = eglGetError();
        logger.i("eglDestroyContext error[%d]", error);
    }
    eglTerminate(eglDisplay);

    logger.i("HelloEGLContext::~HelloEGLContext()");
}

bool HelloEGLContext::init(void *sharedContext)
{
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    int major[1];
    int minor[1];
    if (!eglInitialize(eglDisplay, major, minor))
    {
        logger.i("eglInitialize error : %d", eglGetError());
        return false;
    }

    logger.i("eglInitialize success major = %d, minor = %d", major[0], minor[0]);

    int attr_list[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_BUFFER_SIZE, 32, // 单个像素buffer大小
            EGL_DEPTH_SIZE, 16, // 深度大小，3D渲染时必须设置
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR, // EGL配置支持OpenGL ES3.0
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT, // surface类型支持：窗口渲染，离屏渲染
            EGL_NONE // 配置结束符号，类似字符串中的 \0 用作系统识别来终止继续读取
    };
    const int num = 1;
    EGLConfig configs[num];
    int num_config[num];

    // 配置属性，得到EGLConfig，后续创建 EGLSurface 会用到
    if (!eglChooseConfig(eglDisplay, attr_list, configs, num, num_config))
    {
        logger.i("eglChooseConfig error : %d", eglGetError());
        return false;
    }

    eglConfig = configs[0];

    int attr_list2[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3, // 使用OpenGL ES3.0接口编程
            EGL_NONE
    };

    if (sharedContext == nullptr)
    {
        sharedContext = EGL_NO_CONTEXT;
    }
    // 创建EGL上下文，这里第三个参数参数如果有值，则会使这两个渲染线程共用一个EGL上下文，能共享纹理
    eglContext = eglCreateContext(eglDisplay, eglConfig, sharedContext, attr_list2);
    if (eglContext == EGL_NO_CONTEXT)
    {
        logger.i("eglCreateContext error : %d", eglGetError());
        return false;
    }

    logger.i("eglCreateContext created success!");
    return true;
}

bool HelloEGLContext::setPbufferSurface(int w, int h) {
    if (eglPbufferSurface)return true; // 成功

    const EGLint attrib_list[] = {
            EGL_WIDTH, w,
            EGL_HEIGHT, h,
            EGL_NONE
    };
    eglPbufferSurface = eglCreatePbufferSurface(eglDisplay, eglConfig, attrib_list);
    // 确定使用当前 EGLSurface 作为前置缓冲区buffer运送的目的地
    if (!eglMakeCurrent(eglDisplay, eglPbufferSurface, eglPbufferSurface, eglContext)) {
        logger.i("eglMakeCurrent error : %d", eglGetError());
        return false; // 失败
    }
    return true; // 成功
}

bool HelloEGLContext::usePbufferSurface() {
    if (!eglPbufferSurface) {
        logger.i("call setPbufferSurface(w,h) first");
        return false;
    }
    // 确定使用当前 EGLSurface 作为前置缓冲区buffer运送的目的地
    if (!eglMakeCurrent(eglDisplay, eglPbufferSurface, eglPbufferSurface, eglContext)) {
        logger.i("eglMakeCurrent error : %d", eglGetError());
        return false; // 失败
    }
    // 每次绘制之前，需要清空缓冲区内容，不然会在上一次的基础上绘制，导致雪花屏
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    return true;
}

bool HelloEGLContext::addSurface(void *_surface, bool encode)
{
    OHNativeWindow *surface = static_cast<OHNativeWindow *>(_surface);
    int32_t width = 0;
    int32_t height = 0;
    int ret = OH_NativeWindow_NativeWindowHandleOpt(surface, GET_BUFFER_GEOMETRY, &height, &width);
    if(ret != 0)
    {
        logger.i("Cannot get NativeWindow width and height");
        return false;
    }
    

    int attr_list[] = {
            EGL_NONE
    };
    // 将上层传递下来的surface制作成 EGLSurface
    EGLSurface _eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (EGLNativeWindowType) surface, attr_list);
    if(_eglSurface == EGL_NO_SURFACE) {
        logger.i("eglCreateWindowSurface error : %d", eglGetError());
        return -1;
    }
    logger.i("eglCreateWindowSurface success eglSurface(%p)", _eglSurface);
    
    // 确定使用当前 EGLSurface 作为前置缓冲区buffer运送的目的地
    if (!eglMakeCurrent(eglDisplay, _eglSurface, _eglSurface, eglContext))
    {
        logger.i("eglMakeCurrent error : %d", eglGetError());
        return false;
    }
    logger.i("eglMakeCurrent success!");


    uint64_t key = reinterpret_cast<uint64_t>(_surface);
    std::shared_ptr<EGLSurfaceCtx> ctx(new EGLSurfaceCtx());
    ctx->key = key;
    ctx->eglSurface = _eglSurface;
    ctx->width = width;
    ctx->height = height;
    ctx->mediacodec = encode;

    auto find = eglSurfaces.find(key);
    if (find == eglSurfaces.end())
    {
        eglSurfaces.insert(std::pair<uint64_t, std::shared_ptr<EGLSurfaceCtx>>(key, ctx));
        logger.i("addSurface [%llu] w*h[%d,%d]", key, width, height);
    } else
    {
        logger.i("Surface [%llu] is already added", key);
    }

    return true;
}

bool HelloEGLContext::removeSurface(void *_surface)
{
    uint64_t key = reinterpret_cast<uint64_t>(_surface);
    logger.i("removeSurface [%llu]", key);
    eglSurfaces.erase(key);
    return true;
}

int HelloEGLContext::getSurfaceSize()
{
    return static_cast<int>(eglSurfaces.size());
}

void HelloEGLContext::getSurfaceKeys(std::vector<uint64_t> *keys)
{
    for (const auto &item: eglSurfaces)
    {
        auto key = item.first;
        keys->push_back(key);
    }
}

int HelloEGLContext::getSurfaceWidth(uint64_t key)
{
    auto find = eglSurfaces.find(key);
    if (find != eglSurfaces.end())
    {
        return (*find).second->width;
    }
    return 0;
}

int HelloEGLContext::getSurfaceHeight(uint64_t key)
{
    auto find = eglSurfaces.find(key);
    if (find != eglSurfaces.end())
    {
        return (*find).second->height;
    }
    return 0;
}

void HelloEGLContext::setClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    this->red = r;
    this->green = g;
    this->blue = b;
    this->alpha = a;
}

bool HelloEGLContext::renderStart(uint64_t key)
{
    auto find = eglSurfaces.find(key);
    if (find == eglSurfaces.end())return false;
    std::shared_ptr<EGLSurfaceCtx> eglSurfaceCtx = (*find).second;
    // 确定使用当前 EGLSurface 作为前置缓冲区buffer运送的目的地
    if (!eglMakeCurrent(eglDisplay, eglSurfaceCtx->eglSurface, eglSurfaceCtx->eglSurface,
                        eglContext))
    {
        logger.i("eglMakeCurrent error : %d", eglGetError());
        return false;
    }
    // 恢复渲染目标到当前屏幕
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // 使用黑色清屏
    glClearColor(red, green, blue, alpha);
    // 每次绘制之前，需要清空缓冲区内容，不然会在上一次的基础上绘制，导致雪花屏
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    return true;
}

bool HelloEGLContext::renderEnd(uint64_t key)
{
    return renderEnd(key, -1);
}

bool HelloEGLContext::renderEnd(uint64_t key, int64_t ptsUs)
{
    auto find = eglSurfaces.find(key);
    if (find == eglSurfaces.end())return -1;
    std::shared_ptr<EGLSurfaceCtx> eglSurfaceCtx = (*find).second;

    // eglPresentationTimeANDROID 函数需要在 eglSwapBuffers 之前调用
    bool b1 = true;
    if (ptsUs >= 0 && eglSurfaceCtx->mediacodec)
    {
        // 通知 MediaCodec 编码画面内容
        b1 = eglPresentationTimeANDROID(eglDisplay, eglSurfaceCtx->eglSurface, ptsUs);
        if (!b1) // 出问题了，log输入
            logger.i("eglPresentationTimeANDROID[%ld]", ptsUs);
    }

    // 着色器代码运行完毕，绘制好内容之后，交换前后缓冲区，让画面更新
    bool b2 = eglSwapBuffers(eglDisplay, eglSurfaceCtx->eglSurface);

    return b1 && b2;
}

bool HelloEGLContext::addOutputTexture(int textureId, int width, int height) {
    auto find = eglTextures.find(textureId);
    if (find == eglTextures.end()) {
        std::shared_ptr<TextureCtx> ctx(new TextureCtx());
        ctx->fboId = 0;
        ctx->textureId = textureId;
        ctx->textureWidth = width;
        ctx->textureHeight = height;

        eglTextures.insert(std::pair<uint64_t, std::shared_ptr<TextureCtx>>(textureId, ctx));
        logger.i("addOutputTexture [%d] success", textureId);
    } else {
        logger.i("OutputTexture [%d] is already added", textureId);
    }

    return true;
}

void HelloEGLContext::removeOutputTexture(int textureId) {
    auto find = eglTextures.find(textureId);
    if (find != eglTextures.end()) {
        std::shared_ptr<TextureCtx> &ctx = (*find).second;
        if (ctx->fboId) {
            // 可能需要释放fbo资源
            const GLuint framebuffer = ctx->fboId;
            glDeleteFramebuffers(1, &framebuffer);
            ctx->fboId = 0;
            logger.i("removeOutputTexture delete fbo[%ld]", ctx->fboId);
        }

        size_t count = eglTextures.erase(textureId);
        if (count) {
            logger.i("removeOutputTexture [%ld] success", textureId);
        } else {
            logger.i("removeOutputTexture [%ld] error", textureId);
        }
    } else {
        logger.i("removeOutputTexture [%ld] not found", textureId);
    }
}

bool HelloEGLContext::getOutputTextures(std::vector<std::shared_ptr<TextureCtx>> &textures) {
    for (const auto &item: eglTextures) {
        std::shared_ptr<TextureCtx> ctx = item.second;
        textures.push_back(ctx);
    }

    return true;
}