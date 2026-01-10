#include "Profiler.h"

void Profiler::Start()
{
    #if PROFILING_ACTIVE
    m_startPoint = std::chrono::high_resolution_clock::now();
    #endif
}

///@param granularity Determines how often the output is printed, granularity = 100 means that the average of the last 100 calls is outputted
void Profiler::Stop(int granularity)
{
    #if PROFILING_ACTIVE
    static int callCount = 0;
    static long sum = 0;

    auto endPoint = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startPoint).time_since_epoch().count();
    auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();
    auto duration = end - start;

    sum += duration;

    if(++callCount == granularity){
        long result = sum / granularity;
        std::cout << "time elapsed (avg after " << granularity << " calls):" << result << "us" << " | " << result / 1000 << "ms" << std::endl;
        sum = 0;
        callCount = 0;
    }
    #endif
}

///@param granularity Determines how often the output is printed, granularity = 100 means that the average of the last 100 calls is outputted
void Profiler::Stop(int granularity, const std::string msg)
{
    #if PROFILING_ACTIVE
    static int callCount = 0;
    static long sum = 0;

    auto endPoint = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startPoint).time_since_epoch().count();
    auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();
    auto duration = end - start;

    if(++callCount == granularity){
        long result = sum / granularity;
        std::cout << msg <<  " time elapsed (avg after " << granularity << " calls):" << result << "us" << " | " << result / 1000 << "ms" << std::endl;
        sum = 0;
        result = 0;
    }
    #endif
}