# HKEngine

HKEngine 游戏引擎项目

## 构建要求

- CMake 3.20 或更高版本
- Clang/LLVM 工具链
- CodeLLDB 扩展（用于调试）

## 构建说明

### Debug 构建（默认）

Debug 构建是默认的构建类型，会自动定义 `HK_DEBUG` 宏。

**使用 CMake 命令行：**

```bash
# 配置项目（Debug）
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug

# 构建项目
cmake --build build --config Debug
```

**使用 VS Code：**

1. 按 `Ctrl+Shift+P` 打开命令面板
2. 选择 "Tasks: Run Task"
3. 选择 "Build Debug"

或者直接按 `F5` 启动调试（会自动构建）。

### Release 构建

**使用 CMake 命令行：**

```bash
# 配置项目（Release）
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 构建项目
cmake --build build --config Release
```

**使用 VS Code：**

1. 按 `Ctrl+Shift+P` 打开命令面板
2. 选择 "Tasks: Run Task"
3. 选择 "Build Release"

### 清理构建

```bash
cmake --build build --target clean
```

或在 VS Code 中运行 "Clean" 任务。

## 调试

项目已配置 CodeLLDB 调试器。在 VS Code 中：

1. 按 `F5` 启动调试
2. 或使用 "Run and Debug" 面板选择 "Debug (CodeLLDB)"

调试配置会自动构建 Debug 版本并启动程序。

## 代码风格

项目使用虚幻引擎风格的代码格式：

- **类命名**：使用 `U` 前缀（如 `UObject`, `UEngine`）
- **格式化**：使用 `.clang-format` 配置文件（虚幻引擎风格）
- **缩进**：4 个空格
- **行宽**：120 字符

### 格式化代码

```bash
# 格式化所有源文件
clang-format -i src/**/*.{cpp,h,hpp}
```

## 项目结构

```
HKEngine/
├── CMakeLists.txt          # CMake 配置文件
├── .clang-format           # 代码格式化配置
├── .vscode/                # VS Code 配置
│   ├── launch.json        # 调试配置
│   └── tasks.json         # 构建任务
├── include/                # 头文件目录
├── src/                    # 源文件目录
└── build/                  # 构建输出目录（gitignore）
```

## 注意事项

- 确保已安装 Clang/LLVM 工具链
- 在 Windows 上，可执行文件扩展名为 `.exe`
- Debug 构建会自动定义 `HK_DEBUG` 宏，可在代码中使用 `#ifdef HK_DEBUG` 进行条件编译

