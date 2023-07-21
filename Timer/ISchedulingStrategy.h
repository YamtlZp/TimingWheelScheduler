#ifndef _ISchedulingStrategy_H_
#define _ISchedulingStrategy_H_ 1

#include <memory>

class TimingWheelScheduler;
/** ���Ȳ��Խӿ�
*/
class ISchedulingStrategy
{
public:

    /** ������
    */
    virtual ~ISchedulingStrategy() {}

    /** ���ò��Ե�ִ����
    */
    virtual bool SetSchedulingStrategyExecutor(TimingWheelScheduler* executor) = 0;

    /** ��ʼ����
    */
    virtual bool StartSchedule() = 0;

    /** ֹͣ����
    */
    virtual void StopSchedule() = 0;
};

using ISchedulingStrategyPtr = std::shared_ptr<ISchedulingStrategy>;

#endif  // _ISchedulingStrategy_H_