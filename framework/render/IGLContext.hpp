//
// Created by liaohailong on 2024/6/13.
//

#ifndef HELLOOPENGLES_IGLCONTEXT_HPP
#define HELLOOPENGLES_IGLCONTEXT_HPP


#include <cstdio>
#include <vector>

/**
 * create by liaohailong
 * 2024/6/13 22:22
 * desc: GL上下文环境
 */
class IGLContext {
public:
    typedef struct {
        /**
         * Frame Buffer Object
         */
        int fboId;
        /**
         * 纹理id
         */
        int textureId;
        /**
         * 纹理宽度
         */
        int textureWidth;
        /**
         * 纹理高度
         */
        int textureHeight;
    } TextureCtx;
public:
    IGLContext() = default;

    virtual ~IGLContext() = default;

    virtual bool init(void *sharedContext) = 0;

    virtual bool setPbufferSurface(int width, int height) = 0;

    virtual bool usePbufferSurface() = 0;

    virtual bool addSurface(void *surface, bool encode) = 0;

    virtual bool removeSurface(void *surface) = 0;

    virtual int getSurfaceSize() = 0;

    virtual void getSurfaceKeys(std::vector<uint64_t> *keys) = 0;

    virtual int getSurfaceWidth(uint64_t key) = 0;

    virtual int getSurfaceHeight(uint64_t key) = 0;

    virtual void setClearColor(float red, float green, float blue, float alpha) = 0;

    virtual bool renderStart(uint64_t key) = 0;

    virtual bool renderEnd(uint64_t key, int64_t ptsUs) = 0;

    virtual bool addOutputTexture(int textureId, int width, int height) = 0;

    virtual void removeOutputTexture(int textureId) = 0;

    virtual bool getOutputTextures(std::vector<std::shared_ptr<TextureCtx>> &textures) = 0;

};


#endif //HELLOOPENGLES_IGLCONTEXT_HPP
