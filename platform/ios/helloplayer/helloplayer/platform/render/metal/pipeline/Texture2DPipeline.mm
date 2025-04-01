//
//  Texture2DPipeline.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#include "Texture2DPipeline.hpp"

Texture2DPipeline::Texture2DPipeline(): MTLPipeline("Texture2DPipeline"), texture(nil)
{
    logger.i("Texture2DPipeline::Texture2DPipeline(%p)", this);
}
Texture2DPipeline::~Texture2DPipeline()
{
    logger.i("Texture2DPipeline::~Texture2DPipeline(%p)", this);
}

std::string Texture2DPipeline::getVertextFuncName()
{
    return "textureVertexShader";
}

std::string Texture2DPipeline::getFragmentFuncName()
{
    return "textureFragmentShader";
}

void Texture2DPipeline::setTexture(const std::vector<id<MTLTexture>> &textures)
{
    this->texture = textures[0];
}

bool Texture2DPipeline::draw(float projectMat[16])
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
