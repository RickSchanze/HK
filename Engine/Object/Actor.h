#pragma once
#include "Math/Transform.h"
#include "Math/Vector.h"
#include "Object.h"
#include "ObjectPtr.h"

#include "Actor.generated.h"

HCLASS()
class AActor : public HObject
{
    GENERATED_BODY(AActor)
protected:
    HPROPERTY()
    TArray<TObjectPtr<class CComponent>> Components;

    HPROPERTY()
    FTransform LocalTransform;

    FTransform WorldTransform;

    bool bTransformDirty = true;

    /**
     * @brief 标记 Transform 为 Dirty，需要更新
     */
    void MarkTransformDirty();

    /**
     * @brief 更新世界 Transform（由 TransformManager 调用）
     * @param ParentTransform 父级的世界 Transform（Actor 通常没有父级，传入单位 Transform）
     */
    virtual void UpdateWorldTransform(const FTransform& ParentTransform);

    // 友元声明，允许 TransformManager 访问 protected 成员
    friend class FTransformManager;

public:
    AActor();
    ~AActor() override;

    /////////////////////////////////////////////////////////////////////////////
    // Transform 设置函数（会标记为 Dirty）
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 设置本地位置
     * @param NewPosition 新的本地位置
     */
    void SetLocalPosition(const FVector3f& NewPosition);

    /**
     * @brief 设置本地旋转（欧拉角，弧度）
     * @param NewRotation 新的本地旋转（X=Pitch, Y=Yaw, Z=Roll）
     */
    void SetLocalRotation(const FVector3f& NewRotation);

    /**
     * @brief 设置本地缩放
     * @param NewScale 新的本地缩放
     */
    void SetLocalScale(const FVector3f& NewScale);

    /**
     * @brief 设置本地 Transform
     * @param NewTransform 新的本地 Transform
     */
    void SetLocalTransform(const FTransform& NewTransform);

    /**
     * @brief 添加本地位置偏移
     * @param Delta 位置偏移量
     */
    void AddLocalPosition(const FVector3f& Delta);

    /**
     * @brief 添加本地旋转（欧拉角增量，弧度）
     * @param Delta 旋转增量（X=Pitch增量, Y=Yaw增量, Z=Roll增量）
     */
    void AddLocalRotation(const FVector3f& Delta);

    /**
     * @brief 添加本地缩放
     * @param Delta 缩放增量
     */
    void AddLocalScale(const FVector3f& Delta);

    /////////////////////////////////////////////////////////////////////////////
    // Transform 获取函数
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 获取本地 Transform
     * @return 本地 Transform
     */
    const FTransform& GetLocalTransform() const
    {
        return LocalTransform;
    }

    /**
     * @brief 获取世界 Transform
     * @return 世界 Transform
     */
    const FTransform& GetWorldTransform() const
    {
        return WorldTransform;
    }

    /**
     * @brief 获取本地位置
     * @return 本地位置
     */
    FVector3f GetLocalPosition() const
    {
        return LocalTransform.Position;
    }

    /**
     * @brief 获取世界位置
     * @return 世界位置
     */
    FVector3f GetWorldPosition() const
    {
        return WorldTransform.Position;
    }

    /**
     * @brief 获取世界旋转（欧拉角，弧度）
     * @return 世界旋转（X=Pitch, Y=Yaw, Z=Roll）
     */
    FVector3f GetWorldRotation() const
    {
        return WorldTransform.Rotation;
    }

    /**
     * @brief 获取世界缩放
     * @return 世界缩放
     */
    FVector3f GetWorldScale() const
    {
        return WorldTransform.Scale;
    }

    /////////////////////////////////////////////////////////////////////////////
    // 世界坐标设置函数（通过修改本地 Transform 实现）
    /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 设置世界位置（通过修改本地 Transform 实现）
     * @param NewWorldPosition 新的世界位置
     * @note 此函数会计算所需的本地位置，不直接修改世界位置
     */
    void SetWorldPosition(const FVector3f& NewWorldPosition);

    /**
     * @brief 设置世界旋转（通过修改本地 Transform 实现）
     * @param NewWorldRotation 新的世界旋转（欧拉角，弧度）
     * @note 此函数会计算所需的本地旋转，不直接修改世界旋转
     */
    void SetWorldRotation(const FVector3f& NewWorldRotation);

    /**
     * @brief 设置世界缩放（通过修改本地 Transform 实现）
     * @param NewWorldScale 新的世界缩放
     * @note 此函数会计算所需的本地缩放，不直接修改世界缩放
     */
    void SetWorldScale(const FVector3f& NewWorldScale);

    /**
     * @brief 设置世界 Transform（通过修改本地 Transform 实现）
     * @param NewWorldTransform 新的世界 Transform
     * @note 此函数会计算所需的本地 Transform，不直接修改世界 Transform
     */
    void SetWorldTransform(const FTransform& NewWorldTransform);

    /**
     * @brief 检查 Transform 是否为 Dirty
     * @return 如果为 Dirty 返回 true
     */
    bool IsTransformDirty() const
    {
        return bTransformDirty;
    }

protected:
    /**
     * @brief Transform 更新后的回调（子类可以重写以扩展逻辑）
     */
    virtual void OnTransformUpdated() {}
};