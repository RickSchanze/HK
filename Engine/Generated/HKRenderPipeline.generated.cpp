#include "HKRenderPipeline.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Pipeline/HKRenderPipeline.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FHKRenderPipeline_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FHKRenderPipeline>("HKRenderPipeline");

    // 注册父类: FRenderPipeline
    Type->RegisterParent(FTypeManager::TypeOf<FRenderPipeline>());

}

void FHKRenderPipeline::Z_HKRenderPipeline_Register::Register_FHKRenderPipeline()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FHKRenderPipeline>(Register_FHKRenderPipeline_Impl);
}

#define FHKRenderPipeline_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(FHKRenderPipeline)

#undef FHKRenderPipeline_SERIALIZATION_CODE

