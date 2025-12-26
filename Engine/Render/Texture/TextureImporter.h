#pragma once
#include "Object/AssetImporter.h"

class FTextureImporter : FAssetImporter
{
public:
    bool Import(FStringView Path, EAssetFileType FileType) override;
};
