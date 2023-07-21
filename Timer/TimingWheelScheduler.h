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

/** ʱ���ֵ�����
*/
class TimingWheelScheduler
{
    /** ������������ʵ��
    */
    DECLARE_SINGLETON(TimingWheelScheduler)

public:

    /** ����Ĭ�ϵ�ʱ����ģ��
    @note �Ĳ�ʱ����ģ��
    * ��һ��Ϊ���뼶ʱ���� tickms = 100,wheelsize = 1000/100
    * �ڶ���Ϊ�뼶ʱ����   tickms = 1000,wheelsize = 60
    * ������Ϊ���Ӽ�ʱ���� tickms = 60*1000,wheelsize = 60
    * ���Ĳ�ΪСʱ��ʱ���� tickms = 60*60*1000,wheelsize = 24
    @return �����齨ʱ���ִ�����
    */
    int32_t CreateDefaultTimingWheelModule();

    /** ��׷��ʱ����ģ��
    @note ���ظ����ã�����ӵײ�ʱ���ֿ�ʼ������׷��һ�㣬�ڲ�������Ӹ�ʱ����
    @param [in] tickMs ����ʱ����
    @param [in] wheelSize ʱ�䵥λ����
    @param [in] startMs ʱ������ʼʱ���
    @param [in] isCompleted ʱ�����齨�Ƿ����
    @return �����齨ʱ���ִ�����
    */
    int32_t SelfAppendTimingWheel(uint32_t tickMs, uint32_t wheelSize, uint64_t startMs, bool isCompleted = false);

    /** ����ʱ���ֵ��ȣ��ƶ�ʱ����ת������
    @note ��������ý���ʹ��Ĭ�ϵĵ��Ȳ���
    @param [in] scheduler ���Ȳ���
    */
    void SetSchedulingPolicy(const ISchedulingStrategyPtr& scheduler);

    /** ��ʼִ�е���
    */
    bool StartScheduling();

    /** ��������
    */
    void StopScheduling();

public:

    /** ���ָ������ʱ���Ķ�ʱ����
    @param [in] atTime ָ����ʱ���
    @param [in] task ��ʱ����
    @return ����Ψһ��ʶid
    */
    int32_t AddExpiredAtTimer(uint64_t atTime, const TimerTask& task);

    /** ���ָ������ʱ�����Ķ�ʱ����
    @param [in] intervalTime ���ʱ��
    @param [in] task ��ʱ����
    @param [in] isPolling �Ƿ���ѯ
    @return ����Ψһ��ʶid
    */
    int32_t AddExpiredIntervalTimer(uint32_t intervalTime, const TimerTask& task, bool isPolling);

    /** ȡ��ָ����ʱ������
    @param [in] timerId ��ʱ����id
    */
    void CancelTimer(uint32_t timerId);

    /** �ƶ�ʱ������
    @param [in] timeoutMs �ƶ�ʱ����ת���ʱ�䣨Ӧ����ײ�ʱ���ֿ�ȵ�λʱ����ͬ��
    */
    bool AdvanceClock(uint32_t timeoutMs);

    /** ��ȡ��ײ�ʱ���ֵ�ʱ���
    */
    uint64_t GetLowestTimingWheelTimeMs();

private:

    /** Ĭ�Ϲ��졢��������
    */
    TimingWheelScheduler() = default;
    ~TimingWheelScheduler() = default;

private:

    /** �Ƿ����˳�
    */
    std::atomic_bool m_isExited = false;

    /** ʱ�����Ƿ�׼������
    */
    std::atomic_bool m_isTimingWheelReady = false;

    /** ʱ�����б�
    */
    std::vector<TimingWheelPtr> m_timingWheels;

    /** ��ײ�ʱ����
    */
    TimingWheelPtr m_lowestTimingWheel = nullptr;

    /** ���Ȳ���
    */
    ISchedulingStrategyPtr m_spSchedulingStrategy = nullptr;

    /** ��Ӷ�ʱ������
    */
    std::mutex m_taskLock;

    /** ����Ķ�ʱ�����б�
    */
    using JoinTimerTaskMap = std::unordered_map<uint32_t, TimerTaskEntryPtr>;
    JoinTimerTaskMap m_joinTasks;

    /** ���ñ�ֽ�첽�̳߳�
    */
    AsyncThreadPool* m_asyncThreadPool = nullptr;

};

inline int64_t GetNowTimestamp()
{
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

#endif  // _TimingWheelScheduler_H_