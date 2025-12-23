#pragma once

#include "Core/String/String.h"
#include "Core/String/StringView.h"
#include <filesystem>
#include <fstream>
#include <memory>

/**
 * 文件操作工具类
 */
class HK_API FFileUtility
{
public:
    /**
     * 创建文件流（输出流）
     * @param FilePath 文件路径
     * @param CreateDirectories 是否递归创建不存在的目录
     * @param BinaryMode 是否以二进制模式打开
     * @return 文件流指针，失败返回 nullptr
     */
    static std::unique_ptr<std::ofstream> CreateFileStream(
        const FStringView& FilePath,
        bool CreateDirectories = true,
        bool BinaryMode = true);

    /**
     * 创建文件流（输出流）
     * @param FilePath 文件路径
     * @param CreateDirectories 是否递归创建不存在的目录
     * @param BinaryMode 是否以二进制模式打开
     * @return 文件流指针，失败返回 nullptr
     */
    static std::unique_ptr<std::ofstream> CreateFileStream(
        const FString& FilePath,
        bool CreateDirectories = true,
        bool BinaryMode = true);

    /**
     * 创建文件流（输出流）
     * @param FilePath 文件路径（std::filesystem::path）
     * @param CreateDirectories 是否递归创建不存在的目录
     * @param BinaryMode 是否以二进制模式打开
     * @return 文件流指针，失败返回 nullptr
     */
    static std::unique_ptr<std::ofstream> CreateFileStream(
        const std::filesystem::path& FilePath,
        bool CreateDirectories = true,
        bool BinaryMode = true);

    /**
     * 打开文件流（输入流）
     * @param FilePath 文件路径
     * @param BinaryMode 是否以二进制模式打开
     * @return 文件流指针，失败返回 nullptr
     */
    static std::unique_ptr<std::ifstream> OpenFileStream(
        const FStringView& FilePath,
        bool BinaryMode = true);

    /**
     * 打开文件流（输入流）
     * @param FilePath 文件路径
     * @param BinaryMode 是否以二进制模式打开
     * @return 文件流指针，失败返回 nullptr
     */
    static std::unique_ptr<std::ifstream> OpenFileStream(
        const FString& FilePath,
        bool BinaryMode = true);

    /**
     * 打开文件流（输入流）
     * @param FilePath 文件路径（std::filesystem::path）
     * @param BinaryMode 是否以二进制模式打开
     * @return 文件流指针，失败返回 nullptr
     */
    static std::unique_ptr<std::ifstream> OpenFileStream(
        const std::filesystem::path& FilePath,
        bool BinaryMode = true);

    /**
     * 确保目录存在（递归创建）
     * @param DirPath 目录路径
     * @return 是否成功
     */
    static bool EnsureDirectoryExists(const FStringView& DirPath);

    /**
     * 确保目录存在（递归创建）
     * @param DirPath 目录路径
     * @return 是否成功
     */
    static bool EnsureDirectoryExists(const FString& DirPath);

    /**
     * 确保目录存在（递归创建）
     * @param DirPath 目录路径（std::filesystem::path）
     * @return 是否成功
     */
    static bool EnsureDirectoryExists(const std::filesystem::path& DirPath);

    /**
     * 确保文件所在目录存在（递归创建）
     * @param FilePath 文件路径
     * @return 是否成功
     */
    static bool EnsureFileDirectoryExists(const FStringView& FilePath);

    /**
     * 确保文件所在目录存在（递归创建）
     * @param FilePath 文件路径
     * @return 是否成功
     */
    static bool EnsureFileDirectoryExists(const FString& FilePath);

    /**
     * 确保文件所在目录存在（递归创建）
     * @param FilePath 文件路径（std::filesystem::path）
     * @return 是否成功
     */
    static bool EnsureFileDirectoryExists(const std::filesystem::path& FilePath);

    /**
     * 检查文件是否存在
     * @param FilePath 文件路径
     * @return 是否存在
     */
    static bool FileExists(const FStringView& FilePath);

    /**
     * 检查文件是否存在
     * @param FilePath 文件路径
     * @return 是否存在
     */
    static bool FileExists(const FString& FilePath);

    /**
     * 检查文件是否存在
     * @param FilePath 文件路径（std::filesystem::path）
     * @return 是否存在
     */
    static bool FileExists(const std::filesystem::path& FilePath);
};

