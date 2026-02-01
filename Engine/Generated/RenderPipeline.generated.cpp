#include "RenderPipeline.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Pipeline/RenderPipeline.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FRenderPipeline_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FRenderPipeline>("RenderPipeline");

    // 注册类型属性: Abstract
    Type->RegisterAttribute(FName("Abstract"), "");

}

void FRenderPipeline::Z_RenderPipeline_Register::Register_FRenderPipeline()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FRenderPipeline>(Register_FRenderPipeline_Impl);
}

#define FRenderPipeline_SERIALIZATION_CODE \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(FRenderPipeline)

#undef FRenderPipeline_SERIALIZATION_CODE

