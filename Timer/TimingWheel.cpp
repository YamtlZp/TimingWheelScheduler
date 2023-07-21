#include "TimingWheel.h"

TimingWheel::TimingWheel(uint32_t tickMs, uint32_t wheelSize, uint64_t startMs)
    : m_tickMs(tickMs)
    , m_wheelSize(wheelSize)
    , m_startTimeMs(startMs)
    , m_pOverFlowTimingWheel(nullptr)
    , m_buckets(wheelSize)
    , m_currentTick(0)
{
    m_wholeTimeSpan = tickMs * wheelSize;
    m_currentTimeMs = startMs - (startMs % tickMs);
}

TimingWheel::~TimingWheel()
{
    m_buckets.clear();
}

bool TimingWheel::AddTimer(TimerTaskEntryPtr entry)
{
    int64_t expiration = entry->GetExpireMs();
    //printf("%u-%lld-%llu-m_wheelSize:%lu-m_tickMs:%lu-m_currentTick:%u\n", entry->GetTaskId(), expiration, m_currentTimeMs, m_wheelSize, m_tickMs, m_currentTick);

    if (entry->IsCanceled())
    {
        // �����ѱ�ȡ��
        return false;
    }
    else if (expiration < m_currentTimeMs)
    {
        // �����ѹ��ڣ�������ӵ�ʱ������
        printf("expiration:%llu<m_currentTimeMs + m_tickMs:%llu\n", expiration, m_currentTimeMs + m_tickMs);
        return false;
    }
    else if (expiration < m_currentTimeMs + m_wholeTimeSpan)
    {
        // ���㵱ǰʱ����
        auto virtualId = (expiration - m_currentTimeMs) / m_tickMs;
        auto bucketId = (uint32_t)(virtualId % m_wheelSize);

        //printf("index:%llu-bucketId:%u-m_wheelSize:%lu-m_tickMs:%lu-m_currentTick:%u\n", virtualId, bucketId, m_wheelSize, m_tickMs, m_currentTick);
        m_buckets[(m_currentTick + bucketId)% m_wheelSize].push_back(entry);

        return true;
    }
    else
    {
        // ��ǰʱ�����޷�����ö�ʱ�������Ҫת����һ��ʱ������
        if (m_pOverFlowTimingWheel == nullptr)
        {
            AddOverFlowTimingWheel();
        }

        return m_pOverFlowTimingWheel->AddTimer(entry);
    }
}

void TimingWheel::AdvanceClock(uint64_t timeMs)
{
    //printf("AdvanceClock:timeMs:%llu-m_currentTimeMs:%llu\n", timeMs, m_currentTimeMs);
    if (timeMs >= m_currentTimeMs + m_tickMs)
    {
        m_currentTick += (uint32_t)(timeMs - m_currentTimeMs) / m_tickMs;
        m_currentTimeMs = timeMs - (timeMs % m_tickMs);
        //printf("AdvanceClock:%llu-m_currentTick:%u\n", m_currentTimeMs, m_currentTick);
        if (m_currentTick < m_wheelSize)
        {
            return;
        }

        m_currentTick = m_currentTick % m_wheelSize;
        //printf("AdvanceClock,after:%llu-m_currentTick:%u\n", m_currentTimeMs, m_currentTick);

        if (m_pOverFlowTimingWheel != nullptr)
        {
            m_pOverFlowTimingWheel->AdvanceClock(m_currentTimeMs);

            auto taskList = m_pOverFlowTimingWheel->PollCurrentBucketTasks();
            for (const auto& timer : taskList)
            {
                // ������ˢ��ͼ�ʱ����
                //printf("AdvanceClock-AddTimer:%llu\n", timer->GetExpireMs());
                this->AddTimer(std::shared_ptr<TimerTaskEntry>(timer));
            }
        }
    }
}

uint32_t TimingWheel::GetTickMs() const
{
    return m_tickMs;
}

uint32_t TimingWheel::GetWheelSize() const
{
    return m_wheelSize;
}

uint64_t TimingWheel::GetCurrentTimeMs() const
{
    return m_currentTimeMs;
}

std::list<TimerTaskEntryPtr> TimingWheel::PollCurrentBucketTasks()
{
    std::list<TimerTaskEntryPtr> bucket;
    bucket = std::move(m_buckets[m_currentTick]);
    return bucket;
}

void TimingWheel::SetOverFlowTimingWheel(TimingWheel* overFlowTimingWheel)
{
    m_pOverFlowTimingWheel = overFlowTimingWheel;
}

void TimingWheel::AddOverFlowTimingWheel()
{
    if (m_pOverFlowTimingWheel == nullptr)
    {
        m_pOverFlowTimingWheel = new TimingWheel(m_wholeTimeSpan, m_wheelSize, m_currentTimeMs);
    }
}