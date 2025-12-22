//
// Created by Admin on 2025/12/22.
//

#include "Uuid.h"

FUuid FUuid::New()
{
    FUuid NewUuid;
    NewUuid.Uuid = uuids::uuid_system_generator{}.operator()();
    return NewUuid;
}