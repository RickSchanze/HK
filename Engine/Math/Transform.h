#pragma once
#include "Core/Reflection/Reflection.h"
#include "Core/Utility/Macros.h"
#include "Matrix.h"
#include "Vector.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

/////////////////////////////////////////////////////////////////////////////////
// TTransform
/////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct TTransform
{
    TVector3<T> Position; // 位置
    // 旋转（欧拉角，弧度制，Z-X-Y 顺序：Roll-Pitch-Yaw，与 Unity/Unreal 一致）
    // Rotation.X = Pitch（俯仰角，绕 X 轴）
    // Rotation.Y = Yaw（偏航角，绕 Y 轴）
    // Rotation.Z = Roll（翻滚角，绕 Z 轴）
    TVector3<T> Rotation;
    TVector3<T> Scale; // 缩放

    // 构造函数
    TTransform() : Position(0, 0, 0), Rotation(0, 0, 0), Scale(1, 1, 1) {}

    TTransform(const TVector3<T>& InPosition, const TVector3<T>& InRotation, const TVector3<T>& InScale)
        : Position(InPosition), Rotation(InRotation), Scale(InScale)
    {
    }

    TTransform(const TVector3<T>& InPosition, const TVector3<T>& InRotation)
        : Position(InPosition), Rotation(InRotation), Scale(1, 1, 1)
    {
    }

    TTransform(const TVector3<T>& InPosition) : Position(InPosition), Rotation(0, 0, 0), Scale(1, 1, 1) {}

    // 拷贝构造函数和赋值操作符
    TTransform(const TTransform& Other)            = default;
    TTransform& operator=(const TTransform& Other) = default;

    // 比较操作符
    bool operator==(const TTransform& Other) const
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return Position == Other.Position && Rotation == Other.Rotation && Scale == Other.Scale;
        }
        else
        {
            return Position == Other.Position && Rotation == Other.Rotation && Scale == Other.Scale;
        }
    }

    bool operator!=(const TTransform& Other) const
    {
        return !(*this == Other);
    }

    // 获取变换矩阵（顺序：Scale -> Rotation -> Translation）
    TMatrix4x4<T> ToMatrix() const
    {
        // 创建缩放矩阵
        TMatrix4x4<T> ScaleMat = MatrixUtils::MakeScale<T>(Scale);

        // 创建旋转矩阵（欧拉角转旋转矩阵，Z-X-Y 顺序：先绕 Z 轴，再绕 X 轴，最后绕 Y 轴）
        // 这是 Unity 和 Unreal Engine 的标准旋转顺序（Roll-Pitch-Yaw）
        TMatrix4x4<T> RotMat;
        {
            // Z-X-Y 顺序：先 Roll（Z），再 Pitch（X），最后 Yaw（Y）
            // 使用矩阵乘法组合：R = R_y * R_x * R_z
            TMatrix4x4<T> RotZ = MatrixUtils::MakeRotationZ<T>(Rotation.Z); // Roll
            TMatrix4x4<T> RotX = MatrixUtils::MakeRotationX<T>(Rotation.X); // Pitch
            TMatrix4x4<T> RotY = MatrixUtils::MakeRotationY<T>(Rotation.Y); // Yaw
            RotMat             = RotY * RotX * RotZ;
        }

        // 创建平移矩阵
        TMatrix4x4<T> TransMat = MatrixUtils::MakeTranslation<T>(Position);

        // 组合：先缩放，再旋转，最后平移
        return TransMat * RotMat * ScaleMat;
    }

    // 从矩阵构建变换（分解矩阵）
    static TTransform FromMatrix(const TMatrix4x4<T>& Matrix)
    {
        TTransform Result;

        // 提取平移
        Result.Position.X = Matrix(0, 3);
        Result.Position.Y = Matrix(1, 3);
        Result.Position.Z = Matrix(2, 3);

        // 提取缩放（从矩阵的前三列的长度）
        TVector3<T> Col0(Matrix(0, 0), Matrix(1, 0), Matrix(2, 0));
        TVector3<T> Col1(Matrix(0, 1), Matrix(1, 1), Matrix(2, 1));
        TVector3<T> Col2(Matrix(0, 2), Matrix(1, 2), Matrix(2, 2));
        Result.Scale.X = static_cast<T>(Col0.Length());
        Result.Scale.Y = static_cast<T>(Col1.Length());
        Result.Scale.Z = static_cast<T>(Col2.Length());

        // 提取旋转（从归一化的旋转部分）
        if (Result.Scale.X > static_cast<T>(1e-8))
        {
            Col0 = Col0 / Result.Scale.X;
        }
        if (Result.Scale.Y > static_cast<T>(1e-8))
        {
            Col1 = Col1 / Result.Scale.Y;
        }
        if (Result.Scale.Z > static_cast<T>(1e-8))
        {
            Col2 = Col2 / Result.Scale.Z;
        }

        // 从旋转矩阵提取欧拉角（Z-X-Y 顺序）
        // 注意：从矩阵提取欧拉角可能有歧义，这里使用四元数作为中间表示
        glm::mat<3, 3, T> RotMat3x3;
        RotMat3x3[0][0] = Col0.X;
        RotMat3x3[1][0] = Col0.Y;
        RotMat3x3[2][0] = Col0.Z;
        RotMat3x3[0][1] = Col1.X;
        RotMat3x3[1][1] = Col1.Y;
        RotMat3x3[2][1] = Col1.Z;
        RotMat3x3[0][2] = Col2.X;
        RotMat3x3[1][2] = Col2.Y;
        RotMat3x3[2][2] = Col2.Z;

        // 转换为四元数再提取欧拉角（GLM 默认使用 Z-X-Y 顺序）
        glm::quat      Quat  = glm::quat_cast(RotMat3x3);
        glm::vec<3, T> Euler = glm::eulerAngles(Quat);
        // GLM 的 eulerAngles 返回 (pitch, yaw, roll)，对应 (X, Y, Z)
        Result.Rotation.X = Euler.x; // Pitch
        Result.Rotation.Y = Euler.y; // Yaw
        Result.Rotation.Z = Euler.z; // Roll

        return Result;
    }

    // 组合变换（先应用 Other，再应用 this）
    TTransform operator*(const TTransform& Other) const
    {
        TMatrix4x4<T> ThisMat  = ToMatrix();
        TMatrix4x4<T> OtherMat = Other.ToMatrix();
        TMatrix4x4<T> Combined = ThisMat * OtherMat;
        return FromMatrix(Combined);
    }

    // 变换点（应用位置、旋转、缩放）
    TVector3<T> TransformPoint(const TVector3<T>& Point) const
    {
        TMatrix4x4<T> Mat = ToMatrix();
        return Mat * Point;
    }

    // 变换方向（只应用旋转和缩放，不应用平移）
    TVector3<T> TransformDirection(const TVector3<T>& Direction) const
    {
        // 创建只包含旋转和缩放的矩阵（Z-X-Y 顺序）
        TMatrix4x4<T> ScaleMat = MatrixUtils::MakeScale<T>(Scale);
        TMatrix4x4<T> RotZ     = MatrixUtils::MakeRotationZ<T>(Rotation.Z); // Roll
        TMatrix4x4<T> RotX     = MatrixUtils::MakeRotationX<T>(Rotation.X); // Pitch
        TMatrix4x4<T> RotY     = MatrixUtils::MakeRotationY<T>(Rotation.Y); // Yaw
        TMatrix4x4<T> RotMat   = RotY * RotX * RotZ;
        TMatrix4x4<T> Mat      = RotMat * ScaleMat;
        return Mat * Direction;
    }

    // 逆变换点
    TVector3<T> InverseTransformPoint(const TVector3<T>& Point) const
    {
        TMatrix4x4<T> Mat = ToMatrix().Inverse();
        return Mat * Point;
    }

    // 逆变换方向
    TVector3<T> InverseTransformDirection(const TVector3<T>& Direction) const
    {
        // 创建只包含旋转和缩放的逆矩阵（Z-X-Y 顺序的逆）
        TMatrix4x4<T> ScaleMat = MatrixUtils::MakeScale<T>(TVector3<T>(1.0f / Scale.X, 1.0f / Scale.Y, 1.0f / Scale.Z));
        TMatrix4x4<T> RotZ     = MatrixUtils::MakeRotationZ<T>(-Rotation.Z); // Roll 逆
        TMatrix4x4<T> RotX     = MatrixUtils::MakeRotationX<T>(-Rotation.X); // Pitch 逆
        TMatrix4x4<T> RotY     = MatrixUtils::MakeRotationY<T>(-Rotation.Y); // Yaw 逆
        // 逆旋转顺序相反：R^-1 = R_z^-1 * R_x^-1 * R_y^-1
        TMatrix4x4<T> RotMat = RotZ * RotX * RotY;
        TMatrix4x4<T> Mat    = ScaleMat * RotMat;
        return Mat * Direction;
    }

    // 获取前方向（Forward，通常是 Z 轴正方向）
    TVector3<T> GetForward() const
    {
        return TransformDirection(TVector3<T>(0, 0, 1));
    }

    // 获取右方向（Right，通常是 X 轴正方向）
    TVector3<T> GetRight() const
    {
        return TransformDirection(TVector3<T>(1, 0, 0));
    }

    // 获取上方向（Up，通常是 Y 轴正方向）
    TVector3<T> GetUp() const
    {
        return TransformDirection(TVector3<T>(0, 1, 0));
    }

    // 逆变换（计算逆 Transform）
    TTransform Inverse() const
    {
        // 使用矩阵的逆来获取逆 Transform
        TMatrix4x4<T> Mat = ToMatrix();
        return FromMatrix(Mat.Inverse());
    }

    // 设置位置
    void SetPosition(const TVector3<T>& InPosition)
    {
        Position = InPosition;
    }

    /**
     * @brief 设置旋转（欧拉角，弧度，Z-X-Y 顺序）
     * @param InRotation 旋转向量，X=Pitch, Y=Yaw, Z=Roll
     */
    void SetRotation(const TVector3<T>& InRotation)
    {
        Rotation = InRotation;
    }

    // 设置缩放
    void SetScale(const TVector3<T>& InScale)
    {
        Scale = InScale;
    }

    // 设置统一缩放
    void SetScale(T UniformScale)
    {
        Scale = TVector3<T>(UniformScale, UniformScale, UniformScale);
    }

    // 添加平移
    void Translate(const TVector3<T>& Delta)
    {
        Position += Delta;
    }

    // 添加旋转（欧拉角增量，弧度）
    void Rotate(const TVector3<T>& Delta)
    {
        Rotation += Delta;
    }

    // 添加缩放
    void ScaleBy(const TVector3<T>& Delta)
    {
        Scale *= Delta;
    }

    // 添加统一缩放
    void ScaleBy(T UniformDelta)
    {
        Scale *= UniformDelta;
    }

    // 重置为单位变换
    void Reset()
    {
        Position = TVector3<T>(0, 0, 0);
        Rotation = TVector3<T>(0, 0, 0);
        Scale    = TVector3<T>(1, 1, 1);
    }

    // 序列化
    template <typename Archive>
    void Serialize(Archive& Ar)
    {
        Ar(MakeNamedPair("Position", Position), MakeNamedPair("Rotation", Rotation), MakeNamedPair("Scale", Scale));
    }
};

// 类型别名
typedef TTransform<float>  FTransformf;
typedef TTransform<double> FTransformd;
typedef FTransformf        FTransform;

struct FTRANSFORM_REGISTER
{
    HK_API static void Regsiter();

    FTRANSFORM_REGISTER()
    {
        Regsiter();
    }
};

static inline FTRANSFORM_REGISTER Z_RegisterTransform;
