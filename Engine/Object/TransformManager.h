#pragma once

#include "Core/Container/Array.h"
#include "Core/Singleton/Singleton.h"
#include "Math/Transform.h"
#include "ObjectPtr.h"

// 前向声明
class AActor;
class CSceneComponent;

/**
 * @brief Transform 管理器，统一管理所有 Transform 的更新
 * 使用单例模式，负责收集所有 Dirty 的 Transform 并统一更新
 */
class FTransformManager : public TSingleton<FTransformManager>
{
public:
    /**
     * @brief 启动函数（单例初始化时调用）
     */
    void StartUp() override;

    /**
     * @brief 关闭函数（单例销毁时调用）
     */
    void ShutDown() override;

    /**
     * @brief 注册需要更新的 Actor
     * @param Actor 需要更新的 Actor
     */
    void RegisterDirtyTransform(AActor* Actor);

    /**
     * @brief 注册需要更新的 SceneComponent
     * @param Component 需要更新的 SceneComponent
     */
    void RegisterDirtyTransform(CSceneComponent* Component);

    /**
     * @brief 更新所有 Dirty 的 Transform
     * 遍历所有 Dirty 的 Actor 和 Component，更新它们的世界 Transform
     */
    void UpdateTransforms();

    /**
     * @brief 清空所有待更新的 Transform 队列
     */
    void ClearDirtyQueue();

private:
    // Actor 更新队列
    TArray<TObjectPtr<AActor>> DirtyActors;

    // SceneComponent 更新队列
    TArray<TObjectPtr<CSceneComponent>> DirtyComponents;

    // 单位 Transform（用于 Actor，因为 Actor 通常没有父级）
    static const FTransform IdentityTransform;
};
