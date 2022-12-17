cmake_minimum_required(VERSION 3.5)

project(nonblock-socket-sample-qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt5を使う場合 .. QT NAMES Qt5 REQUIRED  Qt6を使うときは Qt6 REQUIRED にする
# <var>ライブラリ名</var> REQUIRED までがライブラリの指定, COMPONENTS 以下でコ
# ンポーネントの追加.
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        connect_thread.cpp connect_thread.h
        ../ipv6_sock/connect_to_server.cpp ../ipv6_sock/connect_to_server.h
        ../ipv6_sock/debug_print.cpp ../ipv6_sock/debug_print.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(nonblock-socket-sample-qt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET nonblock-socket-sample-qt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(nonblock-socket-sample-qt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(nonblock-socket-sample-qt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(nonblock-socket-sample-qt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(nonblock-socket-sample-qt PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(nonblock-socket-sample-qt)
endif()
