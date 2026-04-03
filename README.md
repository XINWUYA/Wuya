# Wuya

## 环境

- 安装[VulkanSDK](https://vulkan.lunarg.com/sdk/home#windows)（主要用来编译shader cache）
- CMake 3.20+
- Visual Studio 2017/2019/2022/2026（Windows）或 Xcode（macOS）

## 生成工程

### 方式一：CMake构建（推荐）

使用CMake构建系统，自动处理所有依赖和配置文件：

**Windows:**
```bash
# 自动检测已安装的Visual Studio
Win-GenProj-CMake.bat

# 或手动指定VS版本
Win-GenProj-CMake.bat vs2022    # VS 2022
Win-GenProj-CMake.bat vs2019    # VS 2019
Win-GenProj-CMake.bat ninja     # Ninja
```

**macOS:**
```bash
chmod +x Mac-GenProj-CMake.sh
./Mac-GenProj-CMake.sh          # 生成Xcode工程
./Mac-GenProj-CMake.sh ninja    # 生成Ninja工程
```

**优势：**
- ✅ 无需手动配置assimp库
- ✅ 自动生成所需的config.h、revision.h、zconf.h等配置文件
- ✅ 跨平台支持（Windows/macOS/Linux）
- ✅ 支持多种生成器（Visual Studio、Xcode、Ninja、Makefiles等）

构建完成后，打开生成的工程文件即可编译运行：
- Windows: `build/Wuya.sln`
- macOS: `build/Wuya.xcodeproj`



## 功能特性

- FrameGraph
- PBR
- Deferred Shading
- Tracy

## 使用Tracy

1. 先执行`Wuya\Libraries\ThirdParty\tracy\vcpkg\install_vcpkg_dependencies.bat`安装所需依赖；

2. 使用Visual Studio打开Tracy工程：`Wuya\Libraries\ThirdParty\tracy\profiler\build\win32\Tracy.sln`，并运行，点击connect启动监听；
   
   ![](./Documents/Imgs/Tracy.png)

3. 运行Editor，即可连接Tracy分析。
   
   ![](./Documents/Imgs/TracyResult.png)