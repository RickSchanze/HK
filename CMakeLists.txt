cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Admin/Software/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(HKEngine 
    VERSION 1.0.0
    DESCRIPTION "HKEngine Game Engine"
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 默认使用clang/llvm工具链
if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "clang")
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "clang++")
endif()

# 生成 compile_commands.json 供 clangd 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 默认构建类型为Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

# 输出目录配置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# 平台检测和宏定义
if(WIN32)
    add_compile_definitions(HK_WINDOWS)
    message(STATUS "平台: Windows (HK_WINDOWS)")
elseif(APPLE)
    add_compile_definitions(HK_MACOS)
    message(STATUS "平台: macOS (HK_MACOS)")
elseif(UNIX)
    add_compile_definitions(HK_LINUX)
    message(STATUS "平台: Linux (HK_LINUX)")
endif()

# Debug构建时定义HK_DEBUG宏
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(HK_DEBUG)
    message(STATUS "构建类型: Debug (HK_DEBUG)")
else()
    message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
endif()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/Engine)

# 源文件目录
file(GLOB_RECURSE SOURCES 
    "Engine/*.cpp"
    "Engine/*.h"
    "Engine/*.hpp"
    "main.cpp"
)

# 如果还没有源文件，创建一个示例
if(NOT SOURCES)
    message(STATUS "未找到源文件，将创建示例文件")
    # 这里可以创建示例文件，但暂时不创建，让用户自己添加
endif()

# 创建可执行文件或库
if(SOURCES)
    add_executable(${PROJECT_NAME} ${SOURCES})
    
    # 设置目标属性
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )
    
    # 链接库（如果需要）
    # target_link_libraries(${PROJECT_NAME} ...)
endif()

# 安装规则（可选）
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

