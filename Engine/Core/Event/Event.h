#pragma once

#include "Core/Container/Array.h"
#include <functional>

template <typename... Args>
class TEvent
{
public:
    using Handle = size_t;
    using FunctionType = std::function<void(Args...)>;

    TEvent() = default;
    ~TEvent() = default;

    // 绑定lambda或函数对象
    template <typename Functor>
    Handle AddBind(Functor&& Func)
    {
        Handle NewHandle = NextHandle++;
        MyFunctions.Add({NewHandle, std::forward<Functor>(Func)});
        return NewHandle;
    }

    // 绑定函数指针或静态成员函数
    Handle AddBind(void (*Func)(Args...))
    {
        Handle NewHandle = NextHandle++;
        MyFunctions.Add({NewHandle, Func});
        return NewHandle;
    }

    // 绑定成员函数
    template <typename ClassType>
    Handle AddBind(ClassType* Object, void (ClassType::*MemberFunc)(Args...))
    {
        Handle NewHandle = NextHandle++;
        MyFunctions.Add({NewHandle, [Object, MemberFunc](Args... args) { (Object->*MemberFunc)(args...); }});
        return NewHandle;
    }

    // 绑定const成员函数
    template <typename ClassType>
    Handle AddBind(ClassType* Object, void (ClassType::*MemberFunc)(Args...) const)
    {
        Handle NewHandle = NextHandle++;
        MyFunctions.Add({NewHandle, [Object, MemberFunc](Args... args) { (Object->*MemberFunc)(args...); }});
        return NewHandle;
    }

    // 移除绑定
    bool RemoveBind(Handle InHandle)
    {
        for (size_t i = 0; i < MyFunctions.Size(); ++i)
        {
            if (MyFunctions[i].Handle == InHandle)
            {
                MyFunctions.RemoveAt(i);
                return true;
            }
        }
        return false;
    }

    // 清除所有绑定
    void Clear()
    {
        MyFunctions.Clear();
    }

    // 调用所有绑定的函数
    void Invoke(Args... args) const
    {
        for (const auto& Pair : MyFunctions)
        {
            if (Pair.Function)
            {
                Pair.Function(args...);
            }
        }
    }

    // 调用所有绑定的函数（操作符重载）
    void operator()(Args... args) const
    {
        Invoke(args...);
    }

    // 检查是否有绑定
    bool IsBound() const
    {
        return MyFunctions.Size() > 0;
    }

    // 获取绑定数量
    size_t GetBindCount() const
    {
        return MyFunctions.Size();
    }

private:
    struct FunctionPair
    {
        Handle Handle;
        FunctionType Function;
    };

    TArray<FunctionPair> MyFunctions;
    Handle NextHandle = 1; // 从1开始，0作为无效句柄
};