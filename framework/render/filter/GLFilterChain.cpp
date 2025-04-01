//
// Created by 廖海龙 on 2024/11/18.
//

#include "GLFilterChain.hpp"


GLFilterChain::GLFilterChain() : logger("GLFilterChain"), filters() {
    logger.i("GLFilterChain::GLFilterChain(%p)", this);
}

GLFilterChain::~GLFilterChain() {
    logger.i("GLFilterChain::~GLFilterChain(%p)", this);
}


void GLFilterChain::process(std::shared_ptr<GLFilterPacket> packet) {
//    int64_t startMs2 = TimeUtil::getCurrentTimeMs();
    for (const std::shared_ptr<IGLFilterNode> &filter: filters) {
//        int64_t startMs = TimeUtil::getCurrentTimeMs();
        filter->onProcess(packet);
//        int64_t costMs = TimeUtil::getCurrentTimeMs() - startMs;
//        logger.i("[%s] onProcess cost[%d]ms", typeid(*filter).name(), costMs);
    }
//    int64_t costMs2 = TimeUtil::getCurrentTimeMs() - startMs2;
//    logger.i("all onProcess cost[%d]ms", costMs2);
}
