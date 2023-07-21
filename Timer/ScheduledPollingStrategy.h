#ifndef _ScheduledPollingStrategy_H_
#define _ScheduledPollingStrategy_H_ 1

#include "ISchedulingStrategy.h"

#include <stdint.h>
#include <mutex>
#include <future>

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

/** 定时调度策略（轮询间隔时间）
*/
class ScheduledPollingStrategy : public ISchedulingStrategy
{
public:

    /** 构造函数
    */
    explicit ScheduledPollingStrategy(uint32_t scheduleInterval = 100);

    /** 默认构造、析构函数
    */
    //ScheduledPollingStrategy() = default;
    ~ScheduledPollingStrategy() = default;

public:

    /** 设置策略的执行者
    */
    bool SetSchedulingStrategyExecutor(TimingWheelScheduler* executor) override;

    /** 开始调度
    */
    bool StartSchedule() override;

    /** 停止调度
    */
    void StopSchedule() override;

private:

    /** 调度
    */
    void Scheduling();

private:

    /** 调度锁
    */
    std::mutex m_scheduleLock;

    /** 调度线程
    */
    using RCSchedulePollingFuture = std::future<void>;
    std::shared_ptr<RCSchedulePollingFuture> m_schedulePollingFuture = nullptr;

    /** 任务是否已退出
    */
    bool m_isExited = false;

    /** 定时调度间隔时长
    */
    uint32_t m_scheduleInterval = 100;

    /** 调度策略执行者
    */
    TimingWheelScheduler* m_executor = nullptr;
};

#endif  // _ScheduledPollingStrategy_H_