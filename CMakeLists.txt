cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Admin/Software/vcpkg/scripts/buildsystems/vcpkg.cmake")

# ==========================================
# [新增] Slang 路径配置
# ==========================================
set(SLANG_ROOT "C:/Users/Admin/Software/slang")

# 在项目定义前执行代码生成脚本
find_program(PYTHON_EXECUTABLE python python3)
if (PYTHON_EXECUTABLE)
    execute_process(
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/HeaderTools.py
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE GENERATION_RESULT
    )
    if (NOT GENERATION_RESULT EQUAL 0)
        message(WARNING "代码生成脚本执行失败，返回码: ${GENERATION_RESULT}")
    else ()
        message(STATUS "代码生成脚本执行成功")
    endif ()
else ()
    message(WARNING "未找到 Python 解释器，跳过代码生成")
endif ()

project(HKEngine
        VERSION 1.0.0
        DESCRIPTION "HKEngine Game Engine"
        LANGUAGES CXX
)

# ==========================================
# [新增] 配置 Slang 导入目标 (Modern CMake)
# ==========================================
if (EXISTS ${SLANG_ROOT})
    message(STATUS "找到 Slang SDK: ${SLANG_ROOT}")

    # 定义导入目标
    add_library(Slang::Slang SHARED IMPORTED GLOBAL)

    # 设置头文件路径
    set_target_properties(Slang::Slang PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SLANG_ROOT}/include"
    )

    # 设置库文件路径 (Windows)
    if (WIN32)
        set_target_properties(Slang::Slang PROPERTIES
                IMPORTED_IMPLIB "${SLANG_ROOT}/lib/slang.lib"
                IMPORTED_LOCATION "${SLANG_ROOT}/bin/slang.dll"
        )
    else()
        # Linux/Mac 备用
        set_target_properties(Slang::Slang PROPERTIES
                IMPORTED_LOCATION "${SLANG_ROOT}/lib/libslang.so"
        )
    endif()
else()
    message(FATAL_ERROR "未找到 Slang SDK，请检查路径: ${SLANG_ROOT}")
endif()

# 设置C++标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (MSVC)
    # MSVC 使用 /std:c++latest 来支持 C++23
    set(CMAKE_CXX_STANDARD 20)  # MSVC 对 C++23 支持有限，先使用 C++20
    add_compile_options(/std:c++latest)
else ()
    set(CMAKE_CXX_EXTENSIONS OFF)
endif ()

# 默认使用clang/llvm工具链
if (NOT MSVC AND NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "clang")
endif ()
if (NOT MSVC AND NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "clang++")
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 默认构建类型为Debug
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif ()

# 输出目录配置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 编译选项
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /permissive-")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4193")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif ()

# 平台检测和宏定义
if (WIN32)
    add_compile_definitions(HK_WINDOWS)
    message(STATUS "平台: Windows (HK_WINDOWS)")
elseif (APPLE)
    add_compile_definitions(HK_MACOS)
elseif (UNIX)
    add_compile_definitions(HK_LINUX)
endif ()

# Debug构建时定义HK_DEBUG宏
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(HK_DEBUG=1)
else ()
    message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
endif ()

# 性能分析选项
option(HK_ENABLE_PROFILING "启用性能分析 (Tracy)" ON)
if (HK_ENABLE_PROFILING)
    add_compile_definitions(HK_ENABLE_PROFILING=1)
else ()
    add_compile_definitions(HK_ENABLE_PROFILING=0)
endif ()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/Engine)
include_directories(${CMAKE_SOURCE_DIR}/Engine/Generated)

# 收集源文件
file(GLOB_RECURSE ENGINE_SOURCES "Engine/*.cpp" "Engine/*.h" "Engine/*.hpp")
file(GLOB_RECURSE GENERATED_SOURCES "Engine/Generated/*.cpp" "Engine/Generated/*.h")
list(APPEND ENGINE_SOURCES ${GENERATED_SOURCES})

# 创建HK动态库
add_library(HK SHARED ${ENGINE_SOURCES})

# 为动态库定义HK_BUILDING_DLL宏
target_compile_definitions(HK PRIVATE HK_BUILDING_DLL)

set_target_properties(HK PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        OUTPUT_NAME "HK"
)

if (MSVC)
    set_target_properties(HK PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif ()

# 查找并链接依赖库
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(HK PRIVATE spdlog::spdlog_header_only)

find_package(cereal CONFIG REQUIRED)
target_link_libraries(HK PRIVATE cereal::cereal)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(HK PRIVATE fmt::fmt)

find_package(glm CONFIG REQUIRED)
target_link_libraries(HK PRIVATE glm::glm)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(HK PRIVATE SDL3::SDL3)

find_package(Tracy CONFIG REQUIRED)
target_link_libraries(HK PRIVATE Tracy::TracyClient)

find_package(Stb REQUIRED)
target_include_directories(HK PRIVATE ${Stb_INCLUDE_DIR})

find_package(assimp CONFIG REQUIRED)
target_link_libraries(HK PRIVATE assimp::assimp)

find_package(xxHash CONFIG REQUIRED)
target_link_libraries(HK PRIVATE xxHash::xxhash)

# ==========================================
# [新增] 链接 Slang 到 HK 核心库
# ==========================================
target_link_libraries(HK PRIVATE Slang::Slang)

# 创建可执行文件
add_executable(${PROJECT_NAME} main.cpp)

# 配置 Vulkan SDK
set(VULKAN_SDK "C:/VulkanSDK/1.4.335.0")
if (EXISTS ${VULKAN_SDK})
    message(STATUS "找到 Vulkan SDK: ${VULKAN_SDK}")
    set(VULKAN_INCLUDE_DIR "${VULKAN_SDK}/Include")

    if (WIN32)
        set(VULKAN_LIB_DIR "${VULKAN_SDK}/Lib")
        set(VULKAN_LIB "${VULKAN_LIB_DIR}/vulkan-1.lib")
    endif ()

    target_include_directories(HK PRIVATE ${VULKAN_INCLUDE_DIR})

    if (WIN32 AND EXISTS ${VULKAN_LIB})
        target_link_libraries(HK PRIVATE ${VULKAN_LIB})
    endif ()

    if (WIN32)
        set(VULKAN_LOADER_DLL "${VULKAN_SDK}/Bin/vulkan-1.dll")
        if (EXISTS ${VULKAN_LOADER_DLL})
            add_custom_command(TARGET HK POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${VULKAN_LOADER_DLL}
                    $<TARGET_FILE_DIR:HK>
                    COMMENT "复制 Vulkan 加载器 DLL"
            )
        endif ()
    endif ()
else ()
    message(WARNING "Vulkan SDK 路径不存在: ${VULKAN_SDK}")
endif ()

# ==========================================
# [新增] 复制 Slang DLLs 到输出目录
# ==========================================
if (WIN32)
    add_custom_command(TARGET HK POST_BUILD
            # 1. 复制 slang.dll (核心编译器)
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SLANG_ROOT}/bin/slang.dll"
            "$<TARGET_FILE_DIR:HK>"

            # 2. 复制 slang-glslang.dll (这对 Vulkan/SPIR-V 是必须的)
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SLANG_ROOT}/bin/slang-glslang.dll"
            "$<TARGET_FILE_DIR:HK>"

            COMMENT "复制 Slang DLLs 到输出目录"
    )
endif()

# 设置可执行文件目标属性
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        HK
        spdlog::spdlog_header_only
        cereal::cereal
        fmt::fmt
        glm::glm
        Tracy::TracyClient
)

if (MSVC AND EXISTS "${CMAKE_SOURCE_DIR}/Debug/HKEngine.natvis")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            VS_DEBUGGER_VISUALIZERS_FILE "${CMAKE_SOURCE_DIR}/Debug/HKEngine.natvis"
    )
endif ()