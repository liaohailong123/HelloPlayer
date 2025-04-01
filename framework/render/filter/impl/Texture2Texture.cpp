//
// Created by 廖海龙 on 2024/11/27.
//

#include "Texture2Texture.hpp"


Texture2Texture::Texture2Texture(IGLContext *_context) : IGLFilterNode("Texture2Texture"),
                                                         program() {
    context = const_cast<IGLContext *>(_context);
    // 初始化着色器程序
    program.initialized();
    program.setRotation(180); // FBO会倒置纹理，这里手动矫正一下

    logger.i("Texture2Texture::Texture2Texture(%p)", this);
}

Texture2Texture::~Texture2Texture() {
    logger.i("Texture2Texture::~Texture2Texture(%p)", this);
}

void Texture2Texture::onProcess(std::shared_ptr<GLFilterPacket> packet) {

    // 取出所有待渲染的纹理
    std::vector<std::shared_ptr<IGLContext::TextureCtx>> textures;

    if (context->getOutputTextures(textures)) {
        // 把所有外部设置的纹理渲染一遍
        for (std::shared_ptr<IGLContext::TextureCtx> &ctx: textures) {
            int outputTextureId = ctx->textureId;
            // 将 sampler2d 依附到fbo上，接下来往 fbo 绘制内容就等于画到纹理上
            if (ctx->fboId <= 0) {
                ctx->fboId = GLSLUtil::textureAttach2Fbo(outputTextureId);
            }
            int textureWidth = ctx->textureWidth;
            int textureHeight = ctx->textureHeight;

            // 将内容渲染到fbo纹理上
            glBindFramebuffer(GL_FRAMEBUFFER, ctx->fboId);

            program.begin();
            program.setTexture(&packet->texture, 1);
            // 绘制到外部传入的sampler2d纹理上时，直接使用外部纹理的宽高，这样就能确保是按照 “平铺贴满” 的方式渲染画面
            program.draw(textureWidth, textureHeight, glm::value_ptr(packet->prjMat4)); // 绘制
            program.end(); // 着色器结束使用

        }
    }


}