//
// Created by 廖海龙 on 2025/1/16.
//

#ifndef HELLOPLAYER_NV12GLPROGRAM_HPP
#define HELLOPLAYER_NV12GLPROGRAM_HPP

#include "Sampler2DProgram.hpp"

extern "C"
{
#include <libavutil/avutil.h>
}


/**
 * Author: liaohailong
 * Date: 2025/1/16
 * Time: 10:57
 * Description: 渲染 sampler 2D 纹理，NV12
 **/
class NV12GLProgram : public Sampler2DProgram
{
public:
    explicit NV12GLProgram();

    ~NV12GLProgram() override;

    std::string getFragmentSource() override;

    void onProgramCreated(GLuint program) override;

    /**
     * @param data 设置纹理数据
     */
    void setTextureData(std::shared_ptr<TextureData> data) override;

    void setMirror(bool hMirror, bool vMirror) override;

    void onDraw(int width, int height, float projectMat[16]) override;

protected:
    void bindYTexture(GLuint texture, int width, int height, GLint lineSize, const void *pixels);
    void bindUVTexture(GLuint texture, int width, int height, GLint lineSize, const void *pixels);
    void releaseYUVTexture();

private: // GLSL变量
    uniform mvpM;
    attribute position;
    attribute coordinate;

    uniform y_tex; // sampler2D
    uniform uv_tex; // sampler2D


private: // 传入着色器的变量
    /**
     * sampler2d纹理id
     */
    GLuint textureYId;
    GLuint textureUVId;

    int textureWidth;
    int textureHeight;
};


#endif //HELLOPLAYER_NV12GLPROGRAM_HPP
