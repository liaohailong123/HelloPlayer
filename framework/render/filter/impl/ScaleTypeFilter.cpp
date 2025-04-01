//
// Created by 廖海龙 on 2025/3/13.
//

#include "ScaleTypeFilter.hpp"

ScaleTypeFilter::ScaleTypeFilter() : IGLFilterNode("ScaleTypeFilter")
{
    logger.i("ScaleTypeFilter::ScaleTypeFilter(%p)", this);

    program = std::make_shared<ScaleTypeGLProgram>();

    program->initialized();
    program->setScaleType(ScaleTypeGLProgram::ScaleType::FitCenter);
}

ScaleTypeFilter::~ScaleTypeFilter()
{
    logger.i("ScaleTypeFilter::~ScaleTypeFilter(%p)", this);
}

void ScaleTypeFilter::onProcess(std::shared_ptr<GLFilterPacket> packet)
{
    int viewportWidth = packet->width;
    int viewportHeight = packet->height;
    if (fboId <= 0 || fboWidth != viewportWidth || fboHeight != viewportHeight)
    {
        // 生成fbo
        resetFbo(viewportWidth, viewportHeight, -1);
    }
    // 判断fbo和纹理是否创建成功，可能存在初始化失败，或者流程上已经被销毁
    if (fboId == 0 || fboTextureId == 0)return;

    std::shared_ptr<ScaleTypeGLProgram::Rect> src(new ScaleTypeGLProgram::Rect());
    std::shared_ptr<ScaleTypeGLProgram::Rect> dst(new ScaleTypeGLProgram::Rect());

    // 原点在左下,Y轴向上,X轴向右,视口坐标系
    src->left = 0;
    src->top = packet->textureHeight; // 资源
    src->right = packet->textureWidth;
    src->bottom = 0;

    dst->left = 0;
    dst->top = packet->height;
    dst->right = packet->width;
    dst->bottom = 0;

    // 将内容渲染到fbo纹理上
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    // 清除上次内容
    glViewport(0, 0, fboWidth, fboHeight);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    program->begin();
    program->setRect(src, dst);
    program->setTexture(&packet->texture, 1);
    program->draw(viewportWidth, viewportHeight, glm::value_ptr(packet->prjMat4)); // 绘制
    program->end();

    // 把本滤镜的渲染输出，设置到packet上，让下一个滤镜使用
    packet->texture = fboTextureId;  // 交换纹理
    packet->textureWidth = fboWidth; // 交换纹理的宽度
    packet->textureHeight = fboHeight; // 交换纹理的宽度
}