//
//  HelloiOSCallbackCtx.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/21.
//

#include "HelloiOSCallbackCtx.hpp"


HelloiOSCallbackCtx::HelloiOSCallbackCtx(): onPrepared(nullptr),onPlayStateChanged(nullptr),onErrorInfo(nullptr),
                                            onCurrentPosition(nullptr),onBufferPosition(nullptr)
{
    
}

HelloiOSCallbackCtx::~HelloiOSCallbackCtx()
{
    
}
