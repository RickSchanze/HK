#include "Core/Time/TimeDuration.h"

// FTimeDuration实现

int64_t FTimeDuration::AsMilliseconds() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(MyDuration).count();
}

int64_t FTimeDuration::AsMicroseconds() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(MyDuration).count();
}

int64_t FTimeDuration::AsNanoseconds() const
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(MyDuration).count();
}

