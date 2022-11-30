cmake_minimum_required(VERSION 3.22)

project(ShellE VERSION 0.1.2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/x64/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/x64/bin)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort)

add_subdirectory(3rd/Qt-Advanced-Docking-System)
include_directories(3rd/Qt-Advanced-Docking-System/src)





include_directories(source)
include_directories(source/content)
include_directories(source/device)
include_directories(source/others)
file(GLOB_RECURSE PROJECT_SOURCES
        "source/assets/assets.qrc"
        "source/assets/app_win32.rc"
        "source/*.cpp"
        "source/*.h"
        "source/*.ui"
        "source/*.qrc"
        "source/device/*.cpp"
        "source/others/*.cpp"
        "source/content/*.cpp"
        "source/content/*.h"
        "source/content/*.ui"
        )
file(COPY source/content/cp_ini_default.cpini DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
configure_file("source/assets/style.qss" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/style.qss" COPYONLY)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ShellE
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            )
    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET ShellE APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else ()
    if (ANDROID)
        add_library(ShellE SHARED
                ${PROJECT_SOURCES}
                )
        # Define properties for Android with Qt 5 after find_package() calls as:
        #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else ()
        add_executable(ShellE
                ${PROJECT_SOURCES}
                )
    endif ()
endif ()

target_link_libraries(ShellE PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(ShellE PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)
target_link_libraries(ShellE PRIVATE qtadvanceddocking)

set_target_properties(ShellE PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
        )

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ShellE)
endif ()

## 打包相关
if (WIN32 AND "${CMAKE_BUILD_TYPE}" STREQUAL "Release")

    install(TARGETS ShellE
            BUNDLE DESTINATION .
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            )
    install(FILES source/assets/style.qss DESTINATION bin)
    include(InstallRequiredSystemLibraries) ## 添加vcruntime
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE) # 如果应用程序使用Visual Studio 2015或者更新的版本
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "ShellEnhance") ## 设置安装的文件夹名字
    set(CPACK_INSTALL_PREFIX "c:/") #安装目录
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_PACKAGE_NAME "shellE")
    set(CPACK_PACKAGE_VENDOR "xiao")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "用于嵌入式终端交互的上位机")
    set(CPACK_RPM_PACKAGE_GROUP "微纳光学教研室") # 设置 group 名字
    set(CPACK_PACKAGE_VENDOR "微纳光学教研室") # 设置 vendor 名字
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

    ## 拷贝QT库 end # install 从3.12开始支持生成表达式
    # install 从3.12开始支持生成表达式
    get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    add_custom_command(TARGET ShellE POST_BUILD
#            COMMAND "${CMAKE_COMMAND}" -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/qtDeploy/"
            COMMAND "${CMAKE_COMMAND}" -E
            env PATH="${_qt_bin_dir}" windeployqt
            --verbose 0
            --no-compiler-runtime
            --no-system-d3d-compiler
            --no-opengl-sw
            --no-quick-import
            --no-translations
            --dir "${CMAKE_CURRENT_BINARY_DIR}/qtDeploy/" $<TARGET_FILE:ShellE>
            COMMENT "Deploying Qt..."
            )

    install(
            DIRECTORY "${CMAKE_BINARY_DIR}/qtDeploy/"
            DESTINATION bin
    )
    ## 拷贝QT库 end

    ## 生成快捷方式
    set(EXE_TARGET_NAME "ShellE")
    set(EWS_APP_NAME "Shell增强")
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    CreateShortCut \\\"$DESKTOP\\\\${EWS_APP_NAME}.lnk\\\" \\\"$INSTDIR\\\\bin\\\\${EXE_TARGET_NAME}.exe\\\"
    ")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    Delete \\\"$DESKTOP\\\\${EWS_APP_NAME}.lnk\\\"
        ")
    ## 生成快捷方式 end
    INCLUDE(CPack)
endif ()
