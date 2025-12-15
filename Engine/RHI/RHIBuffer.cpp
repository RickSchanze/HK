//
// Created by Admin on 2025/12/15.
//

#include "RHIBuffer.h"
#include "Core/String/String.h"

void FRHIBuffer::Destroy()
{
    if (!IsValid())
    {
        return;
    }

    // 销毁句柄
    FRHIHandleManager::GetRef().DestroyRHIHandle(Handle);

    Handle = FRHIHandle();
    Size = 0;
    Usage = EBufferUsage::None;
    MemoryProperty = EBufferMemoryProperty::None;
    MappedPtr = nullptr;
}

void* FRHIBuffer::Map(UInt64 Offset, UInt64 MapSize)
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

    // TODO: 这里应该调用实际的图形 API 来映射内存
    // MappedPtr = MapNativeBuffer(Handle.Handle, Offset, MapSize);

    // 临时实现：返回 nullptr，实际实现中应该返回映射的指针
    MappedPtr = nullptr;

    return MappedPtr;
}

void FRHIBuffer::Unmap()
{
    if (!IsMapped())
    {
        return;
    }

    // TODO: 这里应该调用实际的图形 API 来取消映射内存
    // UnmapNativeBuffer(Handle.Handle);

    MappedPtr = nullptr;
}
