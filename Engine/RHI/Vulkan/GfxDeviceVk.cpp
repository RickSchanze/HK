//
// Created by Admin on 2025/12/15.
//

#include "GfxDeviceVk.h"
#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "Core/Utility/UniquePtr.h"
#include "RHI/RHIHandle.h"
#include "RHI/RHIWindow.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <set>
#include <stdexcept>
#include <vulkan/vulkan.h>

void FGfxDeviceVk::Init()
{
    try
    {
        // 1. 初始化SDL（如果尚未初始化）
        if (!SDL_WasInit(SDL_INIT_VIDEO))
        {
            if (const auto Success = SDL_Init(SDL_INIT_VIDEO); !Success)
            {
                const auto ErrMsg = SDL_GetError();
                const FString ErrorMsg = std::format("SDL初始化失败! ErrMsg={}.", ErrMsg);
                HK_LOG_FATAL(ELogcat::RHI, "SDL初始化失败: {}", SDL_GetError());
                throw std::runtime_error(ErrorMsg.CStr());
            }
        }

        // 2. 创建Vulkan实例
        CreateInstance();

        // 3. 创建主窗口和Surface（使用默认参数）
        // 注意：这里使用默认值，如果需要自定义，应该在调用 Initialize 之前设置
        const FName DefaultMainWindowName("HKEngine");
        const FVector2i DefaultMainWindowSize(1280, 720);
        CreateMainWindowAndSurface(DefaultMainWindowName, DefaultMainWindowSize);

        // 4. 创建Device（需要Surface）
        CreateDevice();

        HK_LOG_INFO(ELogcat::RHI, "Vulkan设备初始化完成");
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan设备初始化失败: {}", e.what());
        // 清理已创建的资源
        UnInit();
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan设备初始化失败: 未知异常");
        // 清理已创建的资源
        UnInit();
        throw;
    }
}

void FGfxDeviceVk::UnInit()
{
    // 1. 先销毁Device（对称操作：与Initialize相反的顺序）
    if (Device)
    {
        Device.waitIdle();
        Device.destroy();
        Device = nullptr;
        HK_LOG_INFO(ELogcat::RHI, "Vulkan逻辑设备已销毁");
    }

    // 2. 销毁主窗口（包括SwapChain、Surface和Window）
    auto& WindowManager = FRHIWindowManager::GetRef();
    if (WindowManager.Windows[0])
    {
        DestroyMainWindow(*WindowManager.Windows[0]);
    }

    // 3. 销毁Instance
    if (Instance)
    {
        Instance.destroy();
        Instance = nullptr;
        HK_LOG_INFO(ELogcat::RHI, "Vulkan实例已销毁");
    }

    // 注意：SDL的清理应该由应用程序负责，这里不清理
}

void FGfxDeviceVk::CreateMainWindowAndSurface(const FName MainWindowName, FVector2i MainWindowInitSize)
{
    // 确保Instance已创建
    if (!Instance)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan Instance未创建，请先调用CreateInstance()");
        throw std::runtime_error("Vulkan Instance未创建");
    }

    // 获取窗口管理器
    auto& WindowManager = FRHIWindowManager::GetRef();

    // 创建SDL窗口（默认隐藏，需要调用Open()来显示）
    SDL_Window* SDLWindow =
        SDL_CreateWindow(MainWindowName.GetString().CStr(), MainWindowInitSize.X, MainWindowInitSize.Y,
                         SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);

    if (!SDLWindow)
    {
        const FString ErrorMsg = FString("SDL窗口创建失败: ") + FString(SDL_GetError());
        HK_LOG_FATAL(ELogcat::RHI, "SDL窗口创建失败: {}", SDL_GetError());
        throw std::runtime_error(ErrorMsg.CStr());
    }

    // 使用SDL创建Vulkan Surface
    VkSurfaceKHR SurfaceHandle = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(SDLWindow, static_cast<VkInstance>(Instance), nullptr, &SurfaceHandle))
    {
        SDL_DestroyWindow(SDLWindow);
        const FString ErrorMsg = FString("Vulkan Surface创建失败: ") + FString(SDL_GetError());
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan Surface创建失败: {}", SDL_GetError());
        throw std::runtime_error(ErrorMsg.CStr());
    }

    MainWindowSurface = vk::SurfaceKHR(SurfaceHandle);

    // 创建RHI Handle
    auto& HandleManager = FRHIHandleManager::GetRef();
    const FRHIHandle SurfaceRHIHandle = HandleManager.CreateRHIHandle("MainWindowSurface", (SurfaceHandle));

    // 设置窗口信息（通过窗口管理器访问）
    if (!WindowManager.Windows[0])
    {
        WindowManager.Windows[0] = MakeUnique<FRHIWindow>();
    }

    FRHIWindow* MainWindow = WindowManager.Windows[0].Get();
    MainWindow->SetSurface(FRHISurface{SurfaceRHIHandle});
    MainWindow->SetHandle(SDLWindow);
    MainWindow->SetWindowName(MainWindowName);
    MainWindow->SetSize(MainWindowInitSize);
    MainWindow->SetOpened(false); // 窗口创建时默认未打开

    HK_LOG_INFO(ELogcat::RHI, "主窗口Surface创建成功: {}x{}", MainWindowInitSize.X, MainWindowInitSize.Y);
}

void FGfxDeviceVk::CreateMainWindowSurface(const FName MainWindowName, const FVector2i MainWindowInitSize,
                                           FRHIWindow& OutMainWindow)
{
    // 检查主窗口是否已经创建（在 Initialize 中）
    auto& WindowManager = FRHIWindowManager::GetRef();
    if (WindowManager.Windows[0] && MainWindowSurface)
    {
        // 如果已经创建，直接返回
        OutMainWindow = *WindowManager.Windows[0];
        HK_LOG_INFO(ELogcat::RHI, "主窗口已在Initialize中创建，直接返回");
        return;
    }

    // 如果未创建，则创建主窗口和Surface
    CreateMainWindowAndSurface(MainWindowName, MainWindowInitSize);

    // 获取窗口管理器并返回主窗口
    if (WindowManager.Windows[0])
    {
        OutMainWindow = *WindowManager.Windows[0];
    }
    else
    {
        HK_LOG_FATAL(ELogcat::RHI, "主窗口创建失败");
        throw std::runtime_error("主窗口创建失败");
    }
}

void FGfxDeviceVk::CreateMainWindowSwapChain(FRHIWindow& OutMainWindow)
{
    // 确保Device已创建
    if (!Device)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan Device未创建");
        throw std::runtime_error("Vulkan Device未创建");
    }

    if (!MainWindowSurface)
    {
        HK_LOG_FATAL(ELogcat::RHI, "MainWindow Surface未创建");
        throw std::runtime_error("MainWindow Surface未创建");
    }

    // 获取Surface能力
    vk::SurfaceCapabilitiesKHR Capabilities;
    try
    {
        Capabilities = PhysicalDevice.getSurfaceCapabilitiesKHR(MainWindowSurface);
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface能力失败: {}", e.what());
        throw std::runtime_error((FString("获取Surface能力失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface能力失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface能力失败: 未知异常");
        throw std::runtime_error("获取Surface能力失败: 未知异常");
    }

    // 获取Surface格式
    TArray<vk::SurfaceFormatKHR> SurfaceFormats;
    try
    {
        const auto FormatsVector = PhysicalDevice.getSurfaceFormatsKHR(MainWindowSurface);
        SurfaceFormats.Reserve(FormatsVector.size());
        for (const auto& Format : FormatsVector)
        {
            SurfaceFormats.Add(Format);
        }
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface格式失败: {}", e.what());
        throw std::runtime_error((FString("获取Surface格式失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface格式失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface格式失败: 未知异常");
        throw std::runtime_error("获取Surface格式失败: 未知异常");
    }

    // 选择Surface格式
    vk::SurfaceFormatKHR SurfaceFormat = SurfaceFormats[0];
    for (const auto& Format : SurfaceFormats)
    {
        if (Format.format == vk::Format::eB8G8R8A8Srgb && Format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            SurfaceFormat = Format;
            break;
        }
    }

    // 选择呈现模式
    TArray<vk::PresentModeKHR> PresentModes;
    try
    {
        const auto ModesVector = PhysicalDevice.getSurfacePresentModesKHR(MainWindowSurface);
        PresentModes.Reserve(ModesVector.size());
        for (const auto& Mode : ModesVector)
        {
            PresentModes.Add(Mode);
        }
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "获取呈现模式失败: {}", e.what());
        throw std::runtime_error((FString("获取呈现模式失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "获取呈现模式失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "获取呈现模式失败: 未知异常");
        throw std::runtime_error("获取呈现模式失败: 未知异常");
    }

    vk::PresentModeKHR PresentMode = vk::PresentModeKHR::eFifo; // 默认FIFO模式
    for (const auto& Mode : PresentModes)
    {
        if (Mode == vk::PresentModeKHR::eMailbox)
        {
            PresentMode = Mode;
            break;
        }
    }

    // 确定交换链图像数量
    uint32_t ImageCount = Capabilities.minImageCount + 1;
    if (Capabilities.maxImageCount > 0 && ImageCount > Capabilities.maxImageCount)
    {
        ImageCount = Capabilities.maxImageCount;
    }

    // 创建交换链
    vk::SwapchainCreateInfoKHR SwapChainCreateInfo;
    SwapChainCreateInfo.surface = MainWindowSurface;
    SwapChainCreateInfo.minImageCount = ImageCount;
    SwapChainCreateInfo.imageFormat = SurfaceFormat.format;
    SwapChainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
    SwapChainCreateInfo.imageExtent = Capabilities.currentExtent;
    SwapChainCreateInfo.imageArrayLayers = 1;
    SwapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    // 设置队列族索引
    uint32_t QueueFamilyIndicesArray[] = {static_cast<uint32_t>(QueueFamilyIndices.GraphicsFamily),
                                          static_cast<uint32_t>(QueueFamilyIndices.PresentFamily)};

    if (QueueFamilyIndices.GraphicsFamily != QueueFamilyIndices.PresentFamily)
    {
        SwapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        SwapChainCreateInfo.queueFamilyIndexCount = 2;
        SwapChainCreateInfo.pQueueFamilyIndices = QueueFamilyIndicesArray;
    }
    else
    {
        SwapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        SwapChainCreateInfo.queueFamilyIndexCount = 0;
        SwapChainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    SwapChainCreateInfo.preTransform = Capabilities.currentTransform;
    SwapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    SwapChainCreateInfo.presentMode = PresentMode;
    SwapChainCreateInfo.clipped = VK_TRUE;
    SwapChainCreateInfo.oldSwapchain = nullptr;

    vk::SwapchainKHR SwapChain;
    try
    {
        SwapChain = Device.createSwapchainKHR(SwapChainCreateInfo);
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建交换链失败: {}", e.what());
        throw std::runtime_error((FString("创建交换链失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建交换链失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建交换链失败: 未知异常");
        throw std::runtime_error("创建交换链失败: 未知异常");
    }

    // 创建RHI Handle
    auto& HandleManager = FRHIHandleManager::GetRef();
    FRHIHandle SwapChainRHIHandle = HandleManager.CreateRHIHandle(
        "MainWindowSwapChain", reinterpret_cast<void*>(static_cast<VkSwapchainKHR>(SwapChain)));

    // 更新窗口的SwapChain
    OutMainWindow.GetSwapChain().Handle = SwapChainRHIHandle;

    HK_LOG_INFO(ELogcat::RHI, "主窗口SwapChain创建成功");
}

void FGfxDeviceVk::CreateRHIWindow(const FName Name, const FVector2i Size, FRHIWindow& OutWindow)
{
    // 确保Instance和Device已创建
    if (!Instance)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan Instance未创建，请先调用Initialize()");
        throw std::runtime_error("Vulkan Instance未创建");
    }

    if (!Device)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan Device未创建，请先调用Initialize()");
        throw std::runtime_error("Vulkan Device未创建");
    }

    // 获取窗口管理器
    auto& WindowManager = FRHIWindowManager::GetRef();

    // 查找空闲的窗口槽位（从1开始，0是主窗口）
    Int32 FreeIndex = -1;
    for (Int32 i = 1; i < MAX_RHI_WINDOW_COUNT; ++i)
    {
        if (!WindowManager.Windows[i])
        {
            FreeIndex = i;
            break;
        }
    }

    if (FreeIndex < 0)
    {
        HK_LOG_FATAL(ELogcat::RHI, "窗口数量已达上限: {}", MAX_RHI_WINDOW_COUNT);
        throw std::runtime_error("窗口数量已达上限");
    }

    // 创建SDL窗口（默认隐藏，需要调用Open()来显示）
    SDL_Window* SDLWindow = SDL_CreateWindow(Name.GetString().CStr(), Size.X, Size.Y,
                                             SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);

    if (!SDLWindow)
    {
        const FString ErrorMsg = FString("SDL窗口创建失败: ") + FString(SDL_GetError());
        HK_LOG_FATAL(ELogcat::RHI, "SDL窗口创建失败: {}", SDL_GetError());
        throw std::runtime_error(ErrorMsg.CStr());
    }

    // 使用SDL创建Vulkan Surface
    VkSurfaceKHR SurfaceHandle = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(SDLWindow, static_cast<VkInstance>(Instance), nullptr, &SurfaceHandle))
    {
        SDL_DestroyWindow(SDLWindow);
        const FString ErrorMsg = FString("Vulkan Surface创建失败: ") + FString(SDL_GetError());
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan Surface创建失败: {}", SDL_GetError());
        throw std::runtime_error(ErrorMsg.CStr());
    }

    vk::SurfaceKHR Surface = vk::SurfaceKHR(SurfaceHandle);

    // 创建RHI Handle
    auto& HandleManager = FRHIHandleManager::GetRef();
    FString SurfaceDebugName = FString("WindowSurface_") + FString(std::to_string(FreeIndex).c_str());
    FRHIHandle SurfaceRHIHandle =
        HandleManager.CreateRHIHandle(SurfaceDebugName.CStr(), reinterpret_cast<void*>(SurfaceHandle));

    // 创建窗口对象
    WindowManager.Windows[FreeIndex] = MakeUnique<FRHIWindow>();
    FRHIWindow* Window = WindowManager.Windows[FreeIndex].Get();
    Window->SetSurface(FRHISurface{SurfaceRHIHandle});
    Window->SetHandle(SDLWindow);
    Window->SetWindowName(Name);
    Window->SetSize(Size);
    Window->SetOpened(false); // 窗口创建时默认未打开

    // 创建SwapChain
    // 获取Surface能力
    vk::SurfaceCapabilitiesKHR Capabilities;
    try
    {
        Capabilities = PhysicalDevice.getSurfaceCapabilitiesKHR(Surface);
    }
    catch (const vk::SystemError& e)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface能力失败: {}", e.what());
        throw std::runtime_error((FString("获取Surface能力失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface能力失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface能力失败: 未知异常");
        throw std::runtime_error("获取Surface能力失败: 未知异常");
    }

    // 获取Surface格式
    TArray<vk::SurfaceFormatKHR> SurfaceFormats;
    try
    {
        const auto FormatsVector = PhysicalDevice.getSurfaceFormatsKHR(Surface);
        SurfaceFormats.Reserve(FormatsVector.size());
        for (const auto& Format : FormatsVector)
        {
            SurfaceFormats.Add(Format);
        }
    }
    catch (const vk::SystemError& e)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface格式失败: {}", e.what());
        throw std::runtime_error((FString("获取Surface格式失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface格式失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "获取Surface格式失败: 未知异常");
        throw std::runtime_error("获取Surface格式失败: 未知异常");
    }

    // 选择Surface格式
    vk::SurfaceFormatKHR SurfaceFormat = SurfaceFormats[0];
    for (const auto& Format : SurfaceFormats)
    {
        if (Format.format == vk::Format::eB8G8R8A8Srgb && Format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            SurfaceFormat = Format;
            break;
        }
    }

    // 选择呈现模式
    TArray<vk::PresentModeKHR> PresentModes;
    try
    {
        const auto ModesVector = PhysicalDevice.getSurfacePresentModesKHR(Surface);
        PresentModes.Reserve(ModesVector.size());
        for (const auto& Mode : ModesVector)
        {
            PresentModes.Add(Mode);
        }
    }
    catch (const vk::SystemError& e)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "获取呈现模式失败: {}", e.what());
        throw std::runtime_error((FString("获取呈现模式失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "获取呈现模式失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "获取呈现模式失败: 未知异常");
        throw std::runtime_error("获取呈现模式失败: 未知异常");
    }

    vk::PresentModeKHR PresentMode = vk::PresentModeKHR::eFifo;
    for (const auto& Mode : PresentModes)
    {
        if (Mode == vk::PresentModeKHR::eMailbox)
        {
            PresentMode = Mode;
            break;
        }
    }

    // 确定交换链图像数量
    uint32_t ImageCount = Capabilities.minImageCount + 1;
    if (Capabilities.maxImageCount > 0 && ImageCount > Capabilities.maxImageCount)
    {
        ImageCount = Capabilities.maxImageCount;
    }

    // 创建交换链
    vk::SwapchainCreateInfoKHR SwapChainCreateInfo;
    SwapChainCreateInfo.surface = Surface;
    SwapChainCreateInfo.minImageCount = ImageCount;
    SwapChainCreateInfo.imageFormat = SurfaceFormat.format;
    SwapChainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
    SwapChainCreateInfo.imageExtent = Capabilities.currentExtent;
    SwapChainCreateInfo.imageArrayLayers = 1;
    SwapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    // 设置队列族索引
    uint32_t QueueFamilyIndicesArray[] = {static_cast<uint32_t>(QueueFamilyIndices.GraphicsFamily),
                                          static_cast<uint32_t>(QueueFamilyIndices.PresentFamily)};

    if (QueueFamilyIndices.GraphicsFamily != QueueFamilyIndices.PresentFamily)
    {
        SwapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        SwapChainCreateInfo.queueFamilyIndexCount = 2;
        SwapChainCreateInfo.pQueueFamilyIndices = QueueFamilyIndicesArray;
    }
    else
    {
        SwapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        SwapChainCreateInfo.queueFamilyIndexCount = 0;
        SwapChainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    SwapChainCreateInfo.preTransform = Capabilities.currentTransform;
    SwapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    SwapChainCreateInfo.presentMode = PresentMode;
    SwapChainCreateInfo.clipped = VK_TRUE;
    SwapChainCreateInfo.oldSwapchain = nullptr;

    vk::SwapchainKHR SwapChain;
    try
    {
        SwapChain = Device.createSwapchainKHR(SwapChainCreateInfo);
    }
    catch (const vk::SystemError& e)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "创建交换链失败: {}", e.what());
        throw std::runtime_error((FString("创建交换链失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "创建交换链失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        Instance.destroySurfaceKHR(Surface);
        SDL_DestroyWindow(SDLWindow);
        WindowManager.Windows[FreeIndex].Reset();
        HK_LOG_FATAL(ELogcat::RHI, "创建交换链失败: 未知异常");
        throw std::runtime_error("创建交换链失败: 未知异常");
    }

    // 创建SwapChain RHI Handle
    FString SwapChainDebugName = FString("WindowSwapChain_") + FString(std::to_string(FreeIndex).c_str());
    FRHIHandle SwapChainRHIHandle = HandleManager.CreateRHIHandle(
        SwapChainDebugName.CStr(), reinterpret_cast<void*>(static_cast<VkSwapchainKHR>(SwapChain)));

    // 更新窗口的SwapChain
    Window->SetSwapChain(FRHISwapChain{SwapChainRHIHandle});

    OutWindow = *Window;

    HK_LOG_INFO(ELogcat::RHI, "窗口创建成功: {} (索引: {})", Name.GetString().CStr(), FreeIndex);
}

void FGfxDeviceVk::DestroyMainWindow(FRHIWindow& MainWindow)
{
    // 1. 销毁SwapChain
    if (MainWindow.GetSwapChain().Handle.IsValid())
    {
        const auto SwapChainHandle = MainWindow.GetSwapChain().Handle.Cast<VkSwapchainKHR>();
        if (const auto SwapChain = vk::SwapchainKHR(SwapChainHandle); Device && SwapChain)
        {
            Device.destroySwapchainKHR(SwapChain);
        }
        auto& HandleManager = FRHIHandleManager::GetRef();
        HandleManager.DestroyRHIHandle(MainWindow.GetSwapChain().Handle);
        MainWindow.SetSwapChain(FRHISwapChain{FRHIHandle()});
        HK_LOG_INFO(ELogcat::RHI, "主窗口SwapChain已销毁");
    }

    // 2. 销毁Surface
    if (MainWindow.GetSurface().Handle.IsValid())
    {
        const auto SurfaceHandle = MainWindow.GetSurface().Handle.Cast<VkSurfaceKHR>();
        if (const auto Surface = vk::SurfaceKHR(SurfaceHandle); Instance && Surface)
        {
            Instance.destroySurfaceKHR(Surface);
        }
        auto& HandleManager = FRHIHandleManager::GetRef();
        HandleManager.DestroyRHIHandle(MainWindow.GetSurface().Handle);
        MainWindow.SetSurface(FRHISurface{FRHIHandle()});
        MainWindowSurface = nullptr;
        HK_LOG_INFO(ELogcat::RHI, "主窗口Surface已销毁");
    }

    // 3. 销毁SDL窗口
    if (MainWindow.GetHandle())
    {
        SDL_DestroyWindow(static_cast<SDL_Window*>(MainWindow.GetHandle()));
        MainWindow.SetHandle(nullptr);
        HK_LOG_INFO(ELogcat::RHI, "主窗口SDL窗口已销毁");
    }

    // 4. 清理窗口管理器中的窗口
    auto& WindowManager = FRHIWindowManager::GetRef();
    WindowManager.Windows[0].Reset();
}

void FGfxDeviceVk::DestroyRHIWindow(FRHIWindow& Window)
{
    // 1. 销毁SwapChain
    if (Window.GetSwapChain().Handle.IsValid())
    {
        const auto SwapChainHandle = Window.GetSwapChain().Handle.Cast<VkSwapchainKHR>();
        if (const auto SwapChain = vk::SwapchainKHR(SwapChainHandle); Device && SwapChain)
        {
            Device.destroySwapchainKHR(SwapChain);
        }
        auto& HandleManager = FRHIHandleManager::GetRef();
        HandleManager.DestroyRHIHandle(Window.GetSwapChain().Handle);
        Window.SetSwapChain(FRHISwapChain{FRHIHandle()});
    }

    // 2. 销毁Surface
    if (Window.GetSurface().Handle.IsValid())
    {
        const auto SurfaceHandle = Window.GetSurface().Handle.Cast<VkSurfaceKHR>();
        if (const auto Surface = vk::SurfaceKHR(SurfaceHandle); Instance && Surface)
        {
            Instance.destroySurfaceKHR(Surface);
        }
        auto& HandleManager = FRHIHandleManager::GetRef();
        HandleManager.DestroyRHIHandle(Window.GetSurface().Handle);
        Window.SetSurface(FRHISurface{FRHIHandle()});
    }

    // 3. 销毁SDL窗口
    if (Window.GetHandle())
    {
        SDL_DestroyWindow(static_cast<SDL_Window*>(Window.GetHandle()));
        Window.SetHandle(nullptr);
    }

    // 4. 查找并清理窗口管理器中的窗口
    auto& WindowManager = FRHIWindowManager::GetRef();
    for (Int32 i = 1; i < MAX_RHI_WINDOW_COUNT; ++i)
    {
        if (WindowManager.Windows[i] && WindowManager.Windows[i].Get() == &Window)
        {
            WindowManager.Windows[i].Reset();
            HK_LOG_INFO(ELogcat::RHI, "窗口已销毁 (索引: {})", i);
            return;
        }
    }

    HK_LOG_WARN(ELogcat::RHI, "未找到要销毁的窗口");
}

void FGfxDeviceVk::OpenWindow(FRHIWindow& Window)
{
    if (!Window.GetHandle())
    {
        HK_LOG_WARN(ELogcat::RHI, "窗口句柄为空，无法打开窗口");
        return;
    }

    auto SDLWindow = static_cast<SDL_Window*>(Window.GetHandle());

    // SDL3 中，窗口创建后默认就是显示的，但我们可以显式地显示它
    SDL_ShowWindow(SDLWindow);

    Window.SetOpened(true);
    HK_LOG_INFO(ELogcat::RHI, "窗口已打开: {}", Window.GetWindowName().GetString().CStr());
}

void FGfxDeviceVk::CloseWindow(FRHIWindow& Window)
{
    if (!Window.GetHandle())
    {
        HK_LOG_WARN(ELogcat::RHI, "窗口句柄为空，无法关闭窗口");
        return;
    }

    SDL_Window* SDLWindow = static_cast<SDL_Window*>(Window.GetHandle());

    // 隐藏窗口
    SDL_HideWindow(SDLWindow);

    Window.SetOpened(false);
    HK_LOG_INFO(ELogcat::RHI, "窗口已关闭: {}", Window.GetWindowName().GetString().CStr());
}

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
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan实例创建失败: {}", e.what());
        Instance = nullptr;
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan实例创建失败: 未知异常");
        Instance = nullptr;
        throw std::runtime_error("Vulkan实例创建失败: 未知异常");
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
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan: 枚举实例扩展失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan: 枚举实例扩展失败: 未知异常");
        throw std::runtime_error("Vulkan: 枚举实例扩展失败: 未知异常");
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
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan: 枚举验证层失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan: 枚举验证层失败: 未知异常");
        throw std::runtime_error("Vulkan: 枚举验证层失败: 未知异常");
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

void FGfxDeviceVk::CreateDevice()
{
    // 选择物理设备
    SelectPhysicalDevice();

    // 查找队列族
    QueueFamilyIndices = FindQueueFamilies(PhysicalDevice);

    if (!QueueFamilyIndices.IsComplete())
    {
        HK_LOG_FATAL(ELogcat::RHI, "找不到合适的队列族");
        throw std::runtime_error("找不到合适的队列族");
    }

    // 创建队列创建信息
    TArray<vk::DeviceQueueCreateInfo> QueueCreateInfos;
    std::set<Int32> UniqueQueueFamilies = {QueueFamilyIndices.GraphicsFamily, QueueFamilyIndices.PresentFamily};

    float QueuePriority = 1.0f;
    for (Int32 QueueFamily : UniqueQueueFamilies)
    {
        vk::DeviceQueueCreateInfo QueueCreateInfo;
        QueueCreateInfo.queueFamilyIndex = static_cast<uint32_t>(QueueFamily);
        QueueCreateInfo.queueCount = 1;
        QueueCreateInfo.pQueuePriorities = &QueuePriority;
        QueueCreateInfos.Add(QueueCreateInfo);
    }

    // 获取设备扩展
    TArray<const char*> DeviceExtensions = GetRequiredDeviceExtensions();

    // 检查设备扩展支持（Debug Utils 是可选的）
    TArray<const char*> RequiredExtensions;
    TArray<const char*> OptionalExtensions;
    for (const char* Ext : DeviceExtensions)
    {
#ifdef HK_DEBUG
        if (strcmp(Ext, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
        {
            OptionalExtensions.Add(Ext);
        }
        else
        {
            RequiredExtensions.Add(Ext);
        }
#else
        RequiredExtensions.Add(Ext);
#endif
    }

    if (!CheckDeviceExtensionSupport(PhysicalDevice, RequiredExtensions))
    {
        HK_LOG_FATAL(ELogcat::RHI, "设备不支持所需的扩展");
        throw std::runtime_error("设备不支持所需的扩展");
    }

    // 检查可选扩展，如果不支持则从列表中移除
    TArray<const char*> FinalExtensions = RequiredExtensions;
    bDebugUtilsExtensionAvailable = false;
    for (const char* Ext : OptionalExtensions)
    {
        if (CheckDeviceExtensionSupport(PhysicalDevice, TArray<const char*>{Ext}))
        {
            FinalExtensions.Add(Ext);
            if (strcmp(Ext, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
            {
                bDebugUtilsExtensionAvailable = true;
            }
        }
        else
        {
            HK_LOG_WARN(ELogcat::RHI, "设备不支持可选扩展: {}，DebugName功能将不可用", Ext);
        }
    }
    DeviceExtensions = FinalExtensions;

    // 设备特性
    vk::PhysicalDeviceFeatures DeviceFeatures;

    // 创建设备创建信息
    vk::DeviceCreateInfo CreateInfo;
    CreateInfo.queueCreateInfoCount = static_cast<uint32_t>(QueueCreateInfos.Size());
    CreateInfo.pQueueCreateInfos = QueueCreateInfos.Data();
    CreateInfo.pEnabledFeatures = &DeviceFeatures;
    CreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.Size());
    CreateInfo.ppEnabledExtensionNames = DeviceExtensions.Data();

    // 注意：在 Vulkan 1.3+ 中，设备级别的验证层已被弃用并被忽略
    // 验证层应该在实例级别启用，这里不再设置设备级别的验证层

    // 创建逻辑设备
    try
    {
        Device = PhysicalDevice.createDevice(CreateInfo);
        HK_LOG_INFO(ELogcat::RHI, "Vulkan逻辑设备创建成功");

        // 获取队列
        GraphicsQueue = Device.getQueue(static_cast<uint32_t>(QueueFamilyIndices.GraphicsFamily), 0);
        PresentQueue = Device.getQueue(static_cast<uint32_t>(QueueFamilyIndices.PresentFamily), 0);
    }
    catch (const vk::SystemError& e)
    {
        const FString ErrorMsg = FString("Vulkan逻辑设备创建失败: ") + FString(e.what());
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan逻辑设备创建失败: {}", e.what());
        Device = nullptr;
        throw std::runtime_error(ErrorMsg.CStr());
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan逻辑设备创建失败: {}", e.what());
        Device = nullptr;
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan逻辑设备创建失败: 未知异常");
        Device = nullptr;
        throw std::runtime_error("Vulkan逻辑设备创建失败: 未知异常");
    }
}

void FGfxDeviceVk::SelectPhysicalDevice()
{
    // 枚举物理设备
    TArray<vk::PhysicalDevice> Devices;
    try
    {
        const auto DevicesVector = Instance.enumeratePhysicalDevices();
        Devices.Reserve(DevicesVector.size());
        for (const auto& Dev : DevicesVector)
        {
            Devices.Add(Dev);
        }
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "枚举物理设备失败: {}", e.what());
        throw std::runtime_error((FString("枚举物理设备失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "枚举物理设备失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "枚举物理设备失败: 未知异常");
        throw std::runtime_error("枚举物理设备失败: 未知异常");
    }

    if (Devices.IsEmpty())
    {
        HK_LOG_FATAL(ELogcat::RHI, "未找到支持Vulkan的GPU");
        throw std::runtime_error("未找到支持Vulkan的GPU");
    }

    // 选择第一个合适的设备
    for (const auto& MyDevice : Devices)
    {
        if (IsDeviceSuitable(MyDevice))
        {
            PhysicalDevice = MyDevice;
            vk::PhysicalDeviceProperties Properties = MyDevice.getProperties();
            HK_LOG_INFO(ELogcat::RHI, "选择物理设备: {}", Properties.deviceName.data());
            return;
        }
    }

    HK_LOG_FATAL(ELogcat::RHI, "未找到合适的物理设备");
    throw std::runtime_error("未找到合适的物理设备");
}

bool FGfxDeviceVk::CheckDeviceExtensionSupport(const vk::PhysicalDevice Device,
                                               const TArray<const char*>& RequiredExtensions)
{
    // 获取设备支持的扩展
    TArray<vk::ExtensionProperties> AvailableExtensions;
    try
    {
        const auto ExtensionsVector = Device.enumerateDeviceExtensionProperties();
        AvailableExtensions.Reserve(ExtensionsVector.size());
        for (const auto& Ext : ExtensionsVector)
        {
            AvailableExtensions.Add(Ext);
        }
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_ERROR(ELogcat::RHI, "枚举设备扩展失败: {}", e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        HK_LOG_ERROR(ELogcat::RHI, "枚举设备扩展失败: {}", e.what());
        return false;
    }
    catch (...)
    {
        HK_LOG_ERROR(ELogcat::RHI, "枚举设备扩展失败: 未知异常");
        return false;
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
            HK_LOG_ERROR(ELogcat::RHI, "设备扩展不可用: {}", RequiredExt);
            return false;
        }
    }

    return true;
}

TArray<const char*> FGfxDeviceVk::GetRequiredDeviceExtensions()
{
    TArray<const char*> Extensions;
    Extensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    // 添加 Debug Utils 扩展（如果可用，用于设置 DebugName）
#ifdef HK_DEBUG
    Extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return Extensions;
}

bool FGfxDeviceVk::IsDeviceSuitable(const vk::PhysicalDevice InPhysicalDevice) const
{
    // 检查设备扩展支持
    const TArray<const char*> RequiredExtensions = GetRequiredDeviceExtensions();
    if (!CheckDeviceExtensionSupport(InPhysicalDevice, RequiredExtensions))
    {
        return false;
    }

    // 检查队列族支持
    const FQueueFamilyIndices Indices = FindQueueFamilies(InPhysicalDevice);
    if (!Indices.IsComplete())
    {
        return false;
    }

    // 检查Surface支持
    TArray<vk::SurfaceFormatKHR> SurfaceFormats;
    TArray<vk::PresentModeKHR> PresentModes;
    try
    {
        const auto FormatsVector = InPhysicalDevice.getSurfaceFormatsKHR(MainWindowSurface);
        SurfaceFormats.Reserve(FormatsVector.size());
        for (const auto& Format : FormatsVector)
        {
            SurfaceFormats.Add(Format);
        }

        const auto ModesVector = InPhysicalDevice.getSurfacePresentModesKHR(MainWindowSurface);
        PresentModes.Reserve(ModesVector.size());
        for (const auto& Mode : ModesVector)
        {
            PresentModes.Add(Mode);
        }
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_WARN(ELogcat::RHI, "检查Surface支持失败: {}", e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        HK_LOG_WARN(ELogcat::RHI, "检查Surface支持失败: {}", e.what());
        return false;
    }
    catch (...)
    {
        HK_LOG_WARN(ELogcat::RHI, "检查Surface支持失败: 未知异常");
        return false;
    }

    return !SurfaceFormats.IsEmpty() && !PresentModes.IsEmpty();
}

FGfxDeviceVk::FQueueFamilyIndices FGfxDeviceVk::FindQueueFamilies(const vk::PhysicalDevice InPhysicalDevice) const
{
    FQueueFamilyIndices Indices;

    // 获取队列族属性
    TArray<vk::QueueFamilyProperties> QueueFamilies;
    try
    {
        const auto QueueFamiliesVector = InPhysicalDevice.getQueueFamilyProperties();
        QueueFamilies.Reserve(QueueFamiliesVector.size());
        for (const auto& QueueFamily : QueueFamiliesVector)
        {
            QueueFamilies.Add(QueueFamily);
        }
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_ERROR(ELogcat::RHI, "获取队列族属性失败: {}", e.what());
        return Indices;
    }
    catch (const std::exception& e)
    {
        HK_LOG_ERROR(ELogcat::RHI, "获取队列族属性失败: {}", e.what());
        return Indices;
    }
    catch (...)
    {
        HK_LOG_ERROR(ELogcat::RHI, "获取队列族属性失败: 未知异常");
        return Indices;
    }

    // 查找图形队列和呈现队列
    for (Int32 i = 0; i < static_cast<Int32>(QueueFamilies.Size()); ++i)
    {
        const auto& QueueFamily = QueueFamilies[i];

        // 检查图形队列支持
        if (QueueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            if (Indices.GraphicsFamily < 0)
            {
                Indices.GraphicsFamily = i;
            }
        }

        // 检查呈现队列支持（需要Surface）
        if (MainWindowSurface)
        {
            try
            {
                const bool PresentSupport =
                    InPhysicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), MainWindowSurface);
                if (PresentSupport && Indices.PresentFamily < 0)
                {
                    Indices.PresentFamily = i;
                }
            }
            catch (const vk::SystemError&)
            {
                // 忽略错误，继续查找
            }
        }

        if (Indices.IsComplete())
        {
            break;
        }
    }

    return Indices;
}

UInt32 FGfxDeviceVk::FindMemoryType(const UInt32 TypeFilter, const vk::MemoryPropertyFlags Properties) const
{
    vk::PhysicalDeviceMemoryProperties MemProperties = PhysicalDevice.getMemoryProperties();

    for (UInt32 i = 0; i < MemProperties.memoryTypeCount; ++i)
    {
        if ((TypeFilter & (1 << i)) && (MemProperties.memoryTypes[i].propertyFlags & Properties) == Properties)
        {
            return i;
        }
    }

    HK_LOG_FATAL(ELogcat::RHI, "未找到合适的内存类型");
    throw std::runtime_error("未找到合适的内存类型");
}

void FGfxDeviceVk::SetDebugName(const vk::DeviceMemory ObjectHandle, const vk::ObjectType ObjectType,
                                const FStringView& Name) const
{
    if (!Device || Name.IsEmpty() || !bDebugUtilsExtensionAvailable)
    {
        return;
    }

    // 检查是否支持 VK_EXT_debug_utils 扩展
    // 注意：这需要在设备创建时启用该扩展
    try
    {
        // 将 FStringView 转换为以 null 结尾的字符串
        FString NameStr(Name.Data(), Name.Size());

        vk::DebugUtilsObjectNameInfoEXT NameInfo;
        NameInfo.objectType = ObjectType;
        NameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkDeviceMemory>(ObjectHandle));
        NameInfo.pObjectName = NameStr.CStr();

        // 使用动态加载的方式调用扩展函数，避免链接错误
        auto vkSetDebugUtilsObjectNameEXT =
            reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(Device.getProcAddr("vkSetDebugUtilsObjectNameEXT"));

        if (vkSetDebugUtilsObjectNameEXT)
        {
            VkDebugUtilsObjectNameInfoEXT VkNameInfo{};
            VkNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            VkNameInfo.objectType = static_cast<VkObjectType>(ObjectType);
            VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkDeviceMemory>(ObjectHandle));
            VkNameInfo.pObjectName = NameStr.CStr();

            vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
        }
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}

void FGfxDeviceVk::SetDebugName(const vk::Buffer ObjectHandle, const vk::ObjectType ObjectType,
                                const FStringView& Name) const
{
    if (!Device || Name.IsEmpty() || !bDebugUtilsExtensionAvailable)
    {
        return;
    }

    try
    {
        // 将 FStringView 转换为以 null 结尾的字符串
        FString NameStr(Name.Data(), Name.Size());

        // 使用动态加载的方式调用扩展函数，避免链接错误
        auto vkSetDebugUtilsObjectNameEXT =
            reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(Device.getProcAddr("vkSetDebugUtilsObjectNameEXT"));

        if (vkSetDebugUtilsObjectNameEXT)
        {
            VkDebugUtilsObjectNameInfoEXT VkNameInfo{};
            VkNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            VkNameInfo.objectType = static_cast<VkObjectType>(ObjectType);
            VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkBuffer>(ObjectHandle));
            VkNameInfo.pObjectName = NameStr.CStr();

            vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
        }
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}