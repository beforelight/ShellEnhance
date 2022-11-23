#include "ThreadPool.h"
#include <vector>
#include <queue>

#include <mutex>
struct ThreadPool::ThreadPool_impl{
    std::vector<std::thread> workers; //用来工作的线程
    std::queue<std::function<void()>> tasks; //线程池的任务队列
    std::condition_variable cond_;
    std::mutex mutex_;
    bool stop;
    explicit ThreadPool_impl(size_t size) : stop(false) {
        for (int i = 0; i < size; i++) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function < void() > task;
                    {//临界区开始 等待线程池的销毁或者是任务的带来
                        std::unique_lock<std::mutex> lock(mutex_);
                        cond_.wait(lock, [this] { return this->stop || !this->tasks.empty(); }); //条件变量的wait函数 防止虚假唤醒
                        if (this->stop && this->tasks.empty()) return;
                        task = std::move(this->tasks.front()); //将队列的头部移动给task
                        this->tasks.pop(); //记得出队列
                    }//临界区结束 自动释放锁 mutex_
                    task(); //执行任务
                }
            });
        }
    };
    ~ThreadPool_impl(){
        {
            //临界区开始  锁住stop 将其赋值为true 通知线程退出循环
            std::unique_lock<std::mutex> lock(mutex_);
            stop = true;
            //临界区结束 自动释放锁 mutex_
        }
        cond_.notify_all();//通知所有的线程退出循环
        for_each(workers.begin(), workers.end(), std::mem_fn(&std::thread::join)); //等待
    }
};

ThreadPool::ThreadPool(size_t size) :impl(new ThreadPool_impl(size)){}
ThreadPool::~ThreadPool() {
    delete impl;
}
void ThreadPool::Enqueue(const std::function<void(void)> &call) {
    {
        //临界区开始 锁任务队列
        std::unique_lock<std::mutex> lock(impl->mutex_);
        //如果已经将线程池析构,就不允许再入队列
        if (impl->stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        impl->tasks.emplace(call);
    }
    impl->cond_.notify_one(); //通知任意一个线程接收任务
}
size_t ThreadPool::size() {
    return impl->tasks.size();
}
