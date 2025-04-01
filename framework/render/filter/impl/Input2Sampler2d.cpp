
//
// Created by 廖海龙 on 2025/3/13.
//

#include "Input2Sampler2d.hpp"

Input2Sampler2d::Input2Sampler2d(AVPixelFormat format) : IGLFilterNode("Input2Sampler2d"),
                                                         format(AV_PIX_FMT_NONE)
{
    updateAVPixelFormat(format);
    logger.i("Input2Sampler2d::Input2Sampler2d(format[%d],%p)", format, this);
}

Input2Sampler2d::~Input2Sampler2d()
{
    logger.i("Input2Sampler2d::~Input2Sampler2d(%p)", this);
}

void Input2Sampler2d::updateAVPixelFormat(AVPixelFormat f)
{
    if (f == format) return;
    this->format = f;

    switch (format)
    {
        case AV_PIX_FMT_NV12:
            // only NV12
            glProgram = std::make_shared<NV12GLProgram>();
            break;
        case AV_PIX_FMT_YUV422P:
        case AV_PIX_FMT_YUVJ422P:
        case AV_PIX_FMT_YUV420P:
        case AV_PIX_FMT_YUVJ420P:
            // I420P + YUV422P
            glProgram = std::make_shared<YUVGLProgram>();
            break;
        default:
            format = AV_PIX_FMT_YUV420P;
            glProgram = std::make_shared<YUVGLProgram>();
            logger.i("color format is not support default use to YUV420P");
            break;
    }

    // 初始化着色器
    if (glProgram)
    {
        glProgram->initialized();
    }
}

AVPixelFormat Input2Sampler2d::getAVPixelFormat() noexcept
{
    return format;
}

void Input2Sampler2d::onProcess(std::shared_ptr<GLFilterPacket> packet)
{
    int textureWidth = packet->textureWidth;
    int textureHeight = packet->textureHeight;
    if (fboId <= 0 || fboWidth != textureWidth || fboHeight != textureHeight)
    {
        // 生成fbo
        resetFbo(textureWidth, textureHeight, -1);
    }

    // 判断fbo和纹理是否创建成功，可能存在初始化失败，或者流程上已经被销毁
    if (fboId == 0 || fboTextureId == 0)return;

    if(packet->textureData)
    {
        // 将内容渲染到fbo纹理上
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);

        glProgram->begin();
        // YUV内容设置
        glProgram->setTextureData(packet->textureData);
        // 绘制
        glProgram->draw(textureWidth, textureHeight, glm::value_ptr(packet->prjMat4));
        // 着色器结束使用
        glProgram->end();
    }

    // 把本滤镜的渲染输出，设置到packet上，让下一个滤镜使用
    packet->texture = fboTextureId;  // 交换纹理
    packet->textureWidth = fboWidth; // 交换纹理的宽度
    packet->textureHeight = fboHeight; // 交换纹理的宽度
}
