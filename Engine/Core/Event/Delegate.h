#pragma once

#include <functional>
#include <type_traits>

template <typename ReturnType, typename... Args>
class TDelegate
{
public:
    using FunctionType = std::function<ReturnType(Args...)>;

    TDelegate() = default;
    ~TDelegate() = default;

    // 绑定lambda或函数对象
    template <typename Functor>
    void Bind(Functor&& Func)
    {
        MyFunction = std::forward<Functor>(Func);
    }

    // 绑定函数指针或静态成员函数
    void Bind(ReturnType (*Func)(Args...))
    {
        MyFunction = Func;
    }

    // 绑定成员函数
    template <typename ClassType>
    void Bind(ClassType* Object, ReturnType (ClassType::*MemberFunc)(Args...))
    {
        MyFunction = [Object, MemberFunc](Args... args) -> ReturnType { return (Object->*MemberFunc)(args...); };
    }

    // 绑定const成员函数
    template <typename ClassType>
    void Bind(ClassType* Object, ReturnType (ClassType::*MemberFunc)(Args...) const)
    {
        MyFunction = [Object, MemberFunc](Args... args) -> ReturnType { return (Object->*MemberFunc)(args...); };
    }

    // 调用委托
    ReturnType Invoke(Args... args) const
    {
        if (!MyFunction)
        {
            if constexpr (std::is_same_v<ReturnType, void>)
            {
                return;
            }
            else
            {
                return ReturnType{};
            }
        }
        return MyFunction(args...);
    }

    // 调用委托（操作符重载）
    ReturnType operator()(Args... args) const
    {
        return Invoke(args...);
    }

    // 清除绑定
    void Clear()
    {
        MyFunction = nullptr;
    }

    // 检查是否已绑定
    bool IsBound() const
    {
        return MyFunction != nullptr;
    }

private:
    FunctionType MyFunction;
};