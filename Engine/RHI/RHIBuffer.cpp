//
// Created by Admin on 2025/12/15.
//

#include "RHIBuffer.h"
#include "RHI/GfxDevice.h"

void* FRHIBuffer::Map(const UInt64 Offset, UInt64 MapSize)
{
    HK_ASSERT_MSG_RAW(IsValid(), "Cannot map invalid buffer");
    HK_ASSERT_MSG_RAW(!IsMapped(), "Buffer is already mapped");
    HK_ASSERT_MSG_RAW(HasFlag(MemoryProperty, EBufferMemoryProperty::HostVisible),
                      "Buffer must have HostVisible memory property to be mapped");

    if (MapSize == 0)
    {
        MapSize = Size - Offset;
    }

    HK_ASSERT_MSG_RAW(Offset + MapSize <= Size, "Map range exceeds buffer size");

    // 通过 GfxDevice 映射内存
    if (FGfxDevice* GfxDevice = GetRHIDevice())
    {
        MappedPtr = GfxDevice->MapBuffer(*this, Offset, MapSize);
    }
    else
    {
        HK_ASSERT_MSG_RAW(false, "GfxDevice未初始化，无法映射Buffer");
        MappedPtr = nullptr;
    }

    return MappedPtr;
}

void FRHIBuffer::Unmap()
{
    if (!IsMapped())
    {
        return;
    }

    // 通过 GfxDevice 取消映射内存
    if (FGfxDevice* GfxDevice = GetRHIDevice())
    {
        GfxDevice->UnmapBuffer(*this);
    }
    else
    {
        // 即使 GfxDevice 未初始化，也清空映射指针
        MappedPtr = nullptr;
    }
}
