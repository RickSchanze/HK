//
// Created by Admin on 2026/1/24.
//

#include "Actor.h"

#include "Component.h"
#include "SceneComponent.h"
#include "TransformManager.h"

AActor::AActor() : HObject(EObjectFlags::Actor) {}

AActor::~AActor() {}

void AActor::MarkTransformDirty()
{
    if (!bTransformDirty)
    {
        bTransformDirty = true;
        // 注册到 TransformManager 的更新队列
        FTransformManager::Get()->RegisterDirtyTransform(this);
    }
}

void AActor::UpdateWorldTransform(const FTransform& ParentTransform)
{
    // Actor 通常没有父级，所以直接使用相对 Transform 作为世界 Transform
    // 如果有父级系统，可以在这里实现
    WorldTransform  = ParentTransform * LocalTransform;
    bTransformDirty = false;

    // 子类可以重写此函数以扩展更新逻辑
    OnTransformUpdated();

    // 更新所有 SceneComponent 的世界 Transform
    for (auto& Component : Components)
    {
        if (Component && Component->IsActive() && HasFlag(Component->GetFlags(), EObjectFlags::SceneComponent))
        {
            auto* SceneComp = static_cast<CSceneComponent*>(Component.Get());
            SceneComp->UpdateWorldTransform(WorldTransform);
        }
    }
}

void AActor::SetLocalPosition(const FVector3f& NewPosition)
{
    LocalTransform.Position = NewPosition;
    MarkTransformDirty();
}

void AActor::SetLocalRotation(const FVector3f& NewRotation)
{
    LocalTransform.Rotation = NewRotation;
    MarkTransformDirty();
}

void AActor::SetLocalScale(const FVector3f& NewScale)
{
    LocalTransform.Scale = NewScale;
    MarkTransformDirty();
}

void AActor::SetLocalTransform(const FTransform& NewTransform)
{
    LocalTransform = NewTransform;
    MarkTransformDirty();
}

void AActor::AddLocalPosition(const FVector3f& Delta)
{
    LocalTransform.Position += Delta;
    MarkTransformDirty();
}

void AActor::AddLocalRotation(const FVector3f& Delta)
{
    LocalTransform.Rotation += Delta;
    MarkTransformDirty();
}

void AActor::AddLocalScale(const FVector3f& Delta)
{
    LocalTransform.Scale *= Delta;
    MarkTransformDirty();
}

void AActor::SetWorldPosition(const FVector3f& NewWorldPosition)
{
    // Actor 通常没有父级，所以相对位置 = 世界位置
    LocalTransform.Position = NewWorldPosition;
    MarkTransformDirty();
}

void AActor::SetWorldRotation(const FVector3f& NewWorldRotation)
{
    // Actor 通常没有父级，所以相对旋转 = 世界旋转
    LocalTransform.Rotation = NewWorldRotation;
    MarkTransformDirty();
}

void AActor::SetWorldScale(const FVector3f& NewWorldScale)
{
    // Actor 通常没有父级，所以相对缩放 = 世界缩放
    LocalTransform.Scale = NewWorldScale;
    MarkTransformDirty();
}

void AActor::SetWorldTransform(const FTransform& NewWorldTransform)
{
    // Actor 通常没有父级，所以相对 Transform = 世界 Transform
    LocalTransform = NewWorldTransform;
    MarkTransformDirty();
}
