//
// Created by 廖海龙 on 2024/11/18.
//

#ifndef ANDROID_GLFILTERPACKET_HPP
#define ANDROID_GLFILTERPACKET_HPP

#include "../../log/Logger.hpp"
#include "../../util/TimeUtil.hpp"
#include "../program/GLSLUtil.hpp"


#include "../glm/mat4x4.hpp" // glm::mat4
#include "../program/entity/TextureData.hpp" // 纹理数据包


/**
 * Author: liaohailong
 * Date: 2024/11/18
 * Time: 20:23
 * Description: 滤镜数据包
 **/
class GLFilterPacket {

public:
    explicit GLFilterPacket(glm::mat4 prjMat4);

    ~GLFilterPacket();


private:
    Logger logger;

public:
    /**
     * 项目矩阵：默认正交投影
     */
    glm::mat4 prjMat4;
    /**
     * GLContext 中 EGLSurface 的key
     */
    uint64_t key;
    /**
     * 绘制视口大小
     */
    int width, height;
    /**
     * 当前纹理
     */
    GLuint texture;

    /**
     * 纹理数据包
     */
    std::shared_ptr<TextureData> textureData;

    /**
     * 纹理尺寸
     */
    int textureWidth, textureHeight;

};


#endif //ANDROID_GLFILTERPACKET_HPP
