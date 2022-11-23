#ifndef ROBOTOPERATOR_THREADPOOL_H
#define ROBOTOPERATOR_THREADPOOL_H
#include <thread>
#include <functional>

class ThreadPool {
public:
    explicit ThreadPool(size_t size = std::thread::hardware_concurrency());
    ~ThreadPool();
    ThreadPool(const ThreadPool &tp) = delete;// 不允许进行拷贝构造
    ThreadPool(ThreadPool &&tp) = delete;// 不允许进行拷贝构造
    ThreadPool &operator=(const ThreadPool &tp) = delete;// 不允许进行拷贝构造
    ThreadPool &operator=(ThreadPool &&tp) = delete;// 不允许进行拷贝构造
    /// \brief 提交运行任务
    /// \param call lambda表达式
    void Enqueue(const std::function<void(void)> &call);

    size_t size();
private:
    struct ThreadPool_impl;
    friend struct ThreadPool_impl;
    ThreadPool_impl *impl;
};


#endif //ROBOTOPERATOR_THREADPOOL_H
