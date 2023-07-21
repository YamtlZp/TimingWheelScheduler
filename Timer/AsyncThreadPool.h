#ifndef __AsyncThreadPool_h_
#define __AsyncThreadPool_h_ 1

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>

class AsyncThreadPool
{
public:
    AsyncThreadPool(size_t);

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        ->std::future<typename std::result_of<F(Args...)>::type>;

    ~AsyncThreadPool();

private:
    std::queue<std::function<void()>> m_tasks;
    std::vector<std::thread> m_workers;
    std::mutex m_dataMutex;
    std::condition_variable m_condition;
    bool m_exit = false;
};

inline AsyncThreadPool::AsyncThreadPool(size_t threads)
{
    for (size_t i = 0; i < threads; ++i)
        m_workers.emplace_back([this]{
        for (;;)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(this->m_dataMutex);
                this->m_condition.wait(lock, [this] { return this->m_exit || !this->m_tasks.empty(); });
                if (this->m_exit && this->m_tasks.empty())
                    return;
                task = std::move(this->m_tasks.front());
                this->m_tasks.pop();
            }
            task();
        }
    });
}

template<class F, class... Args>
auto AsyncThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_dataMutex);
        if (m_exit)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        m_tasks.emplace([task]() { (*task)(); });
    }
    m_condition.notify_one();
    return res;
}

inline AsyncThreadPool::~AsyncThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_dataMutex);
        m_exit = true;
    }
    m_condition.notify_all();
    for (std::thread &worker : m_workers)
        worker.join();
}

#endif // __AsyncThreadPool_h_