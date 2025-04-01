//
// Created by 廖海龙 on 2025/3/13.
//

#ifndef HELLOPLAYER_SCALETYPEFILTER_HPP
#define HELLOPLAYER_SCALETYPEFILTER_HPP


#include "../IGLFilterNode.hpp"
#include "../../program/ScaleTypeGLProgram.hpp"

/**
 * Author: liaohailong
 * Date: 2025/3/13
 * Time: 15:08
 * Description: 图像简单几何处理: FitCenter CenterCrop
 **/
class ScaleTypeFilter : public IGLFilterNode {
public:
    explicit ScaleTypeFilter();

    ~ScaleTypeFilter() override;

    void onProcess(std::shared_ptr<GLFilterPacket> packet) override;

private:
    std::shared_ptr<ScaleTypeGLProgram> program;
};


#endif //HELLOPLAYER_SCALETYPEFILTER_HPP
