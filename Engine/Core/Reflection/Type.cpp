#include "Type.h"
#include "Method.h"
#include "Property.h"
#include "TypeManager.h"

TArray<FProperty> FTypeImpl::GetAllProperties() const
{
    TArray<FProperty> AllProperties = Properties;
    // 递归获取所有父类的属性
    for (FType Base : Bases)
    {
        if (Base != nullptr)
        {
            TArray<FProperty> BaseProperties = Base->GetAllProperties();
            AllProperties.Append(BaseProperties.begin(), BaseProperties.end());
        }
    }
    return AllProperties;
}

TArray<FMethod> FTypeImpl::GetAllMethods() const
{
    TArray<FMethod> AllMethods = Methods;
    // 递归获取所有父类的方法
    for (FType Base : Bases)
    {
        if (Base != nullptr)
        {
            TArray<FMethod> BaseMethods = Base->GetAllMethods();
            AllMethods.Append(BaseMethods.begin(), BaseMethods.end());
        }
    }
    return AllMethods;
}

bool FTypeImpl::IsSubclassOf(FType InBaseType) const
{
    if (InBaseType == nullptr)
        return false;
    if (this == InBaseType)
        return true;
    for (FType Base : Bases)
    {
        if (Base != nullptr && Base->IsSubclassOf(InBaseType))
            return true;
    }
    return false;
}

bool FTypeImpl::IsDerivedFrom(FType InBaseType) const
{
    return IsSubclassOf(InBaseType);
}

void* FTypeImpl::CreateInstance() const
{
    if (!CanCreateInstance())
    {
        return nullptr;
    }
    // 通过TypeManager查找创建函数
    FTypeManager& Manager = FTypeManager::Get();
    FTypeManager::TypeCreateFunc CreateFunc = Manager.GetCreateFunc(Name);
    if (CreateFunc != nullptr)
    {
        return CreateFunc();
    }
    return nullptr;
}

void FTypeImpl::DestroyInstance(void* InInstance) const
{
    if (InInstance == nullptr)
    {
        return;
    }
    // 通过TypeManager查找销毁函数
    FTypeManager& Manager = FTypeManager::Get();
    FTypeManager::TypeDestroyFunc DestroyFunc = Manager.GetDestroyFunc(Name);
    if (DestroyFunc != nullptr)
    {
        DestroyFunc(InInstance);
    }
}
