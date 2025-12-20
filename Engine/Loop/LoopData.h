#pragma once
#include "Core/Time/TimeDuration.h"
#include "Core/Time/TimePoint.h"

class FLoopData
{
public:
    /**
      * 一帧时间
      */
    FTimeDuration DeltaTime;
    /**
      * 上一帧 时间
      */
    FTimePoint LastFrameTime;
    /**
      * 帧号
      */
    UInt64 FrameNumber;
    /**
      * 是否应该关闭引擎了
      */
    bool bShouldCloseEngine;
};

inline FLoopData GLoopData;

inline const FLoopData& GetEngineLoopData()
{
    return GLoopData;
}
