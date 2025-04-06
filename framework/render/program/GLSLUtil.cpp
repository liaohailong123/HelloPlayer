//
// Created by 廖海龙 on 2024/11/4.
//

#include "GLSLUtil.hpp"

#ifdef __ANDROID__
int GLSLUtil::generateOESTexture(GLuint *textures, int size)
{
    glGenTextures(size, textures);
    for (int i = 0; i < size; ++i)
    {
        GLuint texId = textures[i];
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, texId);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
    }

    return 0;
}
#endif

/**
 * 创建FBO，并且依附到sampler2d纹理上
 * @param texture sampler2d纹理
 * @param width 纹理宽度
 * @param height 纹理高度
 * @return 大于0表示返回fbo id，返回0表示创建失败
 */
GLuint GLSLUtil::generateFboTexture(GLuint &texture, int width, int height) {

    GLuint framebuffer;

    // 创建帧缓冲对象
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // 创建纹理
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 配置纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 将纹理附加到帧缓冲对象的颜色附件上
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // 检查 FBO 完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return 0;
    }

    // 解绑帧缓冲对象
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return framebuffer;
}

/**
 * 将外部创建好的sampler2d纹理依附到fbo上
 * @param texture sampler2d纹理
 * @return fbo id
 */
GLuint GLSLUtil::textureAttach2Fbo(GLuint texture) {
    GLuint framebuffer;

    // 创建帧缓冲对象
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glBindTexture(GL_TEXTURE_2D, texture);
    // 将纹理附加到帧缓冲对象的颜色附件上
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // 检查 FBO 完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return 0;
    }

    // 解绑帧缓冲对象
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return framebuffer;
}


bool GLSLUtil::generateVAOVBO(float *sVertices,int count, GLuint vertexIndex, GLuint textureIndex, GLuint &_vao, GLuint &_vbo)
{

    GLuint vao, vbo;
    
    // 创建 VAO + VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sVertices)*count, sVertices, GL_STATIC_DRAW);

    // 顶点位置属性 layout(location = 0)
    glVertexAttribPointer(vertexIndex, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(vertexIndex);

    // 纹理坐标属性 layout(location = 1)
    glVertexAttribPointer(textureIndex, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(textureIndex);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    _vao = vao;
    _vbo = vbo;
    
    return true;
}

void GLSLUtil::deleteVAOVBO(GLuint &vao, GLuint &vbo)
{
    // 删除 VBO 和 VAO
    if (vbo > 0)
    {
        glDeleteVertexArrays(1, &vao);
    }
    
    if (vao > 0)
    {
        glDeleteBuffers(1, &vbo);
    }

    vao = 0;
    vbo = 0;
}

/**
 * 删除FBO
 * @param framebuffer 渲染缓冲区
 * @param texture 纹理id
 */
void GLSLUtil::deleteFboTexture(GLuint &framebuffer, GLuint &texture) {
    // 销毁 framebuffer
    if (framebuffer != 0) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }

    // 销毁 texture
    if (texture != 0) {
        glDeleteTextures(1, &texture);
        texture = 0;
    }
}
