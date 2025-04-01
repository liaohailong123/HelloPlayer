//
// Created by 廖海龙 on 2024/11/27.
//

#ifndef ANDROID_TEXTURE2TEXTURE_HPP
#define ANDROID_TEXTURE2TEXTURE_HPP

#include "../IGLFilterNode.hpp"
#include "../../IGLContext.hpp"
#include "../../program/Sampler2DProgram.hpp" // 采用sampler2d着色器

/**
 * Author: liaohailong
 * Date: 2024/11/27
 * Time: 10:58
 * Description: 把纹理绘制到纹理上，两者都是sampler2d类型的纹理
 **/
class Texture2Texture : public IGLFilterNode {
public:
    explicit Texture2Texture(IGLContext *context);

    ~Texture2Texture();

    void onProcess(std::shared_ptr<GLFilterPacket> packet) override;

private:
    Sampler2DProgram program;
    IGLContext *context;
};


#endif //ANDROID_TEXTURE2TEXTURE_HPP
