//
//  MTLFilterPacket.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/31.
//

#ifndef MTLFilterPacket_hpp
#define MTLFilterPacket_hpp

#import <Metal/Metal.h>

#include "util/TimeUtil.hpp"

#include "../pipeline/entity/MTLTextureData.hpp"
#include "../pipeline/MTLPipeline.hpp"



class MTLFilterPacket
{
public:
    explicit MTLFilterPacket(glm::mat4 prjMat4);
    ~MTLFilterPacket();
    
private:
    Logger logger;

public:
    /**
     * 项目矩阵：默认正交投影
     */
    glm::mat4 prjMat4;
    /**
     * MetalContext 中 CAMetalLayer 的key
     */
    uint64_t key;
    /**
     * 绘制视口大小
     */
    int width, height;
    
    /**
     * 当前纹理
     */
    id<MTLTexture> texture;
    
    /**
     * 纹理数据包
     */
    std::shared_ptr<MTLTextureData> textureData;

    /**
     * 纹理尺寸
     */
    int textureWidth, textureHeight;
};

#endif /* MTLFilterPacket_hpp */
