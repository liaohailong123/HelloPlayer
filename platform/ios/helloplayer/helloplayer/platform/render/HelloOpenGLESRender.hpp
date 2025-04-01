//
//  HelloOpenGLESRender.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/21.
//

#ifndef HelloOpenGLESRender_hpp
#define HelloOpenGLESRender_hpp

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <chrono>

#include "HelloEAGLContext.hpp"
#include "player/device/HelloVideoRender.hpp"
#include "log/Logger.hpp"
#include "render/program/GLSLUtil.hpp"
#include "render/program/IGLProgram.hpp"
#include "render/program/YUVGLProgram.hpp"
#include "render/program/NV12GLProgram.hpp"
#include "render/entity/HelloVideoTexture.hpp"

// 过滤器
#include "render/filter/GLFilterChain.hpp"
#include "render/filter/GLFilterPacket.hpp"
#include "render/filter/impl/Input2Sampler2d.hpp" // 输入滤镜
#include "render/filter/impl/ScaleTypeFilter.hpp" // 几何处理: FitCenter
#include "render/filter/impl/Sampler2D2Surface.hpp" // 输入滤镜

/**
 * create by liaohailong
 * 2025/2/27 20:32
 * desc: Android平台画面渲染实现，OpenGL ES
 */
class HelloOpenGLESRender : public HelloVideoRender
{
public:
    explicit HelloOpenGLESRender(AVPixelFormat format);

    ~HelloOpenGLESRender() override;

    /**
     * 准备工作：初始化
     */
    void prepare() override;

    /**
     * @return true表示已初始化
     */
    bool isPrepared() override;

    /**
     * @param format 更新像素格式
     * @return true表示更新成功
     */
    bool setAVPixelFormat(AVPixelFormat format) override;

    AVPixelFormat getAVPixelFormat() override;

    /**
     * @param surface 添加画面渲染缓冲区
     * @return true表示操作成功
     */
    bool addOutputs(void *surface) override;

    /**
     * @param surface 移除画面渲染缓冲区
     */
    void removeOutputs(void *surface) override;

    /**
     * 洗刷渲染缓冲区
     * @param r red range is [0.0,1.0]
     * @param g green range is [0.0,1.0]
     * @param b blue range is [0.0,1.0]
     * @param a alpha range is [0.0,1.0]
     * @return true表示洗刷成功
     */
    bool clearColor(float r, float g, float b, float a) override;

    /**
     * @param frame 画面内容
     * @return 绘制画面
     */
    bool draw(std::shared_ptr<HelloVideoFrame> frame) override;

private:
    /**
     * 初始化项目矩阵：默认初始化成正交投影矩阵
     */
    void initProjectMat4();

    /**
     * 渲染线程 和 EGL环境创建好后回调
     */
    void onEGLCreated();

protected:
    glm::mat4 prjMat4; // 项目矩阵：默认正交投影

    AVPixelFormat format;

    std::atomic<bool> prepared; // 是否已经初始化完成

    std::shared_ptr<HelloEAGLContext> glContext; // EGL环境

    /**
     * 滤镜链需要的打包数据
     */
    std::shared_ptr<GLFilterPacket> filterPacket;
    /**
     * 滤镜链
     */
    std::shared_ptr<GLFilterChain> filterChain;

};


#endif /* HelloOpenGLESRender_hpp */
