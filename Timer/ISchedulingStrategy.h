#ifndef _ISchedulingStrategy_H_
#define _ISchedulingStrategy_H_ 1

#include <memory>

class TimingWheelScheduler;
/** 调度策略接口
*/
class ISchedulingStrategy
{
public:

    /** 虚析构
    */
    virtual ~ISchedulingStrategy() {}

    /** 设置策略的执行者
    */
    virtual bool SetSchedulingStrategyExecutor(TimingWheelScheduler* executor) = 0;

    /** 开始调度
    */
    virtual bool StartSchedule() = 0;

    /** 停止调度
    */
    virtual void StopSchedule() = 0;
};

using ISchedulingStrategyPtr = std::shared_ptr<ISchedulingStrategy>;

#endif  // _ISchedulingStrategy_H_