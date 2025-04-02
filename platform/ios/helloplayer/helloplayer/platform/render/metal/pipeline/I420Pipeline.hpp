//
//  I420Pipeline.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/31.
//

#ifndef I420Pipeline_hpp
#define I420Pipeline_hpp

#include "MTLPipeline.hpp"

extern "C"
{
#include <libavutil/avutil.h>
}

class I420Pipeline : public MTLPipeline
{
public:
    explicit I420Pipeline();
    ~I420Pipeline() override;
    
    std::string getVertextFuncName() override;
    
    std::string getFragmentFuncName() override;
    
    void setTextureData(const std::shared_ptr<MTLTextureData> &data) override;
    
    bool draw(float projectMat[4*4]) override;
    
private:
    
    // 顶点坐标 + 纹理坐标
    AAPLTextureVertex sVertices[6] =
    {
        // Positions     ,  Texcoords
        { {  -1.0,  -1.0 }, { 0.0, 1.0 } },
        { {   1.0,  -1.0 }, { 1.0, 1.0 } },
        { {  -1.0,   1.0 }, { 0.0, 0.0 } },
        { {   1.0,   1.0 }, { 1.0, 0.0 } },
    };
    
    // 矩阵 + 镜像
    AAPLObjectParams sObjParams;
    
    // 渲染纹理 source
    id<MTLTexture> textureY;
    id<MTLTexture> textureU;
    id<MTLTexture> textureV;
};


#endif /* I420Pipeline_hpp */
