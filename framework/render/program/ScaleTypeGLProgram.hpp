//
// Created by 廖海龙 on 2025/3/13.
//

#ifndef HELLOPLAYER_SCALETYPEGLPROGRAM_HPP
#define HELLOPLAYER_SCALETYPEGLPROGRAM_HPP

#include "Sampler2DProgram.hpp"

/**
 * Author: liaohailong
 * Date: 2025/3/13
 * Time: 12:36
 * Description: 简单几何处理 sampler2d 纹理 的着色器
 **/
class ScaleTypeGLProgram : public Sampler2DProgram {
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
    explicit ScaleTypeGLProgram();

    ~ScaleTypeGLProgram() override;

    void onDraw(int width, int height, float projectMat[16]) override;

    void setScaleType(ScaleType type);

    void setRect(const std::shared_ptr<Rect> &src, const std::shared_ptr<Rect> &dst);

private:
    static glm::mat4 makeFitCenterMat4(const std::shared_ptr<Rect> &src,
                                const std::shared_ptr<Rect> &dst);
    static glm::mat4 makeCropCenterMat4(const std::shared_ptr<Rect> &src,
                                 const std::shared_ptr<Rect> &dst);

private: // 传入着色器的变量
    /**
     * 模型矩阵
     */
    glm::mat4 moduleMat4;

    /**
     * 缩放类型
     */
    ScaleType scaleType;
};


#endif //HELLOPLAYER_SCALETYPEGLPROGRAM_HPP
