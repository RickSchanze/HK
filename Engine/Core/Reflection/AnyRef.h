#pragma once

#include "Core/Reflection/ReflectionFwd.h"
#include "Core/Utility/Macros.h"
#include <cstddef>

// 前向声明TypeOf
template <typename T>
FType TypeOf();

class FAnyRef
{
public:
    FAnyRef() : MyType(nullptr), MyData(nullptr) {}

    template <typename T>
    explicit FAnyRef(T& InValue) : MyType(TypeOf<T>()), MyData(&InValue)
    {
    }

    FAnyRef(const FAnyRef& Other) = default;
    FAnyRef& operator=(const FAnyRef& Other) = default;

    FType GetType() const
    {
        return MyType;
    }

    bool IsValid() const
    {
        return MyType != nullptr && MyData != nullptr;
    }

    bool IsEmpty() const
    {
        return MyType == nullptr || MyData == nullptr;
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

    void Reset()
    {
        MyType = nullptr;
        MyData = nullptr;
    }

private:
    FType MyType;
    void* MyData;
};
