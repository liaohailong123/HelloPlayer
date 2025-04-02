//
//  PipelineUtil.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/31.
//

#ifndef PipelineUtil_hpp
#define PipelineUtil_hpp

#import <Metal/Metal.h>
#include <stdio.h>

// glm依赖，向量和矩阵相关的操作
#include "render/glm/glm.hpp"
#include "render/glm/vec3.hpp" // glm::vec3
#include "render/glm/vec4.hpp" // glm::vec4
#include "render/glm/mat4x4.hpp" // glm::mat4
#include "render/glm/ext/matrix_transform.hpp" // glm::identity glm::translate, glm::rotate, glm::scale
#include "render/glm/ext/matrix_clip_space.hpp" // glm::perspective
#include "render/glm/ext/scalar_constants.hpp" // glm::pi
#include "render/glm/gtc/type_ptr.hpp" // glm::value_ptr glm::make_mat4


#include "../HelloMTLContext.hpp"
#include "shader/HelloMTLShaderTypes.h"

namespace PipelineUtil
{
    id<MTLTexture> genrateTexure2D(std::shared_ptr<HelloMTLContext> ctx,int width, int height,
                                 MTLPixelFormat pixelFormat = MTLPixelFormatBGRA8Unorm, int rotation = -1);
}


#endif /* PipelineUtil_hpp */
