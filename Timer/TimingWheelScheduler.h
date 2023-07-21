#ifndef _TimingWheelScheduler_H_
#define _TimingWheelScheduler_H_ 1

#include "Singleton.inl"
#include "TimingWheel.h"
#include "ISchedulingStrategy.h"
#include "AsyncThreadPool.h"

#include <mutex>
#include <future>
#include <chrono>
#include <unordered_map>

/** 时间轮调度器
*/
class TimingWheelScheduler
{
    /** 采用无锁单例实现
    */
    DECLARE_SINGLETON(TimingWheelScheduler)

public:

    /** 创建默认的时间轮模型
    @note 四层时间轮模型
    * 第一层为毫秒级时间轮 tickms = 100,wheelsize = 1000/100
    * 第二层为秒级时间轮   tickms = 1000,wheelsize = 60
    * 第三层为分钟级时间轮 tickms = 60*1000,wheelsize = 60
    * 第四层为小时级时间轮 tickms = 60*60*1000,wheelsize = 24
    @return 返回组建时间轮错误码
    */
    int32_t CreateDefaultTimingWheelModule();

    /** 自追加时间轮模型
    @note 可重复调用，必须从底层时间轮开始创建，追加一层，内部则会链接该时间轮
    @param [in] tickMs 基本时间跨度
    @param [in] wheelSize 时间单位个数
    @param [in] startMs 时间轮起始时间点
    @param [in] isCompleted 时间轮组建是否完成
    @return 返回组建时间轮错误码
    */
    int32_t SelfAppendTimingWheel(uint32_t tickMs, uint32_t wheelSize, uint64_t startMs, bool isCompleted = false);

    /** 设置时间轮调度（推动时钟运转）策略
    @note 如果不设置将会使用默认的调度策略
    @param [in] scheduler 调度策略
    */
    void SetSchedulingPolicy(const ISchedulingStrategyPtr& scheduler);

    /** 开始执行调度
    */
    bool StartScheduling();

    /** 结束调度
    */
    void StopScheduling();

public:

    /** 添加指定过期时间点的定时任务
    @param [in] atTime 指定的时间点
    @param [in] task 定时任务
    @return 任务唯一标识id
    */
    int32_t AddExpiredAtTimer(uint64_t atTime, const TimerTask& task);

    /** 添加指定过期时间间隔的定时任务
    @param [in] intervalTime 间隔时间
    @param [in] task 定时任务
    @param [in] isPolling 是否轮询
    @return 任务唯一标识id
    */
    int32_t AddExpiredIntervalTimer(uint32_t intervalTime, const TimerTask& task, bool isPolling);

    /** 取消指定定时任务项
    @param [in] timerId 定时任务id
    */
    void CancelTimer(uint32_t timerId);

    /** 推动时钟运行
    @param [in] timeoutMs 推动时钟运转间隔时间（应与最底层时间轮跨度单位时间相同）
    */
    bool AdvanceClock(uint32_t timeoutMs);

    /** 获取最底层时间轮的时间戳
    */
    uint64_t GetLowestTimingWheelTimeMs();

private:

    /** 默认构造、析构函数
    */
    TimingWheelScheduler() = default;
    ~TimingWheelScheduler() = default;

private:

    /** 是否已退出
    */
    std::atomic_bool m_isExited = false;

    /** 时间轮是否准备就绪
    */
    std::atomic_bool m_isTimingWheelReady = false;

    /** 时间轮列表
    */
    std::vector<TimingWheelPtr> m_timingWheels;

    /** 最底层时间轮
    */
    TimingWheelPtr m_lowestTimingWheel = nullptr;

    /** 调度策略
    */
    ISchedulingStrategyPtr m_spSchedulingStrategy = nullptr;

    /** 添加定时任务锁
    */
    std::mutex m_taskLock;

    /** 加入的定时任务列表
    */
    using JoinTimerTaskMap = std::unordered_map<uint32_t, TimerTaskEntryPtr>;
    JoinTimerTaskMap m_joinTasks;

    /** 设置壁纸异步线程池
    */
    AsyncThreadPool* m_asyncThreadPool = nullptr;

};

inline int64_t GetNowTimestamp()
{
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

#endif  // _TimingWheelScheduler_H_