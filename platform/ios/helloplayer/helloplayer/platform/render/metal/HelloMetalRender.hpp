//
//  HelloMetalRender.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#ifndef HelloMetalRender_hpp
#define HelloMetalRender_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <chrono>


// 公共层代码
#include "HelloMTLContext.hpp"
#include "player/device/HelloVideoRender.hpp"
#include "entity/HelloMTLVideoTexture.hpp"
#include "log/Logger.hpp"


// 过滤器
#include "pipeline/MTLPipeline.hpp"
#include "filter/MTLFilterChain.hpp"
#include "filter/MTLFilterPacket.hpp"
#include "filter/impl/MTLInput2Sampler2d.hpp"
#include "filter/impl/MTLScaleTypeFilter.hpp"
#include "filter/impl/MTLTexture2Layer.hpp"


/**
 * create by liaohailong
 * 2025/2/27 20:32
 * desc: iOS平台画面渲染实现，Metal
 */
class HelloMetalRender : public HelloVideoRender
{
public:
    explicit HelloMetalRender(AVPixelFormat format);

    ~HelloMetalRender() override;

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
     * 渲染线程 和 Metal环境创建好后回调
     */
    void onMetalCreated();

protected:
    glm::mat4 prjMat4; // 项目矩阵：默认正交投影

    AVPixelFormat format; // 待渲染的像素格式

    std::atomic<bool> prepared; // 是否已经初始化完成

    std::shared_ptr<HelloMTLContext> mtlContext; // Metal环境
    
    /**
     * 滤镜链需要的打包数据
     */
    std::shared_ptr<MTLFilterPacket> filterPacket;
    /**
     * 滤镜链
     */
    std::shared_ptr<MTLFilterChain> filterChain;

};


#endif /* HelloMetalRender_hpp */
