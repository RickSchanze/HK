#pragma once

#include "Core/Container/Array.h"
#include "Core/String/String.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHIBuffer.h"
#include "vulkan/vulkan.hpp"

class FGfxDeviceVk : public FGfxDevice
{
public:
    void Initialize() override;
    void Uninitialize() override;

    FRHIBuffer CreateBuffer(const FRHIBufferCreateInfo& BufferCreateInfo) override;
    void DestroyBuffer(FRHIBuffer& Buffer) override;

#pragma region 创建Surface
    void CreateMainWindowSurface(FName MainWindowName, FVector2i MainWindowInitSize,
                                 FRHIWindow& OutMainWindow) override;
    void CreateMainWindowSwapChain(FRHIWindow& OutMainWindow) override;
    void CreateWindow(FName Name, FVector2i Size, FRHIWindow& OutWindow) override;
    void DestroyMainWindow(FRHIWindow& MainWindow) override;
    void DestroyWindow(FRHIWindow& Window) override;
    void OpenWindow(FRHIWindow& Window) override;
    void CloseWindow(FRHIWindow& Window) override;
#pragma endregion

    VkDevice GetDevice() const
    {
        return Device;
    }

    vk::Instance GetInstance() const
    {
        return Instance;
    }

private:
    /**
     * 创建VkInstance
     */
    void CreateInstance();

    /**
     * 创建VkDevice
     */
    void CreateDevice();

    /**
     * 创建主窗口和Surface（内部辅助方法）
     * @param MainWindowName 主窗口名称
     * @param MainWindowInitSize 主窗口初始大小
     */
    void CreateMainWindowAndSurface(FName MainWindowName, FVector2i MainWindowInitSize);

    /// 检查扩展是否可用
    /// @param RequiredExtensions
    /// @return
    static bool CheckInstanceExtensionSupport(const TArray<const char*>& RequiredExtensions);

    /// 获取需要的Extension
    /// @return
    static TArray<const char*> GetRequiredExtensions();

    /// 检查验证层的可用性
    /// @param RequiredLayers
    /// @return
    static bool CheckValidationLayerSupport(const TArray<const char*>& RequiredLayers);

    /// 获取验证层Layer名字
    /// @return
    static TArray<const char*> GetRequiredValidationLayers();

    /// 选择物理设备
    void SelectPhysicalDevice();

    /// 检查设备扩展支持
    /// @param Device 物理设备
    /// @param RequiredExtensions 需要的扩展列表
    /// @return
    static bool CheckDeviceExtensionSupport(vk::PhysicalDevice Device, const TArray<const char*>& RequiredExtensions);

    /// 获取需要的设备扩展
    /// @return
    static TArray<const char*> GetRequiredDeviceExtensions();

    /// 检查设备是否适合
    /// @param Device 物理设备
    /// @return
    bool IsDeviceSuitable(vk::PhysicalDevice Device) const;

    /// 查找队列族
    /// @param Device 物理设备
    /// @return 队列族索引（图形队列、呈现队列等）
    struct FQueueFamilyIndices
    {
        Int32 GraphicsFamily = -1;
        Int32 PresentFamily = -1;

        bool IsComplete() const
        {
            return GraphicsFamily >= 0 && PresentFamily >= 0;
        }
    };
    FQueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice Device) const;

    vk::PhysicalDevice PhysicalDevice;
    vk::Device Device;
    vk::Instance Instance;
    vk::SurfaceKHR MainWindowSurface;
    vk::Queue GraphicsQueue;
    vk::Queue PresentQueue;
    FQueueFamilyIndices QueueFamilyIndices;
    bool bValidationLayersEnabled = false;
};
