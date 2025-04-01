//
//  MTLTextureData.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#ifndef MTLTextureData_hpp
#define MTLTextureData_hpp

#include <stdio.h>

class MTLTextureData
{
public:
    virtual uint8_t **getData() = 0;
    virtual int *getLineSize() = 0;
    virtual int getFormat() = 0;
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
};

#endif /* MTLTextureData_hpp */
