#pragma once
#include "Object/AssetImporter.h"

class FShaderImporter : public FAssetImporter
{
public:
    bool Import(FStringView Path, EAssetFileType FileType, EAssetImportOptions Options) override;
};
