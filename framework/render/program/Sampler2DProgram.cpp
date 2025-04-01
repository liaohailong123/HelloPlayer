//
// Created by liaohailong on 2024/6/15.
//

#include "Sampler2DProgram.hpp"

Sampler2DProgram::Sampler2DProgram(const char *tag)
        : IGLProgram(tag), textureId(-1), hMirrorVal(false), vMirrorVal(false), mvpM(-1),
          position(-1), coordinate(-1), texture(-1), hMirror(-1), vMirror(-1)
{
    logger.i("Sampler2DProgram::Sampler2DProgram()");
}

Sampler2DProgram::~Sampler2DProgram()
{
    logger.i("Sampler2DProgram::~Sampler2DProgram()");
}

std::string Sampler2DProgram::getVertexSource()
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

std::string Sampler2DProgram::getFragmentSource()
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
        t_coordinate = vec2(1.0-v_coordinate.x,v_coordinate.y);
    }
    if(u_vMirror) {
        t_coordinate = vec2(t_coordinate.x,1.0-t_coordinate.y);
    }

    gl_FragColor = texture2D(u_texture,t_coordinate);
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
}

void Sampler2DProgram::setTexture(GLuint *textures, int count)
{
    textureId = textures[0];
}

void Sampler2DProgram::setMirror(bool _hMirror, bool _vMirror)
{
    hMirrorVal = _hMirror;
    vMirrorVal = _vMirror;
}

void Sampler2DProgram::draw(int width, int height, float projectMat[4 * 4])
{
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

    // 绘制图元
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 关闭顶点输入
    glDisableVertexAttribArray(position);
    glDisableVertexAttribArray(coordinate);

}

GLenum Sampler2DProgram::getTextureType()
{
    return GL_TEXTURE_2D;
}