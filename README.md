## TimingWheelScheduler：参考kafka，基于时间轮的定时任务调度实现；支持不同策略推动时钟运转

* 默认时间轮为4层级
    * 第一层为毫秒级时间轮 tickms = 10,wheelsize = 100
    * 第二层为秒级时间轮 tickms = 1000,wheelsize = 60
    * 第三层为分钟级时间轮 tickms = 60*1000,wheelsize = 60
    * 第四层为小时级时间轮 tickms = 60*60*1000,wheelsize = 24
* 默认推动时钟策略：以100ms为步长，推动时钟转动
* 自组建时间轮

## 编译
直接拉取，vs2017编译即可

## 注意点
* 使用了C++11特性
* 如果使用默认推动时钟策略，指定步长最好为最底层时钟的时间跨度

## 示例

```javascript
std::string MsTimeFormat() 
{
    auto tNow = std::chrono::system_clock::now();
    auto tSeconds = std::chrono::duration_cast<std::chrono::seconds>(tNow.time_since_epoch());
    auto secNow = tSeconds.count();

    std::tm tmNow;
    localtime_s(&tmNow, &secNow);
    std::ostringstream oss;
    oss << std::put_time(&tmNow, "%H:%M:%S");
    auto tMill = std::chrono::duration_cast<std::chrono::milliseconds>(tNow.time_since_epoch());
    auto ms = tMill - tSeconds;
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

int main()
{
    TimingWheelScheduler::GetInstance().CreateDefaultTimingWheelModule();
    TimingWheelScheduler::GetInstance().StartScheduling();

    TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(500, []() {
        std::cout << "间隔500毫秒后执行，不轮询" << std::endl;
    }, false);

    auto timer_id_5s = TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(5000, []() {
        std::cout << "间隔5秒后执行，轮询:" << MsTimeFormat() << std::endl;
    }, true);

    TimingWheelScheduler::GetInstance().AddExpiredAtTimer(TimingWheelScheduler::GetInstance().GetLowestTimingWheelTimeMs() + 10000, [=]() {
        std::cout << "+10秒后执行:" << MsTimeFormat() << std::endl;
    });

    auto timer_id_30s = TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(30 * 1000, []() {
        std::cout << "间隔30秒后执行，轮询:" << MsTimeFormat() << std::endl;
    }, true);

    auto timer_id_17s = TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(17 * 1000, []() {
        std::cout << "间隔17秒后执行，轮询:" << MsTimeFormat() << std::endl;
    }, true);

    auto timer_id_3m = TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(180 * 1000, []() {
        std::cout << "3分后执行----------，轮询:" << MsTimeFormat() << std::endl;
    }, true);

    auto timer_id_35m = TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(35 * 60 * 1000 + 4000, []() {
        std::cout << "35分4秒执行----------，轮询:" << MsTimeFormat() << std::endl;
    }, true);

    TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(62 * 60 * 1000, []() {
        std::cout << "1小时2分后执行-------------，轮询:" << MsTimeFormat() << std::endl;
    }, true);

    std::this_thread::sleep_for(std::chrono::minutes(20));

    TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(122 * 60 * 1000, []() {
        std::cout << "2小时2分后执行-------------，轮询:" << MsTimeFormat() << std::endl;
    }, true);

    std::this_thread::sleep_for(std::chrono::hours(3));

    TimingWheelScheduler::GetInstance().StopScheduling();

    return 0;
}
```
