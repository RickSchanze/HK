#pragma once

#include "Core/Container/Array.h"
#include "Core/String/String.h"
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

    vk::Instance GetInstance() const
    {
        return Instance;
    }

private:
    void CreateInstance();

    /// 检查扩展是否可用
    /// @param RequiredExtensions
    /// @return
    static bool CheckInstanceExtensionSupport(const TArray<const char*>& RequiredExtensions);

    /// 获取需要的Extension
    /// @return
    TArray<const char*> GetRequiredExtensions();

    /// 检查验证层的可用性
    /// @param RequiredLayers
    /// @return
    static bool CheckValidationLayerSupport(const TArray<const char*>& RequiredLayers);

    /// 获取验证层Layer名字
    /// @return
    static TArray<const char*> GetRequiredValidationLayers();

    vk::Device Device;
    vk::Instance Instance;
    bool bValidationLayersEnabled = false;
};
