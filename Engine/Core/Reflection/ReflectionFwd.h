#pragma once

#include "Core/Container/Map.h"
#include "Core/String/Name.h"
#include "Core/Utility/Macros.h"

struct FTypeImpl;
struct FPropertyImpl;
struct FMethodImpl;
struct FMethodParamImpl;

typedef const FTypeImpl* FType;
typedef FPropertyImpl* FProperty;
typedef const FMethodImpl* FMethod;
typedef const FMethodParamImpl* FMethodParam;

// 注册时返回的可变类型指针
typedef FTypeImpl* FTypeMutable;

typedef TMap<FName, FStringView> FAttributeMap;

enum class ETypeFlags
{
    None = 0,
    Enum = 1 << 0,
};
HK_ENABLE_BITMASK_OPERATORS(ETypeFlags)

enum class EPropertyFlags
{
    None = 0,
    Array = 1 << 0,
    FixedArray = 1 << 1,
    Map = 1 << 2,
    Enum = 1 << 3,
};
HK_ENABLE_BITMASK_OPERATORS(EPropertyFlags)