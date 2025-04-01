//
//  MTLFilterPacket.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/31.
//

#include "MTLFilterPacket.hpp"

MTLFilterPacket::MTLFilterPacket(glm::mat4 _prjMat4): logger("MTLFilterPacket"), commandBuffer(nil), prjMat4(_prjMat4)
{
    logger.i("MTLFilterPacket::MTLFilterPacket(%p)", this);
}
MTLFilterPacket::~MTLFilterPacket()
{
    logger.i("MTLFilterPacket::MTLFilterPacket(%p)", this);
}
