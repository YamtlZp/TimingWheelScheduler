#include "TimingWheelScheduler.h"
#include "ScheduledPollingStrategy.h"

/** 全局定时任务id
*/
static std::atomic_uint32_t g_uniqueTimerIdGenerator(1000);

/** 开启业务执行线程个数
*/
#define WORKER_THREAD_COUNT 2

IMPL_SINGLETON(TimingWheelScheduler)
int32_t TimingWheelScheduler::CreateDefaultTimingWheelModule()
{
    auto startMs = GetNowTimestamp();
    // 第一层为毫秒级时间轮 tickms = 50,wheelsize = 1000/50
    SelfAppendTimingWheel(10, 100, startMs);
    // 第二层为秒级时间轮   tickms = 1000,wheelsize = 60
    SelfAppendTimingWheel(1000, 60, startMs);
    // 第三层为分钟级时间轮 tickms = 60*1000,wheelsize = 60
    SelfAppendTimingWheel(60 * 1000, 60, startMs);
    // 第四层为小时级时间轮 tickms = 60*60*1000,wheelsize = 24
    SelfAppendTimingWheel(60 * 60 * 1000, 24, startMs, true);

    // 设置定时调度策略，默认调度间隔时间为100ms
    SetSchedulingPolicy(std::shared_ptr<ScheduledPollingStrategy>(new ScheduledPollingStrategy(100)));

    return 0;
}

int32_t TimingWheelScheduler::SelfAppendTimingWheel(uint32_t tickMs, uint32_t wheelSize, uint64_t startMs, bool isCompleted)
{
    if (m_isTimingWheelReady)
    {
        return -1;
    }

    // 第一层时间轮
    TimingWheelPtr timingWheel = std::make_shared<TimingWheel>(tickMs, wheelSize, startMs);
    if (m_timingWheels.size() == 0)
    {
        // 设置最底层时间轮对象
        if (m_lowestTimingWheel == nullptr)
        {
            m_lowestTimingWheel = timingWheel;
        }
        m_timingWheels.emplace_back(timingWheel);
        return 0;
    }

    // 非首层时间轮
    auto overFlowTimingWheel = m_timingWheels.back();
    if (overFlowTimingWheel)
    {
        overFlowTimingWheel->SetOverFlowTimingWheel(timingWheel.get());
        m_timingWheels.emplace_back(timingWheel);
    }

    m_isTimingWheelReady = isCompleted;

    return 0;
}

void TimingWheelScheduler::SetSchedulingPolicy(const ISchedulingStrategyPtr& scheduler)
{
    m_spSchedulingStrategy = scheduler;
    m_spSchedulingStrategy->SetSchedulingStrategyExecutor(this);
}

bool TimingWheelScheduler::StartScheduling()
{
    if (m_spSchedulingStrategy)
    {
        m_asyncThreadPool = new AsyncThreadPool(WORKER_THREAD_COUNT);
        m_spSchedulingStrategy->StartSchedule();
    }

    return true;
}

void TimingWheelScheduler::StopScheduling()
{
    m_isExited = true;
    if (m_spSchedulingStrategy)
    {
        m_spSchedulingStrategy->StopSchedule();
        m_spSchedulingStrategy = nullptr;
    }

    if (m_asyncThreadPool)
    {
        m_asyncThreadPool = nullptr;
    }
}

int32_t TimingWheelScheduler::AddExpiredAtTimer(uint64_t atTime, const TimerTask& task)
{
    //printf("AddExpiredAtTimer,atTime:%llu\n", atTime);
    if (m_spSchedulingStrategy == nullptr)
    {
        return -1;
    }

    if (m_lowestTimingWheel == nullptr || !m_isTimingWheelReady)
    {
        return -1;
    }

    ++g_uniqueTimerIdGenerator;
    std::lock_guard<std::mutex> lck(m_taskLock);
    m_lowestTimingWheel->AddTimer(std::make_shared<TimerTaskEntry>(g_uniqueTimerIdGenerator, atTime, 0, task));

    return g_uniqueTimerIdGenerator;
}

int32_t TimingWheelScheduler::AddExpiredIntervalTimer(uint32_t intervalTime, const TimerTask& task, bool isPolling)
{
    if (m_lowestTimingWheel == nullptr)
    {
        return false;
    }
    if (!isPolling)
    {
        //printf("AddExpiredAtTimer,currentTimeMs:%llu\n", currentTimeMs + intervalTime);
        return AddExpiredAtTimer(m_lowestTimingWheel->GetCurrentTimeMs() + intervalTime, task);
    }
    else
    {
        if (m_spSchedulingStrategy == nullptr || !m_isTimingWheelReady)
        {
            return -1;
        }

        ++g_uniqueTimerIdGenerator;
        std::lock_guard<std::mutex> lck(m_taskLock);
        //printf("AddExpiredIntervalTimer,currentTimeMs:%llu\n", currentTimeMs + intervalTime);
        m_lowestTimingWheel->AddTimer(std::make_shared<TimerTaskEntry>(g_uniqueTimerIdGenerator, m_lowestTimingWheel->GetCurrentTimeMs() + intervalTime, intervalTime, task));

        return g_uniqueTimerIdGenerator;
    }
    return 0;
}

void TimingWheelScheduler::CancelTimer(uint32_t timerId)
{
    /*auto findItr = m_joinTasks.find(timerId);
    if (findItr != m_joinTasks.end() && findItr->second != nullptr)
    {
        findItr->second->CancelTask();
    }*/
}

bool TimingWheelScheduler::AdvanceClock(uint32_t timeoutMs)
{
    std::lock_guard<std::mutex> lck(m_taskLock);
    if (m_lowestTimingWheel)
    {
        m_lowestTimingWheel->AdvanceClock(m_lowestTimingWheel->GetCurrentTimeMs() + timeoutMs);
        auto taskList = std::move(m_lowestTimingWheel->PollCurrentBucketTasks());
        for (const auto& timer : taskList) 
        {
            if (!timer || timer->IsCanceled())
            {
                continue;
            }

            if (m_asyncThreadPool)
            {
                m_asyncThreadPool->enqueue([=]() {
                    timer->ExcuteTask();
                });
            }
            //timer->ExcuteTask();

            if (timer->IsTaskPolling())
            {
                timer->UpdateExpireMs();
                //printf("AdvanceClock,UpdateExpireMs:%llu-GetCurrentTimeMs:%llu\n", timer->GetExpireMs(), m_lowestTimingWheel->GetCurrentTimeMs());
                m_lowestTimingWheel->AddTimer(std::shared_ptr<TimerTaskEntry>(timer));
            }
        }
    }
    return true;
}

uint64_t TimingWheelScheduler::GetLowestTimingWheelTimeMs()
{
    if (m_lowestTimingWheel)
    {
        return m_lowestTimingWheel->GetCurrentTimeMs();
    }

    return 0;
}