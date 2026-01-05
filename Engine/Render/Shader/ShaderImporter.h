#pragma once
#include "Object/AssetImporter.h"

#include "ShaderImporter.generated.h"

HCLASS()
class FShaderImportSetting : public FAssetImportSetting
{
    GENERATED_BODY(FShaderImportSetting)
};

class FShaderImporter : public FAssetImporter
{
public:
    void BeginImport() override;
    bool ProcessAssetIntermediate() override;
    bool ProcessImport() override;
    void EndImport(bool Success) override;
};
