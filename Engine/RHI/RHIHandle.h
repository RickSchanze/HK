#pragma once
#include "Core/Container/FixedArray.h"
#include "Core/Singleton/Singleton.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"

#include <mutex>

#define HK_LOG_RHI_RESOURCE 1

enum class ERHIResourceType
{
    Image,
    ImageView,
    Buffer,
    Sampler,
    Pipeline,
    PipelineLayout,
    DescriptorSet,
    DescriptorSetLayout,
    DescriptorPool,
    ShaderModule,
    RenderPass,
    Framebuffer,
    CommandPool,
    CommandBuffer,
    Fence,
    Semaphore,
    Event,
    QueryPool,
};

struct FRHIHandle
{
    typedef Int32 FID;
    typedef void* FHandleType;
    constexpr static FID INVALID_ID = 0;

    FID Id = INVALID_ID;
    FHandleType Handle = nullptr;

#if HK_DEBUG
    FString DebugName;
#endif

    UInt64 GetHashCode() const
    {
        return std::hash<FID>()(Id);
    }

    operator bool() const
    {
        return IsValid();
    }

    bool IsValid() const
    {
        return Id != INVALID_ID && Handle != nullptr;
    }

    template <typename T>
    T Cast()
    {
        return reinterpret_cast<T>(Handle);
    }

    bool operator==(const FRHIHandle& Other) const
    {
        return Id == Other.Id;
    }
};

class FRHIHandleManager : public FSingleton<FRHIHandleManager>
{
public:
    constexpr static Int32 MAX_RHI_HANDLE_COUNT = 2048;

    Int32 FindFreeIndex() const;

    FRHIHandle CreateRHIHandle(FStringView DebugName, FRHIHandle::FHandleType Handle);
    void DestroyRHIHandle(const FRHIHandle& Handle);

    // 关闭时记录未被销毁的 Handle
    void ShutDown() override;

private:
    TFixedArray<FRHIHandle, MAX_RHI_HANDLE_COUNT> RHIHandles;
    TFixedArray<Bool, MAX_RHI_HANDLE_COUNT> Occupied;
    mutable std::mutex Mutex;
};
