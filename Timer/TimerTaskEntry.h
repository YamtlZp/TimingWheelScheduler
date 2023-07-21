#include <functional>
#include <memory>
#include <list>
#include <atomic>

/** ��ʱ����
*/
using TimerTask = std::function<void()>;

/** ��ʱ������
*/
class TimerTaskEntry
{
public:

    /** ���캯��
    @param [in] taskId ȫ������id @note:ʱ���ֵĶ�ʱ��������ִ��˳���ͣ��޷�ʹ��ʱ������ΪΨһ�����ʶ���ʲ���ȫ������id��ΪΨһ�����ʶ
    @param [in] expiraMs ������ʱ��
    @param [in] intervalMs ������ѯ���ʱ��
    @param [in] task ����ִ���壨�ص�������
    */
    TimerTaskEntry(uint32_t taskId, int64_t expiraMs, uint32_t intervalMs, const TimerTask& task);

    /** ��������
    */
    ~TimerTaskEntry();

    /** ��ʱ����ִ��
    */
    void ExcuteTask();

    /** ȡ����ʱ����
    */
    void CancelTask();

public:

    /** �жϸ������Ƿ��ѱ�ȡ��
    */
    bool IsCanceled();

    /** ��ȡ��ʱ�������ʱ��
    */
    int64_t GetExpireMs() const;

    /** �жϸö�ʱ�����Ƿ���Ҫ��ѯ
    */
    bool IsTaskPolling() const;

    /** ���¶�ʱ�������ʱ��
    */
    void UpdateExpireMs();

    /** ������ѯ�Ķ�ʱ������ʱ��
    @param [in] intervalMs ������ѯ�����ʱ����
    @return �����Ƿ�ɹ�
    */
    bool ResetPollingTaskInterval(uint32_t intervalMs);

    /** ��ȡ����id
    */
    uint32_t GetTaskId() const;

private:

    /** Ψһ�����ʶ
    */
    uint32_t m_taskId;

    /** ����ִ�й���ʱ��
    */
    std::atomic_int64_t m_expiraMs;

    /** ����ִ�м��ʱ��
    */
    uint32_t m_intervalMs;

    /** �Ƿ���Ҫ��ѯ
    */
    bool m_isPolling;

    /** ��ʱ����
    */
    TimerTask m_timerTask;

    /** �����Ƿ��ѱ�ȡ��
    */
    bool m_isTaskCanceld;
};

using TimerTaskEntryPtr = std::shared_ptr<TimerTaskEntry>;