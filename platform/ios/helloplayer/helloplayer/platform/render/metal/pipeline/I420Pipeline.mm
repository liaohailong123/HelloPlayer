//
//  I420Pipeline.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/31.
//

#include "I420Pipeline.hpp"


I420Pipeline::I420Pipeline(): MTLPipeline("I420Pipeline"), textureY(nil), textureU(nil), textureV(nil)
{
    logger.i("I420Pipeline::I420Pipeline(%p)", this);
}
I420Pipeline::~I420Pipeline()
{
    logger.i("I420Pipeline::~I420Pipeline(%p)", this);
}

std::string I420Pipeline::getVertextFuncName()
{
    return "textureVertexShader";
}

std::string I420Pipeline::getFragmentFuncName()
{
    return "i420FragmentShader";
}

void I420Pipeline::setTextureData(const std::shared_ptr<MTLTextureData> &image)
{
    int format = image->getFormat(); // 画面的格式 YUV420P YUV422P
    uint8_t **data = image->getData(); // 像素内容 [0]=Y [1]=U [2]=V
    int *lineSize = image->getLineSize(); // 每种像素的宽度（一行有多少个像素）
    
    int yWidth = image->getWidth();
    int yHeight = image->getHeight();

    int uvHeight = yHeight;
    int uvWidth = yWidth;
    
    if (format == AV_PIX_FMT_YUV422P || format == AV_PIX_FMT_YUVJ422P)
    {
        // YUV422
        uvWidth = yWidth / 2;
        uvHeight = yHeight;
    } else if (format == AV_PIX_FMT_YUV420P || format == AV_PIX_FMT_YUVJ420P)
    {
        // YUV420
        uvWidth = yWidth / 2;
        uvHeight = yHeight / 2;
    }
    
    if (textureY && (textureY.width != yWidth || textureY.height != yHeight)) {
        textureY = nil;
    }
    if (textureU && (textureU.width != uvWidth || textureU.height != uvHeight)) {
        textureU = nil;
    }
    if (textureV && (textureV.width != uvWidth || textureV.height != uvHeight)) {
        textureV = nil;
    }
    
    if (!textureY) {
        textureY = PipelineUtil::genrateTexure2D(ctx, yWidth, yHeight);
    }
    if (!textureU) {
        textureU = PipelineUtil::genrateTexure2D(ctx, uvWidth, uvHeight);
    }
    if (!textureV) {
        textureV = PipelineUtil::genrateTexure2D(ctx, uvWidth, uvHeight);
    }
    
    // 硬件解码,返回的buffer可能是一个正方形
    uint32_t _lineSizeY = lineSize[0];
    uint32_t _lineSizeUV = lineSize[0] / 2;
    // 把YUV数据分别填充到 textureY textureU textureV 上
    // **填充 Metal 纹理**
    MTLRegion regionY = MTLRegionMake2D(0, 0, yWidth, yHeight);
    [textureY replaceRegion:regionY mipmapLevel:0 withBytes:data[0] bytesPerRow:_lineSizeY];

    MTLRegion regionU = MTLRegionMake2D(0, 0, uvWidth, uvHeight);
    [textureU replaceRegion:regionU mipmapLevel:0 withBytes:data[1] bytesPerRow:_lineSizeUV];

    MTLRegion regionV = MTLRegionMake2D(0, 0, uvWidth, uvHeight);
    [textureV replaceRegion:regionV mipmapLevel:0 withBytes:data[2] bytesPerRow:_lineSizeUV];
}

bool I420Pipeline::draw(float projectMat[4*4])
{
    // 下面的条件缺一不可
    if (!encoder) return false; // 绘制指令编码器
    if (!textureY || !textureU || !textureV) return false; // 资源纹理
    if (!renderTarget) return false; // 渲染目标
    if (!pipelineState) return false; // 渲染管线
    
    // 使用渲染管线
    [encoder setRenderPipelineState:pipelineState];
    
    // 传数据: 顶点坐标 + 纹理坐标
    [encoder setVertexBytes:&sVertices
                           length:sizeof(sVertices)
                          atIndex:AAPLVertexInputIndexVertices];
    
    // 设置矩阵
    glm::mat4 _projectMat4 = glm::make_mat4(projectMat);
    
    // 旋转角度
    float angle = glm::radians(rotation);
    glm::vec3 axis2(0.0, 0.0, 1.0); // 旋转轴为 Z 轴
    glm::mat4 _moduleMat4 = glm::rotate(moduleMat4, angle, axis2);
    
    glm::mat4 mvpMat4 = _projectMat4 * _moduleMat4;
    // 矩阵数据
    memcpy(&sObjParams.mvpMat4, glm::value_ptr(mvpMat4), sizeof(matrix_float4x4));
    // 镜像数据
    sObjParams.mirror = (vector_int2){ hMirror ? 1 : 0, vMirror ? 1 : 0 };
    
    // 传数据: 矩阵 + 镜像
    [encoder setVertexBytes:&sObjParams
                           length:sizeof(AAPLObjectParams)
                          atIndex:AAPLVertexInputIndexObjParams];
    
    // 设置资源纹理
    [encoder setFragmentTexture:textureY atIndex:AAPLTextureInputIndexColor0];
    [encoder setFragmentTexture:textureU atIndex:AAPLTextureInputIndexColor1];
    [encoder setFragmentTexture:textureV atIndex:AAPLTextureInputIndexColor2];

    // Draw quad with rendered texture.
    [encoder drawPrimitives:MTLPrimitiveTypeTriangle
                      vertexStart:0
                      vertexCount:6];
    
    return true;
}
