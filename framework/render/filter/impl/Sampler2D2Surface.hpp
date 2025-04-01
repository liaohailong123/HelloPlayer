//
// Created by 廖海龙 on 2024/11/18.
//

#ifndef ANDROID_SAMPLER2D2SURFACE_HPP
#define ANDROID_SAMPLER2D2SURFACE_HPP

#include "../IGLFilterNode.hpp"
#include "../../IGLContext.hpp"
#include "../../program/Sampler2DProgram.hpp"

#include <list>

/**
 * Author: liaohailong
 * Date: 2024/11/18
 * Time: 21:00
 * Description: GL滤镜：sampler2d纹理渲染到EGLSurface上
 **/
class Sampler2D2Surface : public IGLFilterNode {
public:
    explicit Sampler2D2Surface(const std::shared_ptr<IGLContext>& context);

    ~Sampler2D2Surface() override;

    void onProcess(std::shared_ptr<GLFilterPacket> packet) override;

private:
    std::shared_ptr<Sampler2DProgram> program;
    std::shared_ptr<IGLContext> context;
};


#endif //ANDROID_SAMPLER2D2SURFACE_HPP
