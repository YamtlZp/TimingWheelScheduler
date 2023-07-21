#include "TimerTaskEntry.h"

TimerTaskEntry::TimerTaskEntry(uint32_t taskId, int64_t expiraMs, uint32_t intervalMs, const TimerTask& task)
    : m_taskId(taskId)
    , m_intervalMs(intervalMs)
    , m_isPolling(intervalMs > 0)
    , m_timerTask(task)
    , m_isTaskCanceld(false)
{
    m_expiraMs.exchange(expiraMs);
    //printf("m_expiraMs:%lld\n", m_expiraMs.load());
}

TimerTaskEntry::~TimerTaskEntry()
{
    
}

void TimerTaskEntry::ExcuteTask()
{
    if (!m_isTaskCanceld && m_timerTask)
    {
        m_timerTask();
    }
}

void TimerTaskEntry::CancelTask()
{
    m_isTaskCanceld = true;
}

bool TimerTaskEntry::IsCanceled()
{
    return m_isTaskCanceld;
}

int64_t TimerTaskEntry::GetExpireMs() const
{
    return m_expiraMs;
}

bool TimerTaskEntry::IsTaskPolling() const
{
    return m_isPolling;
}

void TimerTaskEntry::UpdateExpireMs()
{
    m_expiraMs.fetch_add(m_intervalMs);
}

bool TimerTaskEntry::ResetPollingTaskInterval(uint32_t intervalMs)
{
    m_intervalMs = intervalMs;
    return true;
}

uint32_t TimerTaskEntry::GetTaskId() const
{
    return m_taskId;
}