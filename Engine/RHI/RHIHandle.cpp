//
// Created by Admin on 2025/12/15.
//

#include "RHIHandle.h"
#include "Core/Logging/Logger.h"
#include "Core/String/String.h"

Int32 FRHIHandleManager::FindFreeIndex() const
{
    std::lock_guard<std::mutex> Lock(Mutex);
    for (Int32 i = 1; i < MAX_RHI_HANDLE_COUNT; ++i)
    {
        if (!Occupied[i])
        {
            return i;
        }
    }
    return FRHIHandle::INVALID_ID;
}

FRHIHandle FRHIHandleManager::CreateRHIHandle(FStringView DebugName, FRHIHandle::FHandleType Handle)
{
    Int32 Index = FindFreeIndex();
    HK_ASSERT_MSG_RAW(Index != FRHIHandle::INVALID_ID, "Failed to find free index for RHI handle");
    Occupied[Index] = true;
    RHIHandles[Index].Id = Index;
    RHIHandles[Index].Handle = Handle;
#if HK_DEBUG
    RHIHandles[Index].DebugName = FString(DebugName);
#endif
    return RHIHandles[Index];
}

void FRHIHandleManager::DestroyRHIHandle(const FRHIHandle& Handle)
{
    std::lock_guard<std::mutex> Lock(Mutex);
    HK_ASSERT_MSG_RAW(Handle.IsValid(), "RHI handle is invalid");
    Occupied[Handle.Id] = false;
    RHIHandles[Handle.Id].Id = FRHIHandle::INVALID_ID;
    RHIHandles[Handle.Id].Handle = nullptr;
#if HK_DEBUG
    RHIHandles[Handle.Id].DebugName = FString();
#endif
}

void FRHIHandleManager::Shutdown()
{
    std::lock_guard<std::mutex> Lock(Mutex);

    Int32 LeakedCount = 0;
    for (Int32 i = 1; i < MAX_RHI_HANDLE_COUNT; ++i)
    {
        if (Occupied[i])
        {
            LeakedCount++;
#if HK_DEBUG
            HK_LOG_WARN(ELogcat::Engine, "Leaked RHI Handle: ID={}, Name={}", RHIHandles[i].Id,
                        RHIHandles[i].DebugName.IsEmpty() ? "<Unnamed>" : RHIHandles[i].DebugName.CStr());
#else
            HK_LOG_WARN(ELogcat::Engine, "Leaked RHI Handle: ID={}", RHIHandles[i].Id);
#endif
        }
    }

    if (LeakedCount > 0)
    {
        HK_LOG_WARN(ELogcat::Engine, "Found {} leaked RHI Handle(s) during shutdown", LeakedCount);
    }
    else
    {
        HK_LOG_INFO(ELogcat::Engine, "All RHI Handles properly destroyed");
    }
}
