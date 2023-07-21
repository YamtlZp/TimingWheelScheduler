#include "TimerTaskEntry.h"

#include <string>
#include <vector>
#include <list>
#include <mutex>

/** ʱ����
*/
class TimingWheel final
{
public:

    /** ���캯��
    @param [in] tickMs ����ʱ����
    @param [in] wheelSize ʱ�䵥λ����
    @param [in] startMs ʱ������ʼʱ���
    */
    TimingWheel(uint32_t tickMs, uint32_t wheelSize, uint64_t startMs);

    /** ��������
    */
    ~TimingWheel();

    /** ��Ӷ�ʱ��
    @param [in] entry ��ʱ������
    @return true-��ӳɹ���false-���ʧ��
    */
    bool AddTimer(TimerTaskEntryPtr entry);

    /** �ƶ�ʱ����ת��
    @param [in] timeMs ʱ��ת����ʱ��
    */
    void AdvanceClock(uint64_t timeMs);

public:

    /** ��ȡ��ǰʱ���ֻ���ʱ����
    */
    uint32_t GetTickMs() const;

    /** ��ȡ��ǰʱ����ʱ�䵥λ����
    */
    uint32_t GetWheelSize() const;

    /** ��ȡ��ǰʱ���ֵ�ʱ���
    */
    uint64_t GetCurrentTimeMs() const;

    /** ȡ����ǰʱ����ת�����ڲ�λ�Ķ�ʱ��������
    */
    std::list<TimerTaskEntryPtr> PollCurrentBucketTasks();

    /** ������һ��ʱ����
    */
    void SetOverFlowTimingWheel(TimingWheel* overFlowTimingWheel);

private:

    /** �����һ��ʱ����
    */
    void AddOverFlowTimingWheel();

private:

    /** ����ʱ����
    */
    uint32_t m_tickMs;

    /** ʱ�䵥λ����
    */
    uint32_t m_wheelSize;

    /** �ܵ�ʱ����
    */
    uint32_t m_wholeTimeSpan;

    /** ʱ������ʼʱ���
    */
    uint64_t m_startTimeMs;

    /** ��ǰ��ʱ���
    */
    uint64_t m_currentTimeMs;

    /** ��ǰʱ��������ʱ��̶�
    */
    uint32_t m_currentTick;

    /** ��ʱ�����б�
    */
    std::vector<std::list<TimerTaskEntryPtr>> m_buckets;

    /** �ϲ��ʱ���֣����ȸ����ʱ���֣�
    */
    TimingWheel* m_pOverFlowTimingWheel;

    /** ����ͬ����
    */
    std::recursive_mutex m_taskSyncLock;
};

using TimingWheelPtr = std::shared_ptr<TimingWheel>;