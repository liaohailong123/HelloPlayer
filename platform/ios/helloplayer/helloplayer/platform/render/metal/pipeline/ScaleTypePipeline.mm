//
//  ScaleTypePipeline.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/31.
//

#include "ScaleTypePipeline.hpp"

ScaleTypePipeline::ScaleTypePipeline(): MTLPipeline("ScaleTypePipeline"), scaleType(FitXY)
{
    logger.i("ScaleTypePipeline::ScaleTypePipeline(%p)", this);
}

ScaleTypePipeline::~ScaleTypePipeline()
{
    logger.i("ScaleTypePipeline::~ScaleTypePipeline(%p)", this);
}

std::string ScaleTypePipeline::getVertextFuncName()
{
    return "textureVertexShader";
}

std::string ScaleTypePipeline::getFragmentFuncName()
{
    return "textureFragmentShader";
}

void ScaleTypePipeline::setTexture(const std::vector<id<MTLTexture>> &textures)
{
    this->texture = textures[0];
}

bool ScaleTypePipeline::draw(float projectMat[4*4])
{
    // 下面的条件缺一不可
    if (!encoder) return false; // 绘制指令编码器
    if (!texture) return false; // 资源纹理
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
    [encoder setFragmentTexture:texture atIndex:AAPLTextureInputIndexColor0];

    // Draw quad with rendered texture.
    [encoder drawPrimitives:MTLPrimitiveTypeTriangle
                      vertexStart:0
                      vertexCount:6];
    
    return true;
}


void ScaleTypePipeline::setScaleType(ScaleTypePipeline::ScaleType type)
{
    this->scaleType = type;
}

void ScaleTypePipeline::setRect(const std::shared_ptr<Rect> &src,
                                 const std::shared_ptr<Rect> &dst)
{
    switch (scaleType) {
        case FitXY:
            moduleMat4 = glm::identity<glm::mat4>();
            break;
        case FitCenter:
            moduleMat4 = makeFitCenterMat4(src,dst);
            break;
        case FitStart:
            // 后续支持
            break;
        case FitEnd:
            // 后续支持
            break;
        case CenterCrop:
            // 后续支持
            break;
    }
}

glm::mat4 ScaleTypePipeline::makeFitCenterMat4(const std::shared_ptr<Rect> &src,
                                                const std::shared_ptr<Rect> &dst)
{
    glm::mat4 mat4 = glm::identity<glm::mat4>();

    // 原点在左下,Y轴向上,X轴向右,视口坐标系
    int srcWidth = src->right - src->left;
    int srcHeight = src->top - src->bottom;
    int dstWidth = dst->right - dst->left;
    int dstHeight = dst->top - dst->bottom;

    float scaleX = float(dstWidth) / float(srcWidth);
    float scaleY = float(dstHeight) / float(srcHeight);

    float finalScaleX, finalScaleY;

    // 选择让一个方向的 scale = 1.0
    if (scaleX < scaleY) {
        finalScaleX = 1.0f;
        finalScaleY = scaleX / scaleY;
    } else {
        finalScaleY = 1.0f;
        finalScaleX = scaleY / scaleX;
    }

    mat4 = glm::scale(mat4,glm::vec3(finalScaleX,finalScaleY,1.0f));

    return mat4;
}

glm::mat4 ScaleTypePipeline::makeCropCenterMat4(const std::shared_ptr<Rect> &src,
                             const std::shared_ptr<Rect> &dst)
{
    glm::mat4 mat4 = glm::identity<glm::mat4>();

    return mat4;
}
