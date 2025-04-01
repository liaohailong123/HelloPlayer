//
// Created by liaohailong on 2024/6/13.
//

#ifndef HELLOOPENGLES_HELLOEGLCONTEXT_HPP
#define HELLOOPENGLES_HELLOEGLCONTEXT_HPP


#include "framework/render/IGLContext.hpp"

#include <functional>
#include <vector>
#include <unordered_map>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>

// 输出log日志
#include "framework/log/Logger.hpp"
#include "framework/render/program/GLSLUtil.hpp"

/**
 * create by liaohailong
 * 2024/6/13 22:25
 * desc:  EGL上下文
 */
class HelloEGLContext : public IGLContext {
public:
    typedef struct {
        /**
         * EGLSurface 的一个唯一标识
         */
        uint64_t key;
        /**
         * EGL渲染缓冲区
         */
        EGLSurface eglSurface;
        /**
         * surface width
         */
        int width;

        /**
         * surface height
         */
        int height;

        /**
         * true表示该eglSurface是由MediaCodec创建出来的，用于编码
         */
        bool mediacodec;
    } EGLSurfaceCtx;

public:
    HelloEGLContext();

    ~HelloEGLContext() override;

    bool init(void *sharedContext) override;

    bool setPbufferSurface(int width, int height) override;

    bool usePbufferSurface() override;

    bool addSurface(void *surface, bool encode) override;

    bool removeSurface(void *surface) override;

    int getSurfaceSize() override;

    void getSurfaceKeys(std::vector<uint64_t>* keys) override;

    int getSurfaceWidth(uint64_t key) override;

    int getSurfaceHeight(uint64_t key) override;

    void setClearColor(float red, float green, float blue, float alpha) override;

    bool renderStart(uint64_t key) override;

    bool renderEnd(uint64_t key);

    bool renderEnd(uint64_t key, int64_t ptsUs) override;

    bool addOutputTexture(int textureId, int width, int height) override;

    void removeOutputTexture(int textureId) override;

    bool getOutputTextures(std::vector<std::shared_ptr<TextureCtx>> &textures) override;

private:
    Logger logger;
    EGLDisplay eglDisplay;
    EGLConfig eglConfig;
    EGLContext eglContext;
    EGLSurface eglPbufferSurface;
    std::unordered_map<uint64_t , std::shared_ptr<EGLSurfaceCtx>> eglSurfaces;
    /**
     * 待渲染的纹理
     */
    std::unordered_map<uint64_t, std::shared_ptr<TextureCtx>> eglTextures;

    // clear color
    GLfloat red, green, blue, alpha;
};


#endif //HELLOOPENGLES_HELLOEGLCONTEXT_HPP
