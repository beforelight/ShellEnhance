cmake_minimum_required(VERSION 3.5)

project(ShellE VERSION 0.1 LANGUAGES CXX)

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
configure_file("source/assets/globalStyleSheet.qss" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/style.qss" COPYONLY)

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

install(TARGETS ShellE
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ShellE)
endif ()
