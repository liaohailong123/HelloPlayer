//
//  IMTLFilterNode.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/31.
//

#ifndef IMTLFilterNode_hpp
#define IMTLFilterNode_hpp

#include "MTLFilterPacket.hpp"

/**
 滤镜链统一规范
 */
class IMTLFilterNode
{
public:
    explicit IMTLFilterNode(const char *tag, std::shared_ptr<HelloMTLContext> ctx);
    virtual ~IMTLFilterNode();
    
    virtual void onProcess(std::shared_ptr<MTLFilterPacket> packet) = 0;
    
protected:
    Logger logger;
    
    std::shared_ptr<HelloMTLContext> ctx;
};

#endif /* IMTLFilterNode_hpp */
