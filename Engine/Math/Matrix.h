#pragma once
#include "Core/Reflection/Reflection.h"
#include "Core/Utility/Macros.h"
#include "Vector.h"
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/////////////////////////////////////////////////////////////////////////////////
// TMatrix4x4
// 4x4 矩阵模板类，使用列主序存储（与 GLM 和 OpenGL/Vulkan 一致）
/////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct TMatrix4x4
{
    // 矩阵数据（列主序存储：M[col][row] 表示第 row 行第 col 列的元素）
    // 内存布局：列0的所有元素，列1的所有元素，列2的所有元素，列3的所有元素
    T M[4][4];

    /////////////////////////////////////////////////////////////////////////////
    // 构造函数
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 默认构造函数，创建单位矩阵
     */
    TMatrix4x4()
    {
        // 单位矩阵（列主序）
        M[0][0] = 1;
        M[0][1] = 0;
        M[0][2] = 0;
        M[0][3] = 0;
        M[1][0] = 0;
        M[1][1] = 1;
        M[1][2] = 0;
        M[1][3] = 0;
        M[2][0] = 0;
        M[2][1] = 0;
        M[2][2] = 1;
        M[2][3] = 0;
        M[3][0] = 0;
        M[3][1] = 0;
        M[3][2] = 0;
        M[3][3] = 1;
    }

    /**
     * @brief 从16个元素构造矩阵（按行主序输入，内部转换为列主序存储）
     * @param M00-M33 矩阵元素，按行主序排列
     */
    TMatrix4x4(T M00, T M01, T M02, T M03, T M10, T M11, T M12, T M13, T M20, T M21, T M22, T M23, T M30, T M31, T M32,
               T M33)
    {
        // 输入是行主序，存储为列主序
        M[0][0] = M00; // 第0列第0行
        M[1][0] = M01; // 第1列第0行
        M[2][0] = M02; // 第2列第0行
        M[3][0] = M03; // 第3列第0行
        M[0][1] = M10; // 第0列第1行
        M[1][1] = M11; // 第1列第1行
        M[2][1] = M12; // 第2列第1行
        M[3][1] = M13; // 第3列第1行
        M[0][2] = M20; // 第0列第2行
        M[1][2] = M21; // 第1列第2行
        M[2][2] = M22; // 第2列第2行
        M[3][2] = M23; // 第3列第2行
        M[0][3] = M30; // 第0列第3行
        M[1][3] = M31; // 第1列第3行
        M[2][3] = M32; // 第2列第3行
        M[3][3] = M33; // 第3列第3行
    }

    /**
     * @brief 从 GLM 矩阵构造（GLM 也是列主序，可以直接复制）
     * @param GlmMat GLM 4x4 矩阵
     */
    TMatrix4x4(const glm::mat<4, 4, T>& GlmMat)
    {
        const T* Ptr = glm::value_ptr(GlmMat);
        // GLM 是列主序，直接复制
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                M[col][row] = Ptr[col * 4 + row];
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // 转换函数
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 转换为 GLM 矩阵
     * @return GLM 4x4 矩阵（列主序）
     */
    glm::mat<4, 4, T> ToGlm() const
    {
        glm::mat<4, 4, T> Result;
        T*                Ptr = glm::value_ptr(Result);
        // 列主序直接复制
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                Ptr[col * 4 + row] = M[col][row];
            }
        }
        return Result;
    }

    /////////////////////////////////////////////////////////////////////////////
    // 拷贝和赋值
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 拷贝构造函数
     */
    TMatrix4x4(const TMatrix4x4& Other) = default;

    /**
     * @brief 赋值操作符
     */
    TMatrix4x4& operator=(const TMatrix4x4& Other) = default;

    /////////////////////////////////////////////////////////////////////////////
    // 元素访问
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 访问矩阵元素（行，列）
     * @param Row 行索引 [0-3]
     * @param Col 列索引 [0-3]
     * @return 元素的引用
     */
    T& operator()(int Row, int Col)
    {
        return M[Col][Row]; // 列主序：M[col][row]
    }

    /**
     * @brief 访问矩阵元素（常量版本）
     * @param Row 行索引 [0-3]
     * @param Col 列索引 [0-3]
     * @return 元素的常量引用
     */
    const T& operator()(int Row, int Col) const
    {
        return M[Col][Row]; // 列主序：M[col][row]
    }

    /////////////////////////////////////////////////////////////////////////////
    // 比较操作符
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 相等比较操作符
     * @param Other 要比较的矩阵
     * @return 如果两个矩阵相等返回 true，否则返回 false
     * @note 对于浮点数类型，使用 epsilon 比较以避免精度问题
     */
    bool operator==(const TMatrix4x4& Other) const
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            for (int col = 0; col < 4; ++col)
            {
                for (int row = 0; row < 4; ++row)
                {
                    if (!glm::epsilonEqual(M[col][row], Other.M[col][row], static_cast<T>(1e-6)))
                    {
                        return false;
                    }
                }
            }
            return true;
        }
        else
        {
            for (int col = 0; col < 4; ++col)
            {
                for (int row = 0; row < 4; ++row)
                {
                    if (M[col][row] != Other.M[col][row])
                    {
                        return false;
                    }
                }
            }
            return true;
        }
    }

    /**
     * @brief 不等比较操作符
     * @param Other 要比较的矩阵
     * @return 如果两个矩阵不相等返回 true，否则返回 false
     */
    bool operator!=(const TMatrix4x4& Other) const
    {
        return !(*this == Other);
    }

    /////////////////////////////////////////////////////////////////////////////
    // 算术操作符
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 矩阵加法
     * @param Other 要相加的矩阵
     * @return 相加后的新矩阵
     */
    TMatrix4x4 operator+(const TMatrix4x4& Other) const
    {
        TMatrix4x4 Result;
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                Result.M[col][row] = M[col][row] + Other.M[col][row];
            }
        }
        return Result;
    }

    /**
     * @brief 矩阵减法
     * @param Other 要相减的矩阵
     * @return 相减后的新矩阵
     */
    TMatrix4x4 operator-(const TMatrix4x4& Other) const
    {
        TMatrix4x4 Result;
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                Result.M[col][row] = M[col][row] - Other.M[col][row];
            }
        }
        return Result;
    }

    /**
     * @brief 标量乘法（矩阵 * 标量）
     * @param Scalar 标量值
     * @return 相乘后的新矩阵
     */
    TMatrix4x4 operator*(T Scalar) const
    {
        TMatrix4x4 Result;
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                Result.M[col][row] = M[col][row] * Scalar;
            }
        }
        return Result;
    }

    /**
     * @brief 矩阵乘法（this * Other）
     * @param Other 要相乘的矩阵
     * @return 相乘后的新矩阵
     * @note 矩阵乘法的顺序：Result = this * Other
     *       列主序：M[col][row] 表示第 row 行第 col 列
     *       Result[i][j] = Σ(this[i][k] * Other[k][j])
     *       = Σ(this.M[k][i] * Other.M[j][k])
     */
    TMatrix4x4 operator*(const TMatrix4x4& Other) const
    {
        TMatrix4x4 Result;
        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                T Sum = static_cast<T>(0);
                for (int k = 0; k < 4; ++k)
                {
                    // 列主序：this 的第 row 行第 k 列是 this.M[k][row]
                    // Other 的第 k 行第 col 列是 Other.M[col][k]
                    Sum += M[k][row] * Other.M[col][k];
                }
                Result.M[col][row] = Sum;
            }
        }
        return Result;
    }

    /**
     * @brief 矩阵乘以向量3（自动扩展为齐次坐标 (x, y, z, 1)）
     * @param Vec 3D 向量
     * @return 变换后的 3D 向量
     * @note 如果结果 W 分量不为 1，会进行透视除法
     *       列主序：M[col][row]，第 row 行的元素是 M[0][row], M[1][row], M[2][row], M[3][row]
     */
    TVector3<T> operator*(const TVector3<T>& Vec) const
    {
        TVector3<T> Result;
        // 列主序：第 0 行的元素是 M[0][0], M[1][0], M[2][0], M[3][0]
        Result.X = M[0][0] * Vec.X + M[1][0] * Vec.Y + M[2][0] * Vec.Z + M[3][0] * static_cast<T>(1);
        Result.Y = M[0][1] * Vec.X + M[1][1] * Vec.Y + M[2][1] * Vec.Z + M[3][1] * static_cast<T>(1);
        Result.Z = M[0][2] * Vec.X + M[1][2] * Vec.Y + M[2][2] * Vec.Z + M[3][2] * static_cast<T>(1);
        T W      = M[0][3] * Vec.X + M[1][3] * Vec.Y + M[2][3] * Vec.Z + M[3][3] * static_cast<T>(1);
        if (std::abs(W) > static_cast<T>(1e-8))
        {
            Result.X /= W;
            Result.Y /= W;
            Result.Z /= W;
        }
        return Result;
    }

    /**
     * @brief 矩阵乘以向量4
     * @param Vec 4D 向量
     * @return 变换后的 4D 向量
     * @note 列主序：第 row 行的元素是 M[0][row], M[1][row], M[2][row], M[3][row]
     */
    TVector4<T> operator*(const TVector4<T>& Vec) const
    {
        TVector4<T> Result;
        // 列主序矩阵乘法
        Result.X = M[0][0] * Vec.X + M[1][0] * Vec.Y + M[2][0] * Vec.Z + M[3][0] * Vec.W;
        Result.Y = M[0][1] * Vec.X + M[1][1] * Vec.Y + M[2][1] * Vec.Z + M[3][1] * Vec.W;
        Result.Z = M[0][2] * Vec.X + M[1][2] * Vec.Y + M[2][2] * Vec.Z + M[3][2] * Vec.W;
        Result.W = M[0][3] * Vec.X + M[1][3] * Vec.Y + M[2][3] * Vec.Z + M[3][3] * Vec.W;
        return Result;
    }

    /**
     * @brief 变换 2D 点（自动扩展为齐次坐标 (x, y, 0, 1)）
     * @param Vec 2D 向量
     * @return 变换后的 2D 向量
     * @note 如果结果 W 分量不为 1，会进行透视除法
     *       列主序：Z 分量设为 0
     */
    TVector2<T> TransformPoint2D(const TVector2<T>& Vec) const
    {
        TVector2<T> Result;
        // 列主序矩阵乘法，Z 分量设为 0
        Result.X = M[0][0] * Vec.X + M[1][0] * Vec.Y + M[3][0] * static_cast<T>(1);
        Result.Y = M[0][1] * Vec.X + M[1][1] * Vec.Y + M[3][1] * static_cast<T>(1);
        T W      = M[0][3] * Vec.X + M[1][3] * Vec.Y + M[3][3] * static_cast<T>(1);
        if (std::abs(W) > static_cast<T>(1e-8))
        {
            Result.X /= W;
            Result.Y /= W;
        }
        return Result;
    }

    /////////////////////////////////////////////////////////////////////////////
    // 复合赋值操作符
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 矩阵加法复合赋值
     * @param Other 要相加的矩阵
     * @return 自身的引用
     */
    TMatrix4x4& operator+=(const TMatrix4x4& Other)
    {
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                M[col][row] += Other.M[col][row];
            }
        }
        return *this;
    }

    /**
     * @brief 矩阵减法复合赋值
     * @param Other 要相减的矩阵
     * @return 自身的引用
     */
    TMatrix4x4& operator-=(const TMatrix4x4& Other)
    {
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                M[col][row] -= Other.M[col][row];
            }
        }
        return *this;
    }

    /**
     * @brief 标量乘法复合赋值
     * @param Scalar 标量值
     * @return 自身的引用
     */
    TMatrix4x4& operator*=(T Scalar)
    {
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                M[col][row] *= Scalar;
            }
        }
        return *this;
    }

    /**
     * @brief 矩阵乘法复合赋值
     * @param Other 要相乘的矩阵
     * @return 自身的引用
     * @note 等价于 this = this * Other
     */
    TMatrix4x4& operator*=(const TMatrix4x4& Other)
    {
        *this = *this * Other;
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////
    // 矩阵运算
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 计算转置矩阵
     * @return 转置后的新矩阵
     * @note 转置：M^T[i][j] = M[j][i]
     */
    TMatrix4x4 Transposed() const
    {
        TMatrix4x4 Result;
        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                // 列主序：M[col][row]，转置后是 M[row][col]
                Result.M[col][row] = M[row][col];
            }
        }
        return Result;
    }

    /**
     * @brief 计算行列式
     * @return 矩阵的行列式值
     * @note 使用 GLM 计算行列式
     */
    T Determinant() const
    {
        return glm::determinant(ToGlm());
    }

    /**
     * @brief 计算逆矩阵
     * @return 逆矩阵
     * @note 如果矩阵不可逆，GLM 会返回一个近似值
     */
    TMatrix4x4 Inverse() const
    {
        return TMatrix4x4(glm::inverse(ToGlm()));
    }

    /////////////////////////////////////////////////////////////////////////////
    // 序列化
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 序列化函数
     * @param Ar 归档对象
     */
    template <typename Archive>
    void Serialize(Archive& Ar)
    {
        // 按行主序序列化（便于阅读），但内部存储是列主序
        Ar(MakeNamedPair("M00", M[0][0]), MakeNamedPair("M01", M[1][0]), MakeNamedPair("M02", M[2][0]),
           MakeNamedPair("M03", M[3][0]), MakeNamedPair("M10", M[0][1]), MakeNamedPair("M11", M[1][1]),
           MakeNamedPair("M12", M[2][1]), MakeNamedPair("M13", M[3][1]), MakeNamedPair("M20", M[0][2]),
           MakeNamedPair("M21", M[1][2]), MakeNamedPair("M22", M[2][2]), MakeNamedPair("M23", M[3][2]),
           MakeNamedPair("M30", M[0][3]), MakeNamedPair("M31", M[1][3]), MakeNamedPair("M32", M[2][3]),
           MakeNamedPair("M33", M[3][3]));
    }
};

/////////////////////////////////////////////////////////////////////////////////
// 全局操作符
/////////////////////////////////////////////////////////////////////////////////

/**
 * @brief 标量在左侧的矩阵乘法（标量 * 矩阵）
 * @param Scalar 标量值
 * @param Mat 矩阵
 * @return 相乘后的新矩阵
 */
template <typename T>
inline TMatrix4x4<T> operator*(T Scalar, const TMatrix4x4<T>& Mat)
{
    return Mat * Scalar;
}

/////////////////////////////////////////////////////////////////////////////////
// 类型别名
/////////////////////////////////////////////////////////////////////////////////

typedef TMatrix4x4<float>  FMatrix4x4f;  // 单精度浮点 4x4 矩阵
typedef TMatrix4x4<double> FMatrix4x4d; // 双精度浮点 4x4 矩阵

/////////////////////////////////////////////////////////////////////////////////
// 矩阵工具函数
/////////////////////////////////////////////////////////////////////////////////
namespace MatrixUtils
{
/**
 * @brief 创建平移矩阵
 * @param Translation 平移向量
 * @return 平移矩阵
 * @note 矩阵形式：
 *   [1  0  0  Tx]
 *   [0  1  0  Ty]
 *   [0  0  1  Tz]
 *   [0  0  0  1 ]
 */
template <typename T>
inline TMatrix4x4<T> MakeTranslation(const TVector3<T>& Translation)
{
    TMatrix4x4<T> Result;
    Result(0, 3) = Translation.X; // 第3列第0行
    Result(1, 3) = Translation.Y; // 第3列第1行
    Result(2, 3) = Translation.Z; // 第3列第2行
    return Result;
}

/**
 * @brief 创建缩放矩阵
 * @param Scale 缩放向量
 * @return 缩放矩阵
 * @note 矩阵形式：
 *   [Sx  0   0   0 ]
 *   [0   Sy  0   0 ]
 *   [0   0   Sz  0 ]
 *   [0   0   0   1 ]
 */
template <typename T>
inline TMatrix4x4<T> MakeScale(const TVector3<T>& Scale)
{
    TMatrix4x4<T> Result;
    Result(0, 0) = Scale.X; // 第0列第0行
    Result(1, 1) = Scale.Y; // 第1列第1行
    Result(2, 2) = Scale.Z; // 第2列第2行
    return Result;
}

/**
 * @brief 创建绕 X 轴旋转矩阵
 * @param Angle 旋转角度（弧度）
 * @return 旋转矩阵
 * @note 矩阵形式：
 *   [1   0     0     0 ]
 *   [0   cos  -sin   0 ]
 *   [0   sin   cos   0 ]
 *   [0   0     0     1 ]
 */
template <typename T>
inline TMatrix4x4<T> MakeRotationX(T Angle)
{
    TMatrix4x4<T> Result;
    T             Cos = std::cos(Angle);
    T             Sin = std::sin(Angle);
    Result(1, 1)      = Cos;  // 第1列第1行
    Result(1, 2)      = -Sin; // 第1列第2行
    Result(2, 1)      = Sin;  // 第2列第1行
    Result(2, 2)      = Cos;  // 第2列第2行
    return Result;
}

/**
 * @brief 创建绕 Y 轴旋转矩阵
 * @param Angle 旋转角度（弧度）
 * @return 旋转矩阵
 * @note 矩阵形式：
 *   [cos   0   sin   0 ]
 *   [0     1   0     0 ]
 *   [-sin  0   cos   0 ]
 *   [0     0   0     1 ]
 */
template <typename T>
inline TMatrix4x4<T> MakeRotationY(T Angle)
{
    TMatrix4x4<T> Result;
    T             Cos = std::cos(Angle);
    T             Sin = std::sin(Angle);
    Result(0, 0)      = Cos;  // 第0列第0行
    Result(0, 2)      = Sin;  // 第0列第2行
    Result(2, 0)      = -Sin; // 第2列第0行
    Result(2, 2)      = Cos;  // 第2列第2行
    return Result;
}

/**
 * @brief 创建绕 Z 轴旋转矩阵
 * @param Angle 旋转角度（弧度）
 * @return 旋转矩阵
 * @note 矩阵形式：
 *   [cos  -sin  0   0 ]
 *   [sin  cos   0   0 ]
 *   [0    0     1   0 ]
 *   [0    0     0   1 ]
 */
template <typename T>
inline TMatrix4x4<T> MakeRotationZ(T Angle)
{
    TMatrix4x4<T> Result;
    T             Cos = std::cos(Angle);
    T             Sin = std::sin(Angle);
    Result(0, 0)      = Cos;  // 第0列第0行
    Result(0, 1)      = -Sin; // 第0列第1行
    Result(1, 0)      = Sin;  // 第1列第0行
    Result(1, 1)      = Cos;  // 第1列第1行
    return Result;
}

/**
 * @brief 创建透视投影矩阵
 * @param FovY 垂直视野角度（弧度）
 * @param Aspect 宽高比（宽度/高度）
 * @param Near 近平面距离
 * @param Far 远平面距离
 * @return 透视投影矩阵
 * @note 使用 GLM 的 perspective 函数
 */
template <typename T>
inline TMatrix4x4<T> MakePerspective(T FovY, T Aspect, T Near, T Far)
{
    return TMatrix4x4<T>(glm::perspective(FovY, Aspect, Near, Far));
}

/**
 * @brief 创建正交投影矩阵
 * @param Left 左边界
 * @param Right 右边界
 * @param Bottom 下边界
 * @param Top 上边界
 * @param Near 近平面距离
 * @param Far 远平面距离
 * @return 正交投影矩阵
 * @note 使用 GLM 的 ortho 函数
 */
template <typename T>
inline TMatrix4x4<T> MakeOrthographic(T Left, T Right, T Bottom, T Top, T Near, T Far)
{
    return TMatrix4x4<T>(glm::ortho(Left, Right, Bottom, Top, Near, Far));
}

/**
 * @brief 创建 LookAt 视图矩阵
 * @param Eye 相机位置
 * @param Center 观察目标点
 * @param Up 上方向向量
 * @return 视图矩阵
 * @note 使用 GLM 的 lookAt 函数，用于构建相机视图矩阵
 */
template <typename T>
inline TMatrix4x4<T> MakeLookAt(const TVector3<T>& Eye, const TVector3<T>& Center, const TVector3<T>& Up)
{
    return TMatrix4x4<T>(glm::lookAt(Eye.ToGlm(), Center.ToGlm(), Up.ToGlm()));
}
} // namespace MatrixUtils

/////////////////////////////////////////////////////////////////////////////////
// 反射注册
/////////////////////////////////////////////////////////////////////////////////

/**
 * @brief 矩阵类型反射注册器
 */
struct FMATRIX_REGISTER
{
    /**
     * @brief 注册所有矩阵类型到反射系统
     */
    HK_API static void Regsiter();

    /**
     * @brief 构造函数，自动调用注册
     */
    FMATRIX_REGISTER()
    {
        Regsiter();
    }
};

/**
 * @brief 静态注册器实例，程序启动时自动注册
 */
static inline FMATRIX_REGISTER Z_RegisterMatrix;
