#pragma once

#include "Core/Time/TimeDuration.h"
#include <chrono>
#include <string>

// 定义固定的时钟类型
using FClock = std::chrono::system_clock;

// FTimePoint封装类
class FTimePoint
{
public:
    FTimePoint() = default;
    explicit FTimePoint(const FClock::time_point& InTimePoint) : MyTimePoint(InTimePoint) {}

    // 获取标准库的time_point
    const FClock::time_point& GetStdTimePoint() const noexcept
    {
        return MyTimePoint;
    }

    // 重载-运算符，返回FTimeDuration
    FTimeDuration operator-(const FTimePoint& Other) const;

    // 静态方法：获取当前时间点
    static FTimePoint Now();

    // 转换为时间戳
    double ToSeconds() const;
    int64_t ToMilliseconds() const;
    int64_t ToMicroseconds() const;
    int64_t ToNanoseconds() const;

    // 格式化时间点
    std::string Format(const std::string& Format = "%Y-%m-%d %H:%M:%S") const;
    std::string FormatISO8601() const;

private:
    FClock::time_point MyTimePoint;
};

// 便利函数：获取当前时间点
inline FTimePoint Now()
{
    return FTimePoint::Now();
}

// 便利函数：计算两个时间点的差值
inline FTimeDuration TimeDifference(const FTimePoint& Start, const FTimePoint& End)
{
    return End - Start;
}

