#include "Core/Time/TimePoint.h"
#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

// FTimePoint实现
FTimePoint FTimePoint::Now()
{
    return FTimePoint(FClock::now());
}

double FTimePoint::ToSeconds() const
{
    auto Duration = MyTimePoint.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::duration<double>>(Duration).count();
}

int64_t FTimePoint::ToMilliseconds() const
{
    auto Duration = MyTimePoint.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(Duration).count();
}

int64_t FTimePoint::ToMicroseconds() const
{
    auto Duration = MyTimePoint.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(Duration).count();
}

int64_t FTimePoint::ToNanoseconds() const
{
    auto Duration = MyTimePoint.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(Duration).count();
}

FTimeDuration FTimePoint::operator-(const FTimePoint& Other) const
{
    return FTimeDuration(MyTimePoint - Other.MyTimePoint);
}

std::string FTimePoint::Format(const std::string& Format) const
{
    auto TimeT = FClock::to_time_t(MyTimePoint);
    
#ifdef _WIN32
    struct tm LocalTime;
    localtime_s(&LocalTime, &TimeT);
#else
    struct tm LocalTime;
    localtime_r(&TimeT, &LocalTime);
#endif

    std::ostringstream Oss;
    Oss << std::put_time(&LocalTime, Format.c_str());

    // 添加毫秒部分
    auto Ms = std::chrono::duration_cast<std::chrono::milliseconds>(MyTimePoint.time_since_epoch()) % 1000;
    Oss << '.' << std::setfill('0') << std::setw(3) << Ms.count();

    return Oss.str();
}

std::string FTimePoint::FormatISO8601() const
{
    return Format("%Y-%m-%dT%H:%M:%S");
}

