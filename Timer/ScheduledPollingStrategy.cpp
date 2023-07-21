#include "ScheduledPollingStrategy.h"

#include "TimingWheelScheduler.h"

ScheduledPollingStrategy::ScheduledPollingStrategy(uint32_t scheduleInterval)
    : m_scheduleInterval(scheduleInterval)
{

}

bool ScheduledPollingStrategy::SetSchedulingStrategyExecutor(TimingWheelScheduler* executor)
{
    if (executor == nullptr)
    {
        return false;
    }
    m_executor = executor;

    return true;
}

bool ScheduledPollingStrategy::StartSchedule()
{
    if (m_schedulePollingFuture && m_schedulePollingFuture->valid())
    {
        return true;
    }

    auto future = std::async(std::launch::async, [this]() {
        Scheduling();
    });
    m_schedulePollingFuture = std::make_shared<RCSchedulePollingFuture>(std::move(future));

    return true;
}

void ScheduledPollingStrategy::StopSchedule()
{
    m_isExited = true;
    if (m_schedulePollingFuture && m_schedulePollingFuture->valid())
    {
        m_schedulePollingFuture->wait();
        m_schedulePollingFuture = nullptr;
    }
}

void ScheduledPollingStrategy::Scheduling()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_scheduleInterval));

        if (m_isExited)
        {
            break;
        }

        if (m_executor)
        {
            m_executor->AdvanceClock(m_scheduleInterval);
        }
    }
}