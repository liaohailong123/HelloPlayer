//
// Created by 廖海龙 on 2025/3/13.
//

#include "ScaleTypeGLProgram.hpp"

ScaleTypeGLProgram::ScaleTypeGLProgram()
        : IGLProgram("ScaleTypeGLProgram"), mvpM(-1), position(-1), coordinate(-1), texture(-1),
          hMirror(0), vMirror(0), textureId(-1), hMirrorVal(false), vMirrorVal(false),
          moduleMat4(glm::identity<glm::mat4>()), scaleType(FitXY)
{
    logger.i("CropGLProgram::CropGLProgram(%p)", this);
}

ScaleTypeGLProgram::~ScaleTypeGLProgram()
{
    logger.i("CropGLProgram::~CropGLProgram(%p)", this);
}

std::string ScaleTypeGLProgram::getVertexSource()
{
    const char source[] = R"(
#version 100
uniform mat4 u_mvpM;
attribute vec3 i_position;
attribute vec2 i_coordinate;
varying vec2 v_coordinate;

void main()
{
    v_coordinate = i_coordinate;
    gl_Position = u_mvpM * vec4(i_position,1.0);
}

)";
    return source;
}

std::string ScaleTypeGLProgram::getFragmentSource()
{
    const char source[] = R"(
#version 100
precision mediump float;
varying vec2 v_coordinate;
uniform sampler2D u_texture;
uniform bool u_hMirror;
uniform bool u_vMirror;

void main()
{
    vec2 t_coordinate = v_coordinate;
    if(u_hMirror) {
        t_coordinate = vec2(1.0-t_coordinate.x,t_coordinate.y);
    }
    if(u_vMirror) {
        t_coordinate = vec2(t_coordinate.x,1.0-t_coordinate.y);
    }
    gl_FragColor = texture2D(u_texture,t_coordinate);
}

)";
    return source;
}

void ScaleTypeGLProgram::onProgramCreated(GLuint program)
{
    mvpM = glGetUniformLocation(program, "u_mvpM");
    position = glGetAttribLocation(program, "i_position");
    coordinate = glGetAttribLocation(program, "i_coordinate");
    texture = glGetUniformLocation(program, "u_texture");
    hMirror = glGetUniformLocation(program, "u_hMirror");
    vMirror = glGetUniformLocation(program, "u_vMirror");
}

void ScaleTypeGLProgram::setTexture(GLuint *textures, int count)
{
    textureId = static_cast<GLuint>(textures[0]);
//    logger.i("CropGLProgram::setTexture[%d]", textureId);
}

void ScaleTypeGLProgram::setMirror(bool _hMirror, bool _vMirror)
{
    this->hMirrorVal = _hMirror;
    this->vMirrorVal = _vMirror;
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

void ScaleTypeGLProgram::draw(int width, int height, float projectMat[16]) {
    // 确认视口位置和大小，可用作局部渲染
    glViewport(0, 0, width, height);

    // 绑定顶点坐标数组
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), vertexPos);

    // 绑定纹理坐标数组
    glEnableVertexAttribArray(coordinate);
    glVertexAttribPointer(coordinate, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), texturePos);

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

    // 绘制图元
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 关闭顶点输入
    glDisableVertexAttribArray(position);
    glDisableVertexAttribArray(coordinate);
}