//
// Created by 廖海龙 on 2024/11/18.
//

#include "IGLFilterNode.hpp"


IGLFilterNode::IGLFilterNode(const char *tag) : logger(tag), fboId(0), fboTextureId(0),
                                                fboWidth(0), fboHeight(0) {
    logger.i("IGLFilterNode::IGLFilterNode(%p)", this);
}

IGLFilterNode::~IGLFilterNode() {
    GLSLUtil::deleteFboTexture(fboId, fboTextureId);
    logger.i("IGLFilterNode::IGLFilterNodeNode(%p)", this);
}

/**
 * 创建fbo和texture
 */
void IGLFilterNode::ensureFbo(int width, int height, int rotation) {
    if (fboId > 0)return;

    int fbo_w;
    int fbo_h;
    // 创建滤镜入口纹理fbo
    if (rotation == -1) {
        // 不处理旋转
        fbo_w = width;
        fbo_h = height;
    } else {
        bool portrait = rotation % 180 != 0;
        if (portrait) {
            // 竖屏模式
            fbo_w = width > height ? height : width; // 宽度取小值
            fbo_h = width > height ? width : height; // 高度取大值
        } else {
            // 横屏模式
            fbo_w = width > height ? width : height; // 宽度取大值
            fbo_h = width > height ? height : width; // 高度取小值
        }
    }

    fboId = GLSLUtil::generateFboTexture(fboTextureId, fbo_w, fbo_h);
    fboWidth = fbo_w;
    fboHeight = fbo_h;
}

/**
 * 重制fbo和texture
 */
void IGLFilterNode::resetFbo(int width, int height, int rotation) {
    GLSLUtil::deleteFboTexture(fboId, fboTextureId);
    ensureFbo(width, height, rotation);
}