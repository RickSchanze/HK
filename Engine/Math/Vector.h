#pragma once
#include "Core/Utility/Macros.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

/////////////////////////////////////////////////////////////////////////////////
// TVector2
/////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct TVector2
{
    T X;
    T Y;

    // 构造函数
    TVector2() : X(0), Y(0) {}
    TVector2(T InX, T InY) : X(InX), Y(InY) {}
    TVector2(T InValue) : X(InValue), Y(InValue) {}

    // 拷贝构造函数和赋值操作符
    TVector2(const TVector2& Other) = default;
    TVector2& operator=(const TVector2& Other) = default;

    // 从 glm 转换
    TVector2(const glm::vec<2, T>& V) : X(V.x), Y(V.y) {}

    // 转换到 glm
    glm::vec<2, T> ToGlm() const
    {
        return glm::vec<2, T>(X, Y);
    }

    // 比较操作符
    bool operator==(const TVector2& Other) const
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return glm::epsilonEqual(X, Other.X, static_cast<T>(1e-6)) &&
                   glm::epsilonEqual(Y, Other.Y, static_cast<T>(1e-6));
        }
        else
        {
            return X == Other.X && Y == Other.Y;
        }
    }

    bool operator!=(const TVector2& Other) const
    {
        return !(*this == Other);
    }

    // 算术操作符 - 加法
    TVector2 operator+(const TVector2& Other) const
    {
        return TVector2(X + Other.X, Y + Other.Y);
    }

    // 算术操作符 - 减法
    TVector2 operator-(const TVector2& Other) const
    {
        return TVector2(X - Other.X, Y - Other.Y);
    }

    // 算术操作符 - 标量乘法
    TVector2 operator*(T Scalar) const
    {
        return TVector2(X * Scalar, Y * Scalar);
    }

    // 算术操作符 - 分量乘法
    TVector2 operator*(const TVector2& Other) const
    {
        return TVector2(X * Other.X, Y * Other.Y);
    }

    // 算术操作符 - 标量除法
    TVector2 operator/(T Scalar) const
    {
        return TVector2(X / Scalar, Y / Scalar);
    }

    // 算术操作符 - 分量除法
    TVector2 operator/(const TVector2& Other) const
    {
        return TVector2(X / Other.X, Y / Other.Y);
    }

    // 负号
    TVector2 operator-() const
    {
        return TVector2(-X, -Y);
    }

    // 复合赋值操作符
    TVector2& operator+=(const TVector2& Other)
    {
        X += Other.X;
        Y += Other.Y;
        return *this;
    }

    TVector2& operator-=(const TVector2& Other)
    {
        X -= Other.X;
        Y -= Other.Y;
        return *this;
    }

    TVector2& operator*=(T Scalar)
    {
        X *= Scalar;
        Y *= Scalar;
        return *this;
    }

    TVector2& operator*=(const TVector2& Other)
    {
        X *= Other.X;
        Y *= Other.Y;
        return *this;
    }

    TVector2& operator/=(T Scalar)
    {
        X /= Scalar;
        Y /= Scalar;
        return *this;
    }

    TVector2& operator/=(const TVector2& Other)
    {
        X /= Other.X;
        Y /= Other.Y;
        return *this;
    }

    // 长度相关函数
    T LengthSquared() const
    {
        return X * X + Y * Y;
    }

    auto Length() const
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return std::sqrt(LengthSquared());
        }
        else
        {
            return std::sqrt(static_cast<double>(LengthSquared()));
        }
    }

    // 归一化（仅浮点数版本）
    TVector2 Normalized() const
        requires std::is_floating_point_v<T>
    {
        T Len = static_cast<T>(Length());
        if (Len > static_cast<T>(1e-8))
        {
            return *this / Len;
        }
        return TVector2(0, 0);
    }

    void Normalize()
        requires std::is_floating_point_v<T>
    {
        *this = Normalized();
    }
};

// 标量在左侧的乘法
template <typename T>
inline TVector2<T> operator*(T Scalar, const TVector2<T>& Vec)
{
    return Vec * Scalar;
}

typedef TVector2<float> FVector2f;
typedef TVector2<double> FVector2d;
typedef TVector2<Int32> FVector2i;
typedef TVector2<UInt32> FVector2u;

struct FVECTOR2_REGISTER
{
    HK_API static void Regsiter();

    FVECTOR2_REGISTER()
    {
        Regsiter();
    }
};

static inline FVECTOR2_REGISTER Z_RegisterVector2;

/////////////////////////////////////////////////////////////////////////////////
// TVector3
/////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct TVector3
{
    T X;
    T Y;
    T Z;

    // 构造函数
    TVector3() : X(0), Y(0), Z(0) {}
    TVector3(T InX, T InY, T InZ) : X(InX), Y(InY), Z(InZ) {}
    TVector3(T InValue) : X(InValue), Y(InValue), Z(InValue) {}

    // 拷贝构造函数和赋值操作符
    TVector3(const TVector3& Other) = default;
    TVector3& operator=(const TVector3& Other) = default;

    // 从 glm 转换
    TVector3(const glm::vec<3, T>& V) : X(V.x), Y(V.y), Z(V.z) {}

    // 转换到 glm
    glm::vec<3, T> ToGlm() const
    {
        return glm::vec<3, T>(X, Y, Z);
    }

    // 比较操作符
    bool operator==(const TVector3& Other) const
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return glm::epsilonEqual(X, Other.X, static_cast<T>(1e-6)) &&
                   glm::epsilonEqual(Y, Other.Y, static_cast<T>(1e-6)) &&
                   glm::epsilonEqual(Z, Other.Z, static_cast<T>(1e-6));
        }
        else
        {
            return X == Other.X && Y == Other.Y && Z == Other.Z;
        }
    }

    bool operator!=(const TVector3& Other) const
    {
        return !(*this == Other);
    }

    // 算术操作符 - 加法
    TVector3 operator+(const TVector3& Other) const
    {
        return TVector3(X + Other.X, Y + Other.Y, Z + Other.Z);
    }

    // 算术操作符 - 减法
    TVector3 operator-(const TVector3& Other) const
    {
        return TVector3(X - Other.X, Y - Other.Y, Z - Other.Z);
    }

    // 算术操作符 - 标量乘法
    TVector3 operator*(T Scalar) const
    {
        return TVector3(X * Scalar, Y * Scalar, Z * Scalar);
    }

    // 算术操作符 - 分量乘法
    TVector3 operator*(const TVector3& Other) const
    {
        return TVector3(X * Other.X, Y * Other.Y, Z * Other.Z);
    }

    // 算术操作符 - 标量除法
    TVector3 operator/(T Scalar) const
    {
        return TVector3(X / Scalar, Y / Scalar, Z / Scalar);
    }

    // 算术操作符 - 分量除法
    TVector3 operator/(const TVector3& Other) const
    {
        return TVector3(X / Other.X, Y / Other.Y, Z / Other.Z);
    }

    // 负号
    TVector3 operator-() const
    {
        return TVector3(-X, -Y, -Z);
    }

    // 复合赋值操作符
    TVector3& operator+=(const TVector3& Other)
    {
        X += Other.X;
        Y += Other.Y;
        Z += Other.Z;
        return *this;
    }

    TVector3& operator-=(const TVector3& Other)
    {
        X -= Other.X;
        Y -= Other.Y;
        Z -= Other.Z;
        return *this;
    }

    TVector3& operator*=(T Scalar)
    {
        X *= Scalar;
        Y *= Scalar;
        Z *= Scalar;
        return *this;
    }

    TVector3& operator*=(const TVector3& Other)
    {
        X *= Other.X;
        Y *= Other.Y;
        Z *= Other.Z;
        return *this;
    }

    TVector3& operator/=(T Scalar)
    {
        X /= Scalar;
        Y /= Scalar;
        Z /= Scalar;
        return *this;
    }

    TVector3& operator/=(const TVector3& Other)
    {
        X /= Other.X;
        Y /= Other.Y;
        Z /= Other.Z;
        return *this;
    }

    // 长度相关函数
    T LengthSquared() const
    {
        return X * X + Y * Y + Z * Z;
    }

    auto Length() const
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return std::sqrt(LengthSquared());
        }
        else
        {
            return std::sqrt(static_cast<double>(LengthSquared()));
        }
    }

    // 归一化（仅浮点数版本）
    TVector3 Normalized() const
        requires std::is_floating_point_v<T>
    {
        T Len = static_cast<T>(Length());
        if (Len > static_cast<T>(1e-8))
        {
            return *this / Len;
        }
        return TVector3(0, 0, 0);
    }

    void Normalize()
        requires std::is_floating_point_v<T>
    {
        *this = Normalized();
    }
};

// 标量在左侧的乘法
template <typename T>
inline TVector3<T> operator*(T Scalar, const TVector3<T>& Vec)
{
    return Vec * Scalar;
}

typedef TVector3<float> FVector3f;
typedef TVector3<double> FVector3d;
typedef TVector3<Int32> FVector3i;
typedef TVector3<UInt32> FVector3u;

struct FVECTOR3_REGISTER
{
    HK_API static void Regsiter();

    FVECTOR3_REGISTER()
    {
        Regsiter();
    }
};

static inline FVECTOR3_REGISTER Z_RegisterVector3;

/////////////////////////////////////////////////////////////////////////////////
// TVector4
/////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct TVector4
{
    T X;
    T Y;
    T Z;
    T W;

    // 构造函数
    TVector4() : X(0), Y(0), Z(0), W(0) {}
    TVector4(T InX, T InY, T InZ, T InW) : X(InX), Y(InY), Z(InZ), W(InW) {}
    TVector4(T InValue) : X(InValue), Y(InValue), Z(InValue), W(InValue) {}

    // 拷贝构造函数和赋值操作符
    TVector4(const TVector4& Other) = default;
    TVector4& operator=(const TVector4& Other) = default;

    // 从 glm 转换
    TVector4(const glm::vec<4, T>& V) : X(V.x), Y(V.y), Z(V.z), W(V.w) {}

    // 转换到 glm
    glm::vec<4, T> ToGlm() const
    {
        return glm::vec<4, T>(X, Y, Z, W);
    }

    // 比较操作符
    bool operator==(const TVector4& Other) const
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return glm::epsilonEqual(X, Other.X, static_cast<T>(1e-6)) &&
                   glm::epsilonEqual(Y, Other.Y, static_cast<T>(1e-6)) &&
                   glm::epsilonEqual(Z, Other.Z, static_cast<T>(1e-6)) &&
                   glm::epsilonEqual(W, Other.W, static_cast<T>(1e-6));
        }
        else
        {
            return X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W;
        }
    }

    bool operator!=(const TVector4& Other) const
    {
        return !(*this == Other);
    }

    // 算术操作符 - 加法
    TVector4 operator+(const TVector4& Other) const
    {
        return TVector4(X + Other.X, Y + Other.Y, Z + Other.Z, W + Other.W);
    }

    // 算术操作符 - 减法
    TVector4 operator-(const TVector4& Other) const
    {
        return TVector4(X - Other.X, Y - Other.Y, Z - Other.Z, W - Other.W);
    }

    // 算术操作符 - 标量乘法
    TVector4 operator*(T Scalar) const
    {
        return TVector4(X * Scalar, Y * Scalar, Z * Scalar, W * Scalar);
    }

    // 算术操作符 - 分量乘法
    TVector4 operator*(const TVector4& Other) const
    {
        return TVector4(X * Other.X, Y * Other.Y, Z * Other.Z, W * Other.W);
    }

    // 算术操作符 - 标量除法
    TVector4 operator/(T Scalar) const
    {
        return TVector4(X / Scalar, Y / Scalar, Z / Scalar, W / Scalar);
    }

    // 算术操作符 - 分量除法
    TVector4 operator/(const TVector4& Other) const
    {
        return TVector4(X / Other.X, Y / Other.Y, Z / Other.Z, W / Other.W);
    }

    // 负号
    TVector4 operator-() const
    {
        return TVector4(-X, -Y, -Z, -W);
    }

    // 复合赋值操作符
    TVector4& operator+=(const TVector4& Other)
    {
        X += Other.X;
        Y += Other.Y;
        Z += Other.Z;
        W += Other.W;
        return *this;
    }

    TVector4& operator-=(const TVector4& Other)
    {
        X -= Other.X;
        Y -= Other.Y;
        Z -= Other.Z;
        W -= Other.W;
        return *this;
    }

    TVector4& operator*=(T Scalar)
    {
        X *= Scalar;
        Y *= Scalar;
        Z *= Scalar;
        W *= Scalar;
        return *this;
    }

    TVector4& operator*=(const TVector4& Other)
    {
        X *= Other.X;
        Y *= Other.Y;
        Z *= Other.Z;
        W *= Other.W;
        return *this;
    }

    TVector4& operator/=(T Scalar)
    {
        X /= Scalar;
        Y /= Scalar;
        Z /= Scalar;
        W /= Scalar;
        return *this;
    }

    TVector4& operator/=(const TVector4& Other)
    {
        X /= Other.X;
        Y /= Other.Y;
        Z /= Other.Z;
        W /= Other.W;
        return *this;
    }

    // 长度相关函数
    T LengthSquared() const
    {
        return X * X + Y * Y + Z * Z + W * W;
    }

    auto Length() const
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return std::sqrt(LengthSquared());
        }
        else
        {
            return std::sqrt(static_cast<double>(LengthSquared()));
        }
    }

    // 归一化（仅浮点数版本）
    TVector4 Normalized() const
        requires std::is_floating_point_v<T>
    {
        T Len = static_cast<T>(Length());
        if (Len > static_cast<T>(1e-8))
        {
            return *this / Len;
        }
        return TVector4(0, 0, 0, 0);
    }

    void Normalize()
        requires std::is_floating_point_v<T>
    {
        *this = Normalized();
    }
};

// 标量在左侧的乘法
template <typename T>
inline TVector4<T> operator*(T Scalar, const TVector4<T>& Vec)
{
    return Vec * Scalar;
}

typedef TVector4<float> FVector4f;
typedef TVector4<double> FVector4d;
typedef TVector4<Int32> FVector4i;
typedef TVector4<UInt32> FVector4u;

struct FVECTOR4_REGISTER
{
    HK_API static void Regsiter();

    FVECTOR4_REGISTER()
    {
        Regsiter();
    }
};

static inline FVECTOR4_REGISTER Z_RegisterVector4;
