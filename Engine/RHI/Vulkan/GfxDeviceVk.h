#pragma once

#include "Core/Container/Array.h"
#include "Core/String/String.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHIBuffer.h"
#include "vulkan/vulkan.hpp"

class FGfxDeviceVk : public FGfxDevice
{
public:
    void Init() override;
    void UnInit() override;

#pragma region Buffer操作
    FRHIBuffer CreateBuffer(const FRHIBufferDesc& BufferCreateInfo) override;
    void DestroyBuffer(FRHIBuffer& Buffer) override;
    void* MapBuffer(FRHIBuffer& Buffer, UInt64 Offset, UInt64 Size) override;
    void UnmapBuffer(FRHIBuffer& Buffer) override;
#pragma endregion

#pragma region 窗口操作
    void CreateMainWindowSurface(FName MainWindowName, FVector2i MainWindowInitSize,
                                 FRHIWindow& OutMainWindow) override;
    void CreateMainWindowSwapChain(FRHIWindow& OutMainWindow) override;
    void CreateRHIWindow(FName Name, FVector2i Size, FRHIWindow& OutWindow) override;
    void DestroyMainWindow(FRHIWindow& MainWindow) override;
    void DestroyRHIWindow(FRHIWindow& Window) override;
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

    /**
     * 获取需要的Extension
     * @return
     */
    static TArray<const char*> GetRequiredExtensions();

    /**
     * 检查验证层的可用性
     * @param RequiredLayers
     * @return
     */
    static bool CheckValidationLayerSupport(const TArray<const char*>& RequiredLayers);

    /**
     * 获取验证层Layer名字
     * @return
     */
    static TArray<const char*> GetRequiredValidationLayers();

    /**
     * 选择物理设备
     */
    void SelectPhysicalDevice();

    /**
     * 获取需要的设备扩展
     * @return
     */
    static TArray<const char*> GetRequiredDeviceExtensions();

    /**
     * 检查设备是否适合
     * @param Device 物理设备
     * @return
     */
    bool IsDeviceSuitable(vk::PhysicalDevice Device) const;

    /**
     * 查找队列族
     * @param Device 物理设备
     * @return 队列族索引（图形队列、呈现队列等）
     */
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

    /**
     * 转换 EBufferUsage 到 VkBufferUsageFlags
     * @param Usage 缓冲区使用标志
     * @return Vulkan 缓冲区使用标志
     */
    static vk::BufferUsageFlags ConvertBufferUsage(EBufferUsage Usage);

    /**
     * 转换 EBufferMemoryProperty 到 VkMemoryPropertyFlags
     * @param MemoryProperty 内存属性
     * @return Vulkan 内存属性标志
     */
    static vk::MemoryPropertyFlags ConvertMemoryProperty(EBufferMemoryProperty MemoryProperty);

    /**
     * 查找合适的内存类型索引
     * @param TypeFilter 内存类型过滤器
     * @param Properties 所需的内存属性
     * @return 内存类型索引，失败返回 -1
     */
    UInt32 FindMemoryType(UInt32 TypeFilter, vk::MemoryPropertyFlags Properties) const;

    /**
     * 设置 Vulkan 对象的 DebugName
     * @param ObjectHandle 对象句柄
     * @param ObjectType 对象类型
     * @param Name 调试名称
     */
    void SetDebugName(vk::DeviceMemory ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::Buffer ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;

    vk::PhysicalDevice PhysicalDevice;
    vk::Device Device;
    vk::Instance Instance;
    vk::SurfaceKHR MainWindowSurface;
    vk::Queue GraphicsQueue;
    vk::Queue PresentQueue;
    FQueueFamilyIndices QueueFamilyIndices;
    bool bValidationLayersEnabled = false;
    bool bDebugUtilsExtensionAvailable = false; // Debug Utils扩展是否可用
};
