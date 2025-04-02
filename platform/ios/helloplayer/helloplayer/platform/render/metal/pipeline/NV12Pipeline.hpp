//
//  NV12Pipeline.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/31.
//

#ifndef NV12Pipeline_hpp
#define NV12Pipeline_hpp

#include "MTLPipeline.hpp"

extern "C"
{
#include <libavutil/avutil.h>
}

class NV12Pipeline : public MTLPipeline
{
public:
    explicit NV12Pipeline();
    ~NV12Pipeline() override;
    
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
    id<MTLTexture> textureUV;
};

#endif /* NV12Pipeline_hpp */
