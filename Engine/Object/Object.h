#pragma once
#include "Core/Reflection/Reflection.h"
#include "Core/Utility/Profiler.h"
#include <mutex>
#include <type_traits>

#include "Core/Singleton/Singleton.h"
#include "Object.generated.h"

typedef UInt32             FObjectID;
constexpr inline FObjectID INVALID_OBJECT_ID = 0;

enum class EObjectFlags
{
    None           = 0,
    Asset          = 1 << 0,
    Component      = 1 << 1,
    SceneComponent = 1 << 2,
    Actor          = 1 << 3,
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

    virtual UInt64 GetHashCode() const
    {
        return std::hash<const HObject*>{}(this);
    }

    EObjectFlags GetFlags() const
    {
        return Flags;
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

class HK_API FObjectArray : public TSingleton<FObjectArray>
{
public:
    void StartUp() override;
    void ShutDown() override;

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
        Object->ID   = NewID;
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

    /**
     * 通过名称查找对象
     * @param Name 对象名称
     * @return 找到的对象指针，如果未找到则返回 nullptr
     */
    HObject* FindObjectByName(FName Name) const;

    template <typename T>
    T* FindObjectByName(FName Name) const
    {
        HObject* Obj = FindObjectByName(Name);
        if (Obj)
        {
            return static_cast<T*>(Obj);
        }
        return nullptr;
    }

private:
    FObjectID AllocateID();
    void      ReleaseID(FObjectID ID);

    mutable std::mutex Mutex;
    TArray<HObject*>   AllObjects;

    Int32 NumObjects = 0;
};

template <typename T>
T* CreateObject(const FName Name = FName("New Object"))
{
    return FObjectArray::GetRef().CreateObject<T>(Name);
}

inline HObject* CreateObject(const FType ObjectType, const FName Name = FName("New Object"))
{
    return FObjectArray::GetRef().CreateObject(ObjectType, Name);
}
