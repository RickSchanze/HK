#pragma once

#include <chrono>
#include <ratio>
#include <type_traits>

// 定义固定的时钟类型
using FClock = std::chrono::system_clock;

// 时间单位类型别名
using FSeconds = std::chrono::seconds;
using FMilliseconds = std::chrono::milliseconds;
using FMicroseconds = std::chrono::microseconds;
using FNanoseconds = std::chrono::nanoseconds;
using FMinutes = std::chrono::minutes;
using FHours = std::chrono::hours;

// FTimeDuration封装类
class FTimeDuration
{
public:
    FTimeDuration() = default;
    explicit FTimeDuration(const FClock::duration& InDuration) : MyDuration(InDuration) {}

    // 获取标准库的duration
    const FClock::duration& GetStdDuration() const noexcept
    {
        return MyDuration;
    }

    // 模板方法：转换为指定类型和单位
    template <typename T, typename Unit>
    T As() const
    {
        using UnitDuration = std::chrono::duration<T, typename Unit::period>;
        return std::chrono::duration_cast<UnitDuration>(MyDuration).count();
    }

    // 模板方法：转换为浮点数（秒、毫秒等）- As<Double, Unit>的别名
    template <typename Unit>
    double AsDouble() const
    {
        return As<double, Unit>();
    }

    // 模板方法：转换为浮点数（秒、毫秒等）- As<Float, Unit>的别名
    template <typename Unit>
    float AsFloat() const
    {
        return As<float, Unit>();
    }

    // 转换为整数（毫秒、微秒、纳秒）
    int64_t AsMilliseconds() const;
    int64_t AsMicroseconds() const;
    int64_t AsNanoseconds() const;

private:
    FClock::duration MyDuration;
};
