//
// Created by liaohailong on 2025/2/27.
//

#ifndef HELLOPLAYER_TEXTUREDATA_HPP
#define HELLOPLAYER_TEXTUREDATA_HPP


#include <cstdio>

/**
 * create by liaohailong
 * 2025/2/27 21:02
 * desc: 
 */
class TextureData
{
public:
    explicit TextureData();
    virtual ~TextureData();
public:
    virtual uint8_t **getData() = 0;

    virtual int *getLineSize() = 0;

    virtual int getFormat() = 0;

    virtual int getWidth() = 0;

    virtual int getHeight() = 0;

};


#endif //HELLOPLAYER_TEXTUREDATA_HPP
