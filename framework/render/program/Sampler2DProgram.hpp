//
// Created by liaohailong on 2024/6/15.
//

#ifndef HELLOGL_OESGLPROGRAM_HPP
#define HELLOGL_OESGLPROGRAM_HPP

#include "IGLProgram.hpp"

/**
 * create by liaohailong
 * 2024/6/15 12:47
 * desc: 渲染 sampler2d 纹理，RGBA
 */
class Sampler2DProgram : public IGLProgram
{
public:
    explicit Sampler2DProgram(const char* tag = "Sampler2DProgram");

    ~Sampler2DProgram() override;

    std::string getVertexSource() override;

    std::string getFragmentSource() override;

    void onProgramCreated(GLuint program) override;

    void setTexture(GLuint *textures, int count) override;

    void setMirror(bool hMirror, bool vMirror) override;

    void draw(int width, int height, float projectMat[16]) override final;

    virtual void onDraw(int width, int height, float projectMat[16]);

protected:
    /**
     * @return 获取纹理类型, GL_TEXTURE_EXTERNAL_OES or GL_TEXTURE_2D
     */
    virtual GLenum getTextureType();

protected: // GLSL变量
    
    float sVertices[20] = {
            // positions         // texture coords
            -1.0f, -1.0f, 0.0f,     0.0f, 1.0f,
             1.0f, -1.0f, 0.0f,     1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f,     0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,     1.0f, 0.0f,
    };
    /**
     * 顶点数据buffer
     */
    GLuint vao, vbo;
    
    uniform mvpM;
    attribute position;
    attribute coordinate;

    uniform texture; // sampler2d
    uniform hMirror;
    uniform vMirror;

    /**
     * 纹理id
     */
    GLuint textureId;
    /**
     * 画面水平镜像
     */
    bool hMirrorVal;
    /**
     * 画面垂直镜像
     */
    bool vMirrorVal;

};


#endif //HELLOGL_OESGLPROGRAM_HPP
