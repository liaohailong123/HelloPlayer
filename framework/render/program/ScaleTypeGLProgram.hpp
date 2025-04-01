//
// Created by 廖海龙 on 2025/3/13.
//

#ifndef HELLOPLAYER_SCALETYPEGLPROGRAM_HPP
#define HELLOPLAYER_SCALETYPEGLPROGRAM_HPP

#include "IGLProgram.hpp"

/**
 * Author: liaohailong
 * Date: 2025/3/13
 * Time: 12:36
 * Description: 简单几何处理 sampler2d 纹理 的着色器
 **/
class ScaleTypeGLProgram : public IGLProgram {
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

    std::string getVertexSource() override;

    std::string getFragmentSource() override;

    void onProgramCreated(GLuint program) override;

    void setTexture(GLuint *textures, int count) override;

    void setMirror(bool hMirror, bool vMirror) override;

    void draw(int width, int height, float projectMat[4 * 4]) override;

    void setScaleType(ScaleType type);

    void setRect(const std::shared_ptr<Rect> &src, const std::shared_ptr<Rect> &dst);

private:
    static glm::mat4 makeFitCenterMat4(const std::shared_ptr<Rect> &src,
                                const std::shared_ptr<Rect> &dst);
    static glm::mat4 makeCropCenterMat4(const std::shared_ptr<Rect> &src,
                                 const std::shared_ptr<Rect> &dst);

private: // GLSL变量
    uniform mvpM;
    attribute position;
    attribute coordinate;
    uniform texture; // sampler2D
    uniform hMirror;
    uniform vMirror;

private: // 传入着色器的变量
    float vertexPos[18] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f
    };
    // Android系统加载纹理会倒置画面，这里的纹理坐标点做了兼容处理
    float texturePos[12] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f
    };

    /**
     * 纹理id
     */
    GLuint textureId;

    /**
     * 水平镜像
     */
    bool hMirrorVal;
    /**
     * 垂直镜像
     */
    bool vMirrorVal;
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
