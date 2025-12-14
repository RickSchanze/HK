#pragma once

#include "Core/Container/Map.h"
#include "Core/String/Name.h"
#include "Core/Utility/Macros.h"

struct FTypeImpl;
struct FPropertyImpl;
struct FMethodImpl;
struct FMethodParamImpl;

typedef const FTypeImpl* FType;
typedef const FPropertyImpl* FProperty;
typedef const FMethodImpl* FMethod;
typedef const FMethodParamImpl* FMethodParam;

typedef TMap<FName, FName> FAttributeMap;

enum class ETypeFlags
{
    None = 0,
    Enum = 1 << 0,
};

HK_ENABLE_BITMASK_OPERATORS(ETypeFlags)
