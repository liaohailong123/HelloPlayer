//
//  ScaleTypePipeline.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/31.
//

#ifndef ScaleTypePipeline_hpp
#define ScaleTypePipeline_hpp


#include "MTLPipeline.hpp"

// 简单几何处理 sampler2d 纹理 不同的缩放类型 FitCenter CropCenter ...
class ScaleTypePipeline : public MTLPipeline
{
public:
    typedef struct {
        int left;
        int top;
        int right;
        int bottom;
    } Rect;
    typedef enum {
        /**
         * 平铺
         */
        FitXY = 0,
        /**
         * 等比例缩缩放,居中
         */
        FitCenter,
        /**
         * 等比例缩缩放,靠左(上)
         */
        FitStart,
        /**
         * 等比例缩缩放,靠右(下)
         */
        FitEnd,
        /**
         * 资源最短边,拉满区域,整体放大看中心内容
         */
        CenterCrop
    } ScaleType;
public:
    explicit ScaleTypePipeline();
    
    ~ScaleTypePipeline() override;
    
    std::string getVertextFuncName() override;
    
    std::string getFragmentFuncName() override;
    
    void setTexture(const std::vector<id<MTLTexture>> &textures) override;
    
    bool draw(float projectMat[4*4]) override;
    
    void setScaleType(ScaleType type);

    void setRect(const std::shared_ptr<Rect> &src, const std::shared_ptr<Rect> &dst);
    
private:
    static glm::mat4 makeFitCenterMat4(const std::shared_ptr<Rect> &src,
                                const std::shared_ptr<Rect> &dst);
    static glm::mat4 makeCropCenterMat4(const std::shared_ptr<Rect> &src,
                                 const std::shared_ptr<Rect> &dst);
    
private:    
    /**
     * 缩放类型
     */
    ScaleType scaleType;
    
    // 顶点坐标 + 纹理坐标
    AAPLTextureVertex sVertices[6] =
    {
        // Positions     ,  Texcoords
        { {  -1.0,  -1.0 }, { 0.0, 0.0 } },
        { {   1.0,  -1.0 }, { 1.0, 0.0 } },
        { {  -1.0,   1.0 }, { 0.0, 1.0 } },
        
        { {   1.0,  -1.0 }, { 1.0, 0.0 } },
        { {  -1.0,   1.0 }, { 0.0, 1.0 } },
        { {   1.0,   1.0 }, { 1.0, 1.0 } },
    };
    
    // 矩阵 + 镜像
    AAPLObjectParams sObjParams;
    
    // 渲染纹理 source
    id<MTLTexture> texture;
};

#endif /* ScaleTypePipeline_hpp */
