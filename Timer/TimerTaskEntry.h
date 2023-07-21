#include <functional>
#include <memory>
#include <list>
#include <atomic>

/** 定时任务
*/
using TimerTask = std::function<void()>;

/** 定时任务项
*/
class TimerTaskEntry
{
public:

    /** 构造函数
    @param [in] taskId 全局任务id @note:时间轮的定时任务属于执行顺序型，无法使用时间来作为唯一任务标识，故采用全局任务id作为唯一任务标识
    @param [in] expiraMs 任务到期时间
    @param [in] intervalMs 任务轮询间隔时间
    @param [in] task 任务执行体（回调函数）
    */
    TimerTaskEntry(uint32_t taskId, int64_t expiraMs, uint32_t intervalMs, const TimerTask& task);

    /** 析构函数
    */
    ~TimerTaskEntry();

    /** 定时任务执行
    */
    void ExcuteTask();

    /** 取消定时任务
    */
    void CancelTask();

public:

    /** 判断该任务是否已被取消
    */
    bool IsCanceled();

    /** 获取定时任务过期时间
    */
    int64_t GetExpireMs() const;

    /** 判断该定时任务是否需要轮询
    */
    bool IsTaskPolling() const;

    /** 更新定时任务过期时间
    */
    void UpdateExpireMs();

    /** 重置轮询的定时任务间隔时间
    @param [in] intervalMs 重置轮询任务的时间间隔
    @return 重置是否成功
    */
    bool ResetPollingTaskInterval(uint32_t intervalMs);

    /** 获取任务id
    */
    uint32_t GetTaskId() const;

private:

    /** 唯一任务标识
    */
    uint32_t m_taskId;

    /** 任务执行过期时间
    */
    std::atomic_int64_t m_expiraMs;

    /** 任务执行间隔时间
    */
    uint32_t m_intervalMs;

    /** 是否需要轮询
    */
    bool m_isPolling;

    /** 定时任务
    */
    TimerTask m_timerTask;

    /** 任务是否已被取消
    */
    bool m_isTaskCanceld;
};

using TimerTaskEntryPtr = std::shared_ptr<TimerTaskEntry>;