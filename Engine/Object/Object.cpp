#include "Object.h"

#include "Core/Utility/Profiler.h"
#include <mutex>

FObjectID FObjectArray::AllocateID()
{
    std::lock_guard<std::mutex> Lock(Mutex);

    // 确保数组至少有一个占位位置（索引0不使用）
    if (AllObjects.Size() == 0)
    {
        AllObjects.Resize(1, nullptr);    // 索引0位置占位，不使用
        OccupiedObjects.Resize(1, false); // 索引0位置始终为false
    }

    // 查找第一个空闲的索引（从1开始，因为0不使用）
    FObjectID NewID = 0;
    const FObjectID CurrentSize = OccupiedObjects.Size();

    // 从索引1开始查找空闲位置
    for (FObjectID I = 1; I < CurrentSize; ++I)
    {
        if (!OccupiedObjects.Test(I))
        {
            NewID = static_cast<FObjectID>(I);
            break;
        }
    }

    if (NewID == 0)
    {
        // 没有空闲位置，需要扩展
        NewID = static_cast<FObjectID>(CurrentSize);
        OccupiedObjects.Resize(CurrentSize + 1, false);
        AllObjects.Resize(CurrentSize + 1, nullptr);
    }

    // 标记为占用（ID即是索引）
    OccupiedObjects.Set(NewID);
    return NewID;
}

void FObjectArray::ReleaseID(FObjectID ID)
{
    std::lock_guard<std::mutex> Lock(Mutex);

    if (ID == INVALID_OBJECT_ID)
    {
        return;
    }

    // ID 即是索引，不需要减1
    const FObjectID Index = static_cast<FObjectID>(ID);
    if (Index < OccupiedObjects.Size())
    {
        OccupiedObjects.Clear(Index);
        AllObjects[Index] = nullptr;
    }
}

HObject* FObjectArray::CreateObject(FType ObjectType, FName NewName)
{
    if (ObjectType == nullptr || !ObjectType->CanCreateInstance())
    {
        return nullptr;
    }

    // 分配ID
    const FObjectID NewID = AllocateID();
    if (NewID == 0)
    {
        return nullptr;
    }

    // 创建对象实例（使用反射系统）
    void* RawPtr = ObjectType->CreateInstance();
    if (RawPtr == nullptr)
    {
        ReleaseID(NewID);
        return nullptr;
    }

    HObject* Object = static_cast<HObject*>(RawPtr);

    // 直接设置对象ID（通过友元访问）
    Object->ID = NewID;
    Object->Name = NewName;

    // 将对象存储到数组中（ID即是索引）
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        AllObjects[NewID] = Object;
    }

    return Object;
}

void FObjectArray::DestroyObject(HObject* Object)
{
    if (Object == nullptr)
    {
        return;
    }

    const FObjectID ID = Object->GetID();
    if (ID == INVALID_OBJECT_ID)
    {
        return;
    }

    // ID 即是 Index，不需要减1（索引0不使用）

    {
        const FObjectID Index = static_cast<FObjectID>(ID);
        std::lock_guard<std::mutex> Lock(Mutex);

        if (Index >= AllObjects.Size() || !OccupiedObjects.Test(Index))
        {
            return;
        }

        // 验证对象指针是否匹配
        if (AllObjects[Index] != Object)
        {
            return;
        }

        // 清除占用标记
        OccupiedObjects.Clear(Index);
        AllObjects[Index] = nullptr;
        NumObjects--;
    }

    // 销毁对象（使用模板Delete进行内存跟踪）
    Delete(Object);
}