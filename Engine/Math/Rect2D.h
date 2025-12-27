#pragma once
#include "Core/Reflection/Reflection.h"
#include "Core/Utility/Macros.h"
#include "Vector.h"
#include <glm/gtc/epsilon.hpp>

/////////////////////////////////////////////////////////////////////////////////
// TRect2D
/////////////////////////////////////////////////////////////////////////////////
template <typename TPos, typename TSize>
struct TRect2D
{
    TPos  X;
    TPos  Y;
    TSize Width;
    TSize Height;

    // 构造函数
    TRect2D() : X(0), Y(0), Width(0), Height(0) {}
    TRect2D(TPos InX, TPos InY, TSize InWidth, TSize InHeight) : X(InX), Y(InY), Width(InWidth), Height(InHeight) {}
    TRect2D(const TVector2<TPos>& Position, const TVector2<TSize>& Size)
        : X(Position.X), Y(Position.Y), Width(Size.X), Height(Size.Y)
    {
    }

    // 拷贝构造函数和赋值操作符
    TRect2D(const TRect2D& Other)            = default;
    TRect2D& operator=(const TRect2D& Other) = default;

    // 比较操作符
    bool operator==(const TRect2D& Other) const
    {
        if constexpr (std::is_floating_point_v<TPos> || std::is_floating_point_v<TSize>)
        {
            return glm::epsilonEqual(X, Other.X, static_cast<TPos>(1e-6)) &&
                   glm::epsilonEqual(Y, Other.Y, static_cast<TPos>(1e-6)) &&
                   glm::epsilonEqual(Width, Other.Width, static_cast<TSize>(1e-6)) &&
                   glm::epsilonEqual(Height, Other.Height, static_cast<TSize>(1e-6));
        }
        else
        {
            return X == Other.X && Y == Other.Y && Width == Other.Width && Height == Other.Height;
        }
    }

    bool operator!=(const TRect2D& Other) const
    {
        return !(*this == Other);
    }

    // 获取位置
    TVector2<TPos> GetPosition() const
    {
        return TVector2<TPos>(X, Y);
    }

    // 获取大小
    TVector2<TSize> GetSize() const
    {
        return TVector2<TSize>(Width, Height);
    }

    // 设置位置
    void SetPosition(const TVector2<TPos>& Position)
    {
        X = Position.X;
        Y = Position.Y;
    }

    // 设置大小
    void SetSize(const TVector2<TSize>& Size)
    {
        Width  = Size.X;
        Height = Size.Y;
    }

    // 获取右边界
    TPos GetRight() const
    {
        return X + static_cast<TPos>(Width);
    }

    // 获取底边界
    TPos GetBottom() const
    {
        return Y + static_cast<TPos>(Height);
    }

    // 检查点是否在矩形内
    bool Contains(TPos InX, TPos InY) const
    {
        return InX >= X && InX < GetRight() && InY >= Y && InY < GetBottom();
    }

    bool Contains(const TVector2<TPos>& Point) const
    {
        return Contains(Point.X, Point.Y);
    }

    template <typename Archive>
    void Serialize(Archive& Ar)
    {
        Ar(MakeNamedPair("X", X), MakeNamedPair("Y", Y), MakeNamedPair("Width", Width),
           MakeNamedPair("Height", Height));
    }
};

// 类型别名
typedef TRect2D<Int32, UInt32>  FRect2Di; // 位置为Int32，大小为UInt32（用于裁剪矩形等）
typedef TRect2D<float, float>   FRect2Df; // 位置和大小都为float
typedef TRect2D<double, double> FRect2Dd; // 位置和大小都为double

struct FRECT2D_REGISTER
{
    HK_API static void Regsiter();

    FRECT2D_REGISTER()
    {
        Regsiter();
    }
};

static inline FRECT2D_REGISTER Z_RegisterRect2D;
