#pragma once
#include <chrono>
#include <iostream>
#include <string>

#define PROFILING_ACTIVE 0

class Profiler
{
private:
    Profiler() = delete;
    static inline std::chrono::time_point<std::chrono::high_resolution_clock> m_startPoint;
public:
    static void Start();
    static void Stop(int granularity);
    static void Stop(int granularity, const std::string msg);
};
