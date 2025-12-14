#pragma once
#include "Core/String/String.h"
#include "Core/Time/Time.h"
#include "Core/Utility/Macros.h"

#define HK_LOGCAT_LIST                                                                                                 \
    HK_LOGCAT_ITEM(Engine)                                                                                             \
    HK_LOGCAT_ITEM(Assert)                                                                                             \
    HK_LOGCAT_ITEM(Reflection)                                                                                         \
    HK_LOGCAT_ITEM(Test)

enum class ELogcat
{
#define HK_LOGCAT_ITEM(name) name,
    HK_LOGCAT_LIST
#undef HK_LOGCAT_ITEM
};

inline const char* GetLogcatString(ELogcat InLogcat)
{
#define HK_LOGCAT_ITEM(name)                                                                                           \
    case ELogcat::name:                                                                                                \
        return #name;
    switch (InLogcat)
    {
        HK_LOGCAT_LIST
    }
#undef HK_LOGCAT_ITEM
    return nullptr;
}

enum class ELogLevel
{
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
};

struct FLogContent
{
    ELogLevel Level;
    ELogcat Logcat;
    FString Message;
    Int32 ThreadID;
    FTimePoint Time;
};
