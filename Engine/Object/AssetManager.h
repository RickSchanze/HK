#pragma once
#include "Core/Singleton/Singleton.h"
#include "Core/String/String.h"

class HObject;
class FAssetManager : public FSingleton<FAssetManager>
{
public:
    HObject* LoadAsset(FStringView AssetPath);

    template <typename T>
    T* LoadAsset(FStringView AssetPath)
    {

    }

private:

};