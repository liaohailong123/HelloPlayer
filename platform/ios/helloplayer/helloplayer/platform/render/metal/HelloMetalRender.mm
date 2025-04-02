//
//  HelloMetalRender.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#include "HelloMetalRender.hpp"


HelloMetalRender::HelloMetalRender(AVPixelFormat format): HelloVideoRender("HelloMetalRender"),prjMat4(), format(format),prepared(false),mtlContext(nullptr),filterPacket(nullptr), filterChain(new MTLFilterChain())
{
    mtlContext = std::make_shared<HelloMTLContext>();
    mtlContext->init();
    logger.i("HelloMetalRender::HelloMetalRender(%p)", this);
}

HelloMetalRender::~HelloMetalRender()
{
    prepared = false;
    
    logger.i("HelloMetalRender::~HelloMetalRender(%p)", this);
}

/**
 * 准备工作：初始化
 */
void HelloMetalRender::prepare()
{
    if (prepared)return;
    logger.i("HelloMetalRender::prepare(%p)", this);
    int count = mtlContext->getSurfaceSize();
    if (count <= 0)
    {
        logger.i("At least add one output CAMetalLayer");
        return;
    }
    // Metal环境创建好，开始初始化项目矩阵
    initProjectMat4();
    onMetalCreated();

    prepared = true;
}

/**
 * 初始化项目矩阵：默认初始化成正交投影矩阵
 */
void HelloMetalRender::initProjectMat4()
{
    // RH_? = right hand 右手坐标系
    // ?_ZO Zero to One Z轴深度[0,1] Vulkan/Direct3D/Metal 风格
    // ?_NO Negative to One Z轴深度[-1,1] OpenGL 传统风格
    glm::mat4 projectMat = glm::orthoRH_ZO(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    // 站在z轴看原点
    glm::mat4 viewMat = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // 先作用的矩阵，放在左边:  PRE * MAT * POST
    prjMat4 = projectMat * viewMat;
}

/**
 * 渲染线程 和 Metal环境创建好后回调
 */
void HelloMetalRender::onMetalCreated()
{
    // 滤镜链打包数据
    filterPacket = std::make_shared<MTLFilterPacket>(prjMat4); // 正交投影矩阵，归一化坐标

    // 初始化滤镜
    std::vector<std::shared_ptr<IMTLFilterNode>> nodes;

    nodes.push_back(std::make_shared<MTLInput2Sampler2d>(mtlContext, format)); // 输入滤镜: AVFrame转sampler2d
    nodes.push_back(std::make_shared<MTLScaleTypeFilter>(mtlContext)); // 几何处理: FitCenter
    nodes.push_back(std::make_shared<MTLTexture2Layer>(mtlContext)); // 输出滤镜: 2d纹理绘制到CAMetalLayer上

    // 添加滤镜
    for (const std::shared_ptr<IMTLFilterNode> &item: nodes) {
        filterChain->addFilter(item);
    }
    
}

/**
 * @return true表示已初始化
 */
bool HelloMetalRender::isPrepared()
{
    return prepared;
}

/**
 * @param f 更新像素格式
 * @return true表示更新成功
 */
bool HelloMetalRender::setAVPixelFormat(AVPixelFormat f)
{
    this->format = f;
    auto filter = filterChain->findFilter<MTLInput2Sampler2d>();
    if (filter)
    {
        logger.i("input filter update pixel format[%d]", format);
        filter->updateAVPixelFormat(format);
        return true;
    }
    return false;
}

AVPixelFormat HelloMetalRender::getAVPixelFormat()
{
    auto filter = filterChain->findFilter<MTLInput2Sampler2d>();
    if (filter)
    {
        return filter->getAVPixelFormat();
    }
    return format;
}

/**
 * @param surface 添加画面渲染缓冲区
 * @return true表示操作成功
 */
bool HelloMetalRender::addOutputs(void *surface)
{
    if (mtlContext)
    {
        return mtlContext->addSurface(surface);
    }
    return false;
}

/**
 * @param surface 移除画面渲染缓冲区
 */
void HelloMetalRender::removeOutputs(void *surface)
{
    if (mtlContext)
    {
        mtlContext->removeSurface(surface);
    }
}

/**
 * 洗刷渲染缓冲区
 * @param r red range is [0.0,1.0]
 * @param g green range is [0.0,1.0]
 * @param b blue range is [0.0,1.0]
 * @param a alpha range is [0.0,1.0]
 * @return true表示洗刷成功
 */
bool HelloMetalRender::clearColor(float r, float g, float b, float a)
{
    if (!mtlContext || !prepared) {
        return false;
    }
    
    // 多surface渲染
    std::vector<uint64_t> keys;
    mtlContext->getSurfaceKeys(&keys);
    mtlContext->setClearColor(r, g, b, a);
    for (uint64_t key: keys)
    {
        mtlContext->renderStart(key); // 清空缓冲区内容,使用clearColor擦拭缓冲区

        mtlContext->renderEnd(key); // 提交缓冲区
    }
    
    return true;
}

/**
 * @param frame 画面内容
 * @return 绘制画面
 */
bool HelloMetalRender::draw(std::shared_ptr<HelloVideoFrame> frame)
{
    if (!mtlContext || !prepared) {
        return false;
    }
    
    // 多surface同时渲染
    std::vector<uint64_t> keys;
    mtlContext->getSurfaceKeys(&keys);
    for (uint64_t key: keys)
    {
        id<MTLCommandBuffer> commandBuffer = mtlContext->renderStart(key); // 清空缓冲区内容,使用clearColor擦拭缓冲区
        
        // 执行滤镜链
        filterPacket->key = key;
        filterPacket->commandBuffer = commandBuffer; // Metal渲染指令集合
        filterPacket->width = mtlContext->getSurfaceWidth(key); // 视口宽度
        filterPacket->height = mtlContext->getSurfaceHeight(key); // 视口高度
        filterPacket->textureWidth = frame->getWidth(); // 输入纹理的宽度
        filterPacket->textureHeight = frame->getHeight(); // 输入纹理的高度
        std::shared_ptr<HelloMTLVideoTexture> data = std::make_shared<HelloMTLVideoTexture>(frame);
        filterPacket->textureData = data;

        filterChain->process(filterPacket);

        mtlContext->renderEnd(key); // 提交缓冲区
    }
    
    return true;
}
