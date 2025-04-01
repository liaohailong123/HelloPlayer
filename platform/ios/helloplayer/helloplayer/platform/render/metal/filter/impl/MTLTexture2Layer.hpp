//
//  MTLTexture2Layer.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/4/1.
//

#ifndef MTLTexture2Layer_hpp
#define MTLTexture2Layer_hpp

#include "../IMTLFilterNode.hpp"
#include "../../pipeline/Texture2DPipeline.hpp"

// 2d纹理渲染到
class MTLTexture2Layer : public IMTLFilterNode
{
public:
    explicit MTLTexture2Layer(const std::shared_ptr<HelloMTLContext> &ctx);
    ~MTLTexture2Layer() override;
    
    void onProcess(std::shared_ptr<MTLFilterPacket> packet) override;
    
private:
    std::shared_ptr<Texture2DPipeline> pipeline;
};

#endif /* MTLTexture2Layer_hpp */
