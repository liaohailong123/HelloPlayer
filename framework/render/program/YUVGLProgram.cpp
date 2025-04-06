//
// Created by liaohailong on 2024/6/15.
//

#include "YUVGLProgram.hpp"

YUVGLProgram::YUVGLProgram(): Sampler2DProgram("YUVGLProgram"), mvpM(-1), position(-1), coordinate(-1), y_tex(-1), u_tex(-1),
          v_tex(-1), textureYId(0), textureUId(0), textureVId(0), textureWidth(0), textureHeight(0)
{
    logger.i("YUVGLProgram::YUVGLProgram(%p)", this);
}

YUVGLProgram::~YUVGLProgram()
{
    releaseYUVTexture();
    logger.i("YUVGLProgram::~YUVGLProgram(%p)", this);
}

std::string YUVGLProgram::getFragmentSource()
{
    const char source[] = R"(#version 300 es
precision mediump float;

in vec2 v_coordinate;

uniform sampler2D y_tex;
uniform sampler2D u_tex;
uniform sampler2D v_tex;

out vec4 fragColor; // 输出颜色

void main()
{

    float y = texture(y_tex, v_coordinate).r;
    float u = texture(u_tex, v_coordinate).r - 0.5;
    float v = texture(v_tex, v_coordinate).r - 0.5;

    float r = y + 1.402 * v;
    float g = y - 0.344 * u - 0.714 * v;
    float b = y + 1.772 * u;

    fragColor = vec4(r, g, b, 1.0);
}
    )";

    return source;
}

void YUVGLProgram::onProgramCreated(GLuint program)
{
    mvpM = glGetUniformLocation(program, "u_mvpM");
    position = glGetAttribLocation(program, "i_position");
    coordinate = glGetAttribLocation(program, "i_coordinate");
    y_tex = glGetUniformLocation(program, "y_tex");
    u_tex = glGetUniformLocation(program, "u_tex");
    v_tex = glGetUniformLocation(program, "v_tex");

    GLSLUtil::generateVAOVBO(sVertices, 20, position, coordinate, vao, vbo);
}


void YUVGLProgram::setTextureData(std::shared_ptr<TextureData> image)
{
    int format = image->getFormat(); // 画面的格式 YUV420P YUV422P
    uint8_t **data = image->getData(); // 像素内容 [0]=Y [1]=U [2]=V
    int *lineSize = image->getLineSize(); // 每种像素的宽度（一行有多少个像素）

    // YUV420P 4个Y共用一组UV分量
    // [0][0] [0][1] Y Y
    // [1][0] [1][1] Y Y
    // [0][0] U
    // [0][0] V
    //   Y        U        V
    // [0][0] + [0][0] + [0][0] = YUV
    // [0][1] + [0][0] + [0][0] = YUV
    // [1][0] + [0][0] + [0][0] = YUV
    // [1][1] + [0][0] + [0][0] = YUV
    // 总像素点的 3/2倍
    //
    // YUV422P 2个Y共用一组UV分量（水平方向两个Y共用一组UV）
    // [0][0] [0][1] Y Y
    // [1][0] [1][1] Y Y
    // [0][0] U
    // [1][0] U
    // [0][0] V
    // [1][0] V
    //    Y        U        V
    // [0][0] + [0][0] + [0][0] = YUV
    // [0][1] + [0][0] + [0][0] = YUV
    // [1][0] + [1][0] + [1][0] = YUV
    // [1][1] + [1][0] + [1][0] = YUV
    // 总像素点的 2倍
    // RGBA 是总像素点的 4倍 （每个像素点都有[R,G,B,A]四个字节的值组成）

    int yWidth = image->getWidth();
    int yHeight = image->getHeight();

    int uvHeight = yHeight;
    int uvWidth = yWidth;


    if (format == AV_PIX_FMT_YUV422P || format == AV_PIX_FMT_YUVJ422P)
    {
        // YUV422
        uvWidth = yWidth / 2;
        uvHeight = yHeight;
    } else if (format == AV_PIX_FMT_YUV420P || format == AV_PIX_FMT_YUVJ420P)
    {
        // YUV420
        uvWidth = yWidth / 2;
        uvHeight = yHeight / 2;
    }

    // 判断尺寸是否发生了变化
    if (textureWidth != yWidth || textureHeight != yHeight)
    {
        releaseYUVTexture();
    }
    if (textureYId == 0 || textureUId == 0 || textureVId == 0)
    {
        textureWidth = yWidth;
        textureHeight = yHeight;

        // 创建YUV三个纹理
        GLuint textures[3];
        glGenTextures(3, &(textures[0]));

        textureYId = textures[0];
        textureUId = textures[1];
        textureVId = textures[2];
    }

    // 硬件解码,返回的buffer可能是一个正方形
    GLint _lineSizeY = lineSize[0];
    GLint _lineSizeUV = lineSize[0] / 2;
//    logger.i("w*h[%d,%d] lineSizeY=%d lineSizeUY=%d", yWidth, yHeight, _lineSizeY, _lineSizeUY);
    bindYUVTexture(textureYId, yWidth, yHeight, _lineSizeY, data[0]);
    bindYUVTexture(textureUId, uvWidth, uvHeight, _lineSizeUV, data[1]);
    bindYUVTexture(textureVId, uvWidth, uvHeight, _lineSizeUV, data[2]);
}

void YUVGLProgram::setMirror(bool hMirror, bool vMirror)
{
    // 暂不支持
}

void YUVGLProgram::onDraw(int width, int height, float projectMat[16])
{
    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYId);
    glUniform1i(y_tex, 0);

    // 绑定纹理
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureUId);
    glUniform1i(u_tex, 1);

    // 绑定纹理
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureVId);
    glUniform1i(v_tex, 2);

    // 配置矩阵
    auto modelMat4 = glm::identity<glm::mat4>();
    glm::mat4 _projectMat4 = glm::make_mat4(projectMat);

    // 纹理坐标旋转
    float rotation = getRotation();
    float angle = glm::radians(rotation);
    glm::vec3 axis(0.0f, 0.0f, 1.0f); // 旋转轴是 Z 轴
    modelMat4 = glm::rotate(modelMat4, angle, axis);

    // 计算出mvp矩阵
    glm::mat4 mvpMat4 = _projectMat4 * modelMat4;

    // 第三个参数，transpose 表示是否需要 转置矩阵：将行与列交换（对称矩阵）
    glUniformMatrix4fv(mvpM, 1, GL_FALSE, glm::value_ptr(mvpMat4));
}


void YUVGLProgram::bindYUVTexture(GLuint texture, int width, int height,
                                  GLint lineSize, const void *pixels)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 按照1字节读写
#ifdef HAVE_GL2_EXT
    glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, lineSize); // 一行多少数据
#else
    // iOS上没有 GLES2.0扩展,无法指定 row length 暂时这样处理
    width = width < lineSize? lineSize: width;
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height,
                 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void YUVGLProgram::releaseYUVTexture()
{
    if (textureYId > 0)
    {
        glDeleteTextures(1, &textureYId);
        textureYId = 0;
    }
    if (textureUId > 0)
    {
        glDeleteTextures(1, &textureUId);
        textureUId = 0;
    }
    if (textureVId > 0)
    {
        glDeleteTextures(1, &textureVId);
        textureVId = 0;
    }
}
