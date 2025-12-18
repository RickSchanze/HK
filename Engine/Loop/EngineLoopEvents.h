#pragma once
#include "Core/Event/Event.h"

struct FEngineLoopEvents
{
    TEvent<> OnPreTick;
    TEvent<> OnTick;
    TEvent<> OnPostTick;
};

inline FEngineLoopEvents GEngineLoopEvents;

inline FEngineLoopEvents& GetEngineLoopEvents()
{
    return GEngineLoopEvents;
}
