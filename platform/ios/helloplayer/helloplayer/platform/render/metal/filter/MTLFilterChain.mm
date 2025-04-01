//
//  MTLFilterChain.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/4/1.
//

#include "MTLFilterChain.hpp"

MTLFilterChain::MTLFilterChain() : logger("MTLFilterChain"), filters() {
    logger.i("MTLFilterChain::MTLFilterChain(%p)", this);
}

MTLFilterChain::~MTLFilterChain() {
    logger.i("MTLFilterChain::~MTLFilterChain(%p)", this);
}


void MTLFilterChain::process(std::shared_ptr<MTLFilterPacket> packet) {
//    int64_t startMs2 = TimeUtil::getCurrentTimeMs();
    for (const std::shared_ptr<IMTLFilterNode> &filter: filters) {
//        int64_t startMs = TimeUtil::getCurrentTimeMs();
        filter->onProcess(packet);
//        int64_t costMs = TimeUtil::getCurrentTimeMs() - startMs;
//        logger.i("[%s] onProcess cost[%d]ms", typeid(*filter).name(), costMs);
    }
//    int64_t costMs2 = TimeUtil::getCurrentTimeMs() - startMs2;
//    logger.i("all onProcess cost[%d]ms", costMs2);
}
