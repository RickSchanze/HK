#include "FileUtility.h"
#include "Core/Logging/Logger.h"
#include <filesystem>

TUniquePtr<std::ofstream> FFileUtility::CreateFileStream(
    const FStringView& FilePath,
    bool CreateDirectories,
    bool BinaryMode)
{
    return CreateFileStream(std::filesystem::path(FilePath.GetStdStringView()), CreateDirectories, BinaryMode);
}

TUniquePtr<std::ofstream> FFileUtility::CreateFileStream(
    const FString& FilePath,
    bool CreateDirectories,
    bool BinaryMode)
{
    return CreateFileStream(std::filesystem::path(FilePath.GetStdString()), CreateDirectories, BinaryMode);
}

TUniquePtr<std::ofstream> FFileUtility::CreateFileStream(
    const std::filesystem::path& FilePath,
    bool CreateDirectories,
    bool BinaryMode)
{
    // 如果需要创建目录
    if (CreateDirectories)
    {
        const std::filesystem::path DirPath = FilePath.parent_path();
        if (!DirPath.empty() && !std::filesystem::exists(DirPath))
        {
            std::error_code ErrorCode;
            if (!std::filesystem::create_directories(DirPath, ErrorCode))
            {
                HK_LOG_ERROR(ELogcat::Engine, "Failed to create directories for path: {}", DirPath.string());
                return nullptr;
            }
        }
    }

    // 创建文件流
    std::ios_base::openmode Mode = std::ios::out;
    if (BinaryMode)
    {
        Mode |= std::ios::binary;
    }

    auto FileStream = MakeUnique<std::ofstream>(FilePath.string(), Mode);
    if (!FileStream || !FileStream->is_open())
    {
        HK_LOG_ERROR(ELogcat::Engine, "Failed to open file for writing: {}", FilePath.string());
        return nullptr;
    }

    return FileStream;
}

std::unique_ptr<std::ifstream> FFileUtility::OpenFileStream(
    const FStringView& FilePath,
    bool BinaryMode)
{
    return OpenFileStream(std::filesystem::path(FilePath.GetStdStringView()), BinaryMode);
}

std::unique_ptr<std::ifstream> FFileUtility::OpenFileStream(
    const FString& FilePath,
    bool BinaryMode)
{
    return OpenFileStream(std::filesystem::path(FilePath.GetStdString()), BinaryMode);
}

std::unique_ptr<std::ifstream> FFileUtility::OpenFileStream(
    const std::filesystem::path& FilePath,
    bool BinaryMode)
{
    std::ios_base::openmode Mode = std::ios::in;
    if (BinaryMode)
    {
        Mode |= std::ios::binary;
    }

    auto FileStream = std::make_unique<std::ifstream>(FilePath.string(), Mode);
    if (!FileStream->is_open())
    {
        HK_LOG_ERROR(ELogcat::Engine, "Failed to open file for reading: {}", FilePath.string());
        return nullptr;
    }

    return FileStream;
}

bool FFileUtility::EnsureDirectoryExists(const FStringView& DirPath)
{
    return EnsureDirectoryExists(std::filesystem::path(DirPath.GetStdStringView()));
}

bool FFileUtility::EnsureDirectoryExists(const FString& DirPath)
{
    return EnsureDirectoryExists(std::filesystem::path(DirPath.GetStdString()));
}

bool FFileUtility::EnsureDirectoryExists(const std::filesystem::path& DirPath)
{
    if (DirPath.empty())
    {
        return true; // 空路径视为成功
    }

    if (std::filesystem::exists(DirPath))
    {
        return true; // 已存在
    }

    std::error_code ErrorCode;
    if (!std::filesystem::create_directories(DirPath, ErrorCode))
    {
        HK_LOG_ERROR(ELogcat::Engine, "Failed to create directories: {}", DirPath.string());
        return false;
    }

    return true;
}

bool FFileUtility::EnsureFileDirectoryExists(const FStringView& FilePath)
{
    return EnsureFileDirectoryExists(std::filesystem::path(FilePath.GetStdStringView()));
}

bool FFileUtility::EnsureFileDirectoryExists(const FString& FilePath)
{
    return EnsureFileDirectoryExists(std::filesystem::path(FilePath.GetStdString()));
}

bool FFileUtility::EnsureFileDirectoryExists(const std::filesystem::path& FilePath)
{
    const std::filesystem::path DirPath = FilePath.parent_path();
    return EnsureDirectoryExists(DirPath);
}

bool FFileUtility::FileExists(const FStringView& FilePath)
{
    return FileExists(std::filesystem::path(FilePath.GetStdStringView()));
}

bool FFileUtility::FileExists(const FString& FilePath)
{
    return FileExists(std::filesystem::path(FilePath.GetStdString()));
}

bool FFileUtility::FileExists(const std::filesystem::path& FilePath)
{
    return std::filesystem::exists(FilePath) && std::filesystem::is_regular_file(FilePath);
}

