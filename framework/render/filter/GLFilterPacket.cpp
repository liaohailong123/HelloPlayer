//
// Created by 廖海龙 on 2024/11/18.
//

#include "GLFilterPacket.hpp"


GLFilterPacket::GLFilterPacket(glm::mat4 _prjMat4) :
        logger("GLFilterPacket"), prjMat4(_prjMat4) {
    logger.i("GLFilterPacket::GLFilterPacket(%p)", this);
}

GLFilterPacket::~GLFilterPacket() {
    logger.i("GLFilterPacket::~GLFilterPacket(%p)", this);
}
