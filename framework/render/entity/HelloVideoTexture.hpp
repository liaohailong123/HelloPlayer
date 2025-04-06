//
// Created by liaohailong on 2025/2/27.
//

#ifndef HELLOPLAYER_HELLOVIDEOTEXTURE_HPP
#define HELLOPLAYER_HELLOVIDEOTEXTURE_HPP

#include "../../player/device/HelloVideoFrame.hpp"
#include "../program/entity/TextureData.hpp"

/**
 * create by liaohailong
 * 2025/2/27 21:08
 * desc: TextureData代理，实际调用 HelloVideoFrame
 */
class HelloVideoTexture : public TextureData
{
public:
    explicit HelloVideoTexture(std::shared_ptr<HelloVideoFrame> frame);

    ~HelloVideoTexture() override;

    uint8_t **getData() override;

    int *getLineSize() override;

    int getFormat() override;

    int getWidth() override;

    int getHeight() override;
private:
    Logger logger;
    std::shared_ptr<HelloVideoFrame> frame;
};


#endif //HELLOPLAYER_HELLOVIDEOTEXTURE_HPP
