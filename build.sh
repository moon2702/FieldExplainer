#!/bin/bash

# FieldExplainer 跨平台构建脚本

set -e  # 遇到错误时退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_message() {
    echo -e "${2}${1}${NC}"
}

print_message "=== FieldExplainer 构建脚本 ===" $BLUE

# 检查操作系统
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="Linux"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "win32" ]]; then
    OS="Windows"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macOS"
else
    print_message "不支持的操作系统: $OSTYPE" $RED
    exit 1
fi

print_message "检测到操作系统: $OS" $GREEN

# 检查依赖
print_message "检查构建依赖..." $YELLOW

# 检查CMake
if ! command -v cmake &> /dev/null; then
    print_message "错误: 未找到CMake，请先安装CMake" $RED
    exit 1
fi

# 检查Qt6
if [[ "$OS" == "Linux" ]]; then
    # Linux: 检查Qt6开发包
    if ! pkg-config --exists Qt6Core Qt6Widgets Qt6Gui Qt6Network; then
        print_message "错误: 未找到Qt6开发包" $RED
        print_message "请安装Qt6开发包，例如:" $YELLOW
        print_message "  Ubuntu/Debian: sudo apt install qt6-base-dev qt6-tools-dev" $YELLOW
        print_message "  Arch Linux: sudo pacman -S qt6-base" $YELLOW
        print_message "  Fedora: sudo dnf install qt6-qtbase-devel" $YELLOW
        exit 1
    fi
elif [[ "$OS" == "Windows" ]]; then
    # Windows: 检查Qt6安装
    if [[ -z "$Qt6_DIR" ]]; then
        print_message "警告: 未设置Qt6_DIR环境变量" $YELLOW
        print_message "请设置Qt6安装路径，例如:" $YELLOW
        print_message "  set Qt6_DIR=C:/Qt/6.5.0/msvc2019_64/lib/cmake/Qt6" $YELLOW
    fi
fi

# 构建类型
BUILD_TYPE=${1:-Release}
print_message "构建类型: $BUILD_TYPE" $GREEN

# 创建构建目录
BUILD_DIR="build-${BUILD_TYPE,,}"
print_message "创建构建目录: $BUILD_DIR" $YELLOW
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 配置CMake
print_message "配置CMake..." $YELLOW
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_INSTALL_PREFIX="../install"

# 编译
print_message "开始编译..." $YELLOW
if [[ "$OS" == "Windows" ]]; then
    # Windows使用多线程编译
    cmake --build . --config "$BUILD_TYPE" --parallel
else
    # Linux/macOS使用make
    make -j$(nproc)
fi

# 安装
print_message "安装应用程序..." $YELLOW
cmake --install . --config "$BUILD_TYPE"

print_message "构建完成！" $GREEN
print_message "可执行文件位置: ../install/bin/FieldExplainer" $GREEN

# 如果是Linux，提供运行建议
if [[ "$OS" == "Linux" ]]; then
    print_message "运行应用程序:" $BLUE
    print_message "  cd ../install && ./bin/FieldExplainer" $BLUE
fi

# 如果是Windows，提供运行建议
if [[ "$OS" == "Windows" ]]; then
    print_message "运行应用程序:" $BLUE
    print_message "  cd ../install && ./bin/FieldExplainer.exe" $BLUE
fi
