#include "Mesh.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Mesh/Mesh.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_HMesh_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<HMesh>("HMesh");

    // 注册父类: HAsset
    Type->RegisterParent(FTypeManager::TypeOf<HAsset>());

}

void HMesh::Z_HMesh_Register::Register_HMesh()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<HMesh>(Register_HMesh_Impl);
}

#define HMesh_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(HMesh)

#undef HMesh_SERIALIZATION_CODE

