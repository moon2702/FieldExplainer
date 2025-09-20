@echo off
setlocal enabledelayedexpansion

REM FieldExplainer Windows构建脚本

echo === FieldExplainer Windows构建脚本 ===

REM 检查CMake
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: 未找到CMake，请先安装CMake
    echo 下载地址: https://cmake.org/download/
    pause
    exit /b 1
)

REM 检查Qt6
if "%Qt6_DIR%"=="" (
    echo 警告: 未设置Qt6_DIR环境变量
    echo 请设置Qt6安装路径，例如:
    echo   set Qt6_DIR=C:\Qt\6.5.0\msvc2019_64\lib\cmake\Qt6
    echo.
    echo 或者安装Qt6到默认位置:
    echo   C:\Qt\6.5.0\msvc2019_64\
    echo.
)

REM 获取构建类型参数
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release
echo 构建类型: %BUILD_TYPE%

REM 创建构建目录
set BUILD_DIR=build-%BUILD_TYPE%
echo 创建构建目录: %BUILD_DIR%
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

REM 配置CMake
echo 配置CMake...
cmake .. ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
    -DCMAKE_INSTALL_PREFIX=../install

if %errorlevel% neq 0 (
    echo CMake配置失败
    pause
    exit /b 1
)

REM 编译
echo 开始编译...
cmake --build . --config %BUILD_TYPE% --parallel

if %errorlevel% neq 0 (
    echo 编译失败
    pause
    exit /b 1
)

REM 安装
echo 安装应用程序...
cmake --install . --config %BUILD_TYPE%

if %errorlevel% neq 0 (
    echo 安装失败
    pause
    exit /b 1
)

echo.
echo 构建完成！
echo 可执行文件位置: ..\install\bin\FieldExplainer.exe
echo.
echo 运行应用程序:
echo   cd ..\install ^&^& .\bin\FieldExplainer.exe
echo.

pause
