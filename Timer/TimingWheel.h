#include "TimerTaskEntry.h"

#include <string>
#include <vector>
#include <list>
#include <mutex>

/** 时间轮
*/
class TimingWheel final
{
public:

    /** 构造函数
    @param [in] tickMs 基本时间跨度
    @param [in] wheelSize 时间单位个数
    @param [in] startMs 时间轮起始时间点
    */
    TimingWheel(uint32_t tickMs, uint32_t wheelSize, uint64_t startMs);

    /** 析构函数
    */
    ~TimingWheel();

    /** 添加定时器
    @param [in] entry 定时任务项
    @return true-添加成功，false-添加失败
    */
    bool AddTimer(TimerTaskEntryPtr entry);

    /** 推动时间轮转动
    @param [in] timeMs 时钟转动的时长
    */
    void AdvanceClock(uint64_t timeMs);

public:

    /** 获取当前时间轮基本时间跨度
    */
    uint32_t GetTickMs() const;

    /** 获取当前时间轮时间单位个数
    */
    uint32_t GetWheelSize() const;

    /** 获取当前时间轮的时间点
    */
    uint64_t GetCurrentTimeMs() const;

    /** 取出当前时间轮转动所在槽位的定时任务链表
    */
    std::list<TimerTaskEntryPtr> PollCurrentBucketTasks();

    /** 设置上一层时间轮
    */
    void SetOverFlowTimingWheel(TimingWheel* overFlowTimingWheel);

private:

    /** 添加上一层时间轮
    */
    void AddOverFlowTimingWheel();

private:

    /** 基本时间跨度
    */
    uint32_t m_tickMs;

    /** 时间单位个数
    */
    uint32_t m_wheelSize;

    /** 总的时间跨度
    */
    uint32_t m_wholeTimeSpan;

    /** 时间轮起始时间点
    */
    uint64_t m_startTimeMs;

    /** 当前的时间点
    */
    uint64_t m_currentTimeMs;

    /** 当前时间轮所在时间刻度
    */
    uint32_t m_currentTick;

    /** 定时任务列表
    */
    std::vector<std::list<TimerTaskEntryPtr>> m_buckets;

    /** 上层的时间轮（精度更大的时间轮）
    */
    TimingWheel* m_pOverFlowTimingWheel;

    /** 任务同步锁
    */
    std::recursive_mutex m_taskSyncLock;
};

using TimingWheelPtr = std::shared_ptr<TimingWheel>;