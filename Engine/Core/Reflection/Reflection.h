#pragma once
#include "Any.h"
#include "AnyRef.h"
#include "Core/Serialization/BinaryArchive.h"
#include "Core/Serialization/JsonArchive.h"
#include "Core/Serialization/Serialization.h"
#include "Core/Serialization/XMLArchive.h"
#include "Property.h"
#include "ReflectionFwd.h"
#include "Type.h"
#include "TypeManager.h"

#define HCLASS(...)
#define HPROPERTY(...)
#define HENUM(...)
#define HSTRUCT(...)

#define GENERATED_BODY(ClassName)                                                                                      \
public:                                                                                                                \
GENERATED_HEADER_##ClassName private:

// 用于在.generated.h中声明六个Serialize函数
#define HK_DECL_CLASS_SERIALIZATION(ClassName)                                                                         \
    virtual void Serialize(cereal::JSONOutputArchive& Ar);                                                             \
    virtual void Serialize(cereal::JSONInputArchive& Ar);                                                              \
    virtual void Serialize(FXMLInputArchive& Ar);                                                                      \
    virtual void Serialize(FXMLOutputArchive& Ar);                                                                     \
    virtual void Serialize(FBinaryInputArchive& Ar);                                                                   \
    virtual void Serialize(FBinaryOutputArchive& Ar);

// 用于在.generated.cpp中定义六个Serialize函数
// 需要先定义 {ClassName}_SERIALIZATION_CODE 宏，包含实际的序列化代码
#define HK_DEFINE_CLASS_SERIALIZATION(ClassName)                                                                       \
    void ClassName::Serialize(cereal::JSONOutputArchive& Ar)                                                           \
    {                                                                                                                  \
        ClassName##_SERIALIZATION_CODE                                                                                 \
    }                                                                                                                  \
    void ClassName::Serialize(cereal::JSONInputArchive& Ar)                                                            \
    {                                                                                                                  \
        ClassName##_SERIALIZATION_CODE                                                                                 \
    }                                                                                                                  \
    void ClassName::Serialize(FXMLInputArchive& Ar)                                                                    \
    {                                                                                                                  \
        ClassName##_SERIALIZATION_CODE                                                                                 \
    }                                                                                                                  \
    void ClassName::Serialize(FXMLOutputArchive& Ar)                                                                   \
    {                                                                                                                  \
        ClassName##_SERIALIZATION_CODE                                                                                 \
    }                                                                                                                  \
    void ClassName::Serialize(FBinaryInputArchive& Ar)                                                                 \
    {                                                                                                                  \
        ClassName##_SERIALIZATION_CODE                                                                                 \
    }                                                                                                                  \
    void ClassName::Serialize(FBinaryOutputArchive& Ar)                                                                \
    {                                                                                                                  \
        ClassName##_SERIALIZATION_CODE                                                                                 \
    }