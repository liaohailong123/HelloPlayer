//
// Created by liaohailong on 2024/6/2.
//


#include <utility>

#include "NativeHandler.hpp"


NHandlerThread::NHandlerThread(std::string _name, HandlerMessage _callback)
    : name(std::move(_name)), callback(_callback), onHandler(nullptr), thread(nullptr), args(nullptr), handler(nullptr),
      mutex(), cond(), active(false), start_flag()
{
    NHLog::instance()->i("NHandlerThread::NHandlerThread(%s)", name.c_str());
}

NHandlerThread::~NHandlerThread()
{
    NHLog::instance()->i("~NHandlerThread::NHandlerThread(%s)", name.c_str());
    quite();
}

void NHandlerThread::startSync()
{
    start(
        [](NHandler *h, void *userdata) -> void {
            auto native = reinterpret_cast<NHandlerThread *>(userdata);

            // std::atomic<bool> active; 确保 active 是原子变量
            native->active.store(true, std::memory_order_release);
            NHLog::instance()->i("Victor active.store(true)");

            native->cond.notify_all(); // 唤醒调用线程
            NHLog::instance()->i("Victor cond.notify_all()");
        },
        this);

    // 避免竞争条件并简化同步逻辑
    std::unique_lock<std::mutex> locker(mutex);

    // 使用 while 循环检查条件，避免虚假唤醒
    while (!active.load(std::memory_order_acquire))
    {
        NHLog::instance()->i("Victor cond.wait_for(locker, 10ms)");
        if (cond.wait_for(locker, std::chrono::milliseconds(10)) == std::cv_status::timeout)
        {
            NHLog::instance()->i("Victor cond.wait_for timeout, rechecking active...");
        }
    }

    // 在此处加入日志打印，检查 active 变量的状态
    NHLog::instance()->i("Victor active is now: %d", active.load());
}

void NHandlerThread::start(OnHandlerCallback r, void *_args)
{
    std::call_once(start_flag, [&]() {
        args = _args;
        onHandler = r;
        thread = new std::thread(&NHandlerThread::run, std::ref(*this));

        if (thread == nullptr)
        {
            NHLog::instance()->i("Thread creation failed");
        }
    });
}

void NHandlerThread::run()
{
    NLooper::prepare();

    handler = new NHandler(callback);
    NHLog::instance()->i("Victor new NHandler(%p) success", handler);
    if (onHandler != nullptr)
    {
        onHandler(handler, args);
    }

    NLooper::loop();
}

NHandler *NHandlerThread::getHandler() { return handler; }

void NHandlerThread::quite()
{
    NLooper *looper = nullptr;
    if (thread != nullptr)
    {
        if (handler != nullptr)
        {
            looper = handler->getLooper();
            if (looper != nullptr)
            {
                looper->quite();
            }
            delete handler;
            handler = nullptr;
        }
        // 一般都是其他线程调用 quite 函数
        if (std::this_thread::get_id() != thread->get_id())
        {
            // 等待 NLooper::loop 结束
            if (thread->joinable())
            {
                thread->join();
            }
            delete thread;
        } else
        {
            // 在本线程内执行 quite 的情况兼容一下
            thread->detach();
        }

        if (looper != nullptr)
        {
            delete looper;
            looper = nullptr;
        }
        thread = nullptr;
    }
}
