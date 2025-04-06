//
// Created by 廖海龙 on 2025/3/13.
//

#include "ScaleTypeGLProgram.hpp"

ScaleTypeGLProgram::ScaleTypeGLProgram()
        : Sampler2DProgram("ScaleTypeGLProgram"), moduleMat4(glm::identity<glm::mat4>()), scaleType(FitXY)
{
    logger.i("CropGLProgram::CropGLProgram(%p)", this);
}

ScaleTypeGLProgram::~ScaleTypeGLProgram()
{
    logger.i("CropGLProgram::~CropGLProgram(%p)", this);
}

void ScaleTypeGLProgram::setScaleType(ScaleTypeGLProgram::ScaleType type)
{
    this->scaleType = type;
}

void ScaleTypeGLProgram::setRect(const std::shared_ptr<Rect> &src,
                                 const std::shared_ptr<Rect> &dst)
{
    switch (scaleType) {
        case FitXY:
            moduleMat4 = glm::identity<glm::mat4>();
            break;
        case FitCenter:
            moduleMat4 = makeFitCenterMat4(src,dst);
            break;
        case FitStart:
            // 后续支持
            break;
        case FitEnd:
            // 后续支持
            break;
        case CenterCrop:
            // 后续支持
            break;
    }
}

glm::mat4 ScaleTypeGLProgram::makeFitCenterMat4(const std::shared_ptr<Rect> &src,
                                                const std::shared_ptr<Rect> &dst)
{
    glm::mat4 mat4 = glm::identity<glm::mat4>();

    // 原点在左下,Y轴向上,X轴向右,视口坐标系
    int srcWidth = src->right - src->left;
    int srcHeight = src->top - src->bottom;
    int dstWidth = dst->right - dst->left;
    int dstHeight = dst->top - dst->bottom;

    float scaleX = float(dstWidth) / float(srcWidth);
    float scaleY = float(dstHeight) / float(srcHeight);

    float finalScaleX, finalScaleY;

    // 选择让一个方向的 scale = 1.0
    if (scaleX < scaleY) {
        finalScaleX = 1.0f;
        finalScaleY = scaleX / scaleY;
    } else {
        finalScaleY = 1.0f;
        finalScaleX = scaleY / scaleX;
    }

    mat4 = glm::scale(mat4,glm::vec3(finalScaleX,finalScaleY,1.0f));

    return mat4;
}

glm::mat4 ScaleTypeGLProgram::makeCropCenterMat4(const std::shared_ptr<Rect> &src,
                             const std::shared_ptr<Rect> &dst)
{
    glm::mat4 mat4 = glm::identity<glm::mat4>();

    return mat4;
}

void ScaleTypeGLProgram::onDraw(int width, int height, float projectMat[16])
{
    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(texture, 0);

    // 配置矩阵
    auto _moduleMat4 = glm::identity<glm::mat4>();
    glm::mat4 _projectMat4 = glm::make_mat4(projectMat);

    // 纹理坐标旋转
    float rotation = getRotation();
    float angle = glm::radians(rotation);
    glm::vec3 axis(0.0f, 0.0f, 1.0f); // 旋转轴是 Z 轴
    _moduleMat4 = glm::rotate(this->moduleMat4, angle, axis);

    // 镜像操作
    glUniform1i(hMirror, hMirrorVal ? 1 : 0); // 水平镜像
    glUniform1i(vMirror, vMirrorVal ? 1 : 0); // 垂直镜像

    // 计算出mvp矩阵
    glm::mat4 mvpMat4 = _projectMat4 * _moduleMat4;
    glUniformMatrix4fv(mvpM, 1, GL_FALSE, glm::value_ptr(mvpMat4));
}