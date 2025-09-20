# 当前模块根目录
set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# 指定可执行文件目录 顶层目录下
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# 指定头文件路径
set(INCLUDE_DIR "${ROOT_DIR}/include")
set(EXTERNAL_INCLUDE_DIR "/usr/include /usr/local/include")

# 指定源文件路径
set(SOURCE_DIR "${ROOT_DIR}/src")

# 指定库路径
set(EXTERNAL_LIB_DIR "/usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu")

# 指定资源文件路径
set(RESOURCE_DIR "${ROOT_DIR}/resource")

# 指定UI文件路径
set(UI_DIR "${ROOT_DIR}/ui")

# 指定翻译文件路径
set(TRANSLATION_DIR "${ROOT_DIR}/translations")

# 获取资源文件夹下的所有文件
# file(GLOB CONFIG_FILES "${RESOURCE_DIR}/*")
# 将资源文件全部移动到bin目录下
# file(COPY ${CONFIG_FILES} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

# 指定moc文件路径 当前模块构建目录下
set(MOC_DIR "${CMAKE_CURRENT_BINARY_DIR}/moc")

# 设置Qt6相关路径
if(Qt6_FOUND)
    # Qt6安装路径
    get_target_property(Qt6_DIR Qt6::Core INTERFACE_QT_MAJOR_VERSION)
    message(STATUS "Found Qt6 version: ${Qt6_VERSION}")
    
    # 设置Qt6插件路径
    set(QT_PLUGIN_PATH "${Qt6_DIR}/plugins")
    
    # 设置Qt6库路径
    get_target_property(QT_LIBRARY_DIR Qt6::Core LOCATION)
    get_filename_component(QT_LIBRARY_DIR "${QT_LIBRARY_DIR}" DIRECTORY)
endif()

# 创建必要的目录
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
file(MAKE_DIRECTORY ${MOC_DIR})
