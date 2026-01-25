//
// Created by Admin on 2026/1/24.
//

#include "TransformManager.h"
#include "Actor.h"
#include "SceneComponent.h"

const FTransform FTransformManager::IdentityTransform;

void FTransformManager::StartUp()
{
    DirtyActors.Clear();
    DirtyComponents.Clear();
}

void FTransformManager::ShutDown()
{
    ClearDirtyQueue();
}

void FTransformManager::RegisterDirtyTransform(AActor* Actor)
{
    if (Actor && Actor->IsTransformDirty())
    {
        // 检查是否已经在队列中
        if (DirtyActors.Find(Actor) == static_cast<size_t>(-1))
        {
            DirtyActors.Add(Actor);
        }
    }
}

void FTransformManager::RegisterDirtyTransform(CSceneComponent* Component)
{
    if (Component && Component->IsTransformDirty())
    {
        // 检查是否已经在队列中
        if (DirtyComponents.Find(Component) == static_cast<size_t>(-1))
        {
            DirtyComponents.Add(Component);
        }
    }
}

void FTransformManager::UpdateTransforms()
{
    // 先更新所有 Actor 的 Transform
    // Actor 的更新会触发其子 Component 的更新
    for (auto& ActorPtr : DirtyActors)
    {
        if (ActorPtr)
        {
            AActor* Actor = ActorPtr.Get();
            if (Actor && Actor->IsTransformDirty())
            {
                // Actor 通常没有父级，使用单位 Transform
                Actor->UpdateWorldTransform(IdentityTransform);
            }
        }
    }

    // 然后更新独立的 Component（不属于任何 Actor 的 Component，或者其 Owner 不在 DirtyActors 队列中）
    // 注意：如果 Component 的 Owner 在 DirtyActors 中，它已经在上面被更新了，这里跳过
    for (auto& CompPtr : DirtyComponents)
    {
        if (CompPtr)
        {
            CSceneComponent* Component = CompPtr.Get();
            if (Component && Component->IsTransformDirty())
            {
                // 检查 Owner 是否在 DirtyActors 队列中
                AActor* Owner         = Component->GetOwner();
                bool    OwnerWasDirty = false;
                if (Owner)
                {
                    // 检查 Owner 是否在 DirtyActors 队列中
                    for (const auto& ActorPtr : DirtyActors)
                    {
                        if (ActorPtr && ActorPtr.Get() == Owner)
                        {
                            OwnerWasDirty = true;
                            break;
                        }
                    }
                }

                // 如果 Owner 不在 DirtyActors 队列中，说明需要独立更新
                if (!OwnerWasDirty)
                {
                    if (Owner)
                    {
                        // 使用 Owner 的世界 Transform 作为父级
                        Component->UpdateWorldTransform(Owner->GetWorldTransform());
                    }
                    else
                    {
                        // 没有 Owner，使用单位 Transform
                        Component->UpdateWorldTransform(IdentityTransform);
                    }
                }
            }
        }
    }

    // 清空队列
    ClearDirtyQueue();
}

void FTransformManager::ClearDirtyQueue()
{
    DirtyActors.Clear();
    DirtyComponents.Clear();
}
