//
// Created by 廖海龙 on 2024/11/18.
//

#ifndef ANDROID_IGLFILTERNODE_HPP
#define ANDROID_IGLFILTERNODE_HPP


#include <memory>

#include "../../log/Logger.hpp"
#include "../../util/TimeUtil.hpp"
#include "../program/GLSLUtil.hpp"
#include "GLFilterPacket.hpp" // 滤镜数据包


/**
 * Author: liaohailong
 * Date: 2024/11/18
 * Time: 20:21
 * Description: 滤镜链统一规范
 **/
class IGLFilterNode {
public:
    explicit IGLFilterNode(const char *tag);

    virtual ~IGLFilterNode();

    virtual void onProcess(std::shared_ptr<GLFilterPacket> packet) = 0;

    /**
     * 创建fbo和texture
     */
    void ensureFbo(int width, int height, int rotation);

    /**
     * 重制fbo和texture
     */
    void resetFbo(int width, int height, int rotation);

protected:
    Logger logger;

    GLuint fboId;
    GLuint fboTextureId;

public:
    int fboWidth;
    int fboHeight;
};


#endif //ANDROID_IGLFILTERNODE_HPP
