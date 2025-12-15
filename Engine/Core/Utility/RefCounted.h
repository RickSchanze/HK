#pragma once

#include "Core/Utility/Macros.h"
#include <atomic>
#include <type_traits>

class FRefCounted
{
public:
    FRefCounted() : RefCount(1) {}

    virtual ~FRefCounted() = default;

    // 禁止拷贝和移动
    FRefCounted(const FRefCounted&) = delete;
    FRefCounted& operator=(const FRefCounted&) = delete;
    FRefCounted(FRefCounted&&) = delete;
    FRefCounted& operator=(FRefCounted&&) = delete;

    // 增加引用计数
    void AddRef() const
    {
        RefCount.fetch_add(1, std::memory_order_relaxed);
    }

    // 减少引用计数，返回是否应该释放
    // 当返回 true 时，调用者应该删除对象：if (obj->ReleaseRef()) { delete obj; }
    bool ReleaseRef() const
    {
        const Int32 OldCount = RefCount.fetch_sub(1, std::memory_order_acq_rel);
        HK_ASSERT_RAW(OldCount > 0);
        return OldCount == 1;
    }

    // 获取当前引用计数
    Int32 GetRefCount() const
    {
        return RefCount.load(std::memory_order_acquire);
    }

    // 检查是否唯一引用
    bool IsUnique() const
    {
        return GetRefCount() == 1;
    }

private:
    mutable std::atomic<Int32> RefCount;
};

// 辅助函数：安全释放 RefCounted 对象
template <typename T>
    requires std::is_base_of_v<FRefCounted, T>
void SafeReleaseRef(T*& Obj)
{
    if (Obj != nullptr)
    {
        if (Obj->ReleaseRef())
        {
            delete Obj;
        }
        Obj = nullptr;
    }
}
