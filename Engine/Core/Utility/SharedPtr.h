#pragma once

#include "Core/Logging/Logger.h"
#include "Core/Reflection/TypeManager.h"
#include "Core/String/Name.h"
#include "Core/Utility/Profiler.h"

#include <memory>
#include <type_traits>

// 无状态的删除器，使用 Delete 进行内存跟踪
// 空类，零开销，内存最小化
template <typename T>
struct TDefaultSharedPtrDeleter
{
    constexpr TDefaultSharedPtrDeleter() noexcept = default;

    template <typename U>
    constexpr TDefaultSharedPtrDeleter(const TDefaultSharedPtrDeleter<U>&) noexcept
    {
        static_assert(std::is_convertible_v<U*, T*>, "U* must be convertible to T*");
    }

    void operator()(T* Ptr) const noexcept
    {
        Delete(Ptr);
    }
};

template <typename T>
class TSharedPtr
{
public:
    using ElementType    = T;
    using Pointer        = T*;
    using ConstPointer   = const T*;
    using Reference      = T&;
    using ConstReference = const T&;

    // 默认构造
    TSharedPtr() noexcept : MyPtr() {}

    // 从 nullptr 构造
    TSharedPtr(std::nullptr_t) noexcept : MyPtr() {}

    // 从 std::shared_ptr 构造
    explicit TSharedPtr(std::shared_ptr<T> InPtr) : MyPtr(InPtr) {}

    // 拷贝构造
    TSharedPtr(const TSharedPtr& Other) = default;

    // 移动构造
    TSharedPtr(TSharedPtr&& Other) noexcept = default;

    // 从派生类构造
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TSharedPtr(const TSharedPtr<U>& Other) : MyPtr(Other.MyPtr)
    {
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TSharedPtr(TSharedPtr<U>&& Other) noexcept : MyPtr(std::move(Other.MyPtr))
    {
    }

    // 析构
    ~TSharedPtr() = default;

    // 拷贝赋值
    TSharedPtr& operator=(const TSharedPtr& Other) = default;

    // 移动赋值
    TSharedPtr& operator=(TSharedPtr&& Other) noexcept = default;

    // 从 nullptr 赋值
    TSharedPtr& operator=(std::nullptr_t) noexcept
    {
        MyPtr.reset();
        return *this;
    }

    // 从派生类赋值
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TSharedPtr& operator=(const TSharedPtr<U>& Other)
    {
        MyPtr = Other.MyPtr;
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TSharedPtr& operator=(TSharedPtr<U>&& Other) noexcept
    {
        MyPtr = std::move(Other.MyPtr);
        return *this;
    }

    // 重置
    void Reset() noexcept
    {
        MyPtr.reset();
    }

    template <typename U>
    void Reset(U* InPtr)
    {
        // 使用默认删除器（无状态，零开销）
        MyPtr.reset(InPtr, TDefaultSharedPtrDeleter<U>());
    }

    // 交换
    void Swap(TSharedPtr& Other) noexcept
    {
        MyPtr.swap(Other.MyPtr);
    }

    // 获取原始指针
    Pointer Get() noexcept
    {
        return MyPtr.get();
    }

    ConstPointer Get() const noexcept
    {
        return MyPtr.get();
    }

    // 解引用
    Reference operator*() noexcept
    {
        return *MyPtr;
    }

    ConstReference operator*() const noexcept
    {
        return *MyPtr;
    }

    // 成员访问
    Pointer operator->() noexcept
    {
        return MyPtr.get();
    }

    ConstPointer operator->() const noexcept
    {
        return MyPtr.get();
    }

    // 布尔转换
    explicit operator bool() const noexcept
    {
        return MyPtr != nullptr;
    }

    // 获取引用计数
    long UseCount() const noexcept
    {
        return MyPtr.use_count();
    }

    // 检查是否唯一
    bool IsUnique() const noexcept
    {
        return MyPtr.unique();
    }

    // 获取底层 std::shared_ptr
    std::shared_ptr<T> GetSharedPtr() const noexcept
    {
        return MyPtr;
    }

    // 比较操作符
    bool operator==(const TSharedPtr& Other) const noexcept
    {
        return MyPtr == Other.MyPtr;
    }

    bool operator!=(const TSharedPtr& Other) const noexcept
    {
        return MyPtr != Other.MyPtr;
    }

    bool operator==(std::nullptr_t) const noexcept
    {
        return MyPtr == nullptr;
    }

    bool operator!=(std::nullptr_t) const noexcept
    {
        return MyPtr != nullptr;
    }

    bool operator<(const TSharedPtr& Other) const noexcept
    {
        return MyPtr < Other.MyPtr;
    }

    template <typename Archive>
    void Read(Archive& Ar)
    {
        FName TypeName;
        Ar(MakeNamedPair("TypeName", TypeName));

        if (TypeName != Names::None)
        {
            FType Type = FTypeManager::Get().FindTypeByName(TypeName);
            if (!Type)
            {
                HK_LOG_ERROR(ELogcat::Serialize, "Can't find type {}", TypeName);
                MyPtr.reset();
            }
            else
            {
                T* Ptr = static_cast<T*>(Type->CreateInstance());
                MyPtr.reset(Ptr);
                Ar(MakeNamedPair("Data", *MyPtr));
            }
        }
        else
        {
            MyPtr.reset();
        }
    }

    template <typename Archive>
    void Write(Archive& Ar)
    {
        if (MyPtr)
        {
            // 获取对象的实际类型名称
            Ar(MakeNamedPair("TypeName", MyPtr->GetType()->Name));
            Ar(MakeNamedPair("Data", *MyPtr));
        }
        else
        {
            Ar(MakeNamedPair("TypeName", Names::None));
        }
    }

private:
    std::shared_ptr<T> MyPtr;

    template <typename U>
    friend class TSharedPtr;
};

// 1. 定义一个适配 Profiler 的分配器
template <typename T>
struct TProfilerAllocator
{
    using value_type = T;

    TProfilerAllocator() = default;
    template <typename U>
    TProfilerAllocator(const TProfilerAllocator<U>&)
    {
    }

    T* allocate(std::size_t n)
    {
        // 使用你的 Profiler 跟踪分配
        // 注意：这里分配的是原始字节，shared_ptr 会在上面构造控制块和对象
        return static_cast<T*>(Malloc(n * sizeof(T)));
    }

    void deallocate(T* p, [[maybe_unused]] std::size_t n)
    {
        Free(p);
    }
};

// MakeShared 函数 - 使用 New/Delete 进行内存跟踪
// 使用默认删除器 TDefaultDelete，无状态，零开销
template <typename T, typename... Args>
TSharedPtr<T> MakeShared(Args&&... InArgs)
{
    // allocate_shared 会分配一块足够容纳 [控制块 + T对象] 的连续内存
    // 并且会调用 TProfilerAllocator 进行分配
    auto StdPtr = std::allocate_shared<T>(TProfilerAllocator<T>(), std::forward<Args>(InArgs)...);
    return TSharedPtr<T>(StdPtr);
}

// 从原始指针创建（使用自定义删除器）
template <typename T, typename Deleter>
TSharedPtr<T> MakeShared(T* InPtr, Deleter InDeleter)
{
    return TSharedPtr<T>(std::shared_ptr<T>(InPtr, InDeleter));
}

// MakeSharedWithDeleter - 使用自定义删除器创建对象
// 第一个参数为 Deleter，后面是可变参数用于构造对象
template <typename T, typename Deleter, typename... Args>
TSharedPtr<T> MakeSharedWithDeleter(Deleter InDeleter, Args&&... InArgs)
{
    // 使用 New 分配内存（会加入 Profiler 跟踪）
    T* Ptr = New<T>(std::forward<Args>(InArgs)...);
    // 使用自定义删除器
    return TSharedPtr<T>(std::shared_ptr<T>(Ptr, InDeleter));
}

// DynamicPointerCast - 类型安全的动态转换
template <typename T, typename U>
TSharedPtr<T> DynamicPointerCast(const TSharedPtr<U>& InPtr)
{
    return TSharedPtr<T>(std::dynamic_pointer_cast<T>(InPtr.GetSharedPtr()));
}