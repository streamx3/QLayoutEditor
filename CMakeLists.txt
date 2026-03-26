cmake_minimum_required(VERSION 3.16)

project(QLayoutEditor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        addlanguagedialog.cpp
        addlanguagedialog.h
        resources.qrc
        # third_party/magic_enum/include/magic_enum.hpp
)

if(WIN32)
    list(APPEND PROJECT_SOURCES app.rc)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QLayoutEditor APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/magic_enum/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QLayoutEditor)
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

if (WIN32)
    # app.rc embeds the manifest (CREATEPROCESS_MANIFEST_RESOURCE_ID = 1),
    # so disable the linker's auto-generated manifest to avoid duplicate resource error.
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/MANIFEST:NO"
    )
endif()

# --------------------------------------------------------------------------
# Qt Installer Framework target
# --------------------------------------------------------------------------
# Prerequisites:
#   1. Qt IFW installed (e.g. via Qt Maintenance Tool — "Qt Installer Framework")
#   2. Set QTIFWDIR to the IFW bin folder if not auto-detected, e.g.:
#        cmake -DQTIFWDIR="C:/Qt/QtIFW-4.x.x/bin" ..
#   3. windeployqt must be on PATH (it ships with Qt in <Qt>/bin/)
#
# Usage (from the build directory):
#   cmake --build . --target installer
# --------------------------------------------------------------------------
if (WIN32)
    # Locate windeployqt from Qt's bin directory
    get_target_property(_qt_bin Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qt_bin}" DIRECTORY)
    find_program(WINDEPLOYQT windeployqt HINTS "${_qt_bin_dir}" DOC "windeployqt tool")

    # Locate binarycreator — check QTIFWDIR first, then search under C:/Qt/Tools/QtInstallerFramework/*/bin
    if (DEFINED QTIFWDIR)
        find_program(BINARYCREATOR binarycreator HINTS "${QTIFWDIR}" NO_DEFAULT_PATH DOC "Qt IFW binarycreator")
    else()
        # Derive the Qt root (three levels up from <Qt>/<ver>/<kit>/bin/qmake.exe)
        get_filename_component(_qt_root "${_qt_bin_dir}/../../.." ABSOLUTE)
        file(GLOB _ifw_bin_dirs "${_qt_root}/Tools/QtInstallerFramework/*/bin")
        find_program(BINARYCREATOR binarycreator HINTS ${_ifw_bin_dirs} DOC "Qt IFW binarycreator")
    endif()

    set(_pkg_data_dir "${CMAKE_SOURCE_DIR}/installer/packages/com.streamx3.qlayouteditor/data")
    set(_installer_out "${CMAKE_BINARY_DIR}/QLayoutEditorInstaller.exe")

    add_custom_target(installer
        DEPENDS ${PROJECT_NAME}
        COMMENT "Building Qt Installer Framework package..."

        # 1. Clean and recreate the package data directory
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${_pkg_data_dir}"
        COMMAND ${CMAKE_COMMAND} -E make_directory  "${_pkg_data_dir}"

        # 2. Copy the built executable
        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:${PROJECT_NAME}>"
            "${_pkg_data_dir}/"

        # 3. Deploy Qt runtime DLLs alongside the executable copy
        COMMAND "${WINDEPLOYQT}" --no-translations "${_pkg_data_dir}/QLayoutEditor.exe"

        # 4. Copy the license so the installer can display it
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/LICENSE"
            "${CMAKE_SOURCE_DIR}/installer/packages/com.streamx3.qlayouteditor/meta/LICENSE.txt"

        # 5. Run binarycreator
        COMMAND "${BINARYCREATOR}"
            --config  "${CMAKE_SOURCE_DIR}/installer/config/config.xml"
            --packages "${CMAKE_SOURCE_DIR}/installer/packages"
            "${_installer_out}"

        VERBATIM
    )

    if (NOT BINARYCREATOR)
        message(STATUS "binarycreator not found — 'installer' target will not work. "
                       "Install Qt IFW or set -DQTIFWDIR=<path/to/ifw/bin>.")
    endif()
    if (NOT WINDEPLOYQT)
        message(STATUS "windeployqt not found — 'installer' target will not work. "
                       "Make sure Qt bin directory is in PATH.")
    endif()
endif()
