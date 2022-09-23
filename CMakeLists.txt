cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_VERBOSE_MAKEFILE ON)

project(textfilter)
include_directories(
    "./vendor/usr/include/"
    "./vendor/include/"
    "./src"
) 

add_library(Qt5::Gui_EGL SHARED IMPORTED)
set_property(TARGET Qt5::Gui_EGL APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "./vendor/usr/include/")
set_property(TARGET Qt5::Gui_EGL APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_property(TARGET Qt5::Gui_EGL PROPERTY IMPORTED_LOCATION_RELEASE "./vendor/usr/lib/x86_64-linux-gnu/libEGL.so")

add_library(Qt5::Gui_GL SHARED IMPORTED)
set_property(TARGET Qt5::Gui_GL APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "./vendor/usr/include/")
set_property(TARGET Qt5::Gui_GL APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_property(TARGET Qt5::Gui_GL PROPERTY IMPORTED_LOCATION_RELEASE "./vendor/usr/lib/x86_64-linux-gnu/libGL.so")


set(Qt5_DIR ./vendor/usr/lib/x86_64-linux-gnu/cmake/Qt5)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

if(WIN32)
  # see https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(WIN32)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "./src/*")
file(GLOB_RECURSE VENDOR_SOURCES CONFIGURE_DEPENDS "./vendor/src/*")

add_executable(${PROJECT_NAME} ${APP_SOURCES} ${VENDOR_SOURCES})

target_link_libraries(textfilter PRIVATE Qt5::Widgets)

