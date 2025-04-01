//
//  IMTLFilterNode.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/31.
//

#include "IMTLFilterNode.hpp"


IMTLFilterNode::IMTLFilterNode(const char *tag, std::shared_ptr<HelloMTLContext> ctx): logger(tag), ctx(ctx)
{
    logger.i("IMTLFilterNode::IMTLFilterNode(%p)", this);
}
IMTLFilterNode::~IMTLFilterNode()
{
    logger.i("IMTLFilterNode::~IMTLFilterNode(%p)", this);
}
