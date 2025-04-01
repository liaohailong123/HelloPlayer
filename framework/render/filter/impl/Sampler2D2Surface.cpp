//
// Created by 廖海龙 on 2024/11/18.
//

#include "Sampler2D2Surface.hpp"

Sampler2D2Surface::Sampler2D2Surface(const std::shared_ptr<IGLContext> &ctx) :
        IGLFilterNode("Sampler2D2Surface"), program(std::make_shared<Sampler2DProgram>()) {
    context = ctx;

    // 初始化着色器程序
    program->initialized();

    logger.i("Sampler2D2Surface::Sampler2D2Surface(%p)", this);
}

Sampler2D2Surface::~Sampler2D2Surface() {
    logger.i("Sampler2D2Surface::Sampler2D2Surface(%p)", this);
}

void Sampler2D2Surface::onProcess(std::shared_ptr<GLFilterPacket> packet) {
    if (!context)return;

    bool ret = context->renderStart(packet->key); // 清空缓冲区内容
    if (!ret) return;


    program->begin();
    program->setTexture(&packet->texture, 1);
    program->draw(packet->width, packet->height, glm::value_ptr(packet->prjMat4)); // 绘制
    program->end(); // 着色器结束使用


    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    int64_t ptsUs = ms.count() * 1000L;
    context->renderEnd(packet->key, ptsUs); // 交换buffer到surface上
}