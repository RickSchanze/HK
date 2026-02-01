#pragma once

#include "Core/Container/Array.h"
#include "Core/String/String.h"
#include "Core/Utility/HashUtility.h"
#include "Core/Utility/Macros.h"
#include "RHIHandle.h"
#include "RHIImage.h"
#include "RHIImageView.h"
#include "RHISampler.h"
#include "RHIBuffer.h"

enum class ERHIDescriptorType : UInt32
{
    Sampler                  = 0,  // 采样器
    CombinedImageSampler     = 1,  // 组合图像采样器
    SampledImage             = 2,  // 采样图像
    StorageImage             = 3,  // 存储图像
    UniformTexelBuffer       = 4,  // 统一纹理缓冲区
    StorageTexelBuffer       = 5,  // 存储纹理缓冲区
    UniformBuffer            = 6,  // 统一缓冲区
    StorageBuffer            = 7,  // 存储缓冲区
    UniformBufferDynamic     = 8,  // 动态统一缓冲区
    StorageBufferDynamic     = 9,  // 动态存储缓冲区
    InputAttachment          = 10, // 输入附件
    AccelerationStructureKHR = 11, // 加速结构（光线追踪）
    AccelerationStructureNV  = 12, // 加速结构（NV扩展）
    MutableVALVE             = 13, // 可变描述符（VALVE扩展）
};

enum class ERHIDescriptorPoolCreateFlag : UInt32
{
    None              = 0,
    FreeDescriptorSet = 1 << 0, // 允许释放描述符集
    UpdateAfterBind   = 1 << 1, // 允许绑定后更新
};
HK_ENABLE_BITMASK_OPERATORS(ERHIDescriptorPoolCreateFlag)

struct FRHIDescriptorPoolSize
{
    ERHIDescriptorType Type  = ERHIDescriptorType::UniformBuffer; // 描述符类型
    UInt32             Count = 0;                                 // 该类型的描述符数量

    UInt64 GetHashCode() const
    {
        return FHashUtility::CombineHashes(std::hash<UInt32>{}(static_cast<UInt32>(Type)), std::hash<UInt32>{}(Count));
    }
};

struct FRHIDescriptorPoolDesc
{
    ERHIDescriptorPoolCreateFlag   Flags   = ERHIDescriptorPoolCreateFlag::None; // 创建标志
    UInt32                         MaxSets = 0;                                  // 最大描述符集数量
    TArray<FRHIDescriptorPoolSize> PoolSizes;                                    // 描述符池大小数组
    FString                        DebugName;                                    // 调试名称

    UInt64 GetHashCode() const
    {
        UInt64 hash =
            FHashUtility::CombineHashes(std::hash<UInt32>{}(static_cast<UInt32>(Flags)), std::hash<UInt32>{}(MaxSets));
        for (const auto& PoolSize : PoolSizes)
        {
            hash = FHashUtility::CombineHashes(hash, PoolSize.GetHashCode());
        }
        return hash;
    }
};

// 描述符集布局绑定
struct FRHIDescriptorSetLayoutBinding
{
    UInt32             Binding         = 0;                                 // 绑定索引
    ERHIDescriptorType DescriptorType  = ERHIDescriptorType::UniformBuffer; // 描述符类型
    UInt32             DescriptorCount = 1;                                 // 描述符数量
    ERHIShaderStage    StageFlags      = ERHIShaderStage::None; // 着色器阶段标志（ERHIShaderStage 的位掩码）
    // 注意：ImmutableSamplers 将在后续实现

    UInt64 GetHashCode() const
    {
        return FHashUtility::CombineHashes(
            std::hash<UInt32>{}(Binding), std::hash<UInt32>{}(static_cast<UInt32>(DescriptorType)),
            std::hash<UInt32>{}(DescriptorCount), std::hash<ERHIShaderStage>{}(StageFlags));
    }
};

// 描述符集布局描述
struct FRHIDescriptorSetLayoutDesc
{
    TArray<FRHIDescriptorSetLayoutBinding> Bindings;  // 绑定数组
    FString                                DebugName; // 调试名称

    UInt64 GetHashCode() const
    {
        if (Bindings.IsEmpty())
        {
            return 0;
        }
        UInt64 hash = Bindings[0].GetHashCode();
        for (size_t i = 1; i < Bindings.Size(); ++i)
        {
            hash = FHashUtility::CombineHashes(hash, Bindings[i].GetHashCode());
        }
        return hash;
    }
};

// 描述符集布局类
class FRHIDescriptorSetLayout
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 DescriptorSetLayout（无效）
    FRHIDescriptorSetLayout() = default;

    // 析构函数：不自动销毁资源，必须通过 FRHIDevice::DestroyDescriptorSetLayout 销毁
    ~FRHIDescriptorSetLayout() = default;

    // 允许拷贝和移动
    FRHIDescriptorSetLayout(const FRHIDescriptorSetLayout& Other)                = default;
    FRHIDescriptorSetLayout& operator=(const FRHIDescriptorSetLayout& Other)     = default;
    FRHIDescriptorSetLayout(FRHIDescriptorSetLayout&& Other) noexcept            = default;
    FRHIDescriptorSetLayout& operator=(FRHIDescriptorSetLayout&& Other) noexcept = default;

    // 检查是否有效
    bool IsValid() const
    {
        return Handle.IsValid();
    }

    // 获取底层句柄
    const FRHIHandle& GetHandle() const
    {
        return Handle;
    }

    FRHIHandle& GetHandle()
    {
        return Handle;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHIDescriptorSetLayout& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHIDescriptorSetLayout& Other) const
    {
        return Handle != Other.Handle;
    }

    UInt64 GetHashCode() const
    {
        return Handle.GetHashCode();
    }

private:
    FRHIHandle Handle;
};

struct FRHIDescriptorSetDesc
{
    FRHIDescriptorSetLayout Layout;    // 描述符集布局（必须有效）
    FString                 DebugName; // 调试名称

    UInt64 GetHashCode() const
    {
        return Layout.GetHashCode();
    }
};

// 描述符图像信息（用于 SampledImage、StorageImage、CombinedImageSampler）
struct FRHIDescriptorImageInfo
{
    FRHISampler    Sampler;  // 采样器（可选，仅用于 CombinedImageSampler 和 Sampler）
    FRHIImageView  ImageView; // 图像视图（用于 SampledImage、StorageImage、CombinedImageSampler）
    ERHIImageLayout ImageLayout = ERHIImageLayout::ShaderReadOnlyOptimal; // 图像布局
};

// 描述符缓冲区信息（用于 UniformBuffer、StorageBuffer 等）
struct FRHIDescriptorBufferInfo
{
    FRHIBuffer Buffer; // 缓冲区
    UInt64     Offset  = 0; // 偏移量（字节）
    UInt64     Range   = 0; // 范围（字节），0 表示整个缓冲区
};

// 描述符写入信息（用于更新描述符集）
struct FRHIWriteDescriptorSet
{
    UInt32             DstBinding      = 0;   // 目标绑定索引
    UInt32             DstArrayElement = 0;   // 目标数组元素索引
    UInt32             DescriptorCount = 1;   // 描述符数量
    ERHIDescriptorType DescriptorType  = ERHIDescriptorType::UniformBuffer; // 描述符类型
    TArray<FRHIDescriptorImageInfo>  ImageInfo;  // 图像信息数组（用于图像相关描述符）
    TArray<FRHIDescriptorBufferInfo> BufferInfo;  // 缓冲区信息数组（用于缓冲区相关描述符）
    // 注意：TexelBufferView 和 AccelerationStructure 将在后续实现
};

// 描述符池类
class FRHIDescriptorPool
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 DescriptorPool（无效）
    FRHIDescriptorPool() = default;

    // 析构函数：不自动销毁资源，必须通过 FRHIDevice::DestroyDescriptorPool 销毁
    ~FRHIDescriptorPool() = default;

    // 允许拷贝和移动
    FRHIDescriptorPool(const FRHIDescriptorPool& Other)                = default;
    FRHIDescriptorPool& operator=(const FRHIDescriptorPool& Other)     = default;
    FRHIDescriptorPool(FRHIDescriptorPool&& Other) noexcept            = default;
    FRHIDescriptorPool& operator=(FRHIDescriptorPool&& Other) noexcept = default;

    // 检查是否有效
    bool IsValid() const
    {
        return Handle.IsValid();
    }

    // 获取底层句柄
    const FRHIHandle& GetHandle() const
    {
        return Handle;
    }

    FRHIHandle& GetHandle()
    {
        return Handle;
    }

    // 获取最大描述符集数量
    UInt32 GetMaxSets() const
    {
        return MaxSets;
    }

    // 获取创建标志
    ERHIDescriptorPoolCreateFlag GetFlags() const
    {
        return Flags;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHIDescriptorPool& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHIDescriptorPool& Other) const
    {
        return Handle != Other.Handle;
    }

    UInt64 GetHashCode() const
    {
        return Handle.GetHashCode();
    }

private:
    FRHIHandle                   Handle;
    UInt32                       MaxSets = 0;
    ERHIDescriptorPoolCreateFlag Flags   = ERHIDescriptorPoolCreateFlag::None;
};

// 描述符集类
class FRHIDescriptorSet
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 DescriptorSet（无效）
    FRHIDescriptorSet() = default;

    // 析构函数：不自动销毁资源，必须通过 FRHIDevice::FreeDescriptorSet 或 DestroyDescriptorPool 销毁
    ~FRHIDescriptorSet() = default;

    // 允许拷贝和移动
    FRHIDescriptorSet(const FRHIDescriptorSet& Other)                = default;
    FRHIDescriptorSet& operator=(const FRHIDescriptorSet& Other)     = default;
    FRHIDescriptorSet(FRHIDescriptorSet&& Other) noexcept            = default;
    FRHIDescriptorSet& operator=(FRHIDescriptorSet&& Other) noexcept = default;

    // 检查是否有效
    bool IsValid() const
    {
        return Handle.IsValid();
    }

    // 获取底层句柄
    const FRHIHandle& GetHandle() const
    {
        return Handle;
    }

    FRHIHandle& GetHandle()
    {
        return Handle;
    }

    // 获取关联的描述符池
    const FRHIDescriptorPool* GetPool() const
    {
        return Pool;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHIDescriptorSet& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHIDescriptorSet& Other) const
    {
        return Handle != Other.Handle;
    }

    UInt64 GetHashCode() const
    {
        return Handle.GetHashCode();
    }

private:
    FRHIHandle                Handle;
    const FRHIDescriptorPool* Pool = nullptr; // 关联的描述符池（弱引用）
};
