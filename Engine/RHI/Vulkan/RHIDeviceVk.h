#pragma once

#include "RHI/GfxDevice.h"
#include "vulkan/vulkan.hpp"

class FGfxDeviceVk : public FGfxDevice
{
public:
    FRHIBuffer CreateBuffer(const FRHIBufferCreateInfo& BufferCreateInfo) override;
    void DestroyBuffer(FRHIBuffer& Buffer) override;

    VkDevice GetDevice() const
    {
        return Device;
    }

private:
    void InitializeInstance();

    vk::Device Device;
    vk::Instance Instance;
};
