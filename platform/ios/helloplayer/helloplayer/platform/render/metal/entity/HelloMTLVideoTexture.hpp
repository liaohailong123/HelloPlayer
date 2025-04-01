//
//  HelloMTLVideoTexture.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/4/1.
//

#ifndef HelloMTLVideoTexture_hpp
#define HelloMTLVideoTexture_hpp

#include "player/device/HelloVideoFrame.hpp"
#include "../pipeline/entity/MTLTextureData.hpp"


class HelloMTLVideoTexture : public MTLTextureData
{
public:
    explicit HelloMTLVideoTexture(std::shared_ptr<HelloVideoFrame> frame);
    ~HelloMTLVideoTexture();
    
    uint8_t **getData() override;

    int *getLineSize() override;

    int getFormat() override;

    int getWidth() override;

    int getHeight() override;
    
private:
    Logger logger;
    std::shared_ptr<HelloVideoFrame> frame;
};

#endif /* HelloMTLVideoTexture_hpp */
