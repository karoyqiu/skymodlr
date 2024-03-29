﻿cmake_minimum_required(VERSION 3.12)

project(skymodlr VERSION 0.0.1.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

# Add the cmake modules directory to our module path.
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

include(CheckIPOSupported OPTIONAL)
if(COMMAND check_ipo_supported)
    check_ipo_supported(RESULT PROJECT_IS_IPO_SUPPORTED)

    if(PROJECT_IS_IPO_SUPPORTED)
        message(STATUS "IPO is supported.")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    endif()
endif()

set(QT_LIBS
    WebChannel
    WebEngineWidgets
)

find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_LIBS} LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_LIBS} LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR}Gui COMPONENTS Private REQUIRED)

configure_file(resource.rc.in resource.rc @ONLY)

set(TS_FILES skymodlr_zh_CN.ts)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    downloadschemehandler.cpp downloadschemehandler.h
    downloader.cpp downloader.h
    settingsdialog.cpp settingsdialog.h settingsdialog.ui
    resources.qrc
    icons.qrc
    ${TS_FILES}
    "${CMAKE_CURRENT_BINARY_DIR}/resource.rc"
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(skymodlr WIN32
        ${PROJECT_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(skymodlr SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(skymodlr WIN32
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT skymodlr)
target_sources(skymodlr PRIVATE ${QM_FILES})
target_compile_definitions(skymodlr
    PRIVATE	QT_DEPRECATED_WARNINGS QT_RESTRICTED_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
            QT_NO_CAST_FROM_BYTEARRAY QT_USE_QSTRINGBUILDER QT_NO_PROCESS_COMBINED_ARGUMENT_START
            QT_DISABLE_DEPRECATED_BEFORE=0x060000 WIN32_LEAN_AND_MEAN
)

foreach(L ${QT_LIBS})
    target_link_libraries(skymodlr PRIVATE Qt${QT_VERSION_MAJOR}::${L})
    target_precompile_headers(skymodlr PRIVATE "<Qt${L}>")
endforeach()

target_link_libraries(skymodlr PRIVATE Qt${QT_VERSION_MAJOR}::GuiPrivate)
target_precompile_headers(skymodlr PRIVATE "stable.h")

if(WIN32)
    find_library(WinRT windowsapp)

    if(WinRT)
        target_compile_definitions(skymodlr PRIVATE HAS_WINRT)
        target_link_libraries(skymodlr PRIVATE ${WinRT})
    endif()
endif()
