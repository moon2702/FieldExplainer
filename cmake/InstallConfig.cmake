# 安装配置
# 设置安装前缀
if(NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)
endif()

# 注意: 安装目标将在主CMakeLists.txt中定义，这里只设置安装前缀

# 这些安装规则将在主CMakeLists.txt中定义，避免目标不存在的问题

# Windows特定安装配置
if(WIN32)
    # 使用windeployqt自动部署Qt依赖
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)
    if(WINDEPLOYQT_EXECUTABLE)
        install(CODE "
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E echo \"Deploying Qt libraries for Windows...\"
                COMMAND ${WINDEPLOYQT_EXECUTABLE} --dir \"${CMAKE_INSTALL_PREFIX}/bin\" \"${CMAKE_INSTALL_PREFIX}/bin/${OUT_NAME}.exe\"
            )
        ")
    endif()
endif()

# 通用安装后处理
install(CODE "
    message(STATUS \"FieldExplainer installed to: ${CMAKE_INSTALL_PREFIX}\")
    message(STATUS \"Executable: ${CMAKE_INSTALL_PREFIX}/bin/${OUT_NAME}\")
")
