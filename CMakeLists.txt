cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Admin/Software/vcpkg/scripts/buildsystems/vcpkg.cmake")

# 在项目定义前执行代码生成脚本
find_program(PYTHON_EXECUTABLE python python3)
if(PYTHON_EXECUTABLE)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/HeaderTools.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GENERATION_RESULT
    )
    if(NOT GENERATION_RESULT EQUAL 0)
        message(WARNING "代码生成脚本执行失败，返回码: ${GENERATION_RESULT}")
    else()
        message(STATUS "代码生成脚本执行成功")
    endif()
else()
    message(WARNING "未找到 Python 解释器，跳过代码生成")
endif()

project(HKEngine 
    VERSION 1.0.0
    DESCRIPTION "HKEngine Game Engine"
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
    # MSVC 使用 /std:c++latest 来支持 C++23
    set(CMAKE_CXX_STANDARD 20)  # MSVC 对 C++23 支持有限，先使用 C++20
    add_compile_options(/std:c++latest)
else()
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# 默认使用clang/llvm工具链（仅在未指定编译器且非MSVC时）
# 如果使用 MSVC，CMake 会自动检测，不需要手动设置
if(NOT MSVC AND NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "clang")
endif()
if(NOT MSVC AND NOT DEFINED CMAKE_CXX_COMPILER)
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

# 编译选项 - 根据编译器类型设置
if(MSVC)
    # MSVC 编译选项
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /permissive-")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
else()
    # GCC/Clang 编译选项
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

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
    add_compile_definitions(HK_DEBUG=1)
    message(STATUS "构建类型: Debug (HK_DEBUG)")
else()
    message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
endif()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/Engine)
include_directories(${CMAKE_SOURCE_DIR}/Engine/Generated)

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
    add_executable(${PROJECT_NAME} ${SOURCES}
            Engine/RHI/RHIBuffer.cpp
            Engine/RHI/RHIBuffer.h
            Engine/RHI/GfxDevice.cpp
            Engine/RHI/GfxDevice.h)
    
    # 设置目标属性
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
    )
    
    # 添加 natvis 文件支持（MSVC 调试器可视化）
    if(MSVC AND EXISTS "${CMAKE_SOURCE_DIR}/Debug/HKEngine.natvis")
        set_target_properties(${PROJECT_NAME} PROPERTIES
            VS_DEBUGGER_VISUALIZERS_FILE "${CMAKE_SOURCE_DIR}/Debug/HKEngine.natvis"
        )
    endif()
    
    # 链接库（如果需要）
    # target_link_libraries(${PROJECT_NAME} ...)
endif()

# 安装规则（可选）
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

find_package(cereal CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE cereal::cereal)

