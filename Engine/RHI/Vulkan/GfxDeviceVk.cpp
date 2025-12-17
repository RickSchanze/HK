//
// Created by Admin on 2025/12/15.
//

#include "GfxDeviceVk.h"
#include "Core/Logging/Logger.h"
#include <stdexcept>

void FGfxDeviceVk::Initialize() {}

void FGfxDeviceVk::Uninitialize() {}

FRHIBuffer FGfxDeviceVk::CreateBuffer(const FRHIBufferCreateInfo& BufferCreateInfo)
{

}

void FGfxDeviceVk::DestroyBuffer(FRHIBuffer& Buffer) {}

void FGfxDeviceVk::CreateMainWindowSurface(FName MainWindowName, FVector2i MainWindowInitSize,
                                           FRHIWindow& OutMainWindow)
{
}

void FGfxDeviceVk::CreateMainWindowSwapChain(FRHIWindow& OutMainWindow) {}

void FGfxDeviceVk::CreateWindow(FName Name, FVector2i Size, FRHIWindow& OutWindow)
{
    FGfxDevice::CreateWindow(Name, Size, OutWindow);
}
void FGfxDeviceVk::DestroyMainWindow(FRHIWindow& MainWindow) {}
void FGfxDeviceVk::DestroyWindow(FRHIWindow& Window) {}

void FGfxDeviceVk::CreateInstance()
{
    // 获取需要的扩展列表
    TArray<const char*> RequiredExtensions = GetRequiredExtensions();

    // 检查扩展支持
    if (!CheckInstanceExtensionSupport(RequiredExtensions))
    {
        FString MissingExtensions;
        for (const char* Ext : RequiredExtensions)
        {
            if (!MissingExtensions.IsEmpty())
            {
                MissingExtensions += ", ";
            }
            MissingExtensions += Ext;
        }
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan: 不支持的实例扩展: {}", MissingExtensions.CStr());
        throw std::runtime_error("Vulkan实例扩展不支持");
    }

    // 获取验证层列表
    TArray<const char*> ValidationLayers = GetRequiredValidationLayers();
    bValidationLayersEnabled = !ValidationLayers.IsEmpty() && CheckValidationLayerSupport(ValidationLayers);

    if (!bValidationLayersEnabled && !ValidationLayers.IsEmpty())
    {
        HK_LOG_WARN(ELogcat::RHI, "Vulkan: 验证层不可用，将在没有验证层的情况下运行");
    }

    // 创建应用信息
    vk::ApplicationInfo AppInfo;
    AppInfo.pApplicationName = "HKEngine";
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = "HKEngine";
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_3;

    // 创建实例创建信息
    vk::InstanceCreateInfo CreateInfo;
    CreateInfo.pApplicationInfo = &AppInfo;

    // 设置扩展
    CreateInfo.enabledExtensionCount = static_cast<uint32_t>(RequiredExtensions.Size());
    CreateInfo.ppEnabledExtensionNames = RequiredExtensions.Data();

    // 设置验证层
    if (bValidationLayersEnabled)
    {
        CreateInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.Size());
        CreateInfo.ppEnabledLayerNames = ValidationLayers.Data();
    }
    else
    {
        CreateInfo.enabledLayerCount = 0;
    }

    // 创建实例
    try
    {
        Instance = vk::createInstance(CreateInfo);
        HK_LOG_INFO(ELogcat::RHI, "Vulkan实例创建成功");

        if (bValidationLayersEnabled)
        {
            HK_LOG_INFO(ELogcat::RHI, "Vulkan验证层已启用");
        }
    }
    catch (const vk::SystemError& e)
    {
        const FString ErrorMsg = FString("Vulkan实例创建失败: ") + FString(e.what());
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan实例创建失败: {}", e.what());
        Instance = nullptr;
        throw std::runtime_error(ErrorMsg.CStr());
    }
}

bool FGfxDeviceVk::CheckInstanceExtensionSupport(const TArray<const char*>& RequiredExtensions)
{
    // 获取可用的扩展
    TArray<vk::ExtensionProperties> AvailableExtensions;
    try
    {
        const auto AvailableExtensionsVector = vk::enumerateInstanceExtensionProperties();
        AvailableExtensions.Reserve(AvailableExtensionsVector.size());
        for (const auto& Ext : AvailableExtensionsVector)
        {
            AvailableExtensions.Add(Ext);
        }
    }
    catch (const vk::SystemError& e)
    {
        const FString ErrorMsg = FString("Vulkan: 枚举实例扩展失败: ") + FString(e.what());
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan: 枚举实例扩展失败: {}", e.what());
        throw std::runtime_error(ErrorMsg.CStr());
    }

    // 检查每个需要的扩展是否可用
    for (const char* RequiredExt : RequiredExtensions)
    {
        bool Found = false;
        for (const auto& AvailableExt : AvailableExtensions)
        {
            if (strcmp(AvailableExt.extensionName, RequiredExt) == 0)
            {
                Found = true;
                break;
            }
        }

        if (!Found)
        {
            HK_LOG_ERROR(ELogcat::RHI, "Vulkan扩展不可用: {}", RequiredExt);
            return false;
        }
    }

    return true;
}

bool FGfxDeviceVk::CheckValidationLayerSupport(const TArray<const char*>& RequiredLayers)
{
    // 获取可用的验证层
    TArray<vk::LayerProperties> AvailableLayers;
    try
    {
        const auto AvailableLayersVector = vk::enumerateInstanceLayerProperties();
        AvailableLayers.Reserve(AvailableLayersVector.size());
        for (const auto& Layer : AvailableLayersVector)
        {
            AvailableLayers.Add(Layer);
        }
    }
    catch (const vk::SystemError& e)
    {
        const FString ErrorMsg = FString("Vulkan: 枚举验证层失败: ") + FString(e.what());
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan: 枚举验证层失败: {}", e.what());
        throw std::runtime_error(ErrorMsg.CStr());
    }

    // 检查每个需要的验证层是否可用
    for (const char* RequiredLayer : RequiredLayers)
    {
        bool Found = false;
        for (const auto& AvailableLayer : AvailableLayers)
        {
            if (strcmp(AvailableLayer.layerName, RequiredLayer) == 0)
            {
                Found = true;
                break;
            }
        }

        if (!Found)
        {
            HK_LOG_WARN(ELogcat::RHI, "Vulkan验证层不可用: {}", RequiredLayer);
            return false;
        }
    }

    return true;
}

TArray<const char*> FGfxDeviceVk::GetRequiredExtensions()
{
    TArray<const char*> Extensions;

    // 必需的扩展
    Extensions.Add(VK_KHR_SURFACE_EXTENSION_NAME);

    // 平台特定的窗口扩展
#ifdef HK_WINDOWS
    Extensions.Add("VK_KHR_win32_surface");
#elif defined(HK_MACOS)
    Extensions.Add(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(HK_LINUX)
    Extensions.Add(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif

    // 功能扩展（实例扩展）
    Extensions.Add(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);

    // 注意：VK_EXT_descriptor_indexing 和 VK_EXT_dynamic_state 是设备扩展，
    // 不是实例扩展，需要在设备创建时启用

    return Extensions;
}

TArray<const char*> FGfxDeviceVk::GetRequiredValidationLayers()
{
    TArray<const char*> Layers;

#ifdef HK_DEBUG
    // Debug模式下启用验证层
    Layers.Add("VK_LAYER_KHRONOS_validation");
#endif

    return Layers;
}