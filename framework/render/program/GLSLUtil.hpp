//
// Created by liaohailong on 2024/6/14.
//

#ifndef HELLOGL_GLSLUTIL_HPP
#define HELLOGL_GLSLUTIL_HPP


#include <string>

#ifdef  __ANDROID__
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES3/gl32.h>
#include <GLES3/gl3platform.h>

#define HAVE_GL2_EXT 1
#endif

#ifdef  __OHOS__
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES3/gl32.h>
#include <GLES3/gl3platform.h>

#define HAVE_GL2_EXT 1
#endif

#if __APPLE__
#include <OpenGLES/ES3/gl.h>        // 用于 OpenGL ES 3.0
#include <OpenGLES/ES3/glext.h>     // 用于 OpenGL ES 3.0 扩展
#endif

/*
 * OpenGL 常用于桌面端开发（Mac Windows Linux）
 * OpenGL ES 常用于移动端和嵌入式开发（Android iOS）
 *
 * OpenGL 与 OpenGL ES 之间的版本关系
 *
 * OpenGL 1.x -> OpenGL ES 1.x
 * OpenGL 2.0 -> OpenGL ES 2.0
 * OpenGL 3.x -> OpenGL ES 3.x
 * OpenGL 4.x -> OpenGL ES 3.2
 *
 * OpenGL 与 GLSL 之间的版本关系
 *
 * OpenGL 1.0 - 1.4 固定管线
 * OpenGL 2.0 -> GLSL 1.10    #version 110 core
 * OpenGL 2.1 -> GLSL 1.20
 * OpenGL 3.0 -> GLSL 1.30    支持关键词： in out
 * OpenGL 3.1 -> GLSL 1.40
 * OpenGL 3.2 -> GLSL 1.50
 * OpenGL 3.3 -> GLSL 3.30    #version 330 core 支持关键词：layout
 * OpenGL 4.0 -> GLSL 4.00
 * OpenGL 4.1 -> GLSL 4.10
 * OpenGL 4.2 -> GLSL 4.20
 * OpenGL 4.3 -> GLSL 4.30
 * OpenGL 4.4 -> GLSL 4.40
 * OpenGL 4.5 -> GLSL 4.50
 * OpenGL 4.6 -> GLSL 4.60
 *
 *
 * OpenGL ES 与 GLSL 之间的版本关系
 *
 * OpenGL ES 1.0 - 1.1 固定管线
 * OpenGL ES 2.0 -> GLSL ES 1.00 #version 100
 * OpenGL ES 3.0 -> GLSL ES 3.00 #version 300 es 支持关键词：layout
 * OpenGL ES 3.1 -> GLSL ES 3.10 #version 310 es 支持属性：block
 * OpenGL ES 3.2 -> GLSL ES 3.20 #version 320 es
 *
 *
 * Android 版本	支持的 OpenGL ES 版本
 * 1.0 - 1.5	1.0, 1.1
 * 2.0 - 2.1	1.0, 1.1
 * 2.2	        1.0, 1.1
 * 2.3	        1.0, 1.1, 2.0
 * 3.0	        1.0, 1.1, 2.0
 * 4.0	        1.0, 1.1, 2.0
 * 4.1 - 4.3	1.0, 1.1, 2.0, 3.0
 * 4.4	        1.0, 1.1, 2.0, 3.0
 * 5.0 - 5.1	1.0, 1.1, 2.0, 3.0, 3.1
 * 6.0	        1.0, 1.1, 2.0, 3.0, 3.1
 * 7.0 - 7.1	1.0, 1.1, 2.0, 3.0, 3.1, 3.2
 * 8.0 - 8.1	1.0, 1.1, 2.0, 3.0, 3.1, 3.2
 * 9.0	        1.0, 1.1, 2.0, 3.0, 3.1, 3.2
 * 10	        1.0, 1.1, 2.0, 3.0, 3.1, 3.2
 * 11	        1.0, 1.1, 2.0, 3.0, 3.1, 3.2
 * 12	        1.0, 1.1, 2.0, 3.0, 3.1, 3.2
 *
 * iOS 版本      支持的 OpenGL ES 版本
 * 2.0 - 6.x    1.0 2.0
 * 7.x - 8.x    2.0 3.0
 * 9.x - 11.x   2.0 3.0
 * 12.x - 13.x  2.0 3.0
 * 14.x 及以后   2.0 3.0
 *
 * Metal
 * Metal是Apple推出的低级、高性能的图形和计算API，专门为iOS、macOS和tvOS设计。
 *
 * 特点
 * 高性能：Metal提供了比OpenGL ES更低的开销，允许应用程序更直接地访问GPU，从而提高渲染性能。
 * 丰富的功能：支持复杂的渲染和计算任务，如粒子系统、物理模拟等。
 * 跨平台：除了iOS，还支持macOS和tvOS。
 *
 * */


namespace GLSLUtil
{

#ifdef __ANDROID__
    /**
     * 创建OES纹理
     * @param textures 这个数组用来接收待创建的OES纹理
     * @param size 数组有效长度（创建个数）
     * @return 0=success -1=error
     */
    int generateOESTexture(GLuint *textures, int size);
#endif

    /**
     * 创建FBO，并且依附到sampler2d纹理上
     * @param texture sampler2d纹理
     * @param width 纹理宽度
     * @param height 纹理高度
     * @return fbo id
     */
    GLuint generateFboTexture(GLuint &texture, int width, int height);

    /**
     * 删除FBO
     * @param framebuffer 渲染缓冲区
     * @param texture 纹理id
     */
    void deleteFboTexture(GLuint &framebuffer, GLuint &texture);

    /**
     * 将外部创建好的sampler2d纹理依附到fbo上
     * @param texture sampler2d纹理
     * @return fbo id
     */
    GLuint textureAttach2Fbo(GLuint texture);

    /**
     * 绑定VAO VBO
     * Vertex Buffer Object + Vertex Array Object
     * @param sVertices 固定格式：前三个顶点坐标，后两个纹理坐标
                 // positions         // texture coords
                -1.0f, -1.0f, 0.0f,     0.0f, 1.0f,
     * @param vertexIndex 顶点着色器中，顶点坐标属性句柄
     * @param textureIndex 顶点着色器中，纹理坐标属性句柄
     * @return
     */
    bool generateVAOVBO(float *sVertices,int count, GLuint vertexIndex, GLuint textureIndex, GLuint &vao, GLuint &vbo);

    void deleteVAOVBO(GLuint &vao, GLuint &vbo);
}


#endif //HELLOGL_GLSLUTIL_HPP
