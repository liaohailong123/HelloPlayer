//
// Created by 廖海龙 on 2025/1/7.
//

#ifndef HELLOPLAYER_HELLOPROCESSOR_HPP
#define HELLOPLAYER_HELLOPROCESSOR_HPP

#include "../../handler/NativeHandler.hpp"
#include "../../log/Logger.hpp"
#include "../../util/FFUtil.hpp"

#include <mutex>
#include <list>
#include <atomic>
#include <memory>


/**
 * Author: liaohailong
 * Date: 2025/1/7
 * Time: 16:43
 * Description: 生产者消费者结合，自身是一个消费者，处理后的产物，又交给别的消费者
 *
 * input -> process -> output
 *
 * while(running && !queue.empty())
 *      onProcess();
 *
 * queue 可以一直添加任务，使用serial区分阶段（会话）
 *
 **/
template<typename Input, typename Output>
class HelloProcessor
{
public:
    typedef void(*OutputCallback)(const std::shared_ptr<Output> &data, void *userdata);

    typedef struct
    {
        std::shared_ptr<Input> data;
        HelloProcessor *userdata;
    } InputDataCtx;

    typedef struct
    {
        OutputCallback callback;
        void *userdata;
    } OutputCallbackCtx;

public:
    explicit HelloProcessor(const char *tag = "HelloProcessor") : logger(tag),
                                                                  ht(nullptr), handler(nullptr),
                                                                  callbackCtx(),
                                                                  mutex(), queue(), running(false)
    {
        logger.i("HelloProcessor::HelloProcessor(%p)", this);
        init();
    }

    virtual ~HelloProcessor()
    {
        logger.i("HelloProcessor::~HelloProcessor(%p)", this);
        release();
    }

    void setOutputCallback(OutputCallback callback, void *userdata)
    {
        this->callbackCtx.callback = callback;
        this->callbackCtx.userdata = userdata;
    }

    void pushInputData(const std::shared_ptr<Input> &data, bool frontOfQueue = false)
    {
        std::shared_ptr<InputDataCtx> ctx = std::make_shared<InputDataCtx>();
        ctx->data = data;
        ctx->userdata = this;

        // 挂锁，维护缓存队列数据安全
        std::unique_lock<std::mutex> locker(this->mutex);
        this->queue.push_back(ctx);

        // 通知处理任务
        if (handler)
        {
            if (frontOfQueue)
            {
                handler->postAtFrontOfQueue(&HelloProcessor::process, this);
            } else
            {
                handler->post(&HelloProcessor::process, this);
            }

        }
    }

    size_t getQueueSize()
    {
        // 挂锁，维护缓存队列数据安全
        std::unique_lock<std::mutex> locker(this->mutex);
        return queue.size();
    }

    /**
     * @return 看队列第一个item，但是没从队列中移除
     */
    std::shared_ptr<InputDataCtx> peekFirst()
    {
        // 挂锁，维护缓存队列数据安全
        std::unique_lock<std::mutex> locker(this->mutex);
        if (this->queue.empty())
        {
            return std::make_shared<InputDataCtx>();
        }

        std::shared_ptr<InputDataCtx> ctx = this->queue.front();

        return ctx;
    }

    /**
     * 移除队列头部item
     */
    void popFirst()
    {
        // 挂锁，维护缓存队列数据安全
        std::unique_lock<std::mutex> locker(this->mutex);
        if (this->queue.empty())
        {
            return;
        }

        this->queue.pop_front();
    }

    /**
     * @return 取队列第一个item，从队列中移除
     */
    std::shared_ptr<InputDataCtx> pollFirst()
    {
        // 挂锁，维护缓存队列数据安全
        std::unique_lock<std::mutex> locker(this->mutex);
        if (this->queue.empty())
        {
            return std::make_shared<InputDataCtx>();
        }

        std::shared_ptr<InputDataCtx> ctx = this->queue.front();
        this->queue.pop_front();

        return ctx;
    }

    int64_t clearQueue()
    {
        // 挂锁，维护缓存队列数据安全
        std::unique_lock<std::mutex> locker(this->mutex);
        int64_t count = this->queue.size();
        this->queue.clear();
        return count;
    }

    void init()
    {
        if (ht)return;

        logger.i("Victor init(%p) start handler initialized", this);
        ht = new NHandlerThread("HelloProcessorHandlerThread");
        ht->startSync();
        handler = ht->getHandler();
        logger.i("Victor init(%p) end handler(%p) initialized", this, handler);
    }

    void start()
    {
        this->running = true;

        // 通知处理任务
        if (handler)
        {
            handler->post(&HelloProcessor::process, this);
        }
    }

    inline bool isRunning() noexcept
    {
        return running;
    }

    void callMeLater(int64_t delayUs)
    {
        // 通知处理任务
        if (handler && running)
        {
            handler->postDelayed(&HelloProcessor::process, delayUs / 1000, this);
        }
    }

    void stop()
    {
        this->running = false;
    }

    /**
     * 重置初始化状态
    */
    void reset()
    {
        stop();
        clearQueue();
        onReset();
    }

    void release()
    {
        stop();

        // 挂锁，维护缓存队列数据安全
        std::unique_lock<std::mutex> locker(this->mutex);
        this->queue.clear();

        // 释放线程和Handler
        if (ht)
        {
            delete ht; // 释放 NHandlerThread 时，内部会释放 NHandler
            handler = nullptr;
            ht = nullptr;
        }

    }

private:
    void static process(void *userdata)
    {
        // 一次性消费
        auto *native = reinterpret_cast<HelloProcessor *>(userdata);
        if (native->running)
        {
            std::shared_ptr<InputDataCtx> first = native->peekFirst();
            if (first && first->data)
            {
                bool custom = native->onProcess(first);
                if (custom)
                {
                    native->popFirst();
                    native->callMeLater(0);
                }
            }
        }
    }

protected:
    /**
     * 消费一次数据
     * @param inputData 待消费的数据
     * @return true表示消费成功，可以从队列中移除，并且继续下一次任务，false表示不移除队列，下次源数据还来
     */
    virtual bool onProcess(std::shared_ptr<InputDataCtx> inputData) = 0;

    /**
     * 重新初始化状态，进入空闲模式
     */
    virtual void onReset() = 0;

    /**
     * 子类调用，将处理好的数据，回调给外部（消费者）
     * @param data 产出数据
     */
    void sendCallback(std::shared_ptr<Output> data)
    {
        if (callbackCtx.callback && running)
        {
            callbackCtx.callback(data, callbackCtx.userdata);
        }
    }

protected:
    Logger logger;

    // 单线程，消息驱动模型
    NHandlerThread *ht;
    NHandler *handler;

    // 输出回调
    OutputCallbackCtx callbackCtx;

    // 缓存队列
    std::mutex mutex;
    std::list<std::shared_ptr<InputDataCtx>> queue;
    std::atomic<bool> running;

};


#endif //HELLOPLAYER_HELLOPROCESSOR_HPP
