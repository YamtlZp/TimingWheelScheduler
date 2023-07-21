// Timer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "TimingWheelScheduler.h"

#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>

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
        std::cout << "���500�����ִ�У�����ѯ" << std::endl;
    }, false);

    auto timer_id_5s = TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(5000, []() {
        std::cout << "���5���ִ�У���ѯ:" << MsTimeFormat() << std::endl;
    }, true);

    TimingWheelScheduler::GetInstance().AddExpiredAtTimer(TimingWheelScheduler::GetInstance().GetLowestTimingWheelTimeMs() + 10000, [=]() {
        std::cout << "+10���ִ��:" << MsTimeFormat() << std::endl;
    });

    auto timer_id_30s = TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(30 * 1000, []() {
        std::cout << "���30���ִ�У���ѯ:" << MsTimeFormat() << std::endl;
    }, true);

    auto timer_id_17s = TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(17 * 1000, []() {
        std::cout << "���17���ִ�У���ѯ:" << MsTimeFormat() << std::endl;
    }, true);

    auto timer_id_3m = TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(180 * 1000, []() {
        std::cout << "3�ֺ�ִ��----------����ѯ:" << MsTimeFormat() << std::endl;
    }, true);

    auto timer_id_35m = TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(35 * 60 * 1000 + 4000, []() {
        std::cout << "35��4��ִ��----------����ѯ:" << MsTimeFormat() << std::endl;
    }, true);

    TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(62 * 60 * 1000, []() {
        std::cout << "1Сʱ2�ֺ�ִ��-------------����ѯ:" << MsTimeFormat() << std::endl;
    }, true);

    std::this_thread::sleep_for(std::chrono::minutes(20));

    TimingWheelScheduler::GetInstance().AddExpiredIntervalTimer(122 * 60 * 1000, []() {
        std::cout << "2Сʱ2�ֺ�ִ��-------------����ѯ:" << MsTimeFormat() << std::endl;
    }, true);

    std::this_thread::sleep_for(std::chrono::hours(3));

    TimingWheelScheduler::GetInstance().StopScheduling();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
