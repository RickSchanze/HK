#pragma once
#include "Core/Container/Map.h"
#include "Core/Singleton/Singleton.h"

class HTexture;
struct FGlobalTexturePool : TSingleton<FGlobalTexturePool>
{
    TMap<HTexture*, UInt16> TextureMap;

    void AddTexture(HTexture* InTexture)
    {

    }
};
