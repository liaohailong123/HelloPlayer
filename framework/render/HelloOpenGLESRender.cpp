//
// Created by liaohailong on 2025/2/27.
//

#include "HelloOpenGLESRender.hpp"


HelloOpenGLESRender::HelloOpenGLESRender(AVPixelFormat _format) :
        HelloVideoRender("HelloOpenGLESRender"), prjMat4(), format(_format),
        prepared(false), glContext(nullptr), filterPacket(nullptr), filterChain(new GLFilterChain())
{
    // 创建EGL环境
    glContext = std::make_shared<HelloEGLContext>();
    glContext->init(EGL_NO_CONTEXT);

    logger.i("HelloOpenGLESRender::HelloOpenGLESRender(%p)", this);
}

HelloOpenGLESRender::~HelloOpenGLESRender()
{
    prepared = false;

    logger.i("HelloOpenGLESRender::~HelloOpenGLESRender(%p)", this);
}

/**
 * 准备工作：初始化
 */
void HelloOpenGLESRender::prepare()
{
    if (prepared)return;
    logger.i("HelloOpenGLESRender::prepare(%p)", this);
    int count = glContext->getSurfaceSize();
    if (count <= 0)
    {
        logger.i("At least add one output surface");
        return;
    }
    // EGL环境创建好，开始初始化项目矩阵
    initProjectMat4();
    onEGLCreated();

    prepared = true;
}

/**
 * 初始化项目矩阵：默认初始化成正交投影矩阵
 */
void HelloOpenGLESRender::initProjectMat4()
{
    /*
     * 透视投影矩阵：
     * 透视投影矩阵用于创建逼真的三维场景效果，模拟人眼或摄像机的视角。
     * 在透视投影中，远处的物体看起来更小，而近处的物体看起来更大。
     * 这种效果适用于大多数三维场景的渲染，比如游戏和虚拟现实应用。
     * */

    /*
     * 正交投影矩阵：
     * 正交投影矩阵用于创建没有透视失真的三维场景投影。
     * 正交投影中的物体无论远近都保持相同的大小，这种效果适用于某些特定的应用场景，比如工程图、2D游戏或用户界面。
     * */


    // 透视投影矩阵
//    glm::mat4 viewMatrix = glm::lookAt(
//            glm::vec3(cameraPosX, cameraPosY, cameraPosZ), // 相机位置
//            glm::vec3(targetPosX, targetPosY, targetPosZ), // 相机看的目标
//            glm::vec3(upDirX, upDirY, upDirZ)              // 上方向
//    );
//
//    glm::mat4 projectionMatrix = glm::perspective(
//            glm::radians(fov),       // 视角
//            aspectRatio,             // 宽高比
//            nearPlane,               // 近剪裁平面
//            farPlane                 // 远剪裁平面
//    );
//    glm::mat4 modelMatrix = glm::mat4(1.0f); // 模型矩阵
//    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

    // 正交投影矩阵
//    glm::mat4 viewMatrix = glm::lookAt(
//            glm::vec3(cameraPosX, cameraPosY, cameraPosZ), // 相机位置
//            glm::vec3(targetPosX, targetPosY, targetPosZ), // 相机看的目标
//            glm::vec3(upDirX, upDirY, upDirZ)              // 上方向
//    );
//
//    glm::mat4 orthoMatrix = glm::ortho(
//            left, right,
//            bottom, top,
//            nearPlane, farPlane
//    );
//    glm::mat4 modelMatrix = glm::mat4(1.0f); // 模型矩阵
//    glm::mat4 mvpMatrix = orthoMatrix * viewMatrix * modelMatrix;


    // 补充相机矩阵:
//    相机在 z 轴上，看向原点：
//    glm::mat4 viewMatrix = glm::lookAt(
//            glm::vec3(0.0f, 0.0f, 3.0f), // 相机位置在 z 轴上方
//            glm::vec3(0.0f, 0.0f, 0.0f), // 看向原点
//            glm::vec3(0.0f, 1.0f, 0.0f)  // 上方向为 y 轴正方向
//    );

//    相机在 x 轴上，看向原点：
//    glm::mat4 viewMatrix = glm::lookAt(
//            glm::vec3(3.0f, 0.0f, 0.0f), // 相机位置在 x 轴上方
//            glm::vec3(0.0f, 0.0f, 0.0f), // 看向原点
//            glm::vec3(0.0f, 1.0f, 0.0f)  // 上方向为 y 轴正方向
//    );

//    相机在 y 轴上，看向原点：
//    glm::mat4 viewMatrix = glm::lookAt(
//            glm::vec3(0.0f, 3.0f, 0.0f), // 相机位置在 y 轴上方
//            glm::vec3(0.0f, 0.0f, 0.0f), // 看向原点
//            glm::vec3(0.0f, 0.0f, 1.0f)  // 上方向为 z 轴正方向
//    );

    /*
     * 总结：
     * 透视投影矩阵和正交投影矩阵在 OpenGL ES 中各有用途。
     * 透视投影矩阵用于模拟真实世界的视角效果，适用于大多数三维场景。
     * 正交投影矩阵则用于没有透视失真的场景，适用于工程图、2D 游戏和用户界面。通过正确设置和使用这些矩阵，可以实现不同的视觉效果。
     * */

    // 情况一：
    // 左右，上下，我们都使用了归一化的坐标，那么我们着色器中的顶点坐标也需要归一化的
    // 情况二：
    // 如果我们配置了窗口尺寸作为坐标，例如 left=-200.0f right=200.0f bottom=-200.0f top=200.0f
    // 那么，我们的顶点坐标也需要是这个尺寸的，而非归一化的！
    // 顶点坐标 经过与 投影矩阵 的运算，会得出一个归一化的值 projectMat*vec4(-100.0,100.0,0.0,1.0) -> vec4(-0.5,0.5,0.0,1.0)
    glm::mat4 projectMat = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    // 站在z轴看原点
    glm::mat4 viewMat = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // 矩阵之间相乘，是左到右执行（结合律）
    // 矩阵对向量的作用，是从右到左（最右的先作用）
    prjMat4 = projectMat * viewMat;
}

/**
 * 渲染线程 和 EGL环境创建好后回调
 */
void HelloOpenGLESRender::onEGLCreated()
{
    // 滤镜链打包数据
    filterPacket = std::make_shared<GLFilterPacket>(prjMat4); // 正交投影矩阵，归一化坐标

    // 初始化滤镜
    std::vector<std::shared_ptr<IGLFilterNode>> nodes;

    nodes.push_back(std::make_shared<Input2Sampler2d>(format)); // 输入滤镜: AVFrame转sampler2d
    nodes.push_back(std::make_shared<ScaleTypeFilter>()); // 几何处理: FitCenter
    nodes.push_back(std::make_shared<Sampler2D2Surface>(glContext)); // 输出滤镜: sampler2d绘制到surface上

    // 添加滤镜
    for (const std::shared_ptr<IGLFilterNode> &item: nodes) {
        filterChain->addFilter(item);
    }
}

/**
 * @return true表示已初始化
 */
bool HelloOpenGLESRender::isPrepared()
{
    return prepared;
}

/**
 * @param format 更新像素格式
 * @return true表示更新成功
 */
bool HelloOpenGLESRender::setAVPixelFormat(AVPixelFormat f)
{
    this->format = f;
    auto filter = filterChain->findFilter<Input2Sampler2d>();
    if (filter)
    {
        logger.i("input filter update pixel format[%d]", format);
        filter->updateAVPixelFormat(format);
        return true;
    }
    return false;
}

AVPixelFormat HelloOpenGLESRender::getAVPixelFormat()
{
    auto filter = filterChain->findFilter<Input2Sampler2d>();
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
bool HelloOpenGLESRender::addOutputs(void *surface)
{
    if (glContext)
    {
        return glContext->addSurface(surface, false);
    }
    return false;
}

/**
 * @param surface 移除画面渲染缓冲区
 */
void HelloOpenGLESRender::removeOutputs(void *surface)
{
    if (glContext)
    {
        glContext->removeSurface(surface);
    }
}

/**
 * 洗刷渲染缓冲区
 * @param r red
 * @param g green
 * @param b blue
 * @param a alpha
 * @return true表示洗刷成功
 */
bool HelloOpenGLESRender::clearColor(float r, float g, float b, float a)
{
    if (!glContext || !prepared)return false;
    logger.i("HelloOpenGLESRender::clearColor(%p)", this);

    // 多surface渲染
    std::vector<uint64_t> keys;
    glContext->getSurfaceKeys(&keys);
    glContext->setClearColor(r, g, b, a);
    for (uint64_t key: keys)
    {
        glContext->renderStart(key); // 清空缓冲区内容

        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        int64_t ptsUs = ms.count() * 1000L;
        glContext->renderEnd(key, ptsUs); // 交换buffer到surface上
    }

    return true;
}

/**
 * @param frame 画面内容
 * @return 绘制画面
 */
bool HelloOpenGLESRender::draw(std::shared_ptr<HelloVideoFrame> frame)
{
    if (!glContext || !prepared)return false;

    // 多surface渲染
    std::vector<uint64_t> keys;
    glContext->getSurfaceKeys(&keys);
    logger.i("HelloOpenGLESRender::draw keys.size[%d]",keys.size());
    for (uint64_t key: keys)
    {

        // 执行滤镜链
        filterPacket->key = key;
        filterPacket->width = glContext->getSurfaceWidth(key); // 视口宽度
        filterPacket->height = glContext->getSurfaceHeight(key); // 视口高度
        filterPacket->textureWidth = frame->getWidth(); // 输入纹理的宽度
        filterPacket->textureHeight = frame->getHeight(); // 输入纹理的高度
        std::shared_ptr<HelloVideoTexture> data = std::make_shared<HelloVideoTexture>(frame);
        filterPacket->textureData = data;

        filterChain->process(filterPacket);
    }

    return true;
}
