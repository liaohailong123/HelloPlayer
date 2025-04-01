//
//  HelloMTLShaderTypes.h
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/30.
//

#ifndef HelloMTLShaderTypes_h
#define HelloMTLShaderTypes_h

#include <simd/simd.h>

typedef enum AAPLVertexInputIndex
{
    AAPLVertexInputIndexVertices = 0,
    AAPLVertexInputIndexObjParams = 1,
} AAPLVertexInputIndex;

typedef enum AAPLTextureInputIndex
{
    AAPLTextureInputIndexColor0 = 0,
    AAPLTextureInputIndexColor1 = 1,
    AAPLTextureInputIndexColor2 = 2,
} AAPLTextureInputIndex;

typedef struct
{
    vector_float2 position;
    vector_float2 texcoord;
} AAPLTextureVertex;

typedef struct
{
    matrix_float4x4 mvpMat4;
    vector_int2 mirror;
} AAPLObjectParams;


#endif /* HelloMTLShaderTypes_h */
