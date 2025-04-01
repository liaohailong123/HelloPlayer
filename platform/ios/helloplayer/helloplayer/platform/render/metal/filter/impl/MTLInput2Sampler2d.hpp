//
//  MTLInput2Sampler2d.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/4/1.
//

#ifndef MTLInput2Sampler2d_hpp
#define MTLInput2Sampler2d_hpp

#include "../IMTLFilterNode.hpp"

#include "../../pipeline/MTLPipeline.hpp"
#include "../../pipeline/I420Pipeline.hpp"
#include "../../pipeline/NV12Pipeline.hpp"

extern "C"
{
#include <libavutil/avutil.h>
}

class MTLInput2Sampler2d : public IMTLFilterNode
{
public:
    explicit MTLInput2Sampler2d(const std::shared_ptr<HelloMTLContext> &ctx, AVPixelFormat format);

    ~MTLInput2Sampler2d() override;

    void updateAVPixelFormat(AVPixelFormat format);
    
    AVPixelFormat getAVPixelFormat() noexcept;

    void onProcess(std::shared_ptr<MTLFilterPacket> packet) override;

private:
    AVPixelFormat format;
    std::shared_ptr<MTLPipeline> pipeline;
    
};

#endif /* MTLInput2Sampler2d_hpp */
