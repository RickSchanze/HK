#pragma once
#include "Core/Reflection/Reflection.h"

#include "IConfig.generated.h"

HCLASS(Interface)
class HK_API IConfig
{
    GENERATED_BODY(IConfig)
public:
    virtual ~IConfig() = default;

    /// 将配置保存至文件, 文件路径由属性ConfigPath给出
    /// 没有的话默认存在Config/Engine.xml里
    /// 可以使用ConfigName属性来标注此配置的类别
    void Save();

protected:
    virtual void PostSave() {}
    virtual void PreSave() {}
};
