//
// Created by 廖海龙 on 2025/1/16.
//

#include "NV12GLProgram.hpp"

NV12GLProgram::NV12GLProgram()
        : IGLProgram("NV12GLProgram"), mvpM(-1), position(-1), coordinate(-1), y_tex(-1),
          uv_tex(-1), textureYId(0), textureUVId(0), textureWidth(0), textureHeight(0) {
    logger.i("NV12GLProgram::NV12GLProgram(%p)", this);
}


NV12GLProgram::~NV12GLProgram() {
    releaseYUVTexture();
    logger.i("NV12GLProgram::~NV12GLProgram(%p)", this);
}

std::string NV12GLProgram::getVertexSource() {
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

std::string NV12GLProgram::getFragmentSource() {
    const char source[] = R"(
#version 100
precision mediump float;

varying vec2 v_coordinate;

uniform sampler2D y_tex;
uniform sampler2D uv_tex;


void main()
{

    // 获取 Y 分量
    float y = texture2D(y_tex, v_coordinate).r;

    // 获取 UV 分量
    vec2 uv = texture2D(uv_tex, v_coordinate).ra;

    // 解码 UV 到 U 和 V
    float u = uv.r - 0.5;
    float v = uv.g - 0.5;

    // YUV 转 RGB
    float r = y + 1.402 * v;
    float g = y - 0.344 * u - 0.714 * v;
    float b = y + 1.772 * u;

    gl_FragColor = vec4(r, g, b, 1.0);
}

    )";
    return source;
}

void NV12GLProgram::onProgramCreated(GLuint program) {
    mvpM = glGetUniformLocation(program, "u_mvpM");
    position = glGetAttribLocation(program, "i_position");
    coordinate = glGetAttribLocation(program, "i_coordinate");
    y_tex = glGetUniformLocation(program, "y_tex");
    uv_tex = glGetUniformLocation(program, "uv_tex");

}

void NV12GLProgram::setTextureData(std::shared_ptr<TextureData> image) {
    int format = image->getFormat(); // 画面的格式 YUV420P YUV422P
    uint8_t **data = image->getData(); // 像素内容 [0]=Y [1]=U [2]=V
    int *lineSize = image->getLineSize(); // 每种像素的宽度（一行有多少个像素）

    // NV12 内存格式
    // [Y00, Y01, Y02, Y03, Y10, Y11, Y12, Y13, Y20, Y21, Y22, Y23, Y30, Y31, Y32, Y33]
    // [U00, V00, U02, V02, U20, V20, U22, V22]

    int yWidth = image->getWidth();
    int yHeight = image->getHeight();

    int uvHeight = yHeight;
    int uvWidth = yWidth;

    if (format == AV_PIX_FMT_NV12) {
        uvWidth = yWidth / 2;
        uvHeight = yHeight / 2;
    }

    // 判断尺寸是否发生了变化
    if (textureWidth != yWidth || textureHeight != yHeight) {
        releaseYUVTexture();
    }
    if (textureYId == 0 || textureUVId == 0) {
        // 创建YUV三个纹理
        GLuint textures[2];
        glGenTextures(2, &(textures[0]));

        textureYId = textures[0];
        textureUVId = textures[1];
    }

    // 硬件解码,返回的buffer可能是一个正方形
    GLint _lineSizeY = lineSize[0];
    GLint _lineSizeUV = lineSize[0] / 2;
//    logger.i("w*h[%d,%d] lineSizeY=%d lineSizeUY=%d", yWidth, yHeight, _lineSizeY, _lineSizeUY);
    bindYTexture(textureYId, yWidth, yHeight, _lineSizeY, data[0]);
    bindUVTexture(textureUVId, uvWidth, uvHeight, _lineSizeUV, data[1]);
}

void NV12GLProgram::setMirror(bool hMirror, bool vMirror) {
    // 暂不支持
}

void NV12GLProgram::draw(int width, int height, float projectMat[4 * 4]) {
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
    glBindTexture(GL_TEXTURE_2D, textureYId);
    glUniform1i(y_tex, 0);

    // 绑定纹理
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureUVId);
    glUniform1i(uv_tex, 1);

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

    // 绘制图元
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 关闭顶点输入
    glDisableVertexAttribArray(position);
    glDisableVertexAttribArray(coordinate);

}


void NV12GLProgram::bindYTexture(GLuint texture, int width, int height,
                                 GLint lineSize, const void *pixels) {
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


void NV12GLProgram::bindUVTexture(GLuint texture, int width, int height,
                                  GLint lineSize, const void *pixels) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 2); // 按照2字节读写
#ifdef HAVE_GL2_EXT
    glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, lineSize); // 一行多少数据
#else
    // iOS上没有 GLES2.0扩展,无法指定 row length 暂时这样处理
    width = width < lineSize? lineSize: width;
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height,
                 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void NV12GLProgram::releaseYUVTexture() {
    if (textureYId > 0) {
        glDeleteTextures(1, &textureYId);
        textureYId = 0;
    }
    if (textureUVId > 0) {
        glDeleteTextures(1, &textureUVId);
        textureUVId = 0;
    }
}
