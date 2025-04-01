//
//  MTLScaleTypeFilter.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/4/1.
//

#ifndef MTLScaleTypeFilter_hpp
#define MTLScaleTypeFilter_hpp

#include "../IMTLFilterNode.hpp"
#include "../../pipeline/ScaleTypePipeline.hpp"

class MTLScaleTypeFilter : public IMTLFilterNode
{
public:
    explicit MTLScaleTypeFilter(const std::shared_ptr<HelloMTLContext> &ctx);
    ~MTLScaleTypeFilter() override;
    
    void onProcess(std::shared_ptr<MTLFilterPacket> packet) override;
    
private:
    std::shared_ptr<ScaleTypePipeline> pipeline;
};

#endif /* MTLScaleTypeFilter_hpp */
