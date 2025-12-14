#include "TypeManager.h"
#include "Core/Logging/Logger.h"

FTypeManager& FTypeManager::Get()
{
    static FTypeManager Instance;
    return Instance;
}

FType FTypeManager::FindTypeByName(FName InName) const
{
    std::lock_guard<std::mutex> Lock(Mutex);
    FTypeImpl* const* Found = TypeMap.Find(InName);
    return Found != nullptr ? *Found : nullptr;
}

FType FTypeManager::FindTypeByName(const char* InName) const
{
    return FindTypeByName(FName(InName));
}

void FTypeManager::InitializeAllTypes()
{
    // 先收集所有注册函数，在锁外调用，避免死锁
    TArray<TypeRegistererFunc> RegistererFuncs;
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        for (const auto& Pair : TypeRegistererMap)
        {
            if (Pair.second != nullptr)
            {
                RegistererFuncs.Add(Pair.second);
            }
        }
    }
    
    // 在锁外调用注册函数
    for (TypeRegistererFunc Func : RegistererFuncs)
    {
        if (Func != nullptr)
        {
            Func();
        }
    }
}

FTypeManager::TypeCreateFunc FTypeManager::GetCreateFunc(FName InTypeName) const
{
    std::lock_guard<std::mutex> Lock(Mutex);
    const TypeCreateFunc* Found = TypeCreateMap.Find(InTypeName);
    return Found != nullptr ? *Found : nullptr;
}

FTypeManager::TypeDestroyFunc FTypeManager::GetDestroyFunc(FName InTypeName) const
{
    std::lock_guard<std::mutex> Lock(Mutex);
    const TypeDestroyFunc* Found = TypeDestroyMap.Find(InTypeName);
    return Found != nullptr ? *Found : nullptr;
}

// 自动注册基础类型（在首次使用时注册）
struct FTypeManagerAutoRegister
{
    FTypeManagerAutoRegister()
    {
        // 只注册基本类型：整数、浮点、布尔
        FTypeManager::Register<Int8>("Int8");
        FTypeManager::Register<Int16>("Int16");
        FTypeManager::Register<Int32>("Int32");
        FTypeManager::Register<Int64>("Int64");
        FTypeManager::Register<UInt8>("UInt8");
        FTypeManager::Register<UInt16>("UInt16");
        FTypeManager::Register<UInt32>("UInt32");
        FTypeManager::Register<UInt64>("UInt64");
        FTypeManager::Register<Float>("Float");
        FTypeManager::Register<Double>("Double");
        FTypeManager::Register<Bool>("Bool");
    }
};

static FTypeManagerAutoRegister GTypeManagerAutoRegister;

// TypeName模板特化 - 为已注册的基础类型提供名称映射
// 注意：这需要在类型注册后调用，或者使用其他机制
// 暂时不实现特化，因为类型名在Register时已经确定
