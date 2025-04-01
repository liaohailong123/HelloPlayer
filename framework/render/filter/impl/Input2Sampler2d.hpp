//
// Created by 廖海龙 on 2025/3/13.
//

#ifndef HELLOPLAYER_INPUT2SAMPLER2D_HPP
#define HELLOPLAYER_INPUT2SAMPLER2D_HPP

#include "../IGLFilterNode.hpp"

#include "../../IGLContext.hpp"
#include "../../program/IGLProgram.hpp"
#include "../../program/YUVGLProgram.hpp"
#include "../../program/NV12GLProgram.hpp"

extern "C"
{
#include <libavutil/avutil.h>
}

/**
 * Author: liaohailong
 * Date: 2025/3/13
 * Time: 14:02
 * Description: 输入过滤器
 **/
class Input2Sampler2d : public IGLFilterNode{
public:
    explicit Input2Sampler2d(AVPixelFormat format);

    ~Input2Sampler2d() override;

    void updateAVPixelFormat(AVPixelFormat format);
    
    AVPixelFormat getAVPixelFormat() noexcept;

    void onProcess(std::shared_ptr<GLFilterPacket> packet) override;

private:
    AVPixelFormat format;
    std::shared_ptr<IGLProgram> glProgram;
};


#endif //HELLOPLAYER_INPUT2SAMPLER2D_HPP
