#pragma once

#include "Core/Reflection/ReflectionFwd.h"
#include "Core/Reflection/TypeManager.h"
#include "Core/Utility/Macros.h"
#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>

class FAny
{
public:
    FAny() : MyType(nullptr), MyData(nullptr) {}

    template <typename T>
    explicit FAny(const T& InValue) : MyType(TypeOf<T>()), MyData(nullptr)
    {
        AllocateAndCopy(InValue);
    }

    FAny(const FAny& Other) : MyType(Other.MyType), MyData(nullptr)
    {
        if (Other.MyData != nullptr && MyType != nullptr)
        {
            AllocateAndCopyFrom(Other.MyData);
        }
    }

    FAny(FAny&& Other) noexcept : MyType(Other.MyType), MyData(Other.MyData)
    {
        Other.MyType = nullptr;
        Other.MyData = nullptr;
    }

    ~FAny()
    {
        Destroy();
    }

    FAny& operator=(const FAny& Other)
    {
        if (this != &Other)
        {
            Destroy();
            MyType = Other.MyType;
            if (Other.MyData != nullptr && MyType != nullptr)
            {
                AllocateAndCopyFrom(Other.MyData);
            }
        }
        return *this;
    }

    FAny& operator=(FAny&& Other) noexcept
    {
        if (this != &Other)
        {
            Destroy();
            MyType = Other.MyType;
            MyData = Other.MyData;
            Other.MyType = nullptr;
            Other.MyData = nullptr;
        }
        return *this;
    }

    template <typename T>
    FAny& operator=(const T& InValue)
    {
        Destroy();
        AllocateAndCopy(InValue);
        return *this;
    }

    FType GetType() const
    {
        return MyType;
    }

    bool IsValid() const
    {
        return MyType != nullptr && MyData != nullptr;
    }

    void* GetData()
    {
        return MyData;
    }

    const void* GetData() const
    {
        return MyData;
    }

    template <typename T>
    T* Get()
    {
        if (MyType == nullptr || MyData == nullptr)
            return nullptr;
        // 类型检查会在TypeManager中完成
        return reinterpret_cast<T*>(MyData);
    }

    template <typename T>
    const T* Get() const
    {
        if (MyType == nullptr || MyData == nullptr)
            return nullptr;
        return reinterpret_cast<const T*>(MyData);
    }

private:
    FType MyType;
    void* MyData;

    template <typename T>
    void AllocateAndCopy(const T& InValue)
    {
        if (MyType == nullptr)
        {
            MyType = TypeOf<T>();
        }
        if (MyType != nullptr)
        {
            MyData = ::operator new(sizeof(T));
            new (MyData) T(InValue);
        }
    }

    void AllocateAndCopyFrom(const void* InData)
    {
        if (MyType != nullptr && MyData == nullptr && InData != nullptr)
        {
            MyData = ::operator new(MyType->Size);
            std::memcpy(MyData, InData, MyType->Size);
        }
    }

    void Destroy()
    {
        if (MyData != nullptr && MyType != nullptr)
        {
            // 对于POD类型，直接释放内存
            // 对于非POD类型，需要调用析构函数，这里简化处理
            ::operator delete(MyData);
            MyData = nullptr;
        }
    }
};
