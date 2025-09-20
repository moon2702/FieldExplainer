cmake_minimum_required(VERSION 3.20)
project(FieldExplainer VERSION 1.0.0 LANGUAGES CXX)

# 指定标准并强制要求使用
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用自动MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 判断操作系统类型
if(WIN32)
    message(STATUS "当前是Windows系统下")
    # Windows特定配置
    set(CMAKE_C_COMPILER "D:/Programme_Exploit/MinGW/bin/gcc.exe")
    set(CMAKE_CXX_COMPILER "D:/Programme_Exploit/MinGW/bin/g++.exe")
    # Windows平台特定设置
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
elseif(APPLE)
    message(STATUS "当前是MacOS系统下")
    # MacOS特定配置
    add_definitions(-DAPPLE)
elseif(UNIX AND NOT APPLE)
    message(STATUS "当前是Linux系统下")
    # Linux特定配置
    add_definitions(-DLINUX)
    # 查找X11库 (Linux剪贴板支持)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(X11 REQUIRED x11)
endif()

# 指定项目可执行文件名
set(OUT_NAME "FieldExplainer")

include(cmake/BuildSettings.cmake)
include(cmake/InstallConfig.cmake)

# 查找Qt6库
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network)

# 设置Qt6相关属性
set_target_properties(Qt6::Core PROPERTIES
    INTERFACE_COMPILE_FEATURES cxx_std_17
)

# 查找源码文件
file(GLOB_RECURSE CPPFILE ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE CCFILE ${SOURCE_DIR}/*.cc)
file(GLOB_RECURSE CFILE ${SOURCE_DIR}/*.c)

# 查找Qt资源文件
file(GLOB_RECURSE QRC_FILE ${RESOURCE_DIR}/*.qrc)

# 归档使用的源码文件
add_executable(${OUT_NAME} 
    ${CPPFILE} 
    ${CCFILE} 
    ${CFILE}
    ${QRC_FILE}
)

# 通过CMAKE_BUILD_TYPE变量来指定编译选项
target_compile_options(${OUT_NAME}  
    PRIVATE 
    $<$<CONFIG:Debug>:-g> 
    $<$<CONFIG:Release>:-O3> 
    $<$<CONFIG:RelWithDebInfo>:-O2 -g> 
    $<$<CONFIG:MinSizeRel>:-Os> 
)

# 归档使用的头文件
target_include_directories(${OUT_NAME}
    PRIVATE 
    ${INCLUDE_DIR} 
    ${EXTERNAL_INCLUDE_DIR} 
)

# 链接Qt6库
target_link_libraries(${OUT_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Network
)

# 平台特定的库链接
if(UNIX AND NOT APPLE)
    # Linux平台链接X11库
    target_link_libraries(${OUT_NAME} PRIVATE ${X11_LIBRARIES})
    target_include_directories(${OUT_NAME} PRIVATE ${X11_INCLUDE_DIRS})
endif()

# 设置目标属性
set_target_properties(${OUT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Windows特定设置
if(WIN32)
    set_target_properties(${OUT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()

# 安装规则
# 安装可执行文件
install(TARGETS ${OUT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装资源文件
install(DIRECTORY ${RESOURCE_DIR}/
    DESTINATION share/FieldExplainer
    FILES_MATCHING 
    PATTERN "*.png"
    PATTERN "*.jpg"
    PATTERN "*.qss"
    PATTERN "*.json"
    PATTERN "*.qrc"
)

# 安装配置文件
install(FILES 
    ${CMAKE_SOURCE_DIR}/README.md
    DESTINATION share/FieldExplainer
)

# Linux特定安装配置
if(UNIX AND NOT APPLE)
    # 创建.desktop文件
    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/FieldExplainer.desktop.in
        ${CMAKE_BINARY_DIR}/FieldExplainer.desktop
        @ONLY
    )
    
    install(FILES ${CMAKE_BINARY_DIR}/FieldExplainer.desktop
        DESTINATION share/applications
    )
    
    # 安装图标 (如果存在)
    if(EXISTS ${RESOURCE_DIR}/icons/app_icon.png)
        install(FILES ${RESOURCE_DIR}/icons/app_icon.png
            DESTINATION share/icons/hicolor/256x256/apps
            RENAME FieldExplainer.png
        )
    endif()
endif()
