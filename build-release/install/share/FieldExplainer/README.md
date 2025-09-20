# FieldExplainer

一个快速解释字段含义的跨平台桌面应用程序。通过监控剪贴板内容，在鼠标旁显示悬浮图标，点击后分析字段含义并展示结果。

## 功能特性

- 🔍 **智能字段识别**: 自动识别数据库字段、API字段、配置字段等
- 📋 **剪贴板监控**: 实时监控 `Ctrl+C` 复制的内容
- 🖱️ **悬浮图标**: 在鼠标位置显示可点击的分析图标
- 🌐 **跨平台支持**: 支持 Windows 和 Linux 系统
- ⚡ **快速分析**: 基于规则引擎和模式匹配的快速字段分析
- 📊 **结果展示**: 清晰的字段含义展示界面

## 系统要求

### Linux
- Ubuntu 20.04+ / Fedora 33+ / Arch Linux (最新)
- Qt6 开发包
- CMake 3.20+
- GCC 9+ 或 Clang 10+

### Windows
- Windows 10/11
- Qt6 6.5+
- CMake 3.20+
- Visual Studio 2019+ 或 MinGW

## 安装依赖

### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential
```

### Linux (Arch Linux)
```bash
sudo pacman -S qt6-base cmake gcc
```

### Linux (Fedora)
```bash
sudo dnf install qt6-qtbase-devel cmake gcc-c++
```

### Windows
1. 安装 [Qt6](https://www.qt.io/download) (选择开源版本)
2. 安装 [CMake](https://cmake.org/download/)
3. 安装 Visual Studio 或 MinGW

## 构建项目

### 使用构建脚本 (推荐)

#### Linux
```bash
./build.sh Release
```

#### Windows
```cmd
build.bat Release
```

### 手动构建

#### Linux
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
make install
```

#### Windows
```cmd
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install
cmake --build . --config Release --parallel
cmake --install . --config Release
```

## 运行应用

### Linux
```bash
cd install
./bin/FieldExplainer
```

### Windows
```cmd
cd install
.\bin\FieldExplainer.exe
```

## 使用说明

1. **启动应用**: 运行 FieldExplainer 后，应用会在后台运行
2. **复制字段**: 使用 `Ctrl+C` 复制任何文本字段
3. **查看图标**: 复制后，鼠标旁会出现一个小图标
4. **点击分析**: 点击图标开始分析字段含义
5. **查看结果**: 分析结果会显示在悬浮窗口中

## 项目结构

```
FieldExplainer/
├── cmake/                 # CMake 配置文件
│   ├── BuildSettings.cmake
│   ├── InstallConfig.cmake
│   └── FieldExplainer.desktop.in
├── include/              # 头文件
│   └── FieldExplainer/
│       └── main.h
├── src/                  # 源代码
│   └── main.cpp
├── resource/             # 资源文件
│   ├── resources.qrc
│   ├── icons/           # 图标文件
│   └── styles/          # 样式文件
├── build.sh             # Linux 构建脚本
├── build.bat            # Windows 构建脚本
├── CMakeLists.txt       # 主 CMake 配置
└── README.md           # 项目说明
```

## 开发计划

- [x] 基础项目架构
- [x] CMake 跨平台配置
- [ ] 剪贴板监控功能
- [ ] 悬浮图标组件
- [ ] 字段分析引擎
- [ ] 结果显示界面
- [ ] 用户配置功能
- [ ] 历史记录功能

## 贡献

欢迎提交 Issue 和 Pull Request！

## 许可证

本项目采用 MIT 许可证。详见 LICENSE 文件。
