#pragma once

#include "Core/Utility/Macros.h"

#define GENERATED_HEADER_HObject                                                                                        \
    struct Z_HObject_Register                                                                                            \
    {                                                                                                                  \
        Z_HObject_Register()                                                                                             \
        {                                                                                                              \
            Register_HObject();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_HObject();                                                                                 \
    };                                                                                                                 \
    typedef HObject ThisClass;                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(HObject)                                                                                        \
    static void Register_HObject_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&HObject::Name, "Name");                                                                                        \
        Type->RegisterProperty(&HObject::ID, "ID");                                                                                        \
    }                                                                                        \
    static inline Z_HObject_Register Z_REGISTERER_HOBJECT;
