#pragma once

#include "Config/IConfig.h"

#include "RenderConfig.generated.h"

HENUM()
enum class ERenderCommandCommitStyle
{
    Immediate,     // 立即提交
    Deferred,      // 先扔进队列, 最后在提交
    MultiThreaded, // 扔进渲染命令提交队列, 改队列在渲染线程执行
    Task,          // 多个线程一起提交
    Count,
};

HCLASS(ConfigPath = "Config/RenderConfig.xml")
class FRenderConfig : public IConfig
{
    GENERATED_BODY(FRenderConfig)
public:
    const TArray<FString>& GetShaderPaths() const
    {
        return ShaderPaths;
    }

private:
    HPROPERTY()
    TArray<FString> ShaderPaths;

    // 如果是MuliThreaded, 则开启多线程渲染
    HPROPERTY(DefaultProperty)
    ERenderCommandCommitStyle CommitStyle = ERenderCommandCommitStyle::Immediate;
};

