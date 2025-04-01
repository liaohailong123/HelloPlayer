//
// Created by liaohailong on 2025/2/27.
//

#ifndef HELLOPLAYER_HELLOVIDEORENDER_HPP
#define HELLOPLAYER_HELLOVIDEORENDER_HPP

#include "HelloVideoFrame.hpp"
#include "../../log/Logger.hpp"

#include <memory>

extern "C"
{
#include <libavutil/pixfmt.h>
}


/**
 * create by liaohailong
 * 2025/2/27 20:13
 * desc: 视频渲染器接口,由平台侧实现
 */
class HelloVideoRender
{
public:
    explicit HelloVideoRender(const char *tag, AVPixelFormat format);

    virtual ~HelloVideoRender();

    /**
     * 准备工作：初始化
     */
    virtual void prepare() = 0;

    /**
     * @return true表示已初始化
     */
    virtual bool isPrepared() = 0;

    /**
     * @param format 更新像素格式
     * @return true表示更新成功
     */
    virtual bool setAVPixelFormat(AVPixelFormat format) = 0;

    virtual AVPixelFormat getAVPixelFormat() = 0;

    /**
     * @param surface 添加画面渲染缓冲区
     * @return true表示操作成功
     */
    virtual bool addOutputs(void *surface) = 0;

    /**
     * @param surface 移除画面渲染缓冲区
     */
    virtual void removeOutputs(void *surface) = 0;

    /**
     * 洗刷渲染缓冲区
     * @param r red range is [0.0,1.0]
     * @param g green range is [0.0,1.0]
     * @param b blue range is [0.0,1.0]
     * @param a alpha range is [0.0,1.0]
     * @return true表示洗刷成功
     */
    virtual bool clearColor(float r, float g, float b, float a) = 0;

    /**
     * @param frame 画面内容
     * @return 绘制画面
     */
    virtual bool draw(std::shared_ptr<HelloVideoFrame> frame) = 0;

protected:
    Logger logger;

};


#endif //HELLOPLAYER_HELLOVIDEORENDER_HPP
