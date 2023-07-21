#ifndef _ScheduledPollingStrategy_H_
#define _ScheduledPollingStrategy_H_ 1

#include "ISchedulingStrategy.h"

#include <stdint.h>
#include <mutex>
#include <future>

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

/** ��ʱ���Ȳ��ԣ���ѯ���ʱ�䣩
*/
class ScheduledPollingStrategy : public ISchedulingStrategy
{
public:

    /** ���캯��
    */
    explicit ScheduledPollingStrategy(uint32_t scheduleInterval = 100);

    /** Ĭ�Ϲ��졢��������
    */
    //ScheduledPollingStrategy() = default;
    ~ScheduledPollingStrategy() = default;

public:

    /** ���ò��Ե�ִ����
    */
    bool SetSchedulingStrategyExecutor(TimingWheelScheduler* executor) override;

    /** ��ʼ����
    */
    bool StartSchedule() override;

    /** ֹͣ����
    */
    void StopSchedule() override;

private:

    /** ����
    */
    void Scheduling();

private:

    /** ������
    */
    std::mutex m_scheduleLock;

    /** �����߳�
    */
    using RCSchedulePollingFuture = std::future<void>;
    std::shared_ptr<RCSchedulePollingFuture> m_schedulePollingFuture = nullptr;

    /** �����Ƿ����˳�
    */
    bool m_isExited = false;

    /** ��ʱ���ȼ��ʱ��
    */
    uint32_t m_scheduleInterval = 100;

    /** ���Ȳ���ִ����
    */
    TimingWheelScheduler* m_executor = nullptr;
};

#endif  // _ScheduledPollingStrategy_H_