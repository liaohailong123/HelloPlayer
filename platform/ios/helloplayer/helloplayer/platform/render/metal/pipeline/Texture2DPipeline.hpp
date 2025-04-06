//
//  Texture2DPipeline.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#ifndef Texture2DPipeline_hpp
#define Texture2DPipeline_hpp

#include "MTLPipeline.hpp"

// 渲染2d纹理
class Texture2DPipeline : public MTLPipeline
{
public:
    Texture2DPipeline();
    ~Texture2DPipeline() override;

    std::string getVertextFuncName() override;
    
    std::string getFragmentFuncName() override;
    
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
};




#endif /* Texture2DPipeline_hpp */
