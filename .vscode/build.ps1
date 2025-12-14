# 智能构建脚本 - 只在需要时才运行CMake配置
param(
    [string]$BuildType = "Debug",
    [switch]$ForceConfigure
)

# 获取工作区根目录（脚本在.vscode目录中）
$WorkspaceRoot = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $WorkspaceRoot "build"
$CMakeCache = Join-Path $BuildDir "CMakeCache.txt"
$CMakeLists = Join-Path $WorkspaceRoot "CMakeLists.txt"
$VcpkgJson = Join-Path $WorkspaceRoot "vcpkg.json"

# 检查是否需要重新配置
$NeedConfigure = $ForceConfigure

if (-not $NeedConfigure) {
    # 如果CMakeCache不存在，需要配置
    if (-not (Test-Path $CMakeCache)) {
        Write-Host "CMakeCache.txt 不存在，需要配置..." -ForegroundColor Yellow
        $NeedConfigure = $true
    }
    else {
        # 检查CMakeLists.txt或vcpkg.json是否比CMakeCache新
        $CacheTime = (Get-Item $CMakeCache).LastWriteTime
        $ListsTime = (Get-Item $CMakeLists).LastWriteTime
        
        if ($ListsTime -gt $CacheTime) {
            Write-Host "CMakeLists.txt 已更新，需要重新配置..." -ForegroundColor Yellow
            $NeedConfigure = $true
        }
        
        if (Test-Path $VcpkgJson) {
            $VcpkgTime = (Get-Item $VcpkgJson).LastWriteTime
            if ($VcpkgTime -gt $CacheTime) {
                Write-Host "vcpkg.json 已更新，需要重新配置..." -ForegroundColor Yellow
                $NeedConfigure = $true
            }
        }
    }
}

# 如果需要配置，运行CMake配置
if ($NeedConfigure) {
    Write-Host "运行 CMake 配置..." -ForegroundColor Cyan
    cmake -B "$BuildDir" -S "$WorkspaceRoot" -DCMAKE_BUILD_TYPE=$BuildType -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    if ($LASTEXITCODE -ne 0) {
        Write-Host "CMake 配置失败！" -ForegroundColor Red
        exit $LASTEXITCODE
    }
}
else {
    Write-Host "跳过 CMake 配置（使用缓存）" -ForegroundColor Green
}

# 构建项目
Write-Host "构建项目 ($BuildType)..." -ForegroundColor Cyan
cmake --build "$BuildDir" --config $BuildType --parallel
if ($LASTEXITCODE -ne 0) {
    Write-Host "构建失败！" -ForegroundColor Red
    exit $LASTEXITCODE
}
Write-Host "构建成功！" -ForegroundColor Green
exit 0

