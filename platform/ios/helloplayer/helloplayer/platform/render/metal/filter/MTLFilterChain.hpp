//
//  MTLFilterChain.hpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/4/1.
//

#ifndef MTLFilterChain_hpp
#define MTLFilterChain_hpp

#include <list>
#include <memory>

#include "IMTLFilterNode.hpp"
#include "MTLFilterPacket.hpp"

// 使用 Metal 实现滤镜链
class MTLFilterChain
{
public:
    explicit MTLFilterChain();
    
    ~MTLFilterChain();
    
    /**
     * @param filter 添加滤镜，暂不支持添加重复类型的滤镜（class一致）
     */
    // 限制泛型为 IMTLFilterNode 子类
    template<typename T, typename = typename std::enable_if<std::is_base_of<IMTLFilterNode, T>::value>::type>
    void addFilter(const std::shared_ptr<T> &filter) {
        filters.push_back(filter);
    }

    /**
     * @tparam T 根据类型查找滤镜
     * @return nullptr=没找到，非空就找到了
     */
    template<typename T, typename = typename std::enable_if<std::is_base_of<IMTLFilterNode, T>::value>::type>
    T *findFilter() {
        for (const std::shared_ptr<IMTLFilterNode> &node_ctx_ptr: filters) {
            // dynamic_cast 对虚函数继承的子类才能转成功
            if (T *specificNode = dynamic_cast<T *>(node_ctx_ptr.get())) {
                return specificNode;
            }
        }
        return nullptr;
    }

    /**
     * 触发滤镜链
     * @param packet 数据包，携带输入输出纹理
     */
    void process(std::shared_ptr<MTLFilterPacket> packet);
    
private:
    Logger logger;
    std::list<std::shared_ptr<IMTLFilterNode>> filters;
};


#endif /* MTLFilterChain_hpp */
