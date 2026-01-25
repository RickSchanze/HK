//
// Created by Admin on 2026/1/24.
//

#include "SceneComponent.h"
#include "Actor.h"
#include "TransformManager.h"

CSceneComponent::CSceneComponent() : CComponent(EObjectFlags::SceneComponent), bTransformDirty(true) {}

CSceneComponent::~CSceneComponent() {}

void CSceneComponent::MarkTransformDirty()
{
    if (!bTransformDirty)
    {
        bTransformDirty = true;
        // 注册到 TransformManager 的更新队列
        FTransformManager::Get()->RegisterDirtyTransform(this);
    }
}

void CSceneComponent::UpdateWorldTransform(const FTransform& ParentTransform)
{
    // 计算世界 Transform：世界 = 父级 * 相对
    WorldTransform  = ParentTransform * LocalTransform;
    bTransformDirty = false;

    // 子类可以重写此函数以扩展更新逻辑
    OnTransformUpdated();
}

void CSceneComponent::SetLocalPosition(const FVector3f& NewPosition)
{
    LocalTransform.Position = NewPosition;
    MarkTransformDirty();
}

void CSceneComponent::SetLocalRotation(const FVector3f& NewRotation)
{
    LocalTransform.Rotation = NewRotation;
    MarkTransformDirty();
}

void CSceneComponent::SetLocalScale(const FVector3f& NewScale)
{
    LocalTransform.Scale = NewScale;
    MarkTransformDirty();
}

void CSceneComponent::SetLocalTransform(const FTransform& NewTransform)
{
    LocalTransform = NewTransform;
    MarkTransformDirty();
}

void CSceneComponent::AddLocalPosition(const FVector3f& Delta)
{
    LocalTransform.Position += Delta;
    MarkTransformDirty();
}

void CSceneComponent::AddLocalRotation(const FVector3f& Delta)
{
    LocalTransform.Rotation += Delta;
    MarkTransformDirty();
}

void CSceneComponent::AddLocalScale(const FVector3f& Delta)
{
    LocalTransform.Scale *= Delta;
    MarkTransformDirty();
}

void CSceneComponent::SetWorldPosition(const FVector3f& NewWorldPosition)
{
    // 获取父级 Transform（Owner 的世界 Transform）
    AActor*    Owner = GetOwner();
    FTransform ParentTransform;
    if (Owner)
    {
        ParentTransform = Owner->GetWorldTransform();
    }

    // 计算相对位置：相对位置 = 父级逆变换 * 世界位置
    FTransform InverseParent = ParentTransform.Inverse();
    FVector3f  RelativePos   = InverseParent.TransformPoint(NewWorldPosition);
    LocalTransform.Position  = RelativePos;
    MarkTransformDirty();
}

void CSceneComponent::SetWorldRotation(const FVector3f& NewWorldRotation)
{
    // 获取父级 Transform
    AActor*    Owner = GetOwner();
    FTransform ParentTransform;
    if (Owner)
    {
        ParentTransform = Owner->GetWorldTransform();
    }

    // 计算相对旋转：通过矩阵方式计算
    // 目标世界 Transform
    FTransform DesiredWorldTransform;
    DesiredWorldTransform.Position = WorldTransform.Position; // 保持位置不变（暂时）
    DesiredWorldTransform.Rotation = NewWorldRotation;
    DesiredWorldTransform.Scale    = WorldTransform.Scale; // 保持缩放不变（暂时）

    // 计算：相对 Transform = 父级逆变换 * 世界 Transform
    FTransform InverseParent     = ParentTransform.Inverse();
    FTransform NewLocalTransform = InverseParent * DesiredWorldTransform;

    // 只更新旋转部分
    LocalTransform.Rotation = NewLocalTransform.Rotation;
    MarkTransformDirty();
}

void CSceneComponent::SetWorldScale(const FVector3f& NewWorldScale)
{
    // 获取父级 Transform
    AActor*    Owner = GetOwner();
    FTransform ParentTransform;
    if (Owner)
    {
        ParentTransform = Owner->GetWorldTransform();
    }

    // 计算相对缩放：相对缩放 = 世界缩放 / 父级缩放
    FVector3f ParentScale = ParentTransform.Scale;
    if (std::abs(ParentScale.X) > 1e-8f && std::abs(ParentScale.Y) > 1e-8f && std::abs(ParentScale.Z) > 1e-8f)
    {
        LocalTransform.Scale.X = NewWorldScale.X / ParentScale.X;
        LocalTransform.Scale.Y = NewWorldScale.Y / ParentScale.Y;
        LocalTransform.Scale.Z = NewWorldScale.Z / ParentScale.Z;
    }
    else
    {
        // 父级缩放接近 0，无法计算，保持当前相对缩放
        // 或者可以设置为目标缩放（如果父级没有缩放）
        LocalTransform.Scale = NewWorldScale;
    }
    MarkTransformDirty();
}

void CSceneComponent::SetWorldTransform(const FTransform& NewWorldTransform)
{
    // 获取父级 Transform
    AActor*    Owner = GetOwner();
    FTransform ParentTransform;
    if (Owner)
    {
        ParentTransform = Owner->GetWorldTransform();
    }

    // 计算相对 Transform：相对 Transform = 父级逆变换 * 世界 Transform
    FTransform InverseParent = ParentTransform.Inverse();
    LocalTransform           = InverseParent * NewWorldTransform;
    MarkTransformDirty();
}
