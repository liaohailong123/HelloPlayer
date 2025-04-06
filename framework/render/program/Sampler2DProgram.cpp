//
// Created by liaohailong on 2024/6/15.
//

#include "Sampler2DProgram.hpp"

Sampler2DProgram::Sampler2DProgram(const char *tag)
    : IGLProgram(tag), vao(0), vbo(0), textureId(-1), hMirrorVal(false), vMirrorVal(false), mvpM(-1), position(-1),
      coordinate(-1), texture(-1), hMirror(-1), vMirror(-1)
{
    logger.i("Sampler2DProgram::Sampler2DProgram()");
}

Sampler2DProgram::~Sampler2DProgram()
{
    GLSLUtil::deleteVAOVBO(vao, vbo);
    logger.i("Sampler2DProgram::~Sampler2DProgram()");
}

std::string Sampler2DProgram::getVertexSource()
{
    const char source[] = R"(#version 300 es
uniform mat4 u_mvpM;
layout(location = 0) in vec3 i_position; // 顶点位置
layout(location = 1) in vec2 i_coordinate; // 纹理坐标
out vec2 v_coordinate; // 输出纹理坐标

void main()
{
    v_coordinate = i_coordinate;
    gl_Position = u_mvpM * vec4(i_position, 1.0);
}
    )";
    return source;
}

std::string Sampler2DProgram::getFragmentSource()
{
    const char source[] = R"(#version 300 es
precision mediump float;
in vec2 v_coordinate; // 从顶点着色器传递的纹理坐标
uniform sampler2D u_texture;
uniform bool u_hMirror;
uniform bool u_vMirror;
out vec4 fragColor; // 输出颜色

void main()
{
    vec2 t_coordinate = v_coordinate;
    t_coordinate.x = u_hMirror ? (1.0 - t_coordinate.x) : t_coordinate.x;
    t_coordinate.y = u_vMirror ? (1.0 - t_coordinate.y) : t_coordinate.y;

    fragColor = texture(u_texture, t_coordinate); // 使用纹理采样器
}
    )";

    return source;
}

void Sampler2DProgram::onProgramCreated(GLuint program)
{
    mvpM = glGetUniformLocation(program, "u_mvpM");
    position = glGetAttribLocation(program, "i_position");
    coordinate = glGetAttribLocation(program, "i_coordinate");
    texture = glGetUniformLocation(program, "u_texture");
    hMirror = glGetUniformLocation(program, "u_hMirror");
    vMirror = glGetUniformLocation(program, "u_vMirror");

    GLSLUtil::generateVAOVBO(sVertices, 20, position, coordinate, vao, vbo);
}

void Sampler2DProgram::setTexture(GLuint *textures, int count) { textureId = textures[0]; }

void Sampler2DProgram::setMirror(bool _hMirror, bool _vMirror)
{
    hMirrorVal = _hMirror;
    vMirrorVal = _vMirror;
}

void Sampler2DProgram::draw(int width, int height, float projectMat[4 * 4])
{
    // 确认视口位置和大小，可用作局部渲染
    glViewport(0, 0, width, height);

    // 通知绘制
    onDraw(width, height, projectMat);

    // 直接绑定 VAO 绘制
    glBindVertexArray(vao);
    // 绘制图元
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 解绑 VAO 别让后续着色器改动它
    glBindVertexArray(0);
}

void Sampler2DProgram::onDraw(int width, int height, float projectMat[16])
{
    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(getTextureType(), textureId);
    glUniform1i(texture, 0);
    
    // 配置矩阵
    auto modelMat4 = glm::identity<glm::mat4>();
    glm::mat4 _projectMat4 = glm::make_mat4(projectMat);

    // 纹理坐标旋转
    float rotation = getRotation();
    float angle = glm::radians(rotation);
    glm::vec3 axis(0.0f, 0.0f, 1.0f); // 旋转轴是 Z 轴
    modelMat4 = glm::rotate(modelMat4, angle, axis);
    // 镜像操作
    glUniform1i(hMirror, hMirrorVal ? 1 : 0); // 水平镜像
    glUniform1i(vMirror, vMirrorVal ? 1 : 0); // 垂直镜像
    // 计算出mvp矩阵
    glm::mat4 mvpMat4 = _projectMat4 * modelMat4;

    glUniformMatrix4fv(mvpM, 1, GL_FALSE, glm::value_ptr(mvpMat4));
}

GLenum Sampler2DProgram::getTextureType() { return GL_TEXTURE_2D; }