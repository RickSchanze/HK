#pragma once
#include "Core/Container/Bitmap.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Utility/Profiler.h"
#include <mutex>
#include <type_traits>

#include "Object.generated.h"

typedef UInt32 FObjectID;
constexpr inline FObjectID INVALID_OBJECT_ID = 0;

enum class EObjectFlags
{
    None = 0,
    Asset = 1 << 0,
};
HK_ENABLE_BITMASK_OPERATORS(EObjectFlags)

HCLASS()
class HK_API HObject
{
    GENERATED_BODY(HObject)
    friend class FObjectArray;

public:
    explicit HObject(const EObjectFlags InFlags = EObjectFlags::None) : Flags(InFlags) {}
    virtual ~HObject() = default;

    FObjectID GetID() const
    {
        return ID;
    }
    FName GetName() const
    {
        return Name;
    }

protected:
    HPROPERTY()
    FName Name;

private:
    HPROPERTY()
    FObjectID ID = INVALID_OBJECT_ID;

    HPROPERTY(Transient)
    EObjectFlags Flags = EObjectFlags::None;
};

class HK_API FObjectArray
{
public:
    HObject* CreateObject(FType ObjectType, FName NewName = FName("New Object"));

    template <typename T>
        requires std::is_base_of_v<HObject, T>
    T* CreateObject(FName Name = FName("New Object"))
    {
        static_assert(std::is_base_of_v<HObject, T>, "T must be derived from HObject");

        // 分配ID
        FObjectID NewID = AllocateID();
        if (NewID == 0)
        {
            return nullptr;
        }

        // 直接使用模板New创建对象（不使用反射，性能更好）
        T* Object = New<T>();
        if (Object == nullptr)
        {
            ReleaseID(NewID);
            return nullptr;
        }

        // 直接设置对象ID（通过友元访问）
        Object->ID = NewID;
        Object->Name = Name;

        // 将对象存储到数组中（ID即是索引，索引0不使用）
        {
            std::lock_guard<std::mutex> Lock(Mutex);
            AllObjects[NewID] = Object;
            NumObjects += 1;
        }

        return Object;
    }

    void DestroyObject(HObject* Object);

private:
    FObjectID AllocateID();
    void ReleaseID(FObjectID ID);

    mutable std::mutex Mutex;
    TArray<HObject*> AllObjects;
    FDynamicBitmap OccupiedObjects;

    Int32 NumObjects = 0;
};
